// #include "vector.hpp"

// #include <cstring> // std::memcpy

// namespace usu
// {
//     vector<bool>::vector() :
//         vector<bool>(0, DEFAULT_RESIZE)
//     {
//     }

//     vector<bool>::vector(resize_type resize) :
//         vector<bool>(0, resize)
//     {
//     }

//     vector<bool>::vector(size_type size) :
//         vector<bool>(size, DEFAULT_RESIZE)
//     {
//     }

//     vector<bool>::vector(size_type size, resize_type resize) :
//         m_size(size),
//         m_funcResize(resize)
//     {
//         if (m_size > m_capacity)
//         {
//             m_capacity = m_funcResize(m_size);
//         }

//         m_data = std::make_shared<std::uint8_t[]>(dataSizeFromCapacity(m_capacity));
//         std::memset(m_data.get(), 0, dataSizeFromCapacity(m_capacity));
//     }

//     vector<bool>::vector(std::initializer_list<bool> list) :
//         vector<bool>(list, DEFAULT_RESIZE)
//     {
//     }

//     vector<bool>::vector(std::initializer_list<bool> list, resize_type resize) :
//         vector<bool>(resize)
//     {
//         for (auto&& value : list)
//         {
//             add(value);
//         }
//     }

//     // --------------------------------------------------------------
//     //
//     // Because we can't return a reference to a single bit, we have
//     // this big fat object that is returned, which acts like a
//     // reference to a single bit.
//     //
//     // --------------------------------------------------------------
//     typename vector<bool>::reference vector<bool>::operator[](size_type index)
//     {
//         return vector<bool>::reference(*this, index);
//     }

//     // --------------------------------------------------------------
//     //
//     // We still use the user provided resize function, but the capacity
//     // is now based on number of bits needed.  It is still given in
//     // terms of the number of std::uint8_t needed for storage, but the
//     // number of those is based on how many of them are needed to store
//     // the desired number of bits.
//     //
//     // --------------------------------------------------------------
//     void vector<bool>::add(bool value)
//     {
//         if (m_size == m_capacity)
//         {
//             auto prevCapacity = m_capacity;
//             m_capacity = m_funcResize(m_capacity);
//             auto temp = std::make_shared<std::uint8_t[]>(dataSizeFromCapacity(m_capacity));
//             std::memset(temp.get(), 0, dataSizeFromCapacity(m_capacity));
//             std::memcpy(temp.get(), m_data.get(), dataSizeFromCapacity(prevCapacity));
//             m_data = temp;
//         }

//         m_size++;

//         // We have already incremented the size, so the position we actually want to set is 1 less than the size.
//         set(m_size - 1, value);
//     }

//     // --------------------------------------------------------------
//     //
//     //
//     //
//     // --------------------------------------------------------------
//     void vector<bool>::insert(size_type index, bool value)
//     {
//         // We allow insert at the end of the vector, therefore > instead of >= test
//         if (index > m_size)
//         {
//             throw std::range_error("Index out of bounds");
//         }

//         // If we don't have enough room, let's use the 'add' method to create
//         // the room.
//         if (m_size == m_capacity)
//         {
//             add(false);
//         }
//         else
//         {
//             // We have to do this in the else, because the 'add' method increases the size
//             // and the logic here must match that.
//             m_size++;
//         }

//         // Slide everything one to the right, then we can set the inserted value
//         for (size_type i = m_size - 1; i >= index && i >= 1; i--)
//         {
//             set(i, at(i - 1));
//         }
//         set(index, value);
//     }

//     // --------------------------------------------------------------
//     //
//     //
//     //
//     // --------------------------------------------------------------
//     void vector<bool>::remove(size_type index)
//     {
//         if (index >= m_size)
//         {
//             throw std::range_error("Index out of bounds");
//         }

