#include "vector.hpp"

#include <gtest/gtest.h>
#include <string>
#include <utility> // std::pair
#include <vector>
#include <iostream>

// Set this to false to remove the debugging cout statements
constexpr bool DEBUG_PRINT = true;

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

TEST(Buckets, DefaultConstruction)
{
    usu::vector<int> vec;
    EXPECT_EQ(vec.size(), 0);  // check that the default size is 0
    EXPECT_EQ(vec.capacity(), 10);  // check the defualt capacity is 10
}

TEST(Buckets, Adding)
{
    usu::vector<int> vec;
    vec.add(1);
    EXPECT_EQ(vec.size(), 1);
    EXPECT_EQ(vec[0], 1);
    vec.add(2);
    EXPECT_EQ(vec.size(), 2);
    EXPECT_EQ(vec[1], 2);
}

TEST(Buckets, Inserting)
{
    usu::vector<int> vec;
    vec.add(1);
    vec.add(3);
    vec.insert(1, 2);  // inserting in between
    EXPECT_EQ(vec.size(), 3);
    EXPECT_EQ(vec[0], 1);
    EXPECT_EQ(vec[1], 2);
    EXPECT_EQ(vec[2], 3);
}

TEST(Buckets, Splitting)
{
    usu::vector<int> vec;
    for (int i = 0; i < 10; ++i)  // fill the first bucket
    {
        vec.add(i);
    }
    EXPECT_EQ(vec.size(), 10);
    
    // adding one extra element should cause a split
    vec.add(10);
    // the size of the vector should be 11, and the 10th element should be 10
    EXPECT_EQ(vec.size(), 11);
    for (int i=0; i<vec.size(); i++)
    {
        std::cout << "i: " << i << " => " << vec[i] << std::endl;
    }
    EXPECT_EQ(vec[10], 10);
}



// TEST(Constructor, SizeCapacity)
// {
//     // 10 comes from a private constant in the usu::vector class: DEFAULT_INITIAL_CAPACITY
//     usu::vector<int> v1;
//     EXPECT_EQ(v1.size(), 0);
//     EXPECT_EQ(v1.capacity(), 10);

//     usu::vector<int> v2(2);
//     EXPECT_EQ(v2.size(), 2);
//     EXPECT_EQ(v2.capacity(), 10);

//     // Default capacity increase is 2 * size
//     usu::vector<int> v3(100);
//     EXPECT_EQ(v3.size(), 100);
//     EXPECT_EQ(v3.capacity(), 200);
// }

// TEST(Constructor, InitializerList)
// {
//     using namespace std::string_literals;

//     usu::vector<int> v1{ 1, 2, 3, 5, 7, 11, 13, 17 };
//     EXPECT_EQ(v1.size(), 8);
//     EXPECT_EQ(v1.capacity(), 10);

//     // Make one larger than the default capacity
//     usu::vector<int> v2{ 1, 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41 };
//     EXPECT_EQ(v2.size(), 14);
//     EXPECT_EQ(v2.capacity(), 20);

//     usu::vector<std::string> v3{ "one"s, "two"s, "three"s, "five"s, "seven"s, "eleven"s, "thirteen"s, "seventeen"s };
//     EXPECT_EQ(v3.size(), 8);
//     EXPECT_EQ(v3.capacity(), 10);
// }

// TEST(Resize, DefaultResize)
// {
//     std::vector<int> primes{ 7, 11, 13, 17, 19, 23, 29, 31, 37, 41 };
//     usu::vector<int> v1;

//     v1.add(1);
//     EXPECT_EQ(v1.size(), 1);
//     EXPECT_EQ(v1.capacity(), 10);

//     v1.add(2);
//     EXPECT_EQ(v1.size(), 2);
//     EXPECT_EQ(v1.capacity(), 10);

//     v1.add(3);
//     EXPECT_EQ(v1.size(), 3);
//     EXPECT_EQ(v1.capacity(), 10);

//     v1.add(5);
//     EXPECT_EQ(v1.size(), 4);
//     EXPECT_EQ(v1.capacity(), 10);

