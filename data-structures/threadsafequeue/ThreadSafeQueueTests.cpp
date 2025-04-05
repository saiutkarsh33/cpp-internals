#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include "ThreadSafeQueue.hpp"

TEST(ThreadSafeQueueTest, InitialEmpty) {
    ThreadSafeQueue<int> q;
    EXPECT_TRUE(q.empty());
}

TEST(ThreadSafeQueueTest, PushAndTryPop) {
    ThreadSafeQueue<int> q;
    q.push(42);
    
    int result;
    EXPECT_TRUE(q.try_pop(result));
    EXPECT_EQ(result, 42);
    EXPECT_TRUE(q.empty());
}

TEST(ThreadSafeQueueTest, TryPopOnEmpty) {
    ThreadSafeQueue<std::string> q;
    std::string result;
    EXPECT_FALSE(q.try_pop(result));
}

TEST(ThreadSafeQueueTest, WaitPopBasic) {
    ThreadSafeQueue<int> q;
    int result = 0;
    
    std::thread consumer([&]() {
        q.wait_pop(result);
    });
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    q.push(42);
    consumer.join();
    
    EXPECT_EQ(result, 42);
    EXPECT_TRUE(q.empty());
}

TEST(ThreadSafeQueueTest, RvaluePush) {
    ThreadSafeQueue<std::string> q;
    q.push(std::string("test"));
    
    std::string result;
    EXPECT_TRUE(q.try_pop(result));
    EXPECT_EQ(result, "test");
}

TEST(ThreadSafeQueueTest, MoveConstructor) {
    ThreadSafeQueue<int> original;
    original.push(1);
    original.push(2);
    
    ThreadSafeQueue<int> moved(std::move(original));
    
    int result;
    EXPECT_TRUE(moved.try_pop(result));
    EXPECT_EQ(result, 1);
    EXPECT_TRUE(original.empty());
}

TEST(ThreadSafeQueueTest, MoveAssignment) {
    ThreadSafeQueue<int> source;
    source.push(10);
    source.push(20);
    
    ThreadSafeQueue<int> target;
    target = std::move(source);
    
    int result;
    EXPECT_TRUE(target.try_pop(result));
    EXPECT_EQ(result, 10);
    EXPECT_TRUE(source.empty());
}

TEST(ThreadSafeQueueTest, ConcurrentOperations) {
    ThreadSafeQueue<int> q;
    const int num_items = 1000;
    std::atomic<int> counter{0};
    
    auto producer = [&]() {
        for(int i = 0; i < num_items; ++i) {
            q.push(i);
            counter++;
        }
    };
    
    auto consumer = [&]() {
        int value;
        while(counter > 0) {
            if(q.try_pop(value)) {
                counter--;
            }
        }
    };
    
    std::thread t1(producer);
    std::thread t2(consumer);
    t1.join();
    t2.join();
    
    EXPECT_TRUE(q.empty());
    EXPECT_EQ(counter.load(), 0);
}

TEST(ThreadSafeQueueTest, MultipleConsumers) {
    ThreadSafeQueue<int> q;
    const int num_items = 100;
    std::atomic<int> received{0};
    
    auto producer = [&]() {
        for(int i = 0; i < num_items; ++i) {
            q.push(i);
        }
    };
    
    auto consumer = [&]() {
        int value;
        while(received < num_items) {
            if(q.try_pop(value)) {
                received++;
            }
        }
    };
    
    std::thread t1(producer);
    std::thread t2(consumer);
    std::thread t3(consumer);
    
    t1.join();
    t2.join();
    t3.join();
    
    EXPECT_EQ(received.load(), num_items);
    EXPECT_TRUE(q.empty());
}