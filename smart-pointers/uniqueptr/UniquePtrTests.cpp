#include <gtest/gtest.h>
#include "UniquePtr.hpp"  

// Helper class to track destruction
struct TestObject {
    static bool destroyed;
    ~TestObject() { destroyed = true; }
};
bool TestObject::destroyed = false;

// Test constructor and basic pointer management
TEST(UniquePtrTest, ConstructorAndGet) {
    UniquePtr<int> ptr(new int(42));
    ASSERT_NE(ptr.get(), nullptr);
    EXPECT_EQ(*ptr.get(), 42);
}

// Test if destructor properly deletes the managed object
TEST(UniquePtrTest, Destructor) {
    TestObject::destroyed = false;
    {
        UniquePtr<TestObject> ptr(new TestObject);
    }
    EXPECT_TRUE(TestObject::destroyed);
}

// Test move constructor
TEST(UniquePtrTest, MoveConstructor) {
    UniquePtr<int> original(new int(100));
    UniquePtr<int> moved(std::move(original));
    
    EXPECT_EQ(original.get(), nullptr);
    ASSERT_NE(moved.get(), nullptr);
    EXPECT_EQ(*moved, 100);
}

// Test move assignment operator
TEST(UniquePtrTest, MoveAssignment) {
    UniquePtr<int> source(new int(200));
    UniquePtr<int> target(new int(0));
    
    target = std::move(source);
    
    EXPECT_EQ(source.get(), nullptr);
    ASSERT_NE(target.get(), nullptr);
    EXPECT_EQ(*target, 200);
}

// Test self-assignment via move (should be a no-op)
TEST(UniquePtrTest, MoveSelfAssignment) {
    UniquePtr<int> ptr(new int(300));
    ptr = std::move(ptr);  // Should not cause issues
    
    ASSERT_NE(ptr.get(), nullptr);
    EXPECT_EQ(*ptr, 300);
}

// Test dereference operator
TEST(UniquePtrTest, DereferenceOperator) {
    UniquePtr<int> ptr(new int(55));
    EXPECT_EQ(*ptr, 55);
}

// Test arrow operator for member access
TEST(UniquePtrTest, ArrowOperator) {
    struct TestStruct { int value; };
    UniquePtr<TestStruct> ptr(new TestStruct{99});
    EXPECT_EQ(ptr->value, 99);
}

// Test get() method
TEST(UniquePtrTest, GetMethod) {
    int* raw = new int(10);
    UniquePtr<int> ptr(raw);
    EXPECT_EQ(ptr.get(), raw);
}

// Test release() method
TEST(UniquePtrTest, ReleaseMethod) {
    UniquePtr<int> ptr(new int(20));
    int* raw = ptr.release();
    
    EXPECT_EQ(ptr.get(), nullptr);
    EXPECT_EQ(*raw, 20);
    delete raw;  // Manual cleanup after release
}

// Test reset() method
TEST(UniquePtrTest, ResetMethod) {
    TestObject::destroyed = false;
    TestObject* first = new TestObject;
    {
        UniquePtr<TestObject> ptr(first);
        ptr.reset(new TestObject);  // Reset with new object
        
        EXPECT_TRUE(TestObject::destroyed);  // First object deleted
        TestObject::destroyed = false;
    }
    EXPECT_TRUE(TestObject::destroyed);  // Second object deleted
}

// Test reset with nullptr
TEST(UniquePtrTest, ResetToNull) {
    UniquePtr<int> ptr(new int(30));
    ptr.reset();
    EXPECT_EQ(ptr.get(), nullptr);
}

// Test UniquePtr initialized with nullptr
TEST(UniquePtrTest, ConstructWithNull) {
    UniquePtr<int> ptr(nullptr);
    EXPECT_EQ(ptr.get(), nullptr);
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}