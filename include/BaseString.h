#pragma once

#include <cstring>
#include <immintrin.h>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace Strazzle {

/**
 * OK        => everything ok
 * ALLOC_ERR => something went wrong when allocating
 * IGNORED   => no action was performed, probaply because the action didnt make any sense
 *              for example when I call Shrink(10) but the length before was 5
 */
enum class StringAllocError { OK, ALLOC_ERR, IGNORED };

/**
 * OK        => everything ok
 * ALLOC_ERR => something went wrong when allocating
 *
 */
enum class BaseStringError { OK, ALLOC_ERR };

template<typename CharT, std::size_t TSize = sizeof(CharT)> class BaseString;
template<typename CharT, std::size_t TSize = sizeof(CharT)> class StringAllocator;
template<typename CharT, std::size_t TSize = sizeof(CharT)> struct BaseStringReference;

/**
 * @brief BaseString class template for managing strings with dynamic memory allocation.
 *        Provides functions for string manipulation, resizing, and reference creation.
 *
 * @tparam CharT The character type of the string (e.g., char, wchar_t).
 * @tparam TSize The size of the character type (e.g., sizeof(char), sizeof(wchar_t)).
 */
template<typename CharT, std::size_t TSize> class BaseString {
    friend class StringAllocator<CharT>;
    friend class BaseStringReference<CharT>;

  public:
    /**
     * @brief Default constructor for BaseString.
     */
    BaseString() {
        alloc.ReallocToExp(0);
        alloc.c[0] = 0;
    };

    /**
     * @brief Constructor that initializes the string with the provided content and size.
     *
     * @param str The character array to initialize the string with.
     * @param size The maximum size of the string (default is SIZE_MAX).
     */
    BaseString(const CharT* str, std::size_t size = SIZE_MAX) {
        uint64_t str_len = strlen(str);
        size = size < str_len ? size : str_len;

#ifndef STRAZZLE_NO_ERR_CHECKING
        if(alloc.Resize(size + 1) == StringAllocError::ALLOC_ERR) {
            throw std::bad_alloc();
        }
#else
        alloc.Resize(size + 1);
#endif

        memcpy(alloc.c, str, TSize * size);

        l = size;
        alloc.c[l] = 0;
    }

    /**
     * @brief Constructor that initializes the string with the content of a BaseStringReference.
     *
     * @param ref The reference to initialize the string with.
     */
    BaseString(BaseStringReference<CharT>& ref) {
        BaseString(ref.c, ref.l);
    }

    /**
     * @brief Destructor for BaseString.
     */
    ~BaseString() = default;

    /**
     * @brief Returns a pointer to the underlying character array.
     *
     * @return A const pointer to the character array.
     */
    const CharT* CStr() {
        return alloc.c;
    }

    /**
     * @brief Appends the given string to the end of the current string.
     *
     * @param str The string to append.
     * @param size The maximum size of the appended string (default is SIZE_MAX).
     * @return BaseStringError::OK if the operation is successful, BaseStringError::ALLOC_ERR if memory allocation fails.
     */
    BaseStringError Append(const CharT* str, std::size_t size = SIZE_MAX) {
        uint64_t str_len = strlen(str);
        uint64_t append_size = size < str_len ? size : str_len;

#ifndef STRAZZLE_NO_ERROR_CHECKING
        if(alloc.Resize(l + append_size + 1) == StringAllocError::ALLOC_ERR) {
            return BaseStringError::ALLOC_ERR;
        }
#else
        alloc.Resize(append_size + l + 1);
#endif

        memcpy(alloc.c + l, str, TSize * append_size);

        l = l + append_size;

        alloc.c[l] = 0;

        return BaseStringError::OK;
    }

    /**
     * @brief Inserts the given string at the specified position in the current string.
     *
     * @param i The index at which to insert the string.
     * @param str The string to insert.
     * @param size The maximum size of the inserted string (default is SIZE_MAX).
     * @return BaseStringError::OK if the operation is successful, BaseStringError::ALLOC_ERR if memory allocation fails.
     */
    BaseStringError Insert(std::size_t i, const CharT* str, std::size_t size = SIZE_MAX) {
        if(i == l) {
            return Append(str, size);
        }

        CheckBounds(i, "BaseString::Insert\n");

        uint64_t str_len = strlen(str);
        uint64_t insert_len = size < str_len ? size : str_len;

#ifndef STRAZZLE_NO_ERROR_CHECKING
        if(alloc.Resize(l + insert_len + 1) == StringAllocError::ALLOC_ERR) {
            return BaseStringError::ALLOC_ERR;
        }
#else
        alloc.Resize(l + insert_len + 1);
#endif

        memmove(alloc.c + i + str_len, alloc.c + i, TSize * (l - i));
        memcpy(alloc.c + i, str, TSize * insert_len);

        l = l + insert_len;
        alloc.c[l] = 0;

        return BaseStringError::OK;
    }

    /**
     * @brief Erases a portion of the string starting from the specified position.
     *
     * @param i The index at which to start erasing.
     * @param size The number of characters to erase (default is SIZE_MAX).
     * @return BaseStringError::OK if the operation is successful, BaseStringError::ALLOC_ERR if memory allocation fails.
     */
    BaseStringError Erase(std::size_t i, std::size_t size = SIZE_MAX) {
        CheckBounds(i, "BaseString::Erase");

        size = size > l ? l : size;

        memmove(alloc.c + i, alloc.c + i + size, TSize * l);

        l = l - size;
        alloc.c[l] = 0;

#ifndef STRAZZLE_NO_ERROR_CHECKING
        if(alloc.Resize(l + 1) == StringAllocError::ALLOC_ERR) {
            return BaseStringError::ALLOC_ERR;
        }
#else
        alloc.Resize(l + 1);
#endif

        return BaseStringError::OK;
    }

    /**
     * @brief Resizes the string to the given size and fills it with fill
     *
     * @param size The new size for the string
     * @param fill The string used to fill the new space
     *
     * @return Returns BaseStringError::ALLOC_ERROR if the allocation failed else returns BaseStringError::OK
     */
    BaseStringError Resize(std::size_t size, const CharT* fill = " ") {
#ifndef STRAZZLE_NO_ERROR_CHECKING
        if(alloc.Resize(size + 1) == StringAllocError::ALLOC_ERR) {
            return BaseStringError::ALLOC_ERR;
        }
#else
        alloc.Resize(size + 1);
#endif
        if(size > l) {
            if(fill == "") fill = " ";

            uint64_t sl = strlen(fill);

            for(uint64_t i = l; i < size;) {
                if(i + sl > size) {
                    Append(fill, (i + sl) - size);
                    break;
                }

                Append(fill);
                i += strlen(fill);
            }
        }

        l = size;
        alloc.c[l] = 0;

        return BaseStringError::OK;
    }

    /**
     * @brief Overloaded subscript operator for accessing individual characters in the string.
     *
     * @param i The index of the character to access.
     * @return A reference to the character at the specified index.
     */
    CharT& operator[](std::size_t i) {
        CheckBounds(i, "BaseString::operator[]");

        return alloc.c[i];
    }

    /**
     * @brief Creates a BaseStringReference for a portion of the string.
     *
     * @param i The starting index of the reference.
     * @param Len The length of the reference.
     * @return A BaseStringReference object representing the specified portion of the string.
     */
    BaseStringReference<CharT> Ref(std::size_t i, std::size_t size = SIZE_MAX) {
        CheckBounds(i, "BaseString::Ref");

        size = size > l ? l : size;

        return BaseStringReference<CharT>(alloc.c + i, size, *this);
    }

    /**
     * @brief Creates a substring of the current string.
     *
     * @param i The starting index of the substring.
     * @param Len The length of the substring.
     * @return A new BaseString object representing the specified substring.
     */
    BaseString Substr(std::size_t i, std::size_t size = SIZE_MAX) {
        CheckBounds(i, "BaseString::Substr");

        size = size > l ? l : size;

        return BaseString(alloc.c + i, size);
    }

    /**
     * @brief Returns the length of the string.
     *
     * @return The length of the string.
     */
    uint64_t Len() {
        return l;
    }

  private:
    StringAllocator<CharT> alloc;
    uint64_t l = 0;

    /**
     * @brief Checks if the index is within bounds and throws an exception if not.
     *
     * @param i The index to check.
     * @param from A string indicating the source of the check (for error messages).
     */
    inline void CheckBounds(std::size_t i, const char* from) const {
        if(i >= l) {
            throw std::runtime_error("Index is out of bounds! << " + std::string(from)); // FIXME Maybe dont use std::string
        }
    }
};

