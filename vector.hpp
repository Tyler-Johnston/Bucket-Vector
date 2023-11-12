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

namespace usu {

    template <typename T>
    class vector {
      public:
        using size_type = std::size_t;
        using reference = T&;
        using pointer = std::shared_ptr<T[]>;
        using value_type = T;
        using resize_type = std::function<size_type(size_type)>;

        class Bucket {
          public:
            Bucket(std::uint16_t capacity) :
                m_data(std::make_shared<T[]>(capacity)),
                m_capacity(capacity),
                m_size(0) {}
            
            const std::shared_ptr<T[]>& getData() const { return m_data; }
            std::uint16_t getSize() const { return m_size; }
            std::uint16_t getCapacity() const { return m_capacity; }
            void setSize(std::uint16_t newSize) { m_size = newSize; }
            void setValueAtIndex(std::uint16_t index, const T& value) {
                if (index > m_size) {
                    throw std::out_of_range("Index out of bounds in the SetValueAtIndex in the bucket");
                }
                m_data[index] = value;
            }

          private:
            std::shared_ptr<T[]> m_data;
            size_type m_size;
            size_type m_capacity;
        };

        class iterator {
          public:
            using iterator_category = std::bidirectional_iterator_tag;
            using difference_type = std::size_t;
            using BucketListIterator = typename std::list<std::shared_ptr<Bucket>>::iterator;

            iterator(std::list<std::shared_ptr<Bucket>>& bucketsRef, BucketListIterator bucketIt, size_type pos) 
                : m_bucketsRef(bucketsRef), m_bucketIt(bucketIt), m_pos(pos) {}

            reference operator*() { 
                return (*m_bucketIt)->getData().get()[m_pos];
            }

            auto* operator->() {
                return &((*m_bucketIt)->getData().get()[m_pos]);
            }

            iterator& operator++() {
                if (++m_pos >= (*m_bucketIt)->getSize() && std::next(m_bucketIt) != m_bucketsRef.end()) {
                    ++m_bucketIt;
                    m_pos = 0;
                }
                return *this;
            }

            iterator operator++(int) {
                iterator temp = *this;
                ++(*this);
                return temp;
            }

            iterator& operator--() {
                if (m_pos == 0 && m_bucketIt != m_bucketsRef.begin()) {
                    --m_bucketIt;
                    m_pos = (*m_bucketIt)->getSize();
                } else if (m_pos > 0) {
                    --m_pos;
                }
                return *this;
            }

            iterator operator--(int) {
                iterator temp = *this;
                --(*this);
                return temp;
            }

            bool operator==(const iterator& rhs) const {
                return m_pos == rhs.m_pos && m_bucketIt == rhs.m_bucketIt;
            }

            bool operator!=(const iterator& rhs) const {
                return !(*this == rhs);
            }

          private:
            BucketListIterator m_bucketIt;
            size_type m_pos;
            std::list<std::shared_ptr<Bucket>>& m_bucketsRef;
        };

        vector() : m_size(0), m_capacity(DEFAULT_BUCKET_CAPACITY) {
            auto initialBucket = std::make_shared<Bucket>(m_capacity);
            buckets.push_back(initialBucket);
        }

        reference operator[](size_type index) {
            if (index >= m_size) {
                throw std::range_error("Index out of bounds in operator[]");
            }

            size_type count = 0;
            for (auto& bucket : buckets) {
                size_type bucketSize = bucket->getSize();
                if (index < count + bucketSize) {
                    return bucket->getData().get()[index - count];
                }
                count += bucketSize;
            }

            throw std::range_error("Index out of bounds in the 2nd part of operator[]");
        }

