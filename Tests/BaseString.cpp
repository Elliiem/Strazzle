#include "BaseString.h"

#include <gtest/gtest.h>

TEST(BaseString, Constructor_Default) {
    Strazzle::BaseString<char> default_constructor;

    ASSERT_STREQ(default_constructor.CStr(), "");
}

TEST(BaseString, Constructor_CString) {
    Strazzle::BaseString<char> cstr_constructor("foo");

    ASSERT_STREQ(cstr_constructor.CStr(), "foo");
    ASSERT_EQ(cstr_constructor.Len(), 3);

    Strazzle::BaseString<char> cstr_len_constructor("bar", 2);

    ASSERT_STREQ(cstr_len_constructor.CStr(), "ba");
    ASSERT_EQ(cstr_len_constructor.Len(), 2);

    Strazzle::BaseString<char> cstr_zero_len_constructor("bar", 0);

    ASSERT_STREQ(cstr_zero_len_constructor.CStr(), "");
    ASSERT_EQ(cstr_zero_len_constructor.Len(), 0);

    Strazzle::BaseString<char> cstr_empty_constructor("", 2);

    ASSERT_STREQ(cstr_empty_constructor.CStr(), "");
    ASSERT_EQ(cstr_empty_constructor.Len(), 0);
}

TEST(BaseString, Constructor_Reference) {
}

TEST(BaseString, Append) {
    Strazzle::BaseString<char> str;
    Strazzle::BaseStringError res;

    res = str.Append("foo");
    ASSERT_EQ(res, Strazzle::BaseStringError::OK);

    ASSERT_STREQ(str.CStr(), "foo");
    ASSERT_EQ(str.Len(), 3);

    str.Append("bar");
    ASSERT_EQ(res, Strazzle::BaseStringError::OK);

    ASSERT_STREQ(str.CStr(), "foobar");
    ASSERT_EQ(str.Len(), 6);

    str.Append("xyz", 0);
    ASSERT_EQ(res, Strazzle::BaseStringError::OK);

    ASSERT_STREQ(str.CStr(), "foobar");
    ASSERT_EQ(str.Len(), 6);

    str.Append("zzz", 2);
    ASSERT_EQ(res, Strazzle::BaseStringError::OK);

    ASSERT_STREQ(str.CStr(), "foobarzz");
    ASSERT_EQ(str.Len(), 8);
}

TEST(BaseString, Insert) {
    Strazzle::BaseString<char> str;
    Strazzle::BaseStringError res;

    res = str.Insert(0, "foo");
    ASSERT_EQ(res, Strazzle::BaseStringError::OK);

    ASSERT_STREQ(str.CStr(), "foo");
    ASSERT_EQ(str.Len(), 3);

    res = str.Insert(3, "bar");
    ASSERT_EQ(res, Strazzle::BaseStringError::OK);

    ASSERT_STREQ(str.CStr(), "foobar");
    ASSERT_EQ(str.Len(), 6);

    res = str.Insert(0, "  ");
    ASSERT_EQ(res, Strazzle::BaseStringError::OK);

    ASSERT_STREQ(str.CStr(), "  foobar");
    ASSERT_EQ(str.Len(), 8);

    res = str.Insert(5, "  ");
    ASSERT_EQ(res, Strazzle::BaseStringError::OK);

    ASSERT_STREQ(str.CStr(), "  foo  bar");
    ASSERT_EQ(str.Len(), 10);
}

TEST(BaseString, Erase) {
    Strazzle::BaseString<char> str("##xxx##");
    Strazzle::BaseStringError res;

    res = str.Erase(2, 3);
    ASSERT_EQ(res, Strazzle::BaseStringError::OK);

    ASSERT_STREQ(str.CStr(), "####");
    ASSERT_EQ(str.Len(), 4);

    res = str.Erase(0, 2);
    ASSERT_EQ(res, Strazzle::BaseStringError::OK);

    ASSERT_STREQ(str.CStr(), "##");
    ASSERT_EQ(str.Len(), 2);

    res = str.Erase(0, 2);
    ASSERT_EQ(res, Strazzle::BaseStringError::OK);

    ASSERT_STREQ(str.CStr(), "");
    ASSERT_EQ(str.Len(), 0);
}

TEST(BaseString, Resize) {
    Strazzle::BaseString<char> str;
    Strazzle::BaseStringError res;

    res = str.Resize(4);
    ASSERT_EQ(res, Strazzle::BaseStringError::OK);

    ASSERT_STREQ(str.CStr(), "    ");
    ASSERT_EQ(str.Len(), 4);

    res = str.Resize(11, "xy");
    ASSERT_EQ(res, Strazzle::BaseStringError::OK);

    ASSERT_STREQ(str.CStr(), "    xyxyxyx");
    ASSERT_EQ(str.Len(), 11);

    res = str.Resize(5);
    ASSERT_EQ(res, Strazzle::BaseStringError::OK);

    ASSERT_STREQ(str.CStr(), "    x");
    ASSERT_EQ(str.Len(), 5);
}