#include "BaseString.h"

int main() {
    Strazzle::BaseString<char> foo("Hello, !\n");

    for(uint64_t i = foo.len() + 1; i < foo.alloc.l; i++) {
        foo.alloc.c[i] = 69;
    }

    for(uint64_t i = 0; i < foo.alloc.l; i++) {
        printf("%u\n", foo.alloc.c[i]);
    }
    printf("\n");

    // foo.Insert(7, "SUS");
    foo.Erase(2, 2);

    /*for(uint64_t i = foo.len() + 1; i < foo.alloc.l; i++) {
        foo.alloc.c[i] = 69;
    }*/

    for(uint64_t i = 0; i < foo.alloc.l; i++) {
        printf("%u\n", foo.alloc.c[i]);
    }

    printf("\n");
    printf(foo.CStr());
    return 0;
}