//     for (auto i = 0; i < 6; i++)
//     {
//         v1.add(primes[i]);
//     }
//     EXPECT_EQ(v1.size(), 10);
//     EXPECT_EQ(v1.capacity(), 10);

//     v1.add(primes[6]);
//     EXPECT_EQ(v1.size(), 11);
//     EXPECT_EQ(v1.capacity(), 20);
// }

// TEST(Resize, UserDefinedResize)
// {
//     usu::vector<int> v1(
//         [](usu::vector<int>::size_type currentCapacity)
//         {
//             return static_cast<usu::vector<int>::size_type>(currentCapacity * 1.5);
//         });

//     for (auto pos = 1; pos <= 11; pos++)
//     {
//         v1.add(pos);
//     }
//     EXPECT_EQ(v1.size(), 11);
//     EXPECT_EQ(v1.capacity(), 15);

//     for (auto pos = 1; pos <= 5; pos++)
//     {
//         v1.add(pos);
//     }
//     EXPECT_EQ(v1.size(), 16);
//     EXPECT_EQ(v1.capacity(), 22);
// }

// TEST(Operators, Array)
// {
//     using namespace std::string_literals;

//     // Verify initializer_list constructor and reading from the vector
//     std::vector<int> primes{ 1, 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31 };
//     usu::vector<int> v1{ 1, 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31 };

//     for (std::size_t pos = 0; pos < v1.size(); pos++)
//     {
//         EXPECT_EQ(v1[pos], primes[pos]);
//     }

//     // Verify out of bounds causes exception
//     usu::vector<int> v2;
//     try
//     {
//         v2[0] = 1;
//         EXPECT_EQ(true, false);
//     }
//     catch (std::exception&)
//     {
//         EXPECT_EQ(true, true);
//     }

//     // Make sure the initializer list array does the same
//     try
//     {
//         v1[v1.size()] = 1;
//         EXPECT_EQ(true, false);
//     }
//     catch (std::exception&)
//     {
//         EXPECT_EQ(true, true);
//     }

//     // Verify we can read and write to the vector positions
//     usu::vector<int> v3{ 0, 0, 0, 0 };
//     v3[0] = 1;
//     v3[1] = 2;
//     v3[2] = 3;
//     v3[3] = 5;

//     EXPECT_EQ(v3[0], 1);
//     EXPECT_EQ(v3[1], 2);
//     EXPECT_EQ(v3[2], 3);
//     EXPECT_EQ(v3[3], 5);

//     usu::vector<std::string> v4{ "one"s, "two"s, "three"s, "five"s, "seven"s, "eleven"s, "thirteen"s, "seventeen"s };
//     std::vector<std::string> stringPrimes{ "one"s, "two"s, "three"s, "five"s, "seven"s, "eleven"s, "thirteen"s, "seventeen"s };

//     for (std::size_t pos = 0; pos < v4.size(); pos++)
//     {
//         EXPECT_EQ(v4[pos], stringPrimes[pos]);
//     }

//     usu::vector<std::string> v5{ "zero"s, "zero"s, "zero"s, "zero"s };
//     v5[0] = "one"s;
//     v5[1] = "two"s;
//     v5[2] = "three"s;
//     v5[3] = "five"s;

//     EXPECT_EQ(v5[0], "one"s);
//     EXPECT_EQ(v5[1], "two"s);
//     EXPECT_EQ(v5[2], "three"s);
//     EXPECT_EQ(v5[3], "five"s);
// }

// TEST(Iterators, ForCounted)
// {
//     std::vector<int> primes{ 1, 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41 };
//     usu::vector<int> v1{ 1, 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41 };

//     // Post-increment
//     std::size_t pos = 0;
//     for (auto itr = v1.begin(); itr != v1.end(); itr++, pos++)
//     {
//         EXPECT_EQ(*itr, primes[pos]);
//     }

//     // Pre-increment
//     pos = 0;
//     for (auto itr = v1.begin(); itr != v1.end(); ++itr, pos++)
//     {
//         EXPECT_EQ(*itr, primes[pos]);
//     }

