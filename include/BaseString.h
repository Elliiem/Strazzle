#include <cinttypes>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdexcept>

namespace Strazzle {

/**
 * @brief Utility function to count leading zeros in a size_t value.
 * @param x The input value.
 * @return The count of leading zeros.
 */
inline std::size_t _clz(std::size_t x) {
    return x ? __builtin_clzl(x) : 64;
}

/**
 * @brief Utility function to get the exponent for dynamic memory allocation.
 * @param x The input size.
 * @return The exponent value.
 */
inline uint8_t _GetExponent(std::size_t x) {
    std::size_t clz_x = Strazzle::_clz(x);
    return ((clz_x != Strazzle::_clz(x - 1)) && (clz_x != 0)) ? (63 - clz_x) : (64 - clz_x);
}

inline std::size_t _ExpToNum(uint8_t exp) {
    return 1UL << exp;
}

const std::size_t SSO_SIZE = 16;

/**
 * @brief String class with Small String Optimization (SSO) and dynamic memory allocation support.
 */
struct BaseString {
  public:
    struct Reference {
        friend BaseString;

      private:
        Reference(char* base_c, std::size_t i, std::size_t len, std::size_t base_len) : _base_c(base_c), _i(i), _len(len), _base_len(base_len) {
        }

        char* _base_c;
        std::size_t _i;

        std::size_t _base_len;
        std::size_t _len;

      private:
        void CheckBounds() {
            if(_i + _len > _base_len)
                throw std::out_of_range("Reference is not within bounds of base! << Strazzle::BaseString::Reference::CheckBounds()");
        }
    };

  public:
    BaseString() : _data(_sso_buffer) {
    }

    BaseString(const char* str, std::size_t size = SIZE_MAX) {
        _data = _sso_buffer;

        Strazzle::BaseString::Append(str, size);
    }

    BaseString(Strazzle::BaseString::Reference& ref, std::size_t size = SIZE_MAX) {
    }

#ifndef STRAZZLE_DEBUG_ALL_PUBLIC
  private:
#endif
    enum class Mode : uint8_t { SMALL_STRING = 0, LARGE_STRING = 1 };

    // Small String Optimization buffer
    char _sso_buffer[Strazzle::SSO_SIZE];
    // Pointer to the string buffer
    char* _data = nullptr;

    // Length of the string
    std::size_t _len = 0;

    // Current mode of the string (Small or Large)
    Mode _mode = Strazzle::BaseString::Mode::SMALL_STRING;
#ifdef NDEBUG
    // Size of allocated memory (used when NDEBUG is defined)
    std::size_t allocated_size = Strazzle::SSO_SIZE;
#endif

  public:
    /**
     * @brief Append a string to the end of the current string.
     * @param str The string to append.
     * @param size Maximum size to append (default is SIZE_MAX).
     */
    void Append(const char* str, std::size_t size = SIZE_MAX) {
        size = std::min(strlen(str), size);
        Strazzle::BaseString::Reserve(size + _len);

        std::memcpy(_data + _len, str, size);

        _len = size + _len;

        _data[_len] = '\0';
    }

    /**
     * @brief Insert a string at a specified position in the current string.
     * @param str The string to insert.
     * @param i The position to insert at.
     * @param size Maximum size to insert (default is SIZE_MAX).
     */
    void Insert(const char* str, std::size_t i, std::size_t size = SIZE_MAX) {
        if(i > _len) throw std::out_of_range("Index is out of bounds!\n << Strazzle::BaseString::Insert()");

        size = std::min(strlen(str), size);
        Strazzle::BaseString::Reserve(size + _len);

        std::memmove(_data + i + size, _data + i, size);
        std::memcpy(_data + i, str, size);

        _len = size + _len;

        _data[_len] = '\0';
    }

    /**
     * @brief Erase a portion of the string starting from a specified position.
     * @param i The starting position for erasing.
     * @param size Maximum size to erase (default is SIZE_MAX).
     */
    void Erase(std::size_t i, std::size_t size = SIZE_MAX) {
        if(i >= _len) throw std::out_of_range("Index is out of bounds!\n << Strazzle::BaseString::Erase()");

        size = std::min(_len - i, size);

        std::memmove(_data + i, _data + i + size, _len - i);

        Strazzle::BaseString::Reserve(_len - size);
        _len = _len - size;

        _data[_len] = '\0';
    }

    /**
     * @brief Resize the string to a specified size, filling with a given string.
     * @param size The new size of the string.
     * @param fill The string to fill with (default is a space).
     */
    void Resize(std::size_t size, const char* fill = " ") {
        Strazzle::BaseString::Reserve(size);

        if(size > _len) {
            std::size_t str_len = strlen(fill);

            while(_len < size) {
                std::memcpy(_data + _len, fill, _len + str_len <= size ? str_len : size - _len);
                _len += str_len;
            }
        } else {
            _len = size;
        }

        _data[_len] = '\0';
    }

