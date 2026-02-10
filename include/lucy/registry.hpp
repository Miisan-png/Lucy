#pragma once

#include "entity.hpp"
#include "sparse_set.hpp"
#include "component_pool.hpp"
#include "view.hpp"
#include <vector>
#include <unordered_map>
#include <memory>

namespace lucy {

namespace detail {
    inline uint32_t next_type_id() {
        static uint32_t counter = 0;
        return counter++;
    }

    template<typename T>
    uint32_t type_id() {
        static uint32_t id = next_type_id();
        return id;
    }
}

class Registry {
public:
    Entity create() {
        Entity e;
        if (!free_list_.empty()) {
            uint32_t idx = free_list_.back();
            free_list_.pop_back();
            e = entity::make(idx, generations_[idx]);
        } else {
            uint32_t idx = static_cast<uint32_t>(generations_.size());
            generations_.push_back(0);
            e = entity::make(idx, 0);
        }
        alive_count_++;
        return e;
    }

    void destroy(Entity e) {
        if (!alive(e)) return;

        for (auto& [id, pool] : pools_) {
            if (pool->contains(e)) {
                pool->remove(e);
            }
        }

        uint32_t idx = entity::index(e);
        generations_[idx] = (generations_[idx] + 1) & entity::MAX_GEN;
        free_list_.push_back(idx);
        alive_count_--;
    }

    bool alive(Entity e) const {
        uint32_t idx = entity::index(e);
        if (idx >= generations_.size()) return false;
        return entity::generation(e) == generations_[idx] &&
               !in_free_list(idx);
    }

    template<typename T, typename... Args>
    T& add(Entity e, Args&&... args) {
        return pool<T>().add(e, std::forward<Args>(args)...);
    }

    template<typename T>
    void remove(Entity e) {
        auto* p = try_pool<T>();
        if (p) p->remove(e);
    }

    template<typename T>
    T& get(Entity e) {
        return pool<T>().get(e);
    }

    template<typename T>
    const T& get(Entity e) const {
        return pool<T>().get(e);
    }

    template<typename T>
    T* try_get(Entity e) {
        auto* p = try_pool<T>();
        if (!p) return nullptr;
        return p->try_get(e);
    }

    template<typename T>
    bool has(Entity e) const {
        auto* p = try_pool<T>();
        return p && p->contains(e);
    }

    template<typename... Ts, typename Func>
    void each(Func&& func) {
        auto v = view<Ts...>();
        v.each(std::forward<Func>(func));
    }

    template<typename... Ts>
    View<Ts...> view() {
        return View<Ts...>(pool<Ts>()...);
    }

    template<typename T>
    ComponentPool<T>& pool() {
        uint32_t id = detail::type_id<T>();
        auto it = pools_.find(id);
        if (it == pools_.end()) {
            auto [inserted, _] = pools_.emplace(id, std::make_unique<ComponentPool<T>>());
            return static_cast<ComponentPool<T>&>(*inserted->second);
        }
        return static_cast<ComponentPool<T>&>(*it->second);
    }

    size_t size() const { return alive_count_; }

    void clear() {
        pools_.clear();
        generations_.clear();
        free_list_.clear();
        alive_count_ = 0;
    }

private:
    template<typename T>
    ComponentPool<T>* try_pool() {
        uint32_t id = detail::type_id<T>();
        auto it = pools_.find(id);
        if (it == pools_.end()) return nullptr;
        return static_cast<ComponentPool<T>*>(it->second.get());
    }

    template<typename T>
    const ComponentPool<T>* try_pool() const {
        uint32_t id = detail::type_id<T>();
        auto it = pools_.find(id);
        if (it == pools_.end()) return nullptr;
        return static_cast<const ComponentPool<T>*>(it->second.get());
    }

    bool in_free_list(uint32_t idx) const {
        for (uint32_t f : free_list_) {
            if (f == idx) return true;
        }
        return false;
    }

    std::unordered_map<uint32_t, std::unique_ptr<SparseSet>> pools_;
    std::vector<uint32_t> generations_;
    std::vector<uint32_t> free_list_;
    size_t alive_count_ = 0;
};

}