//     // Post-increment
//     pos = primes.size() - 1;
//     for (auto itr = (--v1.end()); itr != v1.begin(); itr--, pos--)
//     {
//         EXPECT_EQ(*itr, primes[pos]);
//     }

//     // Pre-increment
//     pos = primes.size() - 1;
//     for (auto itr = (--v1.end()); itr != v1.begin(); --itr, pos--)
//     {
//         EXPECT_EQ(*itr, primes[pos]);
//     }
// }

// TEST(Iterators, ForEach)
// {
//     std::vector<int> primes{ 1, 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41 };
//     usu::vector<int> v1{ 1, 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41 };

//     for (std::size_t pos = 0; auto&& v : v1)
//     {
//         EXPECT_EQ(v, primes[pos]);
//         pos++;
//     }
// }

// TEST(Iterators, AccessOperators)
// {
//     {
//         usu::vector<std::pair<int, int>> v1{ { 1, 1 }, { 2, 2 }, { 3, 3 }, { 4, 5 }, { 5, 7 }, { 6, 11 } };

//         auto itr = v1.begin();
//         EXPECT_EQ((*itr).first, 1);
//         EXPECT_EQ((*itr).second, 1);

//         EXPECT_EQ(itr->first, 1);
//         EXPECT_EQ(itr->second, 1);

//         itr++;
//         itr++;
//         itr++;

//         EXPECT_EQ((*itr).first, 4);
//         EXPECT_EQ((*itr).second, 5);

//         EXPECT_EQ(itr->first, 4);
//         EXPECT_EQ(itr->second, 5);
//     }

//     {
//         using namespace std::string_literals;

//         usu::vector<std::pair<int, std::string>> v2{
//             { 1, "one"s },
//             { 2, "two"s },
//             { 3, "three"s },
//             { 4, "five"s },
//             { 5, "seven"s },
//             { 6, "eleven"s }
//         };

//         auto itr = v2.begin();
//         EXPECT_EQ((*itr).first, 1);
//         EXPECT_EQ((*itr).second, "one"s);

//         EXPECT_EQ(itr->first, 1);
//         EXPECT_EQ(itr->second, "one"s);

//         itr++;
//         itr++;
//         itr++;

//         EXPECT_EQ((*itr).first, 4);
//         EXPECT_EQ((*itr).second, "five"s);

//         EXPECT_EQ(itr->first, 4);
//         EXPECT_EQ(itr->second, "five"s);
//     }
// }

// TEST(Modify, Add)
// {
//     std::vector<int> primes{ 1, 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41 };
//     usu::vector<int> v1;
//     usu::vector<int> v2{ 1, 2, 3, 5 };

//     v1.add(1);
//     v1.add(2);
//     v1.add(3);
//     v1.add(5);
//     v1.add(7);
//     v1.add(11);
//     v1.add(13);
//     v1.add(17);
//     v1.add(19);
//     v1.add(23);
//     v1.add(29);
//     v1.add(31);
//     v1.add(37);
//     v1.add(41);

//     // Verifying .add works correctly combined with an initializer list
//     v2.add(7);
//     v2.add(11);
//     v2.add(13);
//     v2.add(17);
//     v2.add(19);
//     v2.add(23);
//     v2.add(29);
//     v2.add(31);
//     v2.add(37);
//     v2.add(41);

//     EXPECT_EQ(v1.size(), primes.size());
//     EXPECT_EQ(v2.size(), primes.size());

//     for (std::size_t pos = 0; pos < primes.size(); pos++)
//     {
//         EXPECT_EQ(v1[pos], primes[pos]);
//         EXPECT_EQ(v2[pos], primes[pos]);
//     }
// }

// TEST(Modify, Insert)
// {
//     using namespace std::string_literals;

//     std::vector<std::string> original{ "a"s, "b"s, "c"s, "d"s, "e"s, "f"s, "g"s };
//     usu::vector<std::string> v1{ "a"s, "b"s, "c"s, "d"s, "e"s, "f"s, "g"s };

//     v1.insert(0, "X"s);
//     EXPECT_EQ(v1[0], "X"s);

