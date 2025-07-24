#include <unordered_map>
#include <cstddef>
template<typename K, typename V>
class UnorderedBimap {
public:
    // Inserts the (key,value) pair.
    // Returns false if either the key or value already exists.
    bool insert(const K& key, const V& value) {
        if (keyToVal.find(key) !=  keyToVal.end() || valToKey.find(value) !=  valToKey.end()) {
            return false;
        }
        keyToVal[key] = value;
        valToKey[value] = key;
        return true;
    }
    // Removes by key or by value.
    // Returns true if something was erased.
    bool erase_by_key(const K& key) {
        //auto& it = keyToVal.find(key);
        // The above line throws compiler error because find returns a prvalue, a tempoeraty ,
        // which cannot be casted to a non const lvalue reference

        //const auto& it = keyToVal.find(key); WORKS ALSO
        // can use auto below as well
        std::unordered_map<K, V>::iterator it= keyToVal.find(key);
        if (it ==  keyToVal.end()) {
            return false;
        }
        auto& value = it->second;
        // you can do keyToVal.erase(key) but it does an additional lookup to get the it
        keyToVal.erase(it);
        valToKey.erase(value);
        return true;
    }

    // Lookup: returns pointer (or nullptr) so you can distinguish “not found.”
    const V* find_by_key(const K& key) const {
        auto it = keyToVal.find(key);
        if ( it ==  keyToVal.end()) {
            return nullptr;
        }
        // can't use [] operator on a const method
        
        return &it->second;        
    }
    // find_by_value and erase_by_value are trivial exercises


private:
    std::unordered_map<K,V> keyToVal;
    std::unordered_map<V,K> valToKey;
};
