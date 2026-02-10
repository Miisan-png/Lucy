#pragma once

#include "entity.hpp"
#include <vector>
#include <cassert>

namespace lucy {

class SparseSet {
public:
    void add(Entity e) {
        uint32_t idx = entity::index(e);
        if (idx >= sparse_.size()) {
            sparse_.resize(idx + 1, entity::null);
        }
        sparse_[idx] = static_cast<uint32_t>(dense_.size());
        dense_.push_back(e);
    }

    void remove(Entity e) {
        uint32_t idx = entity::index(e);
        if (idx >= sparse_.size() || sparse_[idx] == entity::null) return;

        uint32_t dense_idx = sparse_[idx];
        Entity last = dense_.back();

        dense_[dense_idx] = last;
        sparse_[entity::index(last)] = dense_idx;

        dense_.pop_back();
        sparse_[idx] = entity::null;

        on_swap(dense_idx, static_cast<uint32_t>(dense_.size()));
    }

    bool contains(Entity e) const {
        uint32_t idx = entity::index(e);
        if (idx >= sparse_.size()) return false;
        uint32_t dense_idx = sparse_[idx];
        return dense_idx < dense_.size() && dense_[dense_idx] == e;
    }

    uint32_t dense_index(Entity e) const {
        return sparse_[entity::index(e)];
    }

    size_t size() const { return dense_.size(); }
    bool empty() const { return dense_.empty(); }

    const Entity* data() const { return dense_.data(); }
    const Entity* begin() const { return dense_.data(); }
    const Entity* end() const { return dense_.data() + dense_.size(); }

    virtual ~SparseSet() = default;

protected:
    virtual void on_swap(uint32_t a, uint32_t b) { (void)a; (void)b; }

    std::vector<uint32_t> sparse_;
    std::vector<Entity> dense_;
};

}
