#include "gtest/gtest.h"
#include "String.hpp"
#include <cstring> // For std::strlen

// Test default constructor: should create an empty string.
TEST(StringTest, DefaultConstructor) {
    String s;
    EXPECT_EQ(s.size(), 0);
    EXPECT_STREQ(s.c_str(), "");
}

// Test constructing from a C-style string.
TEST(StringTest, CStrConstructor) {
    String s("Hello");
    EXPECT_EQ(s.size(), 5);
    EXPECT_STREQ(s.c_str(), "Hello");
}

// Test constructing from a C-style string with a specified length.
TEST(StringTest, CStrConstructorWithLength) {
    const char* text = "Hello, world!";
    String s(text, 5); // Only copy "Hello"
    EXPECT_EQ(s.size(), 5);
    EXPECT_STREQ(s.c_str(), "Hello");
}

// Test copy constructor.
TEST(StringTest, CopyConstructor) {
    String s1("Copy me");
    String s2(s1);
    EXPECT_EQ(s2.size(), s1.size());
    EXPECT_STREQ(s2.c_str(), s1.c_str());
}

// Test move constructor.
TEST(StringTest, MoveConstructor) {
    String s1("Move me");
    String s2(std::move(s1));
    EXPECT_EQ(s2.size(), std::strlen("Move me"));
    EXPECT_STREQ(s2.c_str(), "Move me");
    // Moved-from object should be left empty.
    EXPECT_EQ(s1.size(), 0);
    EXPECT_STREQ(s1.c_str(), "");
}

// Test copy assignment operator.
TEST(StringTest, CopyAssignment) {
    String s1("Original");
    String s2;
    s2 = s1;
    EXPECT_EQ(s2.size(), s1.size());
    EXPECT_STREQ(s2.c_str(), s1.c_str());
}

// Test move assignment operator.
TEST(StringTest, MoveAssignment) {
    String s1("Move assign");
    String s2;
    s2 = std::move(s1);
    EXPECT_EQ(s2.size(), std::strlen("Move assign"));
    EXPECT_STREQ(s2.c_str(), "Move assign");
    // Moved-from object should be empty.
    EXPECT_EQ(s1.size(), 0);
    EXPECT_STREQ(s1.c_str(), "");
}

// Test appending a C-style string.
TEST(StringTest, AppendCString) {
    String s("Hello");
    s.append(", world!");
    EXPECT_EQ(s.size(), std::strlen("Hello, world!"));
    EXPECT_STREQ(s.c_str(), "Hello, world!");
}

// Test appending another String.
TEST(StringTest, AppendString) {
    String s1("Foo");
    String s2("Bar");
    s1.append(s2);
    EXPECT_EQ(s1.size(), std::strlen("FooBar"));
    EXPECT_STREQ(s1.c_str(), "FooBar");
}

// Test operator+= for C-style strings.
TEST(StringTest, OperatorPlusEqualCString) {
    String s("Start");
    s += " End";
    EXPECT_EQ(s.size(), std::strlen("Start End"));
    EXPECT_STREQ(s.c_str(), "Start End");
}

// Test operator+= for Strings.
TEST(StringTest, OperatorPlusEqualString) {
    String s("First");
    String t(" Second");
    s += t;
    EXPECT_EQ(s.size(), std::strlen("First Second"));
    EXPECT_STREQ(s.c_str(), "First Second");
}

// Test element access operator.
TEST(StringTest, ElementAccess) {
    String s("ABCDE");
    EXPECT_EQ(s[0], 'A');
    s[0] = 'Z';
    EXPECT_EQ(s[0], 'Z');
}

// Test clear function.
TEST(StringTest, ClearFunction) {
    String s("Not empty");
    s.clear();
    EXPECT_EQ(s.size(), 0);
    EXPECT_STREQ(s.c_str(), "");
}

// Test equality operator for String.
TEST(StringTest, EqualityOperatorString) {
    String s1("Test");
    String s2("Test");
    EXPECT_TRUE(s1 == s2);
}

// Test equality operator for C-style string.
TEST(StringTest, EqualityOperatorCString) {
    String s("Hello");
    EXPECT_TRUE(s == "Hello");
}

// Test conversion to std::string.
TEST(StringTest, ToStdStringConversion) {
    String s("Convert me");
    std::string stdStr = s.toStdString();
    EXPECT_EQ(stdStr, "Convert me");
}

// Test iterator support (non-const version).
TEST(StringTest, Iterator) {
    String s("Iterate");
    std::string result;
    for (auto it = s.begin(); it != s.end(); ++it) {
        result.push_back(*it);
    }
    EXPECT_EQ(result, "Iterate");
}

// Test iterator support with a const String.
TEST(StringTest, ConstIterator) {
    const String s("ConstIter");
    std::string result;
    for (auto it = s.begin(); it != s.end(); ++it) {
        result.push_back(*it);
    }
    EXPECT_EQ(result, "ConstIter");
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
