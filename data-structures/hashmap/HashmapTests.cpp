#include <gtest/gtest.h>
#include <string>
#include "Hashmap.hpp"

// DeepSeek Generated Tests

TEST(HashmapTest, DefaultConstructor) {
    Hashmap<int, std::string> map;
    EXPECT_EQ(map.size(), 0);
    EXPECT_EQ(map.bucket_count(), 0);
}

TEST(HashmapTest, InitializerListConstructor) {
    Hashmap<int, std::string> map = {
        {1, "one"},
        {2, "two"},
        {3, "three"}
    };
    EXPECT_EQ(map.size(), 3);
    EXPECT_GE(map.bucket_count(), 3);
}

TEST(HashmapTest, InsertAndFind) {
    Hashmap<std::string, int> map;
    auto [it, inserted] = map.insert({"key", 42});
    EXPECT_TRUE(inserted);
    EXPECT_EQ(it->second, 42);
    
    auto found = map.find("key");
    EXPECT_NE(found, map.end());
    EXPECT_EQ(found->second, 42);
}

TEST(HashmapTest, InsertDuplicate) {
    Hashmap<std::string, int> map;
    map.insert({"key", 42});
    auto [it, inserted] = map.insert({"key", 99});
    EXPECT_FALSE(inserted);
    EXPECT_EQ(it->second, 99);
    EXPECT_EQ(map.size(), 1);
}

TEST(HashmapTest, OperatorBracket) {
    Hashmap<std::string, int> map;
    map["key"] = 42;
    EXPECT_EQ(map["key"], 42);
    EXPECT_EQ(map.size(), 1);
    
    // Test default construction for non-existent key
    EXPECT_EQ(map["new_key"], 0);
    EXPECT_EQ(map.size(), 2);
}

TEST(HashmapTest, At) {
    Hashmap<std::string, int> map;
    map["key"] = 42;
    EXPECT_EQ(map.at("key"), 42);
    
    EXPECT_THROW(map.at("nonexistent"), std::out_of_range);
}

TEST(HashmapTest, Contains) {
    Hashmap<int, std::string> map;
    map[1] = "one";
    EXPECT_TRUE(map.contains(1));
    EXPECT_FALSE(map.contains(2));
}

TEST(HashmapTest, Erase) {
    Hashmap<int, std::string> map;
    map[1] = "one";
    map[2] = "two";
    
    EXPECT_TRUE(map.erase(1));
    EXPECT_EQ(map.size(), 1);
    EXPECT_FALSE(map.contains(1));
    EXPECT_TRUE(map.contains(2));
    
    EXPECT_FALSE(map.erase(99)); // non-existent key
}

TEST(HashmapTest, Rehash) {
    Hashmap<int, std::string> map;
    size_t initial_buckets = 10;
    map.rehash(initial_buckets);
    EXPECT_EQ(map.bucket_count(), initial_buckets);
    
    // Insert enough elements to trigger rehash
    for (int i = 0; i < 20; ++i) {
        map[i] = std::to_string(i);
    }
    EXPECT_GT(map.bucket_count(), initial_buckets);
}

TEST(HashmapTest, Iteration) {
    Hashmap<int, std::string> map = {
        {1, "one"},
        {2, "two"},
        {3, "three"}
    };
    
    int count = 0;
    for (const auto& [key, value] : map) {
        ++count;
        EXPECT_FALSE(value.empty());
    }
    EXPECT_EQ(count, 3);
}

TEST(HashmapTest, CopyConstructor) {
    Hashmap<int, std::string> original;
    original[1] = "one";
    original[2] = "two";
    
    Hashmap<int, std::string> copy(original);
    EXPECT_EQ(copy.size(), 2);
    EXPECT_EQ(copy[1], "one");
    EXPECT_EQ(copy[2], "two");
    
    // Verify it's a deep copy
    original[1] = "modified";
    EXPECT_EQ(copy[1], "one");
}

TEST(HashmapTest, MoveConstructor) {
    Hashmap<int, std::string> original;
    original[1] = "one";
    original[2] = "two";
    
    Hashmap<int, std::string> moved(std::move(original));
    EXPECT_EQ(moved.size(), 2);
    EXPECT_EQ(moved[1], "one");
    EXPECT_EQ(moved[2], "two");
    EXPECT_EQ(original.size(), 0);
}

TEST(HashmapTest, ConstIteration) {
    const Hashmap<int, std::string> map = {
        {1, "one"},
        {2, "two"}
    };
    
    int count = 0;
    for (auto it = map.begin(); it != map.end(); ++it) {
        ++count;
    }
    EXPECT_EQ(count, 2);
}

TEST(HashmapTest, LoadFactor) {
    Hashmap<int, int> map;
    // Insert elements to trigger rehash
    for (int i = 0; i < 100; ++i) {
        map[i] = i;
        EXPECT_LE(static_cast<double>(map.size()) / map.bucket_count(), 0.7);
    }
}

TEST(HashmapTest, ComplexKeyType) {
    Hashmap<std::string, int> map;
    std::string key1 = "key1";
    std::string key2 = "key2";
    
    map[key1] = 1;
    map[key2] = 2;
    
    EXPECT_EQ(map[key1], 1);
    EXPECT_EQ(map[key2], 2);
}

TEST(HashmapTest, ComplexValueType) {
    Hashmap<int, std::vector<int>> map;
    map[1] = {1, 2, 3};
    map[2] = {4, 5, 6};
    
    EXPECT_EQ(map[1].size(), 3);
    EXPECT_EQ(map[2].size(), 3);
    EXPECT_EQ(map[1][0], 1);
    EXPECT_EQ(map[2][2], 6);
}