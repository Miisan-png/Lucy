#pragma once

#include "sparse_set.hpp"
#include <utility>

namespace lucy {

template<typename T>
class ComponentPool : public SparseSet {
public:
    template<typename... Args>
    T& add(Entity e, Args&&... args) {
        SparseSet::add(e);
        components_.emplace_back(std::forward<Args>(args)...);
        return components_.back();
    }

    void remove(Entity e) {
        if (!contains(e)) return;
        SparseSet::remove(e);
    }

    T& get(Entity e) {
        return components_[dense_index(e)];
    }

    const T& get(Entity e) const {
        return components_[dense_index(e)];
    }

    T* try_get(Entity e) {
        if (!contains(e)) return nullptr;
        return &components_[dense_index(e)];
    }

    T* component_data() { return components_.data(); }
    const T* component_data() const { return components_.data(); }

protected:
    void on_swap(uint32_t a, uint32_t b) override {
        if (a < components_.size() && b < components_.size()) {
            std::swap(components_[a], components_[b]);
        } else if (b < components_.size()) {
            components_[a] = std::move(components_[b]);
        }
        components_.pop_back();
    }

private:
    std::vector<T> components_;
};

}
