#include <gtest/gtest.h>
#include "ThreadSafeLinkedList.hpp"
#include <thread>

TEST(ThreadSafeLinkedListTest, ConstructorCreatesEmptyList) {
    ThreadSafeLinkedList<int> list;
    EXPECT_TRUE(list.empty());
}

TEST(ThreadSafeLinkedListTest, PushFrontAddsElement) {
    ThreadSafeLinkedList<int> list;
    list.push_front(42);
    EXPECT_FALSE(list.empty());
}

TEST(ThreadSafeLinkedListTest, PushBackAddsElement) {
    ThreadSafeLinkedList<int> list;
    list.push_back(42);
    EXPECT_FALSE(list.empty());
}

TEST(ThreadSafeLinkedListTest, RemoveFrontOnEmptyReturnsFalse) {
    ThreadSafeLinkedList<int> list;
    EXPECT_FALSE(list.remove_front());
}

TEST(ThreadSafeLinkedListTest, RemoveFrontRemovesElement) {
    ThreadSafeLinkedList<int> list;
    list.push_front(42);
    EXPECT_TRUE(list.remove_front());
    EXPECT_TRUE(list.empty());
}

TEST(ThreadSafeLinkedListTest, PushFrontPopFrontOrder) {
    ThreadSafeLinkedList<int> list;
    list.push_front(1);
    list.push_front(2);
    EXPECT_TRUE(list.remove_front()); // Removes 2
    EXPECT_TRUE(list.remove_front()); // Removes 1
}

TEST(ThreadSafeLinkedListTest, PushBackRemoveFrontOrder) {
    ThreadSafeLinkedList<int> list;
    list.push_back(1);
    list.push_back(2);
    EXPECT_TRUE(list.remove_front()); // Removes 1
    EXPECT_TRUE(list.remove_front()); // Removes 2
}

TEST(ThreadSafeLinkedListTest, BasicThreadSafety) {
    ThreadSafeLinkedList<int> list;
    const int num_operations = 1000;
    
    auto producer = [&list]() {
        for (int i = 0; i < num_operations; ++i) {
            list.push_front(i);
        }
    };

    auto consumer = [&list]() {
        for (int i = 0; i < num_operations; ++i) {
            list.remove_front();
        }
    };

    std::thread t1(producer);
    std::thread t2(consumer);
    t1.join();
    t2.join();
    
    // Verify list is in valid state
    EXPECT_TRUE(list.empty());
}