        void add(T value) {
            auto& lastBucket = buckets.back();
            if (lastBucket->getSize() == m_capacity) {
                auto firstHalfBucket = std::make_shared<Bucket>(m_capacity);
                auto secondHalfBucket = std::make_shared<Bucket>(m_capacity);

                // Copy first half to firstHalfBucket
                std::copy(lastBucket->getData().get(), lastBucket->getData().get() + m_capacity / 2, firstHalfBucket->getData().get());
                // Copy second half to secondHalfBucket
                std::copy(lastBucket->getData().get() + m_capacity / 2, lastBucket->getData().get() + m_capacity, secondHalfBucket->getData().get());
                // Adjust the sizes
                firstHalfBucket->setSize(m_capacity / 2);
                secondHalfBucket->setSize((m_capacity / 2) + 1);

                // Remove the old lastBucket and add the two new buckets
                buckets.pop_back();
                buckets.push_back(firstHalfBucket);
                buckets.push_back(secondHalfBucket);

                // Add the new value to the end of the secondHalfBucket
                secondHalfBucket->setValueAtIndex(m_capacity / 2, value);
            } else {
                size_type currentSize = lastBucket->getSize();
                lastBucket->setValueAtIndex(currentSize, value);
                lastBucket->setSize(currentSize + 1);
            }
            m_size++;
        }

        void insert(size_type index, T value) {
            if (index > m_size) {
                throw std::range_error("Invalid index in insert");
            }

            // Iterate through buckets to find the correct position
            size_type count = 0;
            for (auto& bucket : buckets) {
                size_type bucketSize = bucket->getSize();
                if (index < count + bucketSize) {
                    // Check if the bucket is full and needs splitting
                    if (bucketSize == m_capacity) {
                        auto firstHalfBucket = std::make_shared<Bucket>(m_capacity);
                        auto secondHalfBucket = std::make_shared<Bucket>(m_capacity);

                        size_type mid = m_capacity / 2;
                        size_type innerIndex = index - count;
                        bool insertInFirstHalf = innerIndex < mid;

                        if (insertInFirstHalf) {
                            std::copy(bucket->getData().get(), bucket->getData().get() + innerIndex, firstHalfBucket->getData().get());
                            firstHalfBucket->setValueAtIndex(innerIndex, value);
                            std::copy(bucket->getData().get() + innerIndex, bucket->getData().get() + mid, firstHalfBucket->getData().get() + innerIndex + 1);
                            std::copy(bucket->getData().get() + mid, bucket->getData().get() + m_capacity, secondHalfBucket->getData().get());

                            firstHalfBucket->setSize(mid + 1);
                            secondHalfBucket->setSize(mid);
                        } else {
                            std::copy(bucket->getData().get(), bucket->getData().get() + mid, firstHalfBucket->getData().get());
                            std::copy(bucket->getData().get() + mid, bucket->getData().get() + innerIndex, secondHalfBucket->getData().get());
                            secondHalfBucket->setValueAtIndex(innerIndex - mid, value);
                            std::copy(bucket->getData().get() + innerIndex, bucket->getData().get() + m_capacity, secondHalfBucket->getData().get() + innerIndex - mid + 1);

                            firstHalfBucket->setSize(mid);
                            secondHalfBucket->setSize(mid + 1);
                        }

                        // Replace the original bucket with the two new buckets
                        auto currentBucketIt = std::find(buckets.begin(), buckets.end(), bucket);
                        if (currentBucketIt != buckets.end()) {
                            buckets.insert(currentBucketIt, secondHalfBucket);
                            buckets.insert(currentBucketIt, firstHalfBucket);
                            buckets.erase(currentBucketIt);
                        }
                    } else {
                        // Insert in a non-full bucket
                        size_type innerIndex = index - count;
                        for (size_type i = bucketSize; i > innerIndex; --i) {
                            bucket->setValueAtIndex(i, bucket->getData().get()[i - 1]);
                        }
                        bucket->setValueAtIndex(innerIndex, value);
                        bucket->setSize(bucketSize + 1);
                    }
                    m_size++;
                    return;
                }
                count += bucketSize;
            }
        }

        void remove(size_type index) {
            // TODO: Implement the remove method
        }

        void clear() {
            buckets.clear();
            m_size = 0;
        }

        size_type size() const { return m_size; }
        size_type capacity() const { return m_capacity; }

        iterator begin() {
            return iterator(buckets, buckets.begin(), 0);
        }

        iterator end() {
            return buckets.empty() ? iterator(buckets, buckets.end(), 0) : iterator(buckets, std::prev(buckets.end()), std::prev(buckets.end())->get()->getSize());
        }

      private:
        static const size_type DEFAULT_BUCKET_CAPACITY = 10;
        std::list<std::shared_ptr<Bucket>> buckets;
        size_type m_size; // the number of elements in the vector (NOT the number of buckets)
        size_type m_capacity; // the capacity of each bucket
    };

}