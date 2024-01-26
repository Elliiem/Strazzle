#pragma once

#include <cinttypes>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdexcept>

namespace Strazzle {
/**
 * @brief Converts from exp to size
 * @param exp The Exponent
 * @return The size
 */
inline std::size_t _ExpToNum(uint8_t exp) {
    return exp != 0 ? 1UL << exp : 0;
}

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

    return ((clz_x != Strazzle::_clz(x - 1)) && (x != 0)) ? (63 - clz_x) : (64 - clz_x);
}

const std::size_t SSO_SIZE = 16;

/**
 * @brief String class with Small String Optimization (SSO)
 *        Intended for use with "small" strings, "large" strings will be handled in a different class
 */
class String {
  public:
    /**
     * @brief Reference to a String ie a pointer to the base that acts as a substr
     */
    struct Reference {
        friend String;

#ifndef STRAZZLE_DEBUG_ALL_PUBLIC
      private:
#endif
        Reference(Strazzle::String& base, std::size_t i, std::size_t len) : _base(base), _i(i), _len(len) {
        }

#ifndef STRAZZLE_DEBUG_ALL_PUBLIC
      private:
#endif
        // Start of substr in base
        std::size_t _i;
        // Len of substr
        std::size_t _len;

        // The base
        const Strazzle::String& _base;

#ifndef STRAZZLE_DEBUG_ALL_PUBLIC
      private:
#endif
        /**
         * @brief Checks if the reference is still within bounds of the base
         */
        void CheckBounds() const {
            if(_i + _len > _base._len)
                throw std::out_of_range("Reference is not within bounds of base! << Strazzle::String::Reference::CheckBounds()");
        }
    };

#ifndef STRAZZLE_DEBUG_ALL_PUBLIC
  public:
#endif
    String() : _data(_sso_buffer) {
        _data[0] = 0;
    }

    String(const char* str, std::size_t size = SIZE_MAX) : _data(_sso_buffer) {
        Strazzle::String::Append(str, size);
    }

    String(const Strazzle::String& str, std::size_t size = SIZE_MAX) : _data(_sso_buffer) {
        size = std::min(str._len, size);

        Append(str._data, size);
    }

    String(const Strazzle::String::Reference& ref, std::size_t size = SIZE_MAX) : _data(_sso_buffer) {
        Append(ref, size);
    }

#ifndef STRAZZLE_DEBUG_ALL_PUBLIC
  private:
#endif
    enum class Mode : uint8_t { NONE = 0, SMALL_STRING = 1, LARGE_STRING = 2 };

    // Small String Optimization buffer
    char _sso_buffer[Strazzle::SSO_SIZE];
    // Pointer to the string buffer
    char* _data = nullptr;

    // Length of the string
    std::size_t _len = 0;

    // Current mode of the string (should NEVER be NONE)
    Strazzle::String::Mode _mode = Strazzle::String::Mode::SMALL_STRING;

    // Exponent that is reserved to
    // allocated memory will ALWAYS be more or equal to this value
    uint8_t _reserved_exp = 0;

#ifdef NDEBUG
    // Size of allocated memory
    uint8_t _allocated_exp = 0;
#endif

#ifndef STRAZZLE_DEBUG_ALL_PUBLIC
  public:
#endif
    /**
     * @brief Append a string to the end of the current string.
     * @param str The string to append.
     * @param size Maximum size to append (default is SIZE_MAX).
     */
    void Append(const char* str, std::size_t size = SIZE_MAX) {
        size = std::min(strlen(str), size);

        Strazzle::String::ResizeAllocation(size + _len + 1);

        std::memcpy(_data + _len, str, size);

        _len = size + _len;

        _data[_len] = '\0';
    }

    /**
     * @brief String version of Append
     * @param str The string to append.
     * @param size Maximum size to append (default is SIZE_MAX).
     */
    void Append(const Strazzle::String& str, std::size_t size = SIZE_MAX) {
        size = std::min(str._len, size);

        Strazzle::String::Append(str._data, size);
    }

