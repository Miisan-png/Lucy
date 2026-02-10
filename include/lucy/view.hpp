#pragma once

#include "entity.hpp"
#include "component_pool.hpp"
#include <tuple>

namespace lucy {

template<typename... Ts>
class View {
public:
    View(ComponentPool<Ts>&... pools)
        : pools_(std::tie(pools...))
    {
        SparseSet* all[] = { static_cast<SparseSet*>(&pools)... };
        smallest_ = all[0];
        for (auto* s : all) {
            if (s->size() < smallest_->size()) smallest_ = s;
        }
    }

    template<typename Func>
    void each(Func&& func) {
        for (size_t i = 0; i < smallest_->size(); i++) {
            Entity e = smallest_->data()[i];
            if (all_contain(e)) {
                func(e, std::get<ComponentPool<Ts>&>(pools_).get(e)...);
            }
        }
    }

    class Iterator {
    public:
        Iterator(View* view, size_t pos) : view_(view), pos_(pos) {
            advance();
        }

        Entity operator*() const { return view_->smallest_->data()[pos_]; }

        Iterator& operator++() {
            ++pos_;
            advance();
            return *this;
        }

        bool operator!=(const Iterator& other) const { return pos_ != other.pos_; }

    private:
        void advance() {
            while (pos_ < view_->smallest_->size()) {
                Entity e = view_->smallest_->data()[pos_];
                if (view_->all_contain(e)) break;
                ++pos_;
            }
        }

        View* view_;
        size_t pos_;
    };

    Iterator begin() { return Iterator(this, 0); }
    Iterator end() { return Iterator(this, smallest_->size()); }

    template<typename... Us>
    std::tuple<Us&...> get(Entity e) {
        return std::tie(std::get<ComponentPool<Us>&>(pools_).get(e)...);
    }

    size_t size_hint() const { return smallest_->size(); }

private:
    bool all_contain(Entity e) const {
        return (std::get<ComponentPool<Ts>&>(pools_).contains(e) && ...);
    }

    std::tuple<ComponentPool<Ts>&...> pools_;
    SparseSet* smallest_;
};

}