//     for (std::size_t pos = 0; pos < original.size(); pos++)
//     {
//         EXPECT_EQ(v1[pos + 1], original[pos]);
//     }

//     usu::vector<std::string> v2{ "a"s, "b"s, "c"s, "d"s, "e"s, "f"s, "g"s };
//     v2.insert(1, "X"s);
//     EXPECT_EQ(v2[0], "a"s);
//     EXPECT_EQ(v2[1], "X"s);

//     for (std::size_t pos = 2; pos < original.size(); pos++)
//     {
//         EXPECT_EQ(v2[pos + 1], original[pos]);
//     }

//     usu::vector<std::string> v3{ "a"s, "b"s, "c"s, "d"s, "e"s, "f"s, "g"s };
//     v3.insert(7, "X"s);
//     EXPECT_EQ(v3[7], "X"s);

//     for (std::size_t pos = 0; pos < original.size(); pos++)
//     {
//         EXPECT_EQ(v3[pos], original[pos]);
//     }

//     std::vector<std::string> original10{ "a"s, "b"s, "c"s, "d"s, "e"s, "f"s, "g"s, "h"s, "i"s, "j"s };
//     usu::vector<std::string> v4{ "a"s, "b"s, "c"s, "d"s, "e"s, "f"s, "g"s, "h"s, "i"s, "j"s };

//     v4.insert(0, "X"s);

//     EXPECT_EQ(v4.size(), 11);
//     EXPECT_EQ(v4.capacity(), 20);
//     EXPECT_EQ(v4[0], "X"s);

//     for (std::size_t pos = 0; pos < original10.size(); pos++)
//     {
//         EXPECT_EQ(v4[pos + 1], original10[pos]);
//     }

//     // Make sure insert throws a range error when out of bounds
//     try
//     {
//         usu::vector<int> v5{ 1, 2, 3, 5, 7, 11 };

//         v5.insert(v5.size() + 1, 13);
//         EXPECT_EQ(true, false);
//     }
//     catch (const std::range_error&)
//     {
//         EXPECT_EQ(true, true);
//     }
// }

// TEST(Modify, Remove)
// {
//     std::vector<int> primes{ 1, 2, 3, 5, 7, 11 };
//     usu::vector<int> v1{ 1, 2, 3, 5, 7, 11 };

//     v1.remove(0);
//     EXPECT_EQ(v1.size(), 5);

//     for (std::size_t pos = 0; pos < v1.size(); pos++)
//     {
//         EXPECT_EQ(v1[pos], primes[pos + 1]);
//     }

//     usu::vector<int> v2{ 1, 2, 3, 5, 7, 11 };
//     v2.remove(v2.size() - 1);
//     EXPECT_EQ(v2.size(), 5);

//     for (std::size_t pos = 0; pos < v2.size(); pos++)
//     {
//         EXPECT_EQ(v2[pos], primes[pos]);
//     }

//     usu::vector<int> v3{ 1, 2, 3, 5, 7, 11 };
//     v3.remove(2);
//     primes.erase(primes.begin() + 2);
//     EXPECT_EQ(v3.size(), 5);

//     for (std::size_t pos = 0; pos < v3.size(); pos++)
//     {
//         EXPECT_EQ(v3[pos], primes[pos]);
//     }

//     // Make sure remove throws a range error when out of bounds
//     try
//     {
//         usu::vector<int> v4{ 1, 2, 3, 5, 7, 11 };

//         v4.remove(v4.size() + 1);
//         EXPECT_EQ(true, false);
//     }
//     catch (const std::range_error&)
//     {
//         EXPECT_EQ(true, true);
//     }
// }

// TEST(BoolSpecializationConstructor, SizeCapacity)
// {
//     // 10 comes from a private constant in the usu::vector class: DEFAULT_INITIAL_CAPACITY
//     usu::vector<bool> v1;
//     EXPECT_EQ(v1.size(), 0);
//     EXPECT_EQ(v1.capacity(), 10);

//     usu::vector<bool> v2(2);
//     EXPECT_EQ(v2.size(), 2);
//     EXPECT_EQ(v2.capacity(), 10);