//         // Slide everything one to the left, then reduce the size
//         for (size_type i = index; i < m_size - 1; i++)
//         {
//             set(i, at(i + 1));
//         }
//         m_size--;
//     }

//     void vector<bool>::clear()
//     {
//         m_size = 0;
//     }

//     typename vector<bool>::size_type vector<bool>::dataSizeFromCapacity(size_type capacity)
//     {
//         // Reference for rounding up integer division
//         // https://stackoverflow.com/questions/17005364/dividing-two-integers-and-rounding-up-the-result-without-using-floating-point
//         //   Using the selected answer from Ben Voight, because it passes some tests he identifies, while
//         //   Jwodder's version doesn't pass those same tests.
//         auto denominator = sizeof(std::uint8_t) * 8;
//         // Because we know we always use positive values, (capacity < 0) test can be changed to 0 in order
//         // to remove a compiler warning.
//         auto size = capacity / denominator + ((0 ^ (denominator > 0)) && (capacity % denominator));

//         return size;
//     }

//     // --------------------------------------------------------------
//     //
//     // Purpose of this private method to set a value in the vector
//     // is to allow the custom 'reference' type to set values, while
//     // not giving this kind of access to the public interface.
//     //
//     // --------------------------------------------------------------
//     void vector<bool>::set(size_type index, bool value)
//     {
//         if (index >= m_size)
//         {
//             throw std::range_error("Index out of bounds");
//         }

//         // Figure out which bit to return based on the index
//         auto location = index / (sizeof(std::uint8_t) * 8);
//         switch (index % (sizeof(std::uint8_t) * 8))
//         {
//             case 0:
//                 m_data[location] = value ? (m_data[location] | 0b00000001) : (m_data[location] & 0b11111110);
//                 break;
//             case 1:
//                 m_data[location] = value ? (m_data[location] | 0b00000010) : (m_data[location] & 0b11111101);
//                 break;
//             case 2:
//                 m_data[location] = value ? (m_data[location] | 0b00000100) : (m_data[location] & 0b11111011);
//                 break;
//             case 3:
//                 m_data[location] = value ? (m_data[location] | 0b00001000) : (m_data[location] & 0b11110111);
//                 break;
//             case 4:
//                 m_data[location] = value ? (m_data[location] | 0b00010000) : (m_data[location] & 0b11101111);
//                 break;
//             case 5:
//                 m_data[location] = value ? (m_data[location] | 0b00100000) : (m_data[location] & 0b11011111);
//                 break;
//             case 6:
//                 m_data[location] = value ? (m_data[location] | 0b01000000) : (m_data[location] & 0b10111111);
//                 break;
//             case 7:
//                 m_data[location] = value ? (m_data[location] | 0b10000000) : (m_data[location] & 0b01111111);
//                 break;
//         }
//     }

//     // --------------------------------------------------------------
//     //
//     // Purpose of this private method to get a value in the vector
//     // is to allow the custom 'reference' type to get values, while
//     // not giving this kind of access to the public interface.
//     //
//     // --------------------------------------------------------------
//     bool vector<bool>::at(size_type index)
//     {
//         if (index >= m_size)
//         {
//             throw std::range_error("Index out of bounds");
//         }

//         // Figure out which bit to return based on the index
//         auto location = index / (sizeof(std::uint8_t) * 8);
//         switch (index % (sizeof(std::uint8_t) * 8))
//         {
//             case 0:
//                 return m_data[location] & 0b00000001;
//             case 1:
//                 return m_data[location] & 0b00000010;
//             case 2:
//                 return m_data[location] & 0b00000100;
//             case 3:
//                 return m_data[location] & 0b00001000;
//             case 4:
//                 return m_data[location] & 0b00010000;
//             case 5:
//                 return m_data[location] & 0b00100000;
//             case 6:
//                 return m_data[location] & 0b01000000;
//             case 7:
//                 return m_data[location] & 0b10000000;
//         }

//         return false;
//     }
// } // namespace usu
