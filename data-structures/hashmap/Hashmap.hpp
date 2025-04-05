#include <cstddef>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <list>
#include <type_traits>
#include <utility>

// Not thread safe

template <typename K, typename V>
class Hashmap {
private:
    // type alias
    using value_type = std::pair<const K, V>;
    // seperate chaining , linked list
    using Bucket = std::list<value_type>;

public:
    // Iterator is used to go thru each kv pair sequentially bucket by bucket like an array
    template <bool IsConst> 
    // const when u dont want to modify
    struct Iterator {
        // its a forward iterator
        using iterator_category = std::forward_iterator_tag;
        // the difference between 2 iterators; how many elements apart
        using difference_type = std::ptrdiff_t;
        using pointer = typename std::conditional<IsConst, const value_type*, value_type*>::type;
        using reference = typename std::conditional<IsConst, const value_type&, value_type&>::type;

        // Bucket::iterator does not mean that bucket is a namespace here
        // iterator is a nested type inside C++'s std::list

        Iterator(typename Bucket::iterator ptr, Bucket* bucketPtr, size_t currentBucket, size_t numOfBuckets):
                mPtr(ptr), mBucketPtr(bucketPtr), currentBucket(currentBucket), numOfBuckets(numOfBuckets) {}
        // mPtr is current bucket
        // mBucketPtr is a pointer to the beginning of the array of buckets
        reference operator*() const {
            return *mPtr;
        }

        pointer operator->() { return &(*mPtr); }

    
        typename Bucket::iterator getBucketIterator() {
            return mPtr;
        }
        
        // prefix
        // return reference to avoid making a copy and allows for chaining
        Iterator& operator++() {
            next();
            // this is a pointer to current object, and dereferncing gives u the obj as an lvalue (reference to it)
            return *this;
        }
        
        // postfix
        Iterator operator++(int) {
            Iterator tmp{*this};
            next();
            return tmp;
            // doesent return pointer so you cant use chaining
        }
        
        // 2 iterators are equal if they both are at the end of their respective buckets or
        // they both point to the same element in the same bucket

        // friend means non member function operator== can access private members like mPtr etc
        friend bool operator==(const Iterator&a, const Iterator& b) { 
            if (a.numOfBuckets == a.currentBucket && b.numOfBuckets == b.currentBucket) {
                return true;
            }

            if (a.numOfBuckets == a.currentBucket || b.numOfBuckets == b.currentBucket) {
                return false;
            }

            return a.mPtr == b.mPtr && a.currentBucket == b.currentBucket;
        }

        friend bool operator!=(const Iterator&a, const Iterator& b) { 
            return !(a == b);
        }


    private:
        // goes through each k,v pair sequentially almost like an array
        void next() {
            mPtr++;
            if (mPtr != mBucketPtr[currentBucket].end()) {
                return;
            }
            
            currentBucket++;
            while(currentBucket < numOfBuckets && mBucketPtr[currentBucket].empty()) {
                currentBucket++;
            }
            
            if (currentBucket == numOfBuckets) {
                return;
            }
            mPtr = mBucketPtr[currentBucket].begin();
        }

        typename Bucket::iterator mPtr;
        Bucket* mBucketPtr;
        size_t currentBucket;
        size_t numOfBuckets;

    };

    using iterator = Iterator<false>;
    using const_iterator = Iterator<true>;

    // uses compilers default constructor, initiliasises each member using their default constructor

    Hashmap() = default;

    // initialiser list constructor

    Hashmap(std::initializer_list<value_type> lst) {
        rehash(lst.size());
        for (const value_type& item: lst) {
            insert(item);
        }
    }

    //  copy constructor
    Hashmap(const Hashmap& other): maxLoadFactor(other.maxLoadFactor) {
        // initialises this new hashmap with same bucket count as old one, using rehash
        rehash(other.mBucketCount);
        for (const auto& el: other) {
            insert(el);
        }
    }
    
    // move constructor
    Hashmap(Hashmap&& other):  mBucketCount(other.mBucketCount),
        mSize(other.mSize), maxLoadFactor(other.maxLoadFactor) {
        mStore = other.mStore;
        other.mStore = nullptr;
        other.mBucketCount = 0;
        other.mSize = 0;
    }

    // returns value to a key

    V& at(const K& key) {
        auto it = find(key);
        if (it == end()) {
            throw std::out_of_range("invalid key");
        }
        return it->second;
    } 

