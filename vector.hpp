#pragma once

#include <algorithm>
#include <concepts>
#include <cstddef> // for std::size_t
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <stdexcept>

#include <list>

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
        using value_type = T;
        using resize_type = std::function<size_type(size_type)>;

        class Bucket
        {
            public:
                Bucket(std::uint16_t capacity) :
                    m_data(std::make_shared<T[]>(capacity)),
                    m_size(0)
                {}
                const std::shared_ptr<T[]>& getData() const { return m_data; }
                std::uint16_t getSize() const { return m_size; }
                void setSize(std::uint16_t newSize) { m_size = newSize; }
                void setValueAtIndex(std::uint16_t index, const T& value) 
                {
                    if (index > m_size) 
                    {
                        throw std::out_of_range("Index out of bounds in the SetValueAtIndex");
                    }
                    m_data[index] = value;
                }

            private:
                std::shared_ptr<T[]> m_data;
                size_type m_size;
        };

        class iterator
        {
          public:
            using iterator_category = std::bidirectional_iterator_tag;
            using difference_type = std::size_t;

            iterator() :
                iterator(nullptr) // DefaultConstructable
            {
            }
            iterator(const iterator& obj) :
                m_pos(obj.m_pos),
                m_bucketIt(obj.m_bucketIt)
            {
            }
            iterator(pointer data) :
                m_pos(0),
                m_bucketIt(data)
            {
            }
            iterator(size_type pos, pointer data) :
                m_pos(pos),
                m_bucketIt(data)
            {
            }

            reference operator*() 
            { 
                return (*m_bucketIt)->getData().get()[m_pos];
            }

            auto* operator->()
            {
                return &(m_bucketIt[m_pos]);
            }

            iterator operator++();
            iterator operator++(int);
            iterator operator--();
            iterator operator--(int);

            bool operator==(const iterator& rhs)
            {
                return m_pos == rhs.m_pos && m_bucketIt == rhs.m_bucketIt;
            }

            bool operator!=(const iterator& rhs)
            {
                return !operator==(rhs);
            }

          private:
              std::list<std::shared_ptr<Bucket>>::iterator m_bucketIt; // iterator to the current bucket
              size_type m_pos; // index within the current bucket
        };

        vector();

        reference operator[](size_type index);
        void add(T value);
        void insert(size_type index, T value);
        void remove(size_type index);
        void clear();

        size_type size() { return m_size; }
        size_type capacity() { return m_capacity; }

        iterator begin() 
        {
            return buckets.empty() ? iterator(buckets.end(), 0) : iterator(buckets.begin(), 0);
        }

        iterator end() 
        {
            return buckets.empty() ? iterator(buckets.end(), 0) : iterator(std::prev(buckets.end()), buckets.back()->getSize());
        }


      private:
        size_type DEFAULT_BUCKET_CAPACITY = 10;
        std::list<std::shared_ptr<Bucket>> buckets;
        size_type m_size; // the number of elements in the vector (NOT the number of buckets)
        size_type m_capacity; // the capacity of each bucket
    };

    template <typename T>
    vector<T>::vector() :
        m_size(0),
        m_capacity(DEFAULT_BUCKET_CAPACITY)
    {
        auto initialBucket = std::make_shared<Bucket>(m_capacity);
        buckets.push_back(initialBucket);
    }

    // template <typename T>
    // vector<T>::vector(size_type capacity) :
    //     m_size(0),
    //     m_capacity(capacity)
    // {
    //     auto initialBucket = std::make_shared<Bucket>(m_capacity);
    //     buckets.push_back(initialBucket);
    // }

    template <typename T>
    typename vector<T>::reference vector<T>::operator[](size_type index)
    {
        if (index >= m_size)
        {
            throw std::range_error("Index out of bounds in the operator[] section");
        }

        size_type count = 0;
        for (auto& bucket : buckets) {
            size_type bucketSize = bucket->getSize();
            if (index < count + bucketSize) 
            {
                // the right bucket was found, return the element at that index
                return bucket->getData()[index - count];
            }
            count += bucketSize;
        }

        throw std::range_error("Index out of bounds in the 2nd part of the operator[] section");  // this should not happen
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
            // the second half will have one additional item in it
            secondHalfBucket->setSize((m_capacity / 2) + 1);
            
            // remove the old lastBucket
            buckets.pop_back();
            
            // add the two new buckets
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

        // loop through the list of buckets
        size_type count = 0;
        for (auto it = buckets.begin(); it != buckets.end(); ++it)
        {
            auto& targetBucket = *it;
            size_type bucketSize = targetBucket->getSize();

            // find the correct bucket in the list of buckets
            if (index < count + bucketSize)
            {
                size_type innerIndex = index - count;
                if (bucketSize < m_capacity)
                {
                    for (size_type i = bucketSize; i > innerIndex; i--)
                    {
                        targetBucket->setValueAtIndex(i, targetBucket->getData()[i - 1]);
                    }
                    targetBucket->setValueAtIndex(innerIndex, value);
                    targetBucket->setSize(bucketSize + 1);
                }
                else  // the bucket is full so do the splitting
                {
                    auto firstHalfBucket = std::make_shared<Bucket>(m_capacity);
                    auto secondHalfBucket = std::make_shared<Bucket>(m_capacity);

                    size_type mid = m_capacity / 2;
                    bool insertInFirstHalf = innerIndex < mid;
                    if (insertInFirstHalf)
                    {
                        std::copy(targetBucket->getData().get(), targetBucket->getData().get() + innerIndex, firstHalfBucket->getData().get());
                        firstHalfBucket->setValueAtIndex(innerIndex, value);
                        std::copy(targetBucket->getData().get() + innerIndex, targetBucket->getData().get() + mid, firstHalfBucket->getData().get() + innerIndex + 1);
                        std::copy(targetBucket->getData().get() + mid, targetBucket->getData().get() + m_capacity, secondHalfBucket->getData().get());

                        // set the sizes
                        firstHalfBucket->setSize(mid + 1);
                        secondHalfBucket->setSize(mid);
                    }
                    else
                    {
                        std::copy(targetBucket->getData().get(), targetBucket->getData().get() + mid, firstHalfBucket->getData().get());
                        std::copy(targetBucket->getData().get() + mid, targetBucket->getData().get() + innerIndex, secondHalfBucket->getData().get());
                        secondHalfBucket->setValueAtIndex(innerIndex - mid, value);
                        std::copy(targetBucket->getData().get() + innerIndex, targetBucket->getData().get() + m_capacity, secondHalfBucket->getData().get() + innerIndex - mid + 1);

                        // set the sizes    
                        firstHalfBucket->setSize(mid);
                        secondHalfBucket->setSize(mid + 1);
                    }

                    // modify the list of buckets
                    it = buckets.erase(it);
                    it = buckets.insert(it, secondHalfBucket);
                    it = buckets.insert(it, firstHalfBucket);
                }
                m_size++;
                return; // break out of the loop
            }
            count += bucketSize;
        }
    }

    //TODO: finish this
    template <typename T>
    void vector<T>::remove(size_type index)
    {

    }

    template <typename T>
    void vector<T>::clear()
    {
        buckets.clear();
    }

    // TODO: CHAT-GPT PROVIDED PRE/POST FIX OPERATORS, GO BACK AND REVIEW THIS

    // Prefix ++i
    template <typename T>
    typename vector<T>::iterator vector<T>::iterator::operator++() 
    {
        // Move to the next element within the bucket
        if (++m_pos >= (*m_bucketIt)->getSize()) {
            // Move to the next bucket if we've reached the end of the current one
            ++m_bucketIt;
            m_pos = 0; // Reset position to the start of the new bucket
        }
        return *this;
    }

    // Postfix i++
    template <typename T>
    typename vector<T>::iterator vector<T>::iterator::operator++(int) 
    {
        iterator tmp = *this;
        ++(*this); // Use the prefix increment to move to the next element
        return tmp;
    }

    // Prefix --i
    template <typename T>
    typename vector<T>::iterator vector<T>::iterator::operator--() 
    {
        if (m_pos == 0 && m_bucketIt != m_bucketIt.begin()) 
        {
            // If at the beginning of a bucket, move to the previous bucket
            --m_bucketIt;
            m_pos = (*m_bucketIt)->getSize() - 1; // Last element of the previous bucket
        } 
        else if (m_pos > 0) 
        {
            // If not at the beginning, just move back one element within the bucket
            --m_pos;
        }
        // Edge case: if m_pos is already 0 and m_bucketIt is at the beginning,
        // this operation would lead to an invalid state and should be handled accordingly.
        return *this;
    }

    // Postfix i--
    template <typename T>
    typename vector<T>::iterator vector<T>::iterator::operator--(int) 
    {
        iterator tmp = *this;
        --(*this); // Use the prefix decrement to move to the previous element
        return tmp;
    }


} // namespace usu
