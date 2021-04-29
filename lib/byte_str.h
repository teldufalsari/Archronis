#ifndef TLD_BYTE_STR_H
#define TLD_BYTE_STR_H

#include <cstddef>
#include <cstring>

static const std::size_t DEFAULT_CAPACITY = 16U;

class byte_str
{
    std::byte* data_;
    std::size_t size_;
    std::size_t capacity_;

public:
    byte_str();
    byte_str(std::size_t count, std::byte elem);
    explicit
    byte_str(std::size_t initial_capacity);
    byte_str(const byte_str& that);
    byte_str(byte_str&& that) noexcept;
    byte_str& operator =(const byte_str& that);
    byte_str& operator =(byte_str&& that) noexcept;
    ~byte_str();

    void reserve(std::size_t new_capacity);
    void resize(std::size_t new_size);
    void append(const byte_str& s);
    void append(std::byte elem);
    std::size_t size();
    std::size_t capacity();
    bool empty();
    void clear();
    std::byte* data();
    const std::byte* data() const;
    std::byte& operator [](std::size_t index);
    const std::byte& operator [](std::size_t index) const;
    byte_str& operator +=(const byte_str& s);
    byte_str& operator +=(std::byte elem);
    friend byte_str operator +(const byte_str& lhs, const byte_str& rhs);
    friend byte_str operator +(const byte_str& lhs, std::byte rhs);
    friend inline bool operator ==(const byte_str& lhs, const byte_str& rhs);
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