    // returns a mutable iterator that will point to the element in the hashmap, and boolean to indicate
    // if we successfully managed to add

    std::pair<Iterator<false>, bool> insert(const value_type& value) {
        // if no buckets, allocate at least one
        if (mBucketCount == 0) {
            rehash(1);
        }
        // compute the bucket index
        size_t bucketIdx = hashKey(value.first, mBucketCount);

        // check if key already exists and replace
        // call std::list::begin, which returns an iterator pointing to the first element of that bucket's list
        typename Bucket::iterator start = mStore[bucketIdx].begin();
        for (; start != mStore[bucketIdx].end(); start++) {
            if (start->first == value.first) {
                start->second = value.second;
                break;
            }
        }
        
        // return early if key already existed and we replace
        if (start != mStore[bucketIdx].end()) {
            return { Iterator<false>{start, mStore, bucketIdx, mSize }, false };
        }

        if (static_cast<double>(mSize + 1) / mBucketCount >= maxLoadFactor) {
            rehash(mBucketCount * 2);
            bucketIdx = hashKey(value.first, mBucketCount);
        }
        // std::list::insert is invoked, returns an iterator pointing to the newly inserrted element
        start = mStore[bucketIdx].insert(mStore[bucketIdx].end(), value);
        mSize++;

        return { Iterator<false>{start, mStore, bucketIdx, mSize }, true };
    }

    V& operator[](const K& key) {
        auto it = find(key);
        if (it != end()) {
            return it->second;
        }
        // no item found, return default
        auto res = insert({key, V{}});
        return res.first->second;
    }

    iterator find(const K& key) {
        size_t bucketIdx = hashKey(key, mBucketCount);
        if (bucketIdx < mBucketCount) {
            // use std::list::end and begin here
            for (auto start = mStore[bucketIdx].begin(); start != mStore[bucketIdx].end(); start++) {
                if (start->first == key) {
                    return Iterator<false>{start, mStore, bucketIdx, mBucketCount};
                }
            }
        }
        return end();
    }

    bool contains(const K& key) {
        return find(key) != end();
    }

    bool erase(const K& key) {
        iterator it = find(key);
        if (it == end()) {
            return false;
        }
        size_t bucketIdx = hashKey(key, mBucketCount);
        // remove the node from the linked list using std::list:erase
        mStore[bucketIdx].erase(it.getBucketIterator());
        mSize--;
        return true;
    }

    size_t size() const {
        return mSize;
    }

    size_t bucket_count() const {
        return mBucketCount;
    }
    // count is the new number of buckets
    void rehash(size_t count) {
        // new dynamic array of buckets 
        Bucket* newStore = new Bucket[count];

        for (size_t i = 0; i < mBucketCount; i++) {
            for (const value_type& val: mStore[i]) {
                size_t bucketIdx = hashKey(val.first, count);
                // move over
                newStore[bucketIdx].insert(newStore[bucketIdx].end(), std::move(val));
            }
        }
        delete[] mStore;
        mStore = newStore;
        mBucketCount = count;
    }

    const_iterator cbegin() const {
        for (size_t i = 0; i < mBucketCount; i++) {
            if (!mStore[i].empty()) {
                return Iterator<true>{mStore[i].begin(), mStore, i, mBucketCount};
            }
        }
        return cend();
    }
    
    const_iterator cend() const {
        return Iterator<true>{{}, mStore, mBucketCount, mBucketCount};
    }

    iterator begin() {
        for (size_t i = 0; i < mBucketCount; i++) {
            if (!mStore[i].empty()) {
                return Iterator<false>{mStore[i].begin(), mStore, i, mBucketCount};
            }
        }
        return end();
    }
    
    const_iterator begin() const {
        return cbegin();
    }

    const_iterator end() const {
        return cend();
    }

    iterator end() {
        return Iterator<false>{{}, mStore, mBucketCount, mBucketCount};
    }

    ~Hashmap() {
        if (mStore) {
            delete[] mStore;
        }
    }

private:
    size_t hashKey(const K& key, size_t bucketCount) {
        // uses a hashing algorithm
        size_t hashIdx = std::hash<K>()(key);
        return hashIdx % bucketCount;
    }
    
    // points to a dynamically allocated array of buckets
    Bucket* mStore = nullptr;
    size_t mBucketCount = 0;
    size_t mSize = 0;
    double maxLoadFactor = 0.7;
};