    /**
     * @brief Reference version of Append
     * @param str The reference to append.
     * @param size Maximum size to append (default is SIZE_MAX).
     */
    void Append(const Strazzle::String::Reference& ref, std::size_t size = SIZE_MAX) {
        ref.CheckBounds();

        size = std::min(ref._len, size);

        Strazzle::String::Append(ref._base._data + ref._i, size);
    }

    /**
     * @brief Insert a string at a specified position in the current string.
     * @param str The string to insert.
     * @param i The position to insert at.
     * @param size Maximum size to insert (default is SIZE_MAX).
     */
    void Insert(const char* str, std::size_t i, std::size_t size = SIZE_MAX) {
        if(i > _len) throw std::out_of_range("Index is out of bounds!\n << Strazzle::String::Insert()");

        size = std::min(strlen(str), size);

        Strazzle::String::ResizeAllocation(size + _len + 1);

        std::memmove(_data + i + size, _data + i, _len - i);

        std::memcpy(_data + i, str, size);

        _len = size + _len;

        _data[_len] = '\0';
    }

    /**
     * @brief String version of Insert
     * @param str The string to insert.
     * @param i The position to insert at.
     * @param size Maximum size to insert (default is SIZE_MAX).
     */
    void Insert(const Strazzle::String& str, std::size_t i, std::size_t size = SIZE_MAX) {
        size = std::min(str._len, size);

        Strazzle::String::Insert(str._data, i, size);
    }

    /**
     * @brief Reference version of Insert
     * @param ref The reference to insert.
     * @param i The position to insert at.
     * @param size Maximum size to insert (default is SIZE_MAX).
     */
    void Insert(const Strazzle::String::Reference& ref, std::size_t i, std::size_t size = SIZE_MAX) {
        ref.CheckBounds();

        size = std::min(ref._len, size);

        Strazzle::String::Insert(ref._base._data + ref._i, i, size);
    }

    /**
     * @brief Erase a portion of the string starting from a specified position.
     * @param i The starting position for erasing.
     * @param size Maximum size to erase (default is SIZE_MAX).
     */
    void Erase(std::size_t i, std::size_t size = SIZE_MAX) {
        if(i >= _len) throw std::out_of_range("Index is out of bounds!\n << Strazzle::String::Erase()");

        size = std::min(_len - i, size);

        std::memmove(_data + i, _data + i + size, _len - i);

        Strazzle::String::ResizeAllocation(_len - size);

        _len = _len - size;

        _data[_len] = '\0';
    }

    /**
     * @brief Resize the string to a specified size, filling with a character.
     * @param size The new size of the string.
     * @param fill The character to fill with (default is a space).
     */
    void Resize(std::size_t size, char fill = ' ') {
        Strazzle::String::ResizeAllocation(size + 1);

        if(size > _len) {
            std::memset(_data + _len, fill, size - _len);
        }

        _len = size;

        _data[_len] = '\0';
    }

