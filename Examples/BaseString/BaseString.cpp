
#define STRAZZLE_DEBUG_ALL_PUBLIC
#define NDEBUG

#include "BaseString.h"

#include <iostream>
#include <string>

int main() {
    Strazzle::BaseString foo;

    foo.Insert("AAAAAAAAAAAAAAAAAA\n", 0);

    printf("allocated: | %u => %u |\n", foo._allocated_exp, Strazzle::_ExpToNum(foo._allocated_exp));

    printf("%s", foo.Cstr());
}