#ifndef TLD_BYTE_STR_H
#define TLD_BYTE_STR_H
///@file lib/byte_str


#include <cstddef>
#include <cstring>

static const std::size_t DEFAULT_CAPACITY = 16U;

/**
 * @brief Byte string is a container similar to the character string class.
 * It provides operations with raw bytes similar to the ones with strings,
 * such as concatenation and comparison. 
 */
class byte_str
{
    std::byte* data_;
    std::size_t size_;
    std::size_t capacity_;

public:
    /// Constructs empty string with some preallocated memory
    byte_str();

    /**
     * @brief Constructs string with specified elements
     * @param count Number of elements
     * @param elem Element value
     */
    byte_str(std::size_t count, std::byte elem);

    /**
     * @brief Constructs string with preallocated memory
     * @param initial_capacity
     */
    explicit
    byte_str(std::size_t initial_capacity);

    /// Copy constructor
    byte_str(const byte_str& that);

    /// Move constructor
    byte_str(byte_str&& that) noexcept;

    /// Copy assignment operator
    byte_str& operator =(const byte_str& that);

    /// Move assignment operator
    byte_str& operator =(byte_str&& that) noexcept;

    /// Frees allocated memory
    ~byte_str();

    /**
     * @brief Allocate additional memory.
     * @param new_capacity
     * @details It will not shrink the string if new capacity is less that the capacity of the string
     */
    void reserve(std::size_t new_capacity);

    /**
     * @brief Resize the string. It will both truncate the string or reserve additional memory
     * @param new_size
     */
    void resize(std::size_t new_size);

    /**
     * @brief Append string @p s at the end of this string
     * @param s
     * @details If capacity of the string is not enough, the additional memory is allocated
     */
    void append(const byte_str& s);

    /**
     * @brief Append byte @p elem at the end of this string
     * @param elem
     * @details If capacity of the string is not enough, the additional memory is allocated
     */
    void append(std::byte elem);

    /// Returns the length of the string in bytes
    std::size_t size();

    /// Returns size of the allocated memory
    std::size_t capacity();

    /// True if size is non-zero
    bool empty();

    /// Set size of the string zero
    void clear();

    /// Get access to the underlying array
    std::byte* data();

    /// Get access to the underlying array (for const byte strings)
    const std::byte* data() const;

    /** 
     * Access element at the specified index
     * @param index
     */
    std::byte& operator [](std::size_t index);

    /** 
     * Access element at the specified index (for const byte strings)
     * @param index
     */
    const std::byte& operator [](std::size_t index) const;

    /**
     * @brief Append string @p s at the end of this string
     * @param s
     * @details If capacity of the string is not enough, the additional memory is allocated
     */
    byte_str& operator +=(const byte_str& s);

    /**
     * @brief Append byte @p elem at the end of this string
     * @param elem
     * @details If capacity of the string is not enough, the additional memory is allocated
     */
    byte_str& operator +=(std::byte elem);

    /// Concatenate two byte strings
    friend byte_str operator +(const byte_str& lhs, const byte_str& rhs);

    /// Concatenate a byte string and a single byte
    friend byte_str operator +(const byte_str& lhs, std::byte rhs);

    /// Compare two byte string
    friend inline bool operator ==(const byte_str& lhs, const byte_str& rhs);

    /// Compare two byte string lexicografically
    friend inline bool operator <(const byte_str& lhs, const byte_str& rhs);
};

bool operator ==(const byte_str& lhs, const byte_str& rhs)
{
    if (lhs.size_ != rhs.size_) {
        return false;
    } else {
        return (memcmp(lhs.data_, rhs.data_, lhs.size_) == 0 ? true : false);
    }
}

bool operator <(const byte_str& lhs, const byte_str& rhs)
{
    if (lhs.size_ != rhs.size_)
        return (lhs.size_ < rhs.size_);
    else
        return (memcmp(lhs.data_, rhs.data_, lhs.size_) < 0 ? true : false);
}

#endif // TLD_BYTE_STR_H