/**
 * @brief BaseStringReference struct template for creating references to portions of a BaseString.
 *        Provides access to the referenced characters and length of the reference.
 *
 * @tparam CharT The character type of the referenced string (e.g., char, wchar_t).
 * @tparam TSize The size of the character type (e.g., sizeof(char), sizeof(wchar_t)).
 */
template<typename CharT, std::size_t TSize> struct BaseStringReference {
    friend class StringAllocator<CharT>;
    friend class BaseString<CharT>;

  public:
    /**
     * @brief Destructor for BaseStringReference.
     */
    ~BaseStringReference() = default;

    /**
     * @brief Overloaded subscript operator for accessing individual characters in the reference.
     *
     * @param i The index of the character to access.
     * @return A reference to the character at the specified index.
     */
    CharT& operator[](std::size_t i) {
        CheckBaseBounds();
        CheckBounds(i, "BaseStringReference::operator[]");

        return c[i];
    }

    /**
     * @brief Returns the length of the reference.
     *
     * @return The length of the reference.
     */
    uint64_t Len() {
        return l;
    }

  private:
    /**
     * @brief Private constructor for BaseStringReference.
     *
     * @param c Pointer to the start of the reference.
     * @param l Length of the reference.
     * @param base Reference to the BaseString containing the referenced portion.
     */
    BaseStringReference(CharT* c, uint64_t l, BaseString<CharT>& base) : base(base) {
        this->c = c;
        this->l = l;
    }

    /**
     * @brief Checks if the reference is still within the bounds of the base string and throws an exception if not.
     */
    inline void CheckBaseBounds() const {
        if((c - base.alloc.c) + l > base.l) {
            throw std::runtime_error("Reference is no longer in bounds of base! << BaseStringReference::CheckBaseBounds");
        }
    }

    /**
     * @brief Checks if the index is within bounds and throws an exception if not.
     *
     * @param i The index to check.
     * @param from A string indicating the source of the check (for error messages).
     */
    inline void CheckBounds(std::size_t i, const char* from) const {
        if(i >= l) {
            throw std::runtime_error("Index out of bounds! << " + std::string(from)); // FIXME Maybe dont use std::string
        }
    }

    CharT* c = nullptr;
    uint64_t l = 0;

    BaseString<CharT>& base;
};

