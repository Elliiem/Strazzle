
#define STRAZZLE_DEBUG_ALL_PUBLIC
#define NDEBUG

#include "Strazzle/String.h"

#include <iostream>
#include <string>

int main() {
    Strazzle::String foo("ABCDEFG");

    Strazzle::String::Reference foo_ref = foo.RefSubstr(0);

    Strazzle::String bar("00001111\n");

    bar.Insert(foo_ref, 4);

    printf(bar.Cstr());
}