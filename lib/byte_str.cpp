#include "byte_str.h"

byte_str::byte_str() :
    data_(new std::byte[DEFAULT_CAPACITY]),
    size_(0),
    capacity_(DEFAULT_CAPACITY)
{}

byte_str::byte_str(std::size_t count, std::byte elem) :
    data_(new std::byte[count]),
    size_(count),
    capacity_(count)
{
    for (std::size_t i = 0; i < size_; i++)
        data_[i] = elem;
}

byte_str::byte_str(std::size_t initial_capacity) :
    data_(new std::byte[initial_capacity]),
    size_(0),
    capacity_(initial_capacity)
{}

byte_str::byte_str(const byte_str& that) :
    data_(new std::byte[that.capacity_]),
    size_(that.size_),
    capacity_(that.capacity_)
{
    memcpy(this->data_, that.data_, that.size_);
}

byte_str::byte_str(byte_str&& that) noexcept :
    data_(that.data_),
    size_(that.size_),
    capacity_(that.capacity_)
{
    that.data_ = nullptr;
    that.size_ = 0;
    that.capacity_ = 0;
}

byte_str& byte_str::operator =(const byte_str& that)
{
    if (this->data_ != that.data_) {
        delete[] this->data_;
        this->data_ = new std::byte[that.capacity_];
        this->size_ = that.size_;
        this->capacity_ = that.capacity_;
        memcpy(this->data_, that.data_, that.size_);
    }
    return *this;
}

byte_str& byte_str::operator =(byte_str&& that) noexcept
{
    delete[] this->data_;
    this->data_ = that.data_;
    this->size_ = that.size_;
    this->capacity_ = that.capacity_;
    that.data_ = nullptr;
    that.size_ = 0;
    that.capacity_ = 0;
    return *this;
}

byte_str::~byte_str()
{
    delete[] data_;
}

void byte_str::reserve(std::size_t new_capacity)
{
    if (new_capacity <= this->capacity_)
        return;
    auto new_data = new std::byte[new_capacity];
    memcpy(new_data, this->data_, this->size_);
    this->capacity_ = new_capacity;
    delete[] this->data_;
    this->data_ = new_data;
}

void byte_str::resize(std::size_t new_size)
{
    if (new_size > this->capacity_)
        this->resize(new_size);
    this->size_ = new_size;
}

void byte_str::append(const byte_str& s)
{
    std::size_t total_size = this->size_ + s.size_;
    if (total_size > this->capacity_)
        this->reserve(total_size);
    memcpy(this->data_ + this->size_, s.data_, s.size_);
    this->size_ = total_size;
}

std::size_t byte_str::size()
{
    return size_;
}
std::size_t byte_str::capacity()
{
    return capacity_;
}

bool byte_str::empty()
{
    return (size_ == 0 ? true : false);
}

std::byte* byte_str::data()
{
    return data_;
}
const std::byte* byte_str::data() const
{
    return data_;
}

std::byte& byte_str::operator [](std::size_t index)
{
    return data_[index];
}

const std::byte& byte_str::operator [](std::size_t index) const
{
    return data_[index];
}

void byte_str::append(std::byte elem)
{
    if (size_ + 1 > capacity_)
        reserve(capacity_ * 2);
    data_[size_++] = elem;
}

byte_str& byte_str::operator +=(const byte_str& s)
{
    std::size_t total_size = this->size_ + s.size_;
    if (total_size > this->capacity_)
        this->reserve(total_size);
    memcpy(this->data_ + this->size_, s.data_, s.size_);
    this->size_ = total_size;
    return *this;
}

byte_str& byte_str::operator +=(std::byte elem)
{
    if (size_ + 1 > capacity_)
        reserve(capacity_ * 2);
    data_[size_++] = elem;
    return *this;
}

byte_str operator +(const byte_str& lhs, const byte_str& rhs)
{
    byte_str tmp(lhs.size_ + rhs.size_);
    tmp.append(lhs);
    tmp.append(rhs);
    return tmp;
}

byte_str operator +(const byte_str& lhs, std::byte rhs)
{
    byte_str tmp(lhs.size_ + 1);
    tmp.append(lhs);
    tmp.append(rhs);
    return tmp;
}