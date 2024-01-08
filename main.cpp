#include "BaseString.h"

int main() {
    Strazzle::BaseString<char> foo("Hello, world!");
    foo.Resize(20, "sus");
    foo.Insert(20, "baka");

    printf(foo.alloc.c);

    return 0;
}