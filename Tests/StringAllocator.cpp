#include "BaseString.h"

#include <gtest/gtest.h>

TEST(StringAllocator, Constructor_Default) {
    Strazzle::StringAllocator<char> alloc;

    ASSERT_TRUE(alloc.c == nullptr);
    ASSERT_TRUE(alloc.size_exp == 0);
}

TEST(StringAllocator, ReallocToExp) {
    Strazzle::StringAllocator<char> alloc;
    Strazzle::StringAllocError res;

    res = alloc.ReallocToExp(5);
    ASSERT_EQ(res, Strazzle::StringAllocError::OK);

    ASSERT_EQ(alloc.size_exp, 5);
    ASSERT_NE(alloc.c, nullptr);

    res = alloc.ReallocToExp(0);
    ASSERT_EQ(res, Strazzle::StringAllocError::OK);

    ASSERT_EQ(alloc.size_exp, 0);
    ASSERT_NE(alloc.c, nullptr);
}

TEST(StringAllocator, Realloc) {
    Strazzle::StringAllocator<char> alloc;
    Strazzle::StringAllocError res;

    res = alloc.Realloc(100);
    ASSERT_EQ(res, Strazzle::StringAllocError::OK);

    ASSERT_EQ(alloc.size_exp, 7);
    ASSERT_NE(alloc.c, nullptr);

    res = alloc.Realloc(0);
    ASSERT_EQ(res, Strazzle::StringAllocError::OK);

    ASSERT_EQ(alloc.size_exp, 0);
    ASSERT_NE(alloc.c, nullptr);
}

TEST(StringAllocator, Shrink) {
    Strazzle::StringAllocator<char> alloc;
    Strazzle::StringAllocError res;

    alloc.Realloc(16);

    res = alloc.Shrink(7);
    ASSERT_EQ(res, Strazzle::StringAllocError::OK);

    ASSERT_EQ(alloc.size_exp, 3);

    res = alloc.Shrink(16);
    ASSERT_EQ(res, Strazzle::StringAllocError::IGNORED);

    ASSERT_EQ(alloc.size_exp, 3);

    res = alloc.Shrink(0);
    ASSERT_EQ(res, Strazzle::StringAllocError::OK);

    ASSERT_EQ(alloc.size_exp, 0);
}

TEST(StringAllocator, Resize) {
    Strazzle::StringAllocator<char> alloc;
    Strazzle::StringAllocError res;

    res = alloc.Resize(10);
    ASSERT_EQ(res, Strazzle::StringAllocError::OK);

    ASSERT_EQ(alloc.size_exp, 4);

    res = alloc.Resize(16);
    ASSERT_EQ(res, Strazzle::StringAllocError::IGNORED);

    ASSERT_EQ(alloc.size_exp, 4);

    res = alloc.Resize(8);
    ASSERT_EQ(res, Strazzle::StringAllocError::OK);

    ASSERT_EQ(alloc.size_exp, 3);

    res = alloc.Resize(0);
    ASSERT_EQ(res, Strazzle::StringAllocError::OK);

    ASSERT_EQ(alloc.size_exp, 0);
}