//     // Default capacity increase is 2 * size
//     usu::vector<bool> v3(100);
//     EXPECT_EQ(v3.size(), 100);
//     EXPECT_EQ(v3.capacity(), 200);
// }

// TEST(BoolSpecializationConstructor, InitializerList)
// {
//     usu::vector<bool> v1{ true, false, true, false, true, false, true, false };
//     EXPECT_EQ(v1.size(), 8);
//     EXPECT_EQ(v1.capacity(), 10);

//     // Make one larger than the default capacity
//     usu::vector<bool> v2{ true, false, true, false, true, false, true, false, true, false, true };
//     EXPECT_EQ(v2.size(), 11);
//     EXPECT_EQ(v2.capacity(), 20);
// }

// TEST(BoolSpecializationResize, DefaultResize)
// {
//     std::vector<bool> v1{ true, false, true, false, true, false, true, false, true, false };
//     usu::vector<int> v2;

//     v2.add(true);
//     EXPECT_EQ(v2.size(), 1);
//     EXPECT_EQ(v2.capacity(), 10);

//     v2.add(2);
//     EXPECT_EQ(v2.size(), 2);
//     EXPECT_EQ(v2.capacity(), 10);

//     v2.add(3);
//     EXPECT_EQ(v2.size(), 3);
//     EXPECT_EQ(v2.capacity(), 10);

//     v2.add(5);
//     EXPECT_EQ(v2.size(), 4);
//     EXPECT_EQ(v2.capacity(), 10);

//     for (auto i = 0; i < 6; i++)
//     {
//         v2.add(v1[i]);
//     }
//     EXPECT_EQ(v2.size(), 10);
//     EXPECT_EQ(v2.capacity(), 10);

//     v2.add(v1[6]);
//     EXPECT_EQ(v2.size(), 11);
//     EXPECT_EQ(v2.capacity(), 20);
// }

// TEST(BoolSpecializationResize, UserDefinedResize)
// {
//     usu::vector<bool> v1(
//         [](usu::vector<bool>::size_type currentCapacity)
//         {
//             return static_cast<usu::vector<bool>::size_type>(currentCapacity * 1.5);
//         });

//     for (auto pos = 1; pos <= 11; pos++)
//     {
//         v1.add(true);
//     }
//     EXPECT_EQ(v1.size(), 11);
//     EXPECT_EQ(v1.capacity(), 15);

//     for (auto pos = 1; pos <= 5; pos++)
//     {
//         v1.add(true);
//     }
//     EXPECT_EQ(v1.size(), 16);
//     EXPECT_EQ(v1.capacity(), 22);
// }

// TEST(BoolSpecializationOperators, Array)
// {
//     // Verify initializer_list constructor and reading from the vector
//     usu::vector<bool> v1{ true, false, true, false, true, false, true, false, true, false, true, false };
//     std::vector<bool> v2{ true, false, true, false, true, false, true, false, true, false, true, false };

//     for (std::size_t pos = 0; pos < v1.size(); pos++)
//     {
//         // This is written kind of funky because the compiler was having trouble resolving: EXPECT_EQ(v1[pos], v2[pos]);
//         EXPECT_EQ(v1[pos] == v2[pos], true);
//     }

//     // Verify out of bounds causes exception
//     usu::vector<int> v3;
//     try
//     {
//         v3[0] = true;
//         EXPECT_EQ(true, false);
//     }
//     catch (std::exception&)
//     {
//         EXPECT_EQ(true, true);
//     }

//     // Make sure the initializer list array does the same
//     try
//     {
//         v1[v1.size()] = 1;
//         EXPECT_EQ(true, false);
//     }
//     catch (std::exception&)
//     {
//         EXPECT_EQ(true, true);
//     }

//     // Verify we can read and write to the vector positions
//     usu::vector<int> v4{ false, false, false, false };
//     v4[0] = true;
//     v4[1] = true;
//     v4[2] = true;
//     v4[3] = true;

//     EXPECT_EQ(v4[0], true);
//     EXPECT_EQ(v4[1], true);
//     EXPECT_EQ(v4[2], true);
//     EXPECT_EQ(v4[3], true);
// }