    /**
     * @brief Resize the string to a specified size, filling with a character.
     * @param size The new size of the string.
     * @param fill The character to fill with (default is a space).
     */
    void Resize(std::size_t size, char fill = ' ') {
        Strazzle::BaseString::Reserve(size);

        if(size > _len) {
            std::memset(_data + _len, fill, size - _len);
        }

        _len = size;

        _data[_len] = '\0';
    }

    /**
     * @brief Get a pointer to the C-style string.
     * @return A pointer to the C-style string.
     */
    const char* Cstr() {
        return _data;
    }

    /**
     * @brief Get the length of the string.
     * @return The length of the string.
     */
    std::size_t Len() {
        return _len;
    }

    /**
     * @brief Allocates the specified ammount of memory
     * @param size The requested ammount of memory
     */
    void ResizeAllocation(std::size_t size) {
        if(Strazzle::BaseString::ChangeMode(size)) return;

        if(_mode == Strazzle::BaseString::Mode::LARGE_STRING) {
            Strazzle::BaseString::Realloc(Strazzle::_GetExponent(size));
        }
    }

    Strazzle::BaseString Substr(std::size_t i, std::size_t size = SIZE_MAX) {
        if(i >= _len) throw std::out_of_range("Index is out of bounds! << Strazzle::BaseString::Substr()\n");

        size = std::min(_len - i, size);

        return Strazzle::BaseString(_data + i, size);
    }

    Strazzle::BaseString::Reference RefSubstr(std::size_t i, std::size_t size = SIZE_MAX) {
        if(i >= _len) throw std::out_of_range("Index is out of bounds! << Strazzle::BaseString::RefSubstr()\n");

        size = std::min(_len - i, size);

        return Strazzle::BaseString::Reference(_data, i, size, _len);
    }

#ifndef STRAZZLE_DEBUG_ALL_PUBLIC
  private:
#endif
    /**
     * @brief Enlarges the allocation use ResizeAllocation() to also shrink the allocation
     * @param new_len The new length of the string.
     */
    void Reserve(std::size_t new_len) {
        new_len++;

        if(Strazzle::BaseString::ChangeMode(new_len)) return;

        if(_mode == Strazzle::BaseString::Mode::LARGE_STRING) {
            uint8_t cur_exp = Strazzle::_GetExponent(_len);
            uint8_t new_exp = Strazzle::_GetExponent(new_len);

            if(new_exp > cur_exp) {
                Strazzle::BaseString::Realloc(new_exp);
            }
        }
    }

    /**
     * @brief Reallocate memory to the given exponent.
     * @param exp The new exponent for memory allocation.
     */
    void Realloc(uint8_t exp) {
#ifdef NDEBUG
        allocated_size = 1 << exp;
#endif

        std::size_t byte_c = 1 << exp;

        char* p = static_cast<char*>(malloc(byte_c));

        std::memcpy(p, _data, std::min(byte_c, _len));

        free(_data);

        _data = p;
    }

    /**
     * @brief Changes the mode based on the given size and handles moving the current contents to the new buffer
     * @param size The size that will be allocated to in the calling function
     * @return Whether or not the mode was changed
     */
    bool ChangeMode(std::size_t size) {
        if(_len >= Strazzle::SSO_SIZE && size < Strazzle::SSO_SIZE) {
            Strazzle::BaseString::ToSmall();

#ifdef NDEBUG
            allocated_size = Strazzle::SSO_SIZE;
#endif
            return true;
        } else if(_len < Strazzle::SSO_SIZE && size > Strazzle::SSO_SIZE) {
            Strazzle::BaseString::ToLarge(Strazzle::_GetExponent(size));

#ifdef NDEBUG
            allocated_size = 1 << new_exp;
#endif

            return true;
        }

        return false;
    }

    inline void ToLarge(uint8_t exp) {
        char* p = static_cast<char*>(malloc(Strazzle::_ExpToNum(exp)));

        memcpy(p, _data, _len);

        _data = p;

        _mode = Strazzle::BaseString::Mode::LARGE_STRING;
    }

    inline void ToSmall() {
        _len = std::min(Strazzle::SSO_SIZE, _len);

        memcpy(_sso_buffer, _data, _len);

        free(_data);

        _mode = Strazzle::BaseString::Mode::SMALL_STRING;
    }
};

} // namespace Strazzle

/*

    uint8_t _r -> (0) => !r -> (> 0) => r

    _al = 8 -> 32
    => ToLarge();
    => _al = 32

*/