/**
 * @brief StringAllocator class template for managing dynamic memory allocation for strings.
 *        The class provides functions for resizing, shrinking, and reallocating memory.
 *
 * @tparam CharT The character type of the allocated memory (e.g., char, wchar_t).
 * @tparam TSize The size of the character type (e.g., sizeof(char), sizeof(wchar_t)).
 */
template<typename CharT, std::size_t TSize> class StringAllocator {
  public:
    /**
     * @brief Default constructor for StringAllocator.
     */
    StringAllocator() = default;

    /**
     * @brief Destructor for StringAllocator.
     *        Releases the allocated memory using 'delete'.
     */
    ~StringAllocator() {
        Free();
    }

    /**
     * @brief Dynamically resizes the allocated memory for the string.
     *        If the requested size is less than the current size, it invokes Shrink.
     *        Otherwise, it invokes Realloc.
     *
     * @param size The new size to resize the memory to.
     * @return StringAllocError::OK if the operation is successful,
     *         StringAllocError::ALLOC_ERR if memory allocation fails.
     *         StringAllocError::IGNORED if the requested size already was the current size
     */
    StringAllocError Resize(std::size_t size) {
        uint64_t current_size = (1 << size_exp);

        if(c == nullptr) {
            Realloc(size);
        }

        if(size < current_size) {
            return Shrink(size);
        } else if(size > current_size) {
            return Realloc(size);
        } else {
            return StringAllocError::IGNORED;
        }
    }

    /**
     * @brief Shrinks the allocated memory for the string.
     *        If the requested size is greater than or equal to the current size, it returns StringAllocError::IGNORED.
     *        Otherwise, it calculates the new size using NextExponent and updates the allocator's size.
     *
     * @param size The new size to shrink the memory to.
     * @return StringAllocError::OK if the operation is successful,
     *         StringAllocError::ALLOC_ERR if the allocation failed
     *         StringAllocError::IGNORED if the requested size is not smaller than the current size.
     */
    StringAllocError Shrink(std::size_t size) {
        if(size >= (1 << size_exp) || c == nullptr) return StringAllocError::IGNORED;

        size_exp = NextExponent(size != 0 ? size - 1 : 0);

        if(actual_size_exp - size_exp >= 2) {
#ifndef STRAZZLE_NO_ERROR_CHECKING
            if(ReallocToExp(size_exp) == StringAllocError::ALLOC_ERR) return StringAllocError::ALLOC_ERR;
#else
            ReallocToExp(size_exp);
#endif
        }

        return StringAllocError::OK;
    }

    /**
     * @brief Reallocates the memory for the string to the specified size.
     *        Uses ReallocToExp to allocate memory based on the next power of two exponent.
     *
     * @param size The new size to allocate the memory to.
     * @return StringAllocError::OK if the operation is successful,
     *         StringAllocError::ALLOC_ERR if memory allocation fails.
     */
    StringAllocError Realloc(std::size_t size) {
#ifndef STRAZZLE_NO_ERROR_CHECKING
        if(ReallocToExp(NextExponent(size)) == StringAllocError::ALLOC_ERR) return StringAllocError::ALLOC_ERR;
#else
        ReallocToExp(NextExponent(size));
#endif

        return StringAllocError::OK;
    }

    /**
     * @brief Reallocates the memory for the string to the specified exponent.
     *        Uses new to allocate a new block of memory, copies the existing content, and releases the old memory.
     *
     * @param exponent The exponent to determine the new size of the allocated memory.
     * @return StringAllocError::OK if the operation is successful,
     *         StringAllocError::ALLOC_ERR if memory allocation fails.
     */
    StringAllocError ReallocToExp(uint8_t exponent) {
        uint64_t size = 1 << exponent;

        CharT* nc = new CharT[size];
#ifndef STRAZZLE_NO_ERROR_CHECKING
        if(nc == nullptr) return StringAllocError::ALLOC_ERR;
#else
        throw std::bad_alloc();
#endif

        if(c != nullptr) {
            memcpy(nc, c, TSize * size);
            delete[] c;
        }

        c = nc;

        size_exp = exponent;
        actual_size_exp = exponent;

        return StringAllocError::OK;
    }

    /**
     * @brief Frees the allocated memory and resets the size.
     */
    void Free() {
        delete[] c;
        size_exp = 0;
        actual_size_exp = 0;
    }

    CharT* c = nullptr;
    uint8_t size_exp = 0;

  private:
    uint8_t actual_size_exp = 0;

    /**
     * @brief Counts leading zeros in an unsigned 64-bit integer.
     *
     * @param x The input unsigned 64-bit integer.
     * @return The number of leading zeros in the binary representation of x.
     */
    static inline std::size_t clz(std::size_t x) {
        return x ? __builtin_clzl(x) : 64;
    }

    /**
     * @brief Calculates the next power of two exponent based on the provided size.
     *
     * @param x The size used to calculate the next power of two exponent.
     * @return The calculated exponent.
     */
    static inline uint8_t NextExponent(std::size_t x) {
        return 64 - clz(x);
    }
};

} // namespace Strazzle
