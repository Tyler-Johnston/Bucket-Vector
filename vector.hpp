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

    template <typename T, std::size_t BucketCapacity = 10>
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
            vector(size_type size);
            vector(std::initializer_list<T> list);

            reference operator[](size_type index);
            void add(T value);
            void insert(size_type index, T value);
            void remove(size_type index);
            void clear();
            void map(std::function<void(T&)> func);

            size_type size() const { return m_size; }
            size_type capacity() const { return m_capacity; }

            iterator begin() { return iterator(0, *this); }
            iterator end() { return iterator(m_size, *this); }

        private:
            class Bucket
            {
                public:
                    Bucket(size_type capacity = BucketCapacity)
                    {
                        m_bucketData = std::make_shared<T[]>(capacity);
                        m_bucketSize = 0;
                    }
                   
                    const std::shared_ptr<T[]>& getData() const { return m_bucketData; }
                    size_type getSize() const { return m_bucketSize; }
                    void setSize(size_type newSize) { m_bucketSize = newSize; }
                    void setValueAtIndex(size_type index, const T& value);

                private:
                    std::shared_ptr<T[]> m_bucketData;
                    size_type m_bucketSize;
            };
            std::list<std::shared_ptr<Bucket>> buckets;
            size_type m_size; // the number of elements in the vector (NOT the number of buckets)
            size_type m_capacity = BucketCapacity; // the total capacity of the vector, including all bucket space
    };

    template <typename T, std::size_t BucketCapacity>
    vector<T, BucketCapacity>::vector() :
        m_size(0)
    {
        auto initialBucket = std::make_shared<Bucket>();
        buckets.push_back(initialBucket);
    }

    template <typename T, std::size_t BucketCapacity>
    vector<T, BucketCapacity>::vector(size_type size) :
        m_size(size),
        m_capacity(0)
    {
        size_type numberOfBuckets = (size + BucketCapacity - 1) / BucketCapacity;

        for (size_type i = 0; i < numberOfBuckets; ++i) 
        {
            auto bucket = std::make_shared<Bucket>();
            bucket->setSize(BucketCapacity);
            buckets.push_back(bucket);
            m_capacity += BucketCapacity;
        }
    }

    template <typename T, std::size_t BucketCapacity>
    vector<T, BucketCapacity>::vector(std::initializer_list<T> list) :
        m_size(0)
    {
        auto initialBucket = std::make_shared<Bucket>();
        buckets.push_back(initialBucket);
        for (const auto& value : list)
        {
            add(value);
        }
    }

    template <typename T, std::size_t BucketCapacity>
    typename vector<T, BucketCapacity>::reference vector<T, BucketCapacity>::operator[](size_type index)
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

    template <typename T, std::size_t BucketCapacity>
    void vector<T, BucketCapacity>::add(T value)
    {
        auto& lastBucket = buckets.back();
        if (lastBucket->getSize() == BucketCapacity)
        {
            size_type mid = BucketCapacity / 2;
            // set the size of the original 'lastBucket' to mid, so the remaining elements will be overridden
            lastBucket->setSize(mid);
            // copy the second half of the bucket to a new 'secondHalfBucket'
            auto secondHalfBucket = std::make_shared<Bucket>(BucketCapacity);
            std::copy(lastBucket->getData().get() + mid, lastBucket->getData().get() + BucketCapacity, secondHalfBucket->getData().get());
            secondHalfBucket->setSize(mid + 1);
            // add new element to secondHalfBucket and append bucket to list of buckets
            secondHalfBucket->setValueAtIndex(mid, value);
            buckets.push_back(secondHalfBucket);
            m_capacity += BucketCapacity;
        }
        else
        {
            size_type currentSize = lastBucket->getSize();
            lastBucket->setValueAtIndex(currentSize, value);
            lastBucket->setSize(currentSize + 1);
        }
        m_size++;
    }

    template <typename T, std::size_t BucketCapacity>
    void vector<T, BucketCapacity>::insert(size_type index, T value)
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
                if (bucketSize == BucketCapacity)
                {
                    size_type mid = BucketCapacity / 2;
                    // adjust the size of the original bucket
                    (*bucketIt)->setSize(mid);

                    // move the second half of the elements to the secondHalfBucket
                    auto secondHalfBucket = std::make_shared<Bucket>(BucketCapacity);
                    std::copy((*bucketIt)->getData().get() + mid, (*bucketIt)->getData().get() + BucketCapacity, secondHalfBucket->getData().get());
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
                        for (size_type i = (BucketCapacity - mid); i > newIndex; --i)
                        {
                            secondHalfBucket->getData().get()[i] = secondHalfBucket->getData().get()[i - 1];
                        }
                        secondHalfBucket->getData().get()[newIndex] = value;
                        secondHalfBucket->setSize(mid + 1);
                    }
                    // insert the new bucket after the bucket iterator
                    buckets.insert(std::next(bucketIt), secondHalfBucket);
                    m_capacity += BucketCapacity;
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

    template <typename T, std::size_t BucketCapacity>
    void vector<T, BucketCapacity>::remove(size_type index)
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

    template <typename T, std::size_t BucketCapacity>
    void vector<T, BucketCapacity>::clear()
    {
        buckets.clear();
        m_size = 0;
    }

    template <typename T, std::size_t BucketCapacity>
    void usu::vector<T, BucketCapacity>::map(std::function<void(T&)> func) 
    {
        for (auto& bucket : buckets) 
        {
            for (size_type i = 0; i < bucket->getSize(); ++i) 
            {
                func(bucket->getData().get()[i]);
            }
        }
    }



    template <typename T, std::size_t BucketCapacity>
    typename vector<T, BucketCapacity>::iterator& vector<T, BucketCapacity>::iterator::operator++()
    {
        ++m_pos;
        return *this;
    }

    template <typename T, std::size_t BucketCapacity>
    typename vector<T, BucketCapacity>::iterator vector<T, BucketCapacity>::iterator::operator++(int)
    {
        iterator temp = *this;
        ++(*this);
        return temp;
    }

    template <typename T, std::size_t BucketCapacity>
    typename vector<T, BucketCapacity>::iterator& vector<T, BucketCapacity>::iterator::operator--()
    {
        --m_pos;
        return *this;
    }

    template <typename T, std::size_t BucketCapacity>
    typename vector<T, BucketCapacity>::iterator vector<T, BucketCapacity>::iterator::operator--(int)
    {
        iterator temp = *this;
        --(*this);
        return temp;
    }

    template <typename T, std::size_t BucketCapacity>
    void vector<T, BucketCapacity>::Bucket::setValueAtIndex(size_type index, const T& value)
    {
        if (index > m_bucketSize)
        {
            throw std::range_error("Index out of bounds");
        }
        m_bucketData[index] = value;
    }
}