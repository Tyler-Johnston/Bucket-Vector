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
            using pointer = std::shared_ptr<T>;

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

            iterator begin();
            iterator end();

        private:
            class Bucket 
            {
                public:
                    Bucket(std::uint16_t capacity) :
                        m_bucketData(std::make_shared<T[]>(capacity)),
                        m_bucketCapacity(capacity),
                        m_bucketSize(0) {}
                    
                    const std::shared_ptr<T[]>& getData() const { return m_bucketData; }
                    std::uint16_t getSize() const { return m_bucketSize; }
                    std::uint16_t getCapacity() const { return m_bucketCapacity; }
                    void setSize(std::uint16_t newSize) { m_bucketSize = newSize; }
                    void setValueAtIndex(std::uint16_t index, const T& value);

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
            throw std::range_error("Index out of bounds in operator[]");
        }

        size_type count = 0;
        for (auto& bucket : buckets) 
        {
            size_type bucketSize = bucket->getSize();
            if (index < count + bucketSize) 
            {
                return bucket->getData().get()[index - count];
            }
            count += bucketSize;
        }
        throw std::range_error("Index out of bounds in the 2nd part of operator[]");
    }

    template <typename T>
    void vector<T>::add(T value)
    {
        auto& lastBucket = buckets.back();
        if (lastBucket->getSize() == m_capacity) 
        {
            auto firstHalfBucket = std::make_shared<Bucket>(m_capacity);
            auto secondHalfBucket = std::make_shared<Bucket>(m_capacity);

            // copy first half to firstHalfBucket
            std::copy(lastBucket->getData().get(), lastBucket->getData().get() + m_capacity / 2, firstHalfBucket->getData().get());
            // copy second half to secondHalfBucket
            std::copy(lastBucket->getData().get() + m_capacity / 2, lastBucket->getData().get() + m_capacity, secondHalfBucket->getData().get());
            // adjust the sizes
            firstHalfBucket->setSize(m_capacity / 2);
            secondHalfBucket->setSize((m_capacity / 2) + 1); // this is +1 because the new element will be added to the secondHalfBucket
            // remove the old lastBucket and add the two new buckets
            buckets.pop_back();
            buckets.push_back(firstHalfBucket);
            buckets.push_back(secondHalfBucket);
            // add the new value to the end of the secondHalfBucket
            secondHalfBucket->setValueAtIndex(m_capacity / 2, value);
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

        size_type count = 0;
        for (auto& bucket : buckets) 
        {
            size_type bucketSize = bucket->getSize();
            if (index <= count + bucketSize) 
            {
                if (bucketSize == m_capacity) 
                {
                    // create a temporary bucket may not be the most space-efficient solution,
                    // but it ensures the sub-buckets are split evenly and easily
                    auto tempBucket = std::make_shared<Bucket>(m_capacity + 1);
                    int tempIndex = 0;
                    for (int i = 0; i < m_capacity; i++) 
                    {
                        if (i == index - count) 
                        {
                            tempBucket->getData().get()[tempIndex] = value;
                            tempIndex++;
                        }
                        tempBucket->getData().get()[tempIndex] = bucket->getData().get()[i];
                        tempIndex++;
                    }

                    if (index - count == m_capacity) 
                    {
                        tempBucket->getData().get()[tempIndex] = value;
                    }
                    tempBucket->setSize(m_capacity + 1);

                    auto firstHalfBucket = std::make_shared<Bucket>(m_capacity);
                    auto secondHalfBucket = std::make_shared<Bucket>(m_capacity);

                    size_type mid = (m_capacity + 2) / 2;
                    std::copy(tempBucket->getData().get(), tempBucket->getData().get() + mid, firstHalfBucket->getData().get());
                    std::copy(tempBucket->getData().get() + mid, tempBucket->getData().get() + m_capacity + 1, secondHalfBucket->getData().get());

                    firstHalfBucket->setSize(mid);
                    secondHalfBucket->setSize(m_capacity + 1 - mid);

                    auto bucketIt = std::find(buckets.begin(), buckets.end(), bucket);
                    if (bucketIt != buckets.end()) 
                    {
                        *bucketIt = firstHalfBucket;
                        buckets.insert(std::next(bucketIt), secondHalfBucket);
                    }

                    m_size++;
                    return;
                }
                // Handle the case where the bucket is not full
                else 
                {
                    size_type innerIndex = index - count;
                    for (size_type i = bucketSize; i > innerIndex; i--) 
                    {
                        bucket->setValueAtIndex(i, bucket->getData()[i - 1]);
                    }
                    bucket->setValueAtIndex(innerIndex, value);
                    bucket->setSize(bucketSize + 1);
                    m_size++;
                    return;
                }
            }
            count += bucketSize;
        }
        throw std::range_error("Index out of bounds in the second part of the insert method");
    }

    template <typename T>
    void vector<T>::remove(size_type index) 
    {
        if (index >= m_size) 
        {
            throw std::range_error("Index out of range");
        }

        bool found = false;
        for (auto& bucket : buckets) 
        {
            size_type bucketSize = bucket->getSize();
            if (index <= bucketSize) 
            { // found the right bucket
                found = true;
                // shift elements left to fill the gap
                for (size_type i = index; i < bucketSize - 1; i++) 
                {
                    bucket->setValueAtIndex(i, bucket->getData()[i + 1]);
                }
                // decrease the size of the bucket
                bucket->setSize(bucketSize - 1);
                break;
            }
            index -= bucketSize;
        }

        if (!found) 
        {
            throw std::range_error("Element to remove not found"); // this shouldnt happen if the user input is correct
        }

        m_size--;
    }

    template <typename T>
    void vector<T>::clear()
    {
        buckets.clear();
        m_size = 0;
    }

    template <typename T>
    typename vector<T>::iterator vector<T>::begin() 
    {
        return iterator(0, *this);
    }

    template <typename T>
    typename vector<T>::iterator vector<T>::end() 
    {
        return iterator(m_size, *this);
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
    void vector<T>::Bucket::setValueAtIndex(std::uint16_t index, const T& value) 
    {
        if (index > m_bucketSize) {
            throw std::range_error("Index out of bounds in the SetValueAtIndex in the bucket");
        }
        m_bucketData[index] = value;
    }

}