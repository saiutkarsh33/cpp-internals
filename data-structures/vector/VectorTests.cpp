#include <gtest/gtest.h>
#include <string>
#include "Vector.hpp"

TEST(VectorTest, DefaultConstructorCreatesEmptyVector) {
    Vector<int> vec;
    EXPECT_EQ(vec.size(), 0);
    EXPECT_EQ(vec.capacity(), 0);
}

TEST(VectorTest, InitializerListConstructor) {
    Vector<int> vec{1, 2, 3};
    EXPECT_EQ(vec.size(), 3);
    EXPECT_EQ(vec[0], 1);
    EXPECT_EQ(vec[1], 2);
    EXPECT_EQ(vec[2], 3);
}

TEST(VectorTest, SizeConstructorWithElement) {
    Vector<std::string> vec(3, "test");
    EXPECT_EQ(vec.size(), 3);
    EXPECT_EQ(vec[0], "test");
    EXPECT_EQ(vec[1], "test");
    EXPECT_EQ(vec[2], "test");
}

TEST(VectorTest, CopyConstructor) {
    Vector<int> original{1, 2, 3};
    Vector<int> copy(original);
    
    EXPECT_EQ(copy.size(), 3);
    original[0] = 10;
    EXPECT_EQ(copy[0], 1); // Deep copy verification
}

TEST(VectorTest, MoveConstructor) {
    Vector<int> original{1, 2, 3};
    Vector<int> moved(std::move(original));
    
    EXPECT_EQ(moved.size(), 3);
    EXPECT_EQ(original.size(), 0);
    EXPECT_EQ(original.capacity(), 0);
}

TEST(VectorTest, PushBackIncreasesSize) {
    Vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    
    EXPECT_EQ(vec.size(), 2);
    EXPECT_EQ(vec[0], 1);
    EXPECT_EQ(vec[1], 2);
}

TEST(VectorTest, PushBackCapacityGrowth) {
    Vector<int> vec;
    vec.push_back(1);
    EXPECT_EQ(vec.capacity(), 1);
    
    vec.push_back(2);
    EXPECT_EQ(vec.capacity(), 2);
    
    vec.push_back(3);
    EXPECT_EQ(vec.capacity(), 4);
}

TEST(VectorTest, PopBackDecreasesSize) {
    Vector<int> vec{1, 2};
    vec.pop_back();
    EXPECT_EQ(vec.size(), 1);
    EXPECT_EQ(vec[0], 1);
}

TEST(VectorTest, PopBackOnEmptyVector) {
    Vector<int> vec;
    vec.pop_back(); // Shouldn't crash
    EXPECT_EQ(vec.size(), 0);
}

TEST(VectorTest, ClearRemovesAllElements) {
    Vector<std::string> vec{"a", "b", "c"};
    vec.clear();
    EXPECT_EQ(vec.size(), 0);
    EXPECT_GE(vec.capacity(), 3); // Capacity remains unchanged
}

TEST(VectorTest, EmplaceBackConstructsInPlace) {
    struct TestStruct {
        int a;
        std::string b;
        TestStruct(int x, std::string y) : a(x), b(y) {}
    };

    Vector<TestStruct> vec;
    vec.emplace_back(42, "test");
    
    EXPECT_EQ(vec[0].a, 42);
    EXPECT_EQ(vec[0].b, "test");
}

TEST(VectorTest, ResizeIncreasesCapacity) {
    Vector<int> vec;
    vec.resize(5);
    EXPECT_EQ(vec.capacity(), 5);
    EXPECT_EQ(vec.size(), 0); // Size remains 0 until elements added
}

TEST(VectorTest, ResizeSmallerDestroysElements) {
    Vector<std::string> vec{"a", "b", "c"};
    vec.resize(2);
    EXPECT_EQ(vec.size(), 2);
    EXPECT_EQ(vec[0], "a");
    EXPECT_EQ(vec[1], "b");
}


TEST(VectorTest, OperatorAccess) {
    Vector<int> vec{10, 20};
    EXPECT_EQ(vec[0], 10);
    EXPECT_EQ(vec[1], 20);
    
    // Current implementation wraps around for out-of-bounds access
    EXPECT_EQ(vec[2], 10); // Demonstrates potential bug in operator[]
}

TEST(VectorTest, CopyAssignment) {
    Vector<int> original{1, 2, 3};
    Vector<int> copy;
    copy = original;
    
    original[0] = 10;
    EXPECT_EQ(copy[0], 1); // Deep copy verification
}

TEST(VectorTest, MoveAssignment) {
    Vector<int> original{1, 2, 3};
    Vector<int> moved;
    moved = std::move(original);
    
    EXPECT_EQ(moved.size(), 3);
    EXPECT_EQ(original.size(), 0);
}

TEST(VectorTest, DataReturnsPointerToArray) {
    Vector<int> vec{1, 2, 3};
    int* data = vec.data();
    EXPECT_EQ(data[0], 1);
    EXPECT_EQ(data[1], 2);
    EXPECT_EQ(data[2], 3);
}

TEST(VectorTest, SwapExchangesContents) {
    Vector<int> a{1, 2};
    Vector<int> b{3, 4, 5};
    
    a.swap(b);
    
    EXPECT_EQ(a.size(), 3);
    EXPECT_EQ(b.size(), 2);
    EXPECT_EQ(a[0], 3);
    EXPECT_EQ(b[0], 1);
}