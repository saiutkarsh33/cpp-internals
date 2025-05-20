#pragma once

#include <vector>
#include <functional

// eg you initilise a flatmap like FlatMap<int, int> and comparator is optional (theres a default)
// here its default ascending
template<typename Key, typename Value, typename Compare = std::less<Key>>
class FlatMap {
public:
    using value_type = std::pair<Key, T>;
    using container_type = std::vector<value_type>;
    using iterator = typename container_type::iterator;
    using const_iterator = typename container_type::const_iterator;

    flat_map() = default;
    explicit flat_map(const Compare& comp)
        : comp_(comp) {}

    // Find element by key; returns end() if not found
    iterator find(const Key& key) {
        auto it = lower_bound(key);
        if (it != data_.end() && !comp_(key, it->first)) {
            return it;
        }
        return data_.end();
    }

    const_iterator find(const Key& key) const {
        auto it = lower_bound(key);
        if (it != data_.end() && !comp_(key, it->first)) {
            return it;
        }
        return data_.end();
    }

    // Insert or assign
    std::pair<iterator, bool> insert_or_assign(const Key& key, const T& value) {
        auto it = lower_bound(key);
        if (it != data_.end() && !comp_(key, it->first)) {
            // key already exists, overwrite
            it->second = value;
            return {it, false};
        }
        // insert at position
        it = data_.insert(it, std::make_pair(key, value));
        return {it, true};
    }

    // Erase by key; returns number of elements removed (0 or 1)
    size_t erase(const Key& key) {
        auto it = find(key);
        if (it != data_.end()) {
            data_.erase(it);
            return 1;
        }
        return 0;
    }

    // Lower bound: first element not less than key
    iterator lower_bound(const Key& key) {
        // lower bound uses binary search under the hood
        return std::lower_bound(
            data_.begin(), data_.end(), key,
            // capturing this so u can use compare
            [this](auto& pair, const Key& k) {
                return comp_(pair.first, k);
            }
        );
    }
    const_iterator lower_bound(const Key& key) const {
        return std::lower_bound(
            data_.begin(), data_.end(), key,
            [this](auto& pair, const Key& k) {
                return comp_(pair.first, k);
            }
        );
    }

    // Iterators
    // we using vector's iterators -> pointers!
    iterator begin() { return data_.begin(); }
    iterator end()   { return data_.end();   }
    const_iterator begin() const { return data_.begin(); }
    const_iterator end()   const { return data_.end();   }

    bool empty() const { return data_.empty(); }
    size_t size() const { return data_.size(); }
    void clear() { data_.clear(); }

private:
    container_type data_;
    // default initiliaser - if didnt put anything in, its thhe std::less
    Compare comp_{};
};














}

