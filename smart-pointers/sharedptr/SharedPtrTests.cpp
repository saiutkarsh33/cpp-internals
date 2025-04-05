#include "gtest/gtest.h"
#include "SharedPtr.hpp" // Make sure this path is correct for your SharedPtr implementation.
#include <string>

// Test default constructor.
TEST(SharedPtrTest, DefaultConstructor) {
    SharedPtr<int> sp;
    // The default constructor should yield a null pointer.
    EXPECT_EQ(sp.get(), nullptr);
    // And the reference count should be 0.
    EXPECT_EQ(sp.get_count(), 0);
}

// Test construction from a raw pointer.
TEST(SharedPtrTest, ConstructorFromRawPointer) {
    SharedPtr<int> sp(new int(42));
    EXPECT_NE(sp.get(), nullptr);
    EXPECT_EQ(*sp, 42);
    // Reference count should be 1.
    EXPECT_EQ(sp.get_count(), 1);
}

// Test copy constructor.
TEST(SharedPtrTest, CopyConstructor) {
    SharedPtr<int> sp1(new int(100));
    EXPECT_EQ(sp1.get_count(), 1);
    // Copy sp1 into sp2.
    SharedPtr<int> sp2(sp1);
    // Both shared pointers should refer to the same underlying pointer.
    EXPECT_EQ(sp1.get(), sp2.get());
    // The reference count should now be 2.
    EXPECT_EQ(sp1.get_count(), 2);
    EXPECT_EQ(sp2.get_count(), 2);
}

// Test copy assignment operator.
TEST(SharedPtrTest, CopyAssignmentOperator) {
    SharedPtr<int> sp1(new int(10));
    SharedPtr<int> sp2;
    sp2 = sp1;
    EXPECT_EQ(sp1.get(), sp2.get());
    EXPECT_EQ(sp1.get_count(), 2);
    EXPECT_EQ(sp2.get_count(), 2);
}

// Test move constructor.
TEST(SharedPtrTest, MoveConstructor) {
    SharedPtr<int> sp1(new int(55));
    EXPECT_EQ(sp1.get_count(), 1);
    // Move construct sp2 from sp1.
    SharedPtr<int> sp2(std::move(sp1));
    // sp2 should now own the pointer.
    EXPECT_NE(sp2.get(), nullptr);
    EXPECT_EQ(*sp2, 55);
    // sp1 should be in an empty state.
    EXPECT_EQ(sp1.get(), nullptr);
    EXPECT_EQ(sp1.get_count(), 0);
    EXPECT_EQ(sp2.get_count(), 1);
}

// Test move assignment operator.
TEST(SharedPtrTest, MoveAssignmentOperator) {
    SharedPtr<int> sp1(new int(77));
    SharedPtr<int> sp2;
    sp2 = std::move(sp1);
    EXPECT_NE(sp2.get(), nullptr);
    EXPECT_EQ(*sp2, 77);
    // sp1 should now be empty.
    EXPECT_EQ(sp1.get(), nullptr);
    EXPECT_EQ(sp1.get_count(), 0);
    EXPECT_EQ(sp2.get_count(), 1);
}

// Test arrow operator.
TEST(SharedPtrTest, ArrowOperator) {
    struct Test {
        int x;
        int getX() const { return x; }
    };

    SharedPtr<Test> sp(new Test{99});
    EXPECT_EQ(sp->x, 99);
    EXPECT_EQ(sp->getX(), 99);
}

// Test self-assignment for copy.
TEST(SharedPtrTest, SelfAssignmentCopy) {
    SharedPtr<int> sp(new int(5));
    sp = sp; // Self-assignment.
    EXPECT_NE(sp.get(), nullptr);
    EXPECT_EQ(*sp, 5);
    EXPECT_EQ(sp.get_count(), 1);
}

// Test self-assignment for move.
TEST(SharedPtrTest, SelfAssignmentMove) {
    SharedPtr<int> sp(new int(123));
    sp = std::move(sp);
    // After move self-assignment, sp should remain valid.
    EXPECT_NE(sp.get(), nullptr);
    EXPECT_EQ(*sp, 123);
    EXPECT_EQ(sp.get_count(), 1);
}
