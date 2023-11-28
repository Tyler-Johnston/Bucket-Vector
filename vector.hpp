#pragma once

#include <algorithm>
#include <cstddef> // for std::size_t
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <list>
#include <memory>
#include <stdexcept>
#include <iostream>

namespace usu
{
    template <typename T>
    concept Array = requires(T x)
    {
        x.operator[](0);
        {
            x.size()
        } -> std::convertible_to<std::size_t>;
    };

    template <typename T>
    concept BeginEnd = requires(T x)
    {
        x.begin();
        x.end();
    };

    template <typename T>
    concept Vector = Array<T> && BeginEnd<T>;

    template <typename T>
    class vector
    {
        public:
            using size_type = std::size_t;
            using reference = T&;
            using pointer = std::shared_ptr<T[]>;

            class iterator
            {
                public:
                    using iterator_category = std::bidirectional_iterator_tag;
                    using difference_type = std::ptrdiff_t;

                    iterator() :
                        iterator(nullptr)
                    {
                    }

                    iterator(const iterator& obj) :
                        m_pos(obj.m_pos),
                        m_data(obj.m_data)
                    {
                    }

                    iterator(pointer data) :
                        m_pos(0),
                        m_data(data)
                    {
                    }

                    iterator(size_type pos, vector<T>& data) :
                        m_pos(pos),
                        m_data(data)
                    {
                    }

                    reference operator*() const { return m_data[m_pos]; }
                    auto* operator->() const { return &m_data[m_pos]; }

                    iterator& operator++();
                    iterator operator++(int);
                    iterator& operator--();
                    iterator operator--(int);

                    bool operator==(const iterator& other) const { return m_pos == other.m_pos; }
                    bool operator!=(const iterator& other) const { return m_pos != other.m_pos; }

                private:
                    size_type m_pos;
                    vector<T>& m_data;
                };

            vector();
            vector(std::initializer_list<T> list);

            reference operator[](size_type index);
            void add(T value);
            void insert(size_type index, T value);
            void remove(size_type index);
            void clear();

            size_type size() const { return m_size; }
            size_type capacity() const { return m_capacity; }

            iterator begin() { return iterator(0, *this); }
            iterator end() { return iterator(m_size, *this); }

        private:
            class Bucket
            {
                public:
                    Bucket(size_type capacity)
                    {
                        if (capacity % 2 != 0)
                        {
                            throw std::invalid_argument("Bucket capacity must be an even number");
                        }
                        m_bucketData = std::make_shared<T[]>(capacity);
                        m_bucketCapacity = capacity;
                        m_bucketSize = 0;
                    }
                   
                    const std::shared_ptr<T[]>& getData() const { return m_bucketData; }
                    size_type getSize() const { return m_bucketSize; }
                    size_type getCapacity() const { return m_bucketCapacity; }
                    void setSize(size_type newSize) { m_bucketSize = newSize; }
                    void setValueAtIndex(size_type index, const T& value);

                private:
                    std::shared_ptr<T[]> m_bucketData;
                    size_type m_bucketSize;
                    size_type m_bucketCapacity;
            };
            static const size_type DEFAULT_BUCKET_CAPACITY = 10;
            std::list<std::shared_ptr<Bucket>> buckets;
            size_type m_size; // the number of elements in the vector (NOT the number of buckets)
            size_type m_capacity;
    };

    template <typename T>
    vector<T>::vector() :
        m_size(0),
        m_capacity(DEFAULT_BUCKET_CAPACITY)
    {
        auto initialBucket = std::make_shared<Bucket>(m_capacity);
        buckets.push_back(initialBucket);
    }

    template <typename T>
    vector<T>::vector(std::initializer_list<T> list) :
        m_size(0),
        m_capacity(DEFAULT_BUCKET_CAPACITY)
    {
        auto initialBucket = std::make_shared<Bucket>(m_capacity);
        buckets.push_back(initialBucket);
        for (const auto& value : list)
        {
            add(value);
        }
    }

    template <typename T>
    typename vector<T>::reference vector<T>::operator[](size_type index)
    {
        if (index >= m_size)
        {
            throw std::range_error("Index out of bounds");
        }

        auto bucketIt = buckets.begin();
        while (bucketIt != buckets.end() && index >= (*bucketIt)->getSize())
        {
            index -= (*bucketIt)->getSize();
            ++bucketIt;
        }

        if (bucketIt == buckets.end())
        {
            throw std::range_error("Index out of bounds");
        }

        return (*bucketIt)->getData().get()[index];
    }

