#ifndef TLD_VECTOR_H
#define TLD_VECTOR_H

#include <cstring>
#include <iostream>

namespace tld {
/**
 * @brief A random-access container with automatically allocated memory
 * @tparam T data type stored in vector
 */
template <typename T>
class vector 
{
const size_t DEFAULT_CAPACITY = 16UL;

/**
 * @brief This structure is thrown as an exception when trying to access not allocated memory using at() method
 * @param capacity Capacity vector had when access was attempted
 * @param pos Data position that was attempted to access
 */
struct out_of_range {
    size_t capacity;
    size_t pos;
    out_of_range(size_t _capacity, size_t _pos) :capacity(_capacity), pos(_pos) {}
};

private:
    T* m_data_;
    size_t size_;
    size_t capacity_;

public:
    /// @brief Constructs vector with some pre-allocated space and default values
    vector();

    /**
     * @brief Creates vector with specified preallocated space and default values
     * @param capacity initial vector capacity
     */
    explicit vector(size_t capacity);

    /**
     * @brief Copy constructor
     * @param that source object
     */
    vector(const vector& that);

    /**
     * @brief Move constructor
     * @param that source object
     */
    vector(vector&& that) noexcept;

    /**
     * @brief Frees all allocated memory
     */
    ~vector();

    /**
     * @brief Copy assignment operator
     * @param that source object
     */
    vector& operator =(const vector& that);

    /**
     * @brief Move assignment operator
     * @param that source object
     */
    vector& operator =(vector&& that) noexcept;

    /**
     * @brief Fast random access operator without boundary check
     * @param pos position of the element
     */
    T& operator[](size_t pos) noexcept;

    /**
     * @brief Fast random access operator without boundary check
     * @param pos position of the element
     */
    const T& operator[](size_t pos) const noexcept;

    /**
     * @brief Random access method with boundary check
     * @param pos position of the element
     */
    T& at(size_t pos);

    /**
     * @brief Random access method with boundary check
     * @param pos position of the element
     */
    const T& at(size_t pos) const;

    /// @brief Get access to the underlying data array
    T* data();

    /// @brief Get access to the underlying data array
    const T* data() const;

    /// @brief Destruct all stored elements and free all allocated memory 
    void clear();

    /**
     * @brief Place new element at the end of the vector
     * @param elem new value
     */
    void push_back(const T& elem);
    void pop_back();
    bool empty() const;
    size_t capacity() const;
    size_t size() const;
    void reserve(size_t new_capacity);
    void shrink();
};

template <typename T>
vector<T>::vector() :
    m_data_(new T[DEFAULT_CAPACITY]),
    size_(0),
    capacity_(DEFAULT_CAPACITY)
{}

template <typename T>
vector<T>::vector(size_t capacity) :
    m_data_(new T[capacity]),
    size_(capacity),
    capacity_(capacity)
{}

template <typename T>
vector<T>::vector(const vector<T>& that) :
    m_data_(new T[that.capacity_]),
    size_(that.size_),
    capacity_(that.capacity_)
{
    for (size_t i = 0; i < size_; i++)
        m_data_[i] = that.m_data_[i];
}

template <typename T>
vector<T>::vector(vector<T>&& that) noexcept:
    m_data_(that.m_data_),
    size_(that.size_),
    capacity_(that.capacity_)
{
    that.m_data_ = nullptr;
    that.size_ = 0;
    that.capacity_ = 0;
}

template <typename T>
vector<T>::~vector()
{
    delete[] m_data_;
}

template <typename T>
vector<T>& vector<T>::operator =(const vector<T>& that)
{
    if (this == &that)
        return *this;
    delete[] this->m_data_;
    this->m_data_ = new T[that.capacity_];
    this->capacity_ = that.capacity_;
    this->size_ = that.size_;
    for (size_t i = 0; i < size_; i++)
        this->m_data_[i] = that.m_data_[i];
    return *this;
}

template <typename T>
vector<T>& vector<T>::operator =(vector<T>&& that) noexcept
{
    delete[] this->m_data_;
    this->m_data_ = that.m_data_;
    this->capacity_ = that.capacity_;
    this->size_ = that.size_;
    that.m_data_ = nullptr;
    return *this;
}

template <typename T>
bool vector<T>::empty() const
{
    return (size_ == 0);
}

template <typename T>
size_t vector<T>::capacity() const
{
    return capacity_;
}

template <typename T>
size_t vector<T>::size() const
{
    return size_;
}

template <typename T>
void vector<T>::reserve(size_t new_capacity)
{
    if (this->capacity_ >= new_capacity)
        return;
    T* temp_buf = new T[new_capacity];
    for (size_t i = 0; i < size_; i++)
        temp_buf[i] = m_data_[i];
    delete[] m_data_;
    m_data_ = temp_buf;
    capacity_ = new_capacity;
}

template <typename T>
void vector<T>::push_back(const T& elem)
{
    if (capacity_ == 0) {
        reserve(DEFAULT_CAPACITY);
    } else if (size_ == capacity_) {
        reserve(capacity_ * 2);
    }
    m_data_[size_++] = elem;
}

template <typename T>
T& vector<T>::at(size_t pos)
{
    if (pos >= size_)
        throw out_of_range(capacity_, pos);
    return m_data_[pos];
}

template <typename T>
const T& vector<T>::at(size_t pos) const
{
    if (pos >= size_)
        throw out_of_range(capacity_, pos);
    return m_data_[pos];
}

template <typename T>
T& vector<T>::operator [](size_t pos) noexcept
{
    return m_data_[pos];
}

template <typename T>
const T& vector<T>::operator [](size_t pos) const noexcept
{
    return m_data_[pos];
}

template <typename T>
T* vector<T>::data()
{
    return m_data_;
}

template <typename T>
const T* vector<T>::data() const
{
    return m_data_;
}

template <typename T>
void vector<T>::clear()
{
    delete[] m_data_;
    m_data_ = nullptr;
    size_ = 0;
    capacity_ = 0;
}

template <typename T>
void vector<T>::pop_back()
{
    if (size_ > 0) {
        std::cout << "I'm calling the police!" << std::endl;
        m_data_[size_ - 1].~T();
        size_--;
    }
}

template <typename T>
void vector<T>::shrink()
{
    if (capacity_ == size_)
        return;
    T* temp_buf = new T[size_];
    for (size_t i = 0; i < size_; i++)
        temp_buf[i] = m_data_[i];
    delete[] m_data_;
    m_data_ = temp_buf;
    capacity_ = size_;
}

} // namespace tld
#endif // TLD_VECTOR_H