// TEST(BoolSpecializationIterators, ForCounted)
// {
//     usu::vector<bool> v1{ true, false, true, false, true, false, true, false, true, false, true, false, true, false };
//     std::vector<bool> v2{ true, false, true, false, true, false, true, false, true, false, true, false, true, false };

//     // Post-increment
//     std::size_t pos = 0;
//     for (auto itr = v1.begin(); itr != v1.end(); itr++, pos++)
//     {
//         // Same reason for this test as in previous unit test
//         EXPECT_EQ(*itr == v2[pos], true);
//     }

//     // Pre-increment
//     pos = 0;
//     for (auto itr = v1.begin(); itr != v1.end(); ++itr, pos++)
//     {
//         EXPECT_EQ(*itr == v2[pos], true);
//     }

//     // Post-increment
//     pos = v2.size() - 1;
//     for (auto itr = (--v1.end()); itr != v1.begin(); itr--, pos--)
//     {
//         EXPECT_EQ(*itr == v2[pos], true);
//     }

//     // Pre-increment
//     pos = v2.size() - 1;
//     for (auto itr = (--v1.end()); itr != v1.begin(); --itr, pos--)
//     {
//         EXPECT_EQ(*itr == v2[pos], true);
//     }
// }

// TEST(BoolSpecializationIterators, ForEach)
// {
//     usu::vector<bool> v1{ true, false, true, false, true, false, true, false, true, false, true, false, true, false };
//     std::vector<bool> v2{ true, false, true, false, true, false, true, false, true, false, true, false, true, false };

//     for (std::size_t pos = 0; auto&& v : v1)
//     {
//         EXPECT_EQ(v == v2[pos], true);
//         pos++;
//     }
// }

// TEST(BoolSpecializationIterators, AccessOperators)
// {
//     usu::vector<std::pair<int, bool>> v1{ { 1, true }, { 2, false }, { 3, true }, { 4, false }, { 5, true }, { 6, false } };

//     auto itr = v1.begin();
//     EXPECT_EQ((*itr).first, 1);
//     EXPECT_EQ((*itr).second, true);

//     EXPECT_EQ(itr->first, 1);
//     EXPECT_EQ(itr->second, true);

//     itr++;
//     itr++;
//     itr++;

//     EXPECT_EQ((*itr).first, 4);
//     EXPECT_EQ((*itr).second, false);

//     EXPECT_EQ(itr->first, 4);
//     EXPECT_EQ(itr->second, false);
// }

// TEST(BoolSpecializationModify, Add)
// {
//     usu::vector<int> v1;
//     usu::vector<int> v2{ true, false, true, false };
//     std::vector<bool> v3{ true, false, true, false, true, false, true, false, true, false, true, false, true, false };

//     v1.add(true);
//     v1.add(false);
//     v1.add(true);
//     v1.add(false);
//     v1.add(true);
//     v1.add(false);
//     v1.add(true);
//     v1.add(false);
//     v1.add(true);
//     v1.add(false);
//     v1.add(true);
//     v1.add(false);
//     v1.add(true);
//     v1.add(false);

//     // Verifying .add works correctly combined with an initializer list
//     v2.add(true);
//     v2.add(false);
//     v2.add(true);
//     v2.add(false);
//     v2.add(true);
//     v2.add(false);
//     v2.add(true);
//     v2.add(false);
//     v2.add(true);
//     v2.add(false);

//     EXPECT_EQ(v1.size(), v3.size());
//     EXPECT_EQ(v2.size(), v3.size());

//     for (std::size_t pos = 0; pos < v3.size(); pos++)
//     {
//         EXPECT_EQ(v1[pos], v3[pos]);
//         EXPECT_EQ(v2[pos], v3[pos]);
//     }
// }

// TEST(BoolSpecializationModify, Insert)
// {
//     using namespace std::string_literals;

//     std::vector<bool> original{ true, false, true, false, true, false, true };
//     usu::vector<bool> v1{ true, false, true, false, true, false, true };

//     v1.insert(0, false);
//     EXPECT_EQ(v1[0] == false, true);

