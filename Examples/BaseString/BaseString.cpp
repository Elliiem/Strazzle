
#define STRAZZLE_DEBUG_ALL_PUBLIC

#include "BaseString.h"

#include <iostream>
#include <string>

int main() {
    Strazzle::BaseString foo;

    foo.Append("foobar\n");
    foo._data;

    printf("%s", foo.Cstr());
}