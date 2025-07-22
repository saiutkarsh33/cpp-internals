#include <vector>      
#include <utility>     
#include <algorithm>   
template <typename K, typename V>
class FlatMap {

    // sorted vector -> use binary search
    std::vector<std::pair<K,V>> v;
    public:

    // lets start with the rule of 5

    FlatMap() = default;

    FlatMap(const FlatMap& other) : v(other.v) {
    }

    FlatMap(FlatMap&& other) noexcept: v(std::move(other.v)){
    }

    FlatMap& operator=(const FlatMap& other) { 
        if (this != &other) {
            v = other.v;
        }
        return *this;
    }

    FlatMap& operator=(FlatMap&& other) noexcept { 
        if (this != &other) {
            v = std::move(other.v);
        }
        return *this;
    }

    ~FlatMap() = default;

    V& operator[](const K& key) {
        // get the first value thats >= this
        auto it = std::lower_bound(v.begin(), v.end(), key,
        [](const std::pair<K, V>& p, const K& k) {
            return p.first < k;
        });
        if (it == v.end() || it->first != key ) {
            it = v.insert(it, std::make_pair(key, V{}));
        }
        return it->second;
    }

    bool erase(const K& key) {
        auto it = std::lower_bound(v.begin(), v.end(), key,
        [](const std::pair<K, V>& p, const K& k) {
            return p.first < k;
        });
        if (it == v.end() || it->first != key ) {
            return false;
        }
        v.erase(it);
        return true;

    }

};