//     for (std::size_t pos = 0; pos < original.size(); pos++)
//     {
//         EXPECT_EQ(v1[pos + 1] == original[pos], true);
//     }

//     usu::vector<bool> v2{ true, false, true, false, true, false, true };
//     v2.insert(1, false);
//     EXPECT_EQ(v2[0] == true, true);
//     EXPECT_EQ(v2[1] == false, true);

//     for (std::size_t pos = 2; pos < original.size(); pos++)
//     {
//         EXPECT_EQ(v2[pos + 1] == original[pos], true);
//     }

//     usu::vector<bool> v3{ true, false, true, false, true, false, true };
//     v3.insert(7, false);
//     EXPECT_EQ(v3[7] == false, true);

//     for (std::size_t pos = 0; pos < original.size(); pos++)
//     {
//         EXPECT_EQ(v3[pos] == original[pos], true);
//     }

//     std::vector<bool> original10{ true, false, true, false, true, false, true, false, true, false };
//     usu::vector<bool> v4{ true, false, true, false, true, false, true, false, true, false };

//     v4.insert(0, false);

//     EXPECT_EQ(v4.size(), 11);
//     EXPECT_EQ(v4.capacity(), 20);
//     EXPECT_EQ(v4[0] == false, true);

//     for (std::size_t pos = 0; pos < original10.size(); pos++)
//     {
//         EXPECT_EQ(v4[pos + 1] == original10[pos], true);
//     }

//     // Make sure insert throws a range error when out of bounds
//     try
//     {
//         usu::vector<bool> v5{ true, false, true, false, true, false };

//         v5.insert(v5.size() + 1, true);
//         EXPECT_EQ(true, false);
//     }
//     catch (const std::range_error&)
//     {
//         EXPECT_EQ(true, true);
//     }
// }

// TEST(BoolSpecializationModify, Remove)
// {
//     std::vector<bool> v0{ true, false, true, false, true, false };
//     usu::vector<bool> v1{ true, false, true, false, true, false };

//     v1.remove(0);
//     EXPECT_EQ(v1.size(), 5);

//     for (std::size_t pos = 0; pos < v1.size(); pos++)
//     {
//         EXPECT_EQ(v1[pos] == v0[pos + 1], true);
//     }

//     usu::vector<bool> v2{ true, false, true, false, true, false };
//     v2.remove(v2.size() - 1);
//     EXPECT_EQ(v2.size(), 5);

//     for (std::size_t pos = 0; pos < v2.size(); pos++)
//     {
//         EXPECT_EQ(v2[pos] == v0[pos], true);
//     }

//     usu::vector<bool> v3{ true, false, true, false, true, false };
//     v3.remove(2);
//     v0.erase(v0.begin() + 2);
//     EXPECT_EQ(v3.size(), 5);

//     for (std::size_t pos = 0; pos < v3.size(); pos++)
//     {
//         EXPECT_EQ(v3[pos] == v0[pos], true);
//     }

//     // Make sure remove throws a range error when out of bounds
//     try
//     {
//         usu::vector<bool> v4{ true, false, true, false, true, false };

//         v4.remove(v4.size() + 1);
//         EXPECT_EQ(true, false);
//     }
//     catch (const std::range_error&)
//     {
//         EXPECT_EQ(true, true);
//     }

//     // Test for when bits have to slide from one byte to another
//     usu::vector<bool> v5{ true, false, true, false, true, false, true, false, true, false };
//     v5.remove(3);
//     EXPECT_EQ(v5[0] == true, true);
//     EXPECT_EQ(v5[v5.size() - 1] == false, true);
//     EXPECT_EQ(v5[7] == true, true);
// }

// TEST(BoolSpecializationModify, Iterator)
// {
//     {
//         usu::vector<bool> v1{ true, false };

//         auto i = v1.begin();
//         auto ref = *i;
//         ref = false;
//         EXPECT_EQ(v1[0] == false, true);
//     }

//     {
//         usu::vector<bool> v1{ true, false };

//         auto i = v1.begin();
//         ++i;
//         auto ref = *i;
//         ref = true;
//         EXPECT_EQ(v1[1] == true, true);
//     }
// }