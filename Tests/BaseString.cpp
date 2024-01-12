#include "BaseString.h"

#include <gtest/gtest.h>

TEST(BaseString, Constructor_Default) {
    Strazzle::BaseString<char> default_constructor;

    ASSERT_STREQ(default_constructor.CStr(), "");
}

TEST(BaseString, Constructor_CString) {
    Strazzle::BaseString<char> cstr_constructor("foo");

    ASSERT_STREQ(cstr_constructor.CStr(), "foo");

    Strazzle::BaseString<char> cstr_len_constructor("bar", 2);

    ASSERT_STREQ(cstr_len_constructor.CStr(), "ba");
}

TEST(BaseString, Constructor_Reference) {
}