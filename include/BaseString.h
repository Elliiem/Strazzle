#pragma once

#include <cstring>
#include <immintrin.h>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace Strazzle {

enum class StringAllocError { OK, ALLOC_ERR };
enum class BaseStringError { OK, ALLOC_ERR };

unsigned clz(unsigned long long x) {
    return x ? __builtin_clzll(x) : 64;
}

std::size_t ScalingFunction(std::size_t size) {
    return 1 << (64 - clz(size));
}

template<typename CharT> class BaseString;
template<typename CharT> class StringAllocator;
template<typename CharT> struct BaseStringReference;

// TODO Fix access specifiers
template<typename CharT> class BaseString {
    friend StringAllocator;

  public:
    StringAllocator<CharT> alloc;
    uint64_t l;

    BaseString() {
        alloc.base = this;
    }

    BaseString(const char* str) {
        alloc.base = this;

        uint64_t sl = strlen(str);

        Realloc(sl);

        memcpy(alloc.c, str, sl);

        l = sl;
    }

    BaseStringError Realloc(std::size_t size) {
        size_t nl = ScalingFunction(size);

        if(alloc.l != nl) {
#ifndef STRAZZLE_NO_ERROR_CHECKING
            if(alloc.Realloc(nl) == StringAllocError::ALLOC_ERR) {
                return BaseStringError::ALLOC_ERR;
            }
#else
            alloc.Realloc(nl);
#endif
        }

        return BaseStringError::OK;
    }

    BaseStringError Append(const char* str) {
        uint64_t string_len = strlen(str);
        uint64_t new_len = l + string_len;

#ifndef STRAZZLE_NO_ERROR_CHECKING
        if(Realloc(new_len) != BaseStringError::OK) {
            return BaseStringError::ALLOC_ERR;
        }
#else
        Realloc(new_len)
#endif
        memcpy(alloc.c + l, str, string_len);

        l = new_len;

        return BaseStringError::OK;
    };

    BaseStringError Append(const char* str, std::size_t size) {
        uint64_t new_len = l + size;

#ifndef STRAZZLE_NO_ERROR_CHECKING
        if(Realloc(new_len) != BaseStringError::OK) {
            return BaseStringError::ALLOC_ERR;
        }
#else
        Realloc(new_len)
#endif
        memcpy(alloc.c + l, str, size);

        l = new_len;

        return BaseStringError::OK;
    }

    BaseStringError Insert(std::size_t i, const char* str) {
        if(i > l) {
            throw std::runtime_error("Index is out of bounds! << BaseString::Insert()\n");
        }

        uint64_t string_len = strlen(str);
        uint64_t new_len = l + string_len;

#ifndef STRAZZLE_NO_ERROR_CHECKING
        if(Realloc(new_len) != BaseStringError::OK) {
            return BaseStringError::ALLOC_ERR;
        }
#else
        Realloc(new_len);
#endif

        memmove(alloc.c + i + string_len, alloc.c + i, l - i);

        memcpy(alloc.c + i, str, string_len);

        l = new_len;

        return BaseStringError::OK;
    }

    BaseStringError Resize(std::size_t size, const char* replace) {
#ifndef STRAZZLE_NO_ERROR_CHECKING
        if(Realloc(size) != BaseStringError::OK) {
            return BaseStringError::ALLOC_ERR;
        }
#else
        Realloc(size);
#endif
        uint64_t sl = strlen(replace);

        for(uint64_t i = l; i < size;) {
            if(i + sl > size) {
                Append(replace, (i + sl) - size - 1);
                break;
            }

            Append(replace);
            i += strlen(replace);
        }
    }

    char& operator[](std::size_t i) {
        if(i <= l) {
            throw std::runtime_error("Index out of bounds! << BaseString::operator[]()\n");
        }
        return alloc.c[i];
    }

    // TODO BaseStringReference SubstrRef(std::size_t i, uint64_t len);
    // TODO BaseString Substr(std::size_t i, uint64_t len);
};

// TODO Fix access specifiers
template<typename CharT> struct BaseStringReference {
    friend StringAllocator;
    CharT* c = nullptr;
    uint64_t l = 0;
    StringAllocator<CharT>& alloc;

    BaseStringReference(CharT* c, uint64_t l, StringAllocator<CharT>& alloc) {
        this->c = c;
        this->l = l;
        this->alloc = alloc;
    }

    char& operator[](std::size_t i) {
        if(i <= l) {
            throw std::runtime_error("Index out of bounds! << BaseString::operator[]()\n");
        }
        return c[i];
    }

    // TODO BaseString Detach();

  private:
    void Shift(int64_t shift) {
        c += shift;
    }

    void ShiftResize(int64_t shift, uint64_t size) {
        c += shift;
        l = size;
    }
};

// TODO Fix access specifiers
template<typename CharT> class StringAllocator {
    friend BaseString;
    friend BaseStringReference;

  public:
    StringAllocator() {
    }

    StringAllocator(BaseString<CharT>& base) {
        this->base = base;
    }

    StringAllocator(BaseString<CharT>& base, std::size_t size) {
        Realloc(size);
    }

    ~StringAllocator() {
        delete c;
    }

    StringAllocError Realloc(std::size_t size) {
        CharT* nc = new CharT[size];

        if(nc == nullptr) {
#ifndef STRAZZLE_NO_ERROR_CHECKING
            return StringAllocError::ALLOC_ERR;
#else
            throw std::runtime_error("Failed to allocate memory! << StringAllocator::Realloc()\n");
#endif
        }

        if(c != nullptr) {
            memcpy(nc, c, l > size ? l - (l - size) : l);
            delete c;
        }

        c = nc;
        l = size;
        return StringAllocError::OK;
    }

    CharT* c = nullptr;
    std::size_t l = 0;

#ifndef STRAZZLE_NO_DYNAMIC_REFERENCES
    BaseString<CharT>* base;
    std::vector<BaseStringReference<CharT>> refs;
#endif
};

} // namespace Strazzle