    template <typename T>
    void vector<T>::add(T value)
    {
        auto& lastBucket = buckets.back();
        if (lastBucket->getSize() == m_capacity)
        {
            size_type mid = m_capacity / 2;
            // set the size of the original 'lastBucket' to mid, so the remaining elements will be overridden
            lastBucket->setSize(mid);
            // copy the second half of the bucket to a new 'secondHalfBucket'
            auto secondHalfBucket = std::make_shared<Bucket>(m_capacity);
            std::copy(lastBucket->getData().get() + mid, lastBucket->getData().get() + m_capacity, secondHalfBucket->getData().get());
            secondHalfBucket->setSize(mid + 1);
            // add new element to secondHalfBucket and append bucket to list of buckets
            secondHalfBucket->setValueAtIndex(mid, value);
            buckets.push_back(secondHalfBucket);
        }
        else
        {
            size_type currentSize = lastBucket->getSize();
            lastBucket->setValueAtIndex(currentSize, value);
            lastBucket->setSize(currentSize + 1);
        }
        m_size++;
    }

    template <typename T>
    void vector<T>::insert(size_type index, T value)
    {
        if (index > m_size)
        {
            throw std::range_error("Invalid insert index");
        }

        bool inserted = false;
        auto bucketIt = buckets.begin();
        size_type count = 0;
        while (bucketIt != buckets.end() && !inserted)
        {
            size_type bucketSize = (*bucketIt)->getSize();
            if (index <= count + bucketSize)
            {
                if (bucketSize == m_capacity)
                {
                    size_type mid = m_capacity / 2;
                    // adjust the size of the original bucket
                    (*bucketIt)->setSize(mid);

                    // move the second half of the elements to the secondHalfBucket
                    auto secondHalfBucket = std::make_shared<Bucket>(m_capacity);
                    std::copy((*bucketIt)->getData().get() + mid, (*bucketIt)->getData().get() + m_capacity, secondHalfBucket->getData().get());
                    secondHalfBucket->setSize(mid);

                    // determine if the new value should be inserted in the orignal (first) bucket or the second bucket
                    if (index - count < mid)
                    {
                        for (size_type i = mid; i > index - count; --i)
                        {
                            (*bucketIt)->getData().get()[i] = (*bucketIt)->getData().get()[i - 1];
                        }
                        (*bucketIt)->getData().get()[index - count] = value;
                        (*bucketIt)->setSize(mid + 1);
                    }
                    else
                    {
                        size_type newIndex = index - count - mid;
                        for (size_type i = (m_capacity - mid); i > newIndex; --i)
                        {
                            secondHalfBucket->getData().get()[i] = secondHalfBucket->getData().get()[i - 1];
                        }
                        secondHalfBucket->getData().get()[newIndex] = value;
                        secondHalfBucket->setSize(mid + 1);
                    }
                    // insert the new bucket after the bucket iterator
                    buckets.insert(std::next(bucketIt), secondHalfBucket);
                }
                else
                {
                    // handle the case where the bucket is not full
                    for (size_type i = bucketSize; i > index - count; --i)
                    {
                        (*bucketIt)->setValueAtIndex(i, (*bucketIt)->getData()[i - 1]);
                    }
                    (*bucketIt)->setValueAtIndex(index - count, value);
                    (*bucketIt)->setSize(bucketSize + 1);
                }
                m_size++;
                inserted = true;
            }
            count += bucketSize;
            ++bucketIt;
        }

        if (!inserted)
        {
            throw std::range_error("Index out of bounds");
        }
    }

    template <typename T>
    void vector<T>::remove(size_type index)
    {
        if (index >= m_size)
        {
            throw std::range_error("Index out of range");
        }

        // find the correct bucket
        auto bucketIt = buckets.begin();
        while (bucketIt != buckets.end() && index >= (*bucketIt)->getSize())
        {
            index -= (*bucketIt)->getSize();
            ++bucketIt;
        }

        if (bucketIt == buckets.end())
        {
            throw std::range_error("Element to remove not found");
        }
       
        // shift elements left to fill the gap
        for (size_type i = index; i < (*bucketIt)->getSize() - 1; ++i)
        {
            (*bucketIt)->setValueAtIndex(i, (*bucketIt)->getData()[i + 1]);
        }
        // decrease the size of the bucket
        (*bucketIt)->setSize((*bucketIt)->getSize() - 1);
        m_size--;
    }

    template <typename T>
    void vector<T>::clear()
    {
        buckets.clear();
        m_size = 0;
    }

    template <typename T>
    typename vector<T>::iterator& vector<T>::iterator::operator++()
    {
        ++m_pos;
        return *this;
    }

    template <typename T>
    typename vector<T>::iterator vector<T>::iterator::operator++(int)
    {
        iterator temp = *this;
        ++(*this);
        return temp;
    }

    template <typename T>
    typename vector<T>::iterator& vector<T>::iterator::operator--()
    {
        --m_pos;
        return *this;
    }

    template <typename T>
    typename vector<T>::iterator vector<T>::iterator::operator--(int)
    {
        iterator temp = *this;
        --(*this);
        return temp;
    }

    template <typename T>
    void vector<T>::Bucket::setValueAtIndex(size_type index, const T& value)
    {
        if (index > m_bucketSize)
        {
            throw std::range_error("Index out of bounds");
        }
        m_bucketData[index] = value;
    }
}