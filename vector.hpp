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
                m_data(obj.m_data)
            {
            }
            iterator(pointer data) :
                m_pos(0),
                m_data(data)
            {
            }
            iterator(size_type pos, pointer data) :
                m_pos(pos),
                m_data(data)
            {
            }

            reference operator*() { return m_data[m_pos]; }
            auto* operator->()
            {
                return &(m_data[m_pos]);
            }

            iterator operator++();
            iterator operator++(int);
            iterator operator--();
            iterator operator--(int);

            bool operator==(const iterator& rhs)
            {
                return m_pos == rhs.m_pos && m_data == rhs.m_data;
            }

            bool operator!=(const iterator& rhs)
            {
                return !operator==(rhs);
            }

          private:
            size_type m_pos;
            pointer m_data;
        };

        class Bucket
        {
            public:
                Bucket(std::uint16_t capacity) :
                    m_data(std::make_shared<T[]>(capacity)),
                    m_size(0)
                {}
                const std::shared_ptr<T[]>& getData() const { return m_data; }
                std::uint16_t getSize() const { return m_size; }
                void setSize(std::uint16_t newSize) { m_size = newSize }
                void setValueAtIndex(std::uint16_t index, const T& value) 
                {
                    if (index >= m_size) {
                        throw std::out_of_range("Index out of bounds");
                    }
                    m_data[index] = value;
                }

            private:
                std::shared_ptr<T[]> m_data;
                std::uint16_t m_size;
        };

        vector();
        vector(resize_type resize);
        vector(size_type size);
        vector(size_type size, resize_type resize);
        vector(std::initializer_list<T> list);
        vector(std::initializer_list<T> list, resize_type resize);

        reference operator[](size_type index);
        void add(T value);
        void insert(size_type index, T value);
        void remove(size_type index);
        void clear();

        size_type size() { return m_size; }
        size_type capacity() { return m_capacity; }

        iterator begin() { return iterator(m_data); }
        iterator end() { return iterator(m_size, m_data); }

      private:
        static const size_type DEFAULT_INITIAL_CAPACITY = 10;
        std::list<std::shared_ptr<Bucket>> buckets;
        size_type m_size;
        size_type m_capacity{ DEFAULT_INITIAL_CAPACITY };
    };

    template <typename T>
    vector<T>::vector() :
        vector(0, DEFAULT_RESIZE)
    {
    }

    template <typename T>
    vector<T>::vector(resize_type resize) :
        vector(0, resize)
    {
    }

    template <typename T>
    vector<T>::vector(size_type size) :
        vector(size, DEFAULT_RESIZE)
    {
    }

    template <typename T>
    vector<T>::vector(size_type size, resize_type resize) :
        m_size(size),
        m_funcResize(resize)
    {
        if (m_size > m_capacity)
        {
            m_capacity = m_funcResize(m_size);
        }

        m_data = std::make_shared<T[]>(m_capacity);
    }

    template <typename T>
    vector<T>::vector(std::initializer_list<T> list) :
        vector<T>(list, DEFAULT_RESIZE)
    {
    }

    template <typename T>
    vector<T>::vector(std::initializer_list<T> list, resize_type resize) :
        vector<T>(resize)
    {
        for (auto&& value : list)
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

        return m_data[index];
    }

    template <typename T>
    void vector<T>::add(T value)
    {

    }

    template <typename T>
    void vector<T>::insert(size_type index, T value)
    {

    }

    template <typename T>
    void vector<T>::remove(size_type index)
    {

    }

    template <typename T>
    void vector<T>::clear()
    {
        buckets.clear()
    }

    // Prefix ++i
    template <typename T>
    typename vector<T>::iterator vector<T>::iterator::operator++()
    {
        m_pos++;
        return *this;
    }

    // Postfix i++
    template <typename T>
    typename vector<T>::iterator vector<T>::iterator::operator++(int)
    {
        iterator i = *this;
        m_pos++;
        return i;
    }

    // Prefix --i
    template <typename T>
    typename vector<T>::iterator vector<T>::iterator::operator--()
    {
        m_pos--;
        return *this;
    }

    // Postfix i--
    template <typename T>
    typename vector<T>::iterator vector<T>::iterator::operator--(int)
    {
        iterator i = *this;
        m_pos--;
        return i;
    }

} // namespace usu