    /**
     * @brief Resize the string to a specified size, filling with a given string.
     * @param size The new size of the string.
     * @param fill The string to fill with (default is a space).
     */
    void Resize(std::size_t size, const char* fill) {
        Strazzle::String::ResizeAllocation(size + 1);

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
     * @brief Returns a substr
     * @param i The starting index
     * @param size The lenght of the substr
     */
    Strazzle::String Substr(std::size_t i, std::size_t size = SIZE_MAX) {
        if(i >= _len) throw std::out_of_range("Index is out of bounds! << Strazzle::String::Substr()\n");

        size = std::min(_len - i, size);

        return Strazzle::String(_data + i, size);
    }

    /**
     * @brief Returns a reference substr
     * @param i The starting index
     * @param size The lenght of the substr
     */
    Strazzle::String::Reference RefSubstr(std::size_t i, std::size_t size = SIZE_MAX) {
        if(i >= _len) throw std::out_of_range("Index is out of bounds! << Strazzle::String::RefSubstr()\n");

        size = std::min(_len - i, size);

        return Strazzle::String::Reference(*this, i, size);
    }

    /**
     * @brief Reserves to the given size. Reserving means that there will never be less allocated than reserved
     * @param size The size to reserve to
     */
    void Reserve(std::size_t size) {
        _reserved_exp = Strazzle::_GetExponent(size);

        uint8_t cur_exp = Strazzle::_GetExponent(_len);

        if(cur_exp < _reserved_exp) {
            Strazzle::String::ResizeAllocation(size);
        }
    }

    bool operator==(Strazzle::String& other) {
        return std::strcmp(other._data, _data) == 0;
    }

#ifndef STRAZZLE_DEBUG_ALL_PUBLIC
  private:
#endif

    /**
     * @brief Resize the current allocation, handles changing mode
     * @param size The size to alloc to (will allo to the next exp)
     */
    void ResizeAllocation(std::size_t size) {
        std::size_t new_exp = Strazzle::_GetExponent(size);

        if(new_exp < _reserved_exp) {
            return;
        }

        Strazzle::String::Mode new_mode = GetNewMode(size);

        if(new_mode != Strazzle::String::Mode::NONE) {
            ChangeMode(new_mode, new_exp);
            return;
        }

        if(_mode == Strazzle::String::Mode::LARGE_STRING) {
            Strazzle::String::Realloc(new_exp);
        }
    }

    /**
     * @brief Reallocate memory to the given exponent.
     * @param exp The new exponent for memory allocation.
     */
    void Realloc(uint8_t exp) {
#ifdef NDEBUG
        _allocated_exp = exp;
#endif

        std::size_t byte_c = Strazzle::_ExpToNum(exp);

        char* p = static_cast<char*>(malloc(byte_c));

        std::memcpy(p, _data, std::min(byte_c, _len));

        free(_data);

        _data = p;
    }

    /**
     * @brief Returns the mode that needs to be changed to when allocating to the given size
     * @param size The size we want to allocate to in the calling function
     * @return Returns a Strazzle::String::Mode this indicates the mode we need to change to
     *         if we dont need to change the mode Strazzle::String::Mode::NONE is returned
     */
    inline Strazzle::String::Mode GetNewMode(std::size_t size) {
        if(_len >= Strazzle::SSO_SIZE && size < Strazzle::SSO_SIZE) {
            return Strazzle::String::Mode::SMALL_STRING;
        }

        if(_len < Strazzle::SSO_SIZE && size > Strazzle::SSO_SIZE) {
            return Strazzle::String::Mode::LARGE_STRING;
        }

        return Strazzle::String::Mode::NONE;
    }

    /**
     * @brief Changes the current mode to the given mode
     * @param mode The mode that will be changed to if this is Strazzle::String::Mode::NONE nothing will be done
     * @param size The size that will be allocated to when changing to a large string
     */
    inline void ChangeMode(Strazzle::String::Mode mode, uint8_t exp) {
        switch(mode) {
            case Strazzle::String::Mode::LARGE_STRING:
                Strazzle::String::ToLarge(exp);
                break;
            case Strazzle::String::Mode::SMALL_STRING:
                Strazzle::String::ToSmall();
                break;
            default:
                break;
        }
    }

    /**
     * @brief Changes the mode to LARGE_STRING and hadles moving to the new buffer
     * @param exp the exponent of the size the heap allocation will be
     */
    inline void ToLarge(uint8_t exp) {
#ifdef NDEBUG
        _allocated_exp = exp;
#endif

        char* p = static_cast<char*>(malloc(Strazzle::_ExpToNum(exp)));

        memcpy(p, _data, _len);

        _data = p;

        _mode = Strazzle::String::Mode::LARGE_STRING;
    }

    /**
     * @brief Changes the mode to SMALL_STRING and hadles moving to the new buffer
     */
    inline void ToSmall() {
#ifdef NDEBUG
        _allocated_exp = 0;
#endif

        _len = std::min(Strazzle::SSO_SIZE, _len);

        memcpy(_sso_buffer, _data, _len);

        free(_data);

        _mode = Strazzle::String::Mode::SMALL_STRING;
    }
};

} // namespace Strazzle