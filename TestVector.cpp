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
    // [1,3]
    vec.add(1);
    vec.add(3);
    vec.insert(1, 2);  // inserting in between
    // [1,2,3]
    EXPECT_EQ(vec.size(), 3);
    EXPECT_EQ(vec[0], 1);
    EXPECT_EQ(vec[1], 2);
    EXPECT_EQ(vec[2], 3);
}

TEST(Buckets, SplittingFromAdd)
{
    usu::vector<int> vec;
    for (int i = 0; i < 10; ++i)  // [0,1,2,3,4,5,6,7,8,9]
    {
        vec.add(i);
    }
    EXPECT_EQ(vec.size(), 10);
    
    // adding one extra element should cause a split
    vec.add(10); 

    // the size of the vector should be 11, and the 10th element should be 10
    EXPECT_EQ(vec.size(), 11);

    // ensure all elements are still correct, including new element that caused the split
    for (int i=0; i<vec.size(); i++)
    {
        EXPECT_EQ(vec[i], i);
    }
}

TEST(Buckets, Iterators) 
{
    usu::vector<int> vec;
    auto start = vec.begin();
    auto end = vec.end();
    
    // test1: when the vector is empty, vec.begin() should equal vec.end().
    EXPECT_EQ(start, end);

    // test 2: Incrementing iterator
    vec.add(10);
    vec.add(20);
    vec.add(30);
    auto it = vec.begin();
    EXPECT_EQ(*it, 10);
    ++it;
    EXPECT_EQ(*it, 20);
    ++it;
    EXPECT_EQ(*it, 30);
    ++it;
    EXPECT_EQ(it, vec.end());
}

TEST(Buckets, SplittingFromInsert)
{
    usu::vector<int> vec;
    for (int i = 0; i < 10; ++i)  // fill the first bucket
    {
        vec.add(i);
    }
    EXPECT_EQ(vec.size(), 10);
    
    vec.insert(1,99);
    EXPECT_EQ(vec.size(), 11);
    EXPECT_EQ(vec[0], 0);
    EXPECT_EQ(vec[1], 99);
    EXPECT_EQ(vec[2], 1);
    EXPECT_EQ(vec[3], 2);
    EXPECT_EQ(vec[4], 3);
    EXPECT_EQ(vec[5], 4);
    EXPECT_EQ(vec[6], 5);
    EXPECT_EQ(vec[7], 6);
}

TEST(Buckets, Removing)
{
    usu::vector<int> vec;
    for (int i = 0; i < 10; ++i)  // [0,1,2,3,4,5,6,7,8,9]
    {
        vec.add(i);
    }
    vec.remove(0); 
    EXPECT_EQ(vec[0], 1); // [1,2,3,4,5,6,7,8,9]
    vec.remove(0);
    EXPECT_EQ(vec[0], 2); // [2,3,4,5,6,7,8,9]
    vec.remove(1);
    EXPECT_EQ(vec[0], 2); // [2,4,5,6,7,8,9]
    vec.remove(6);
    for (int i = 1; i < vec.size(); i++)  // [2,4,5,6,7,8] check if 4,5,6,7,8 is correct
    {
        EXPECT_EQ(vec[i], i+3);
    }
    // there is no 999th index
    ASSERT_THROW(vec.remove(999), std::range_error);

}

TEST(Constructor, InitializerList)
{
    using namespace std::string_literals;

    usu::vector<int> v1{ 1, 2, 3, 5, 7, 11, 13, 17 };
    EXPECT_EQ(v1.size(), 8);

    // Make one larger than the default capacity
    usu::vector<int> v2{ 1, 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41 };
    EXPECT_EQ(v2.size(), 14);

    usu::vector<std::string> v3{ "one"s, "two"s, "three"s, "five"s, "seven"s, "eleven"s, "thirteen"s, "seventeen"s };
    EXPECT_EQ(v3.size(), 8);
}

TEST(Operators, Array)
{
    using namespace std::string_literals;

    // Verify initializer_list constructor and reading from the vector
    std::vector<int> primes{ 1, 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31 };
    usu::vector<int> v1{ 1, 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31 };

    for (std::size_t pos = 0; pos < v1.size(); pos++)
    {
        EXPECT_EQ(v1[pos], primes[pos]);
    }

    // Verify out of bounds causes exception
    usu::vector<int> v2;
    try
    {
        v2[0] = 1;
        EXPECT_EQ(true, false);
    }
    catch (std::exception&)
    {
        EXPECT_EQ(true, true);
    }

    // Make sure the initializer list array does the same
    try
    {
        v1[v1.size()] = 1;
        EXPECT_EQ(true, false);
    }
    catch (std::exception&)
    {
        EXPECT_EQ(true, true);
    }

    // Verify we can read and write to the vector positions
    usu::vector<int> v3{ 0, 0, 0, 0 };
    v3[0] = 1;
    v3[1] = 2;
    v3[2] = 3;
    v3[3] = 5;

    EXPECT_EQ(v3[0], 1);
    EXPECT_EQ(v3[1], 2);
    EXPECT_EQ(v3[2], 3);
    EXPECT_EQ(v3[3], 5);

    usu::vector<std::string> v4{ "one"s, "two"s, "three"s, "five"s, "seven"s, "eleven"s, "thirteen"s, "seventeen"s };
    std::vector<std::string> stringPrimes{ "one"s, "two"s, "three"s, "five"s, "seven"s, "eleven"s, "thirteen"s, "seventeen"s };

    for (std::size_t pos = 0; pos < v4.size(); pos++)
    {
        EXPECT_EQ(v4[pos], stringPrimes[pos]);
    }

    usu::vector<std::string> v5{ "zero"s, "zero"s, "zero"s, "zero"s };
    v5[0] = "one"s;
    v5[1] = "two"s;
    v5[2] = "three"s;
    v5[3] = "five"s;

    EXPECT_EQ(v5[0], "one"s);
    EXPECT_EQ(v5[1], "two"s);
    EXPECT_EQ(v5[2], "three"s);
    EXPECT_EQ(v5[3], "five"s);
}

TEST(Iterators, ForCounted)
{
    std::vector<int> primes{ 1, 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41 };
    usu::vector<int> v1{ 1, 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41 };

    // Post-increment
    std::size_t pos = 0;
    for (auto itr = v1.begin(); itr != v1.end(); itr++, pos++)
    {
        EXPECT_EQ(*itr, primes[pos]);
    }

    // Pre-increment
    pos = 0;
    for (auto itr = v1.begin(); itr != v1.end(); ++itr, pos++)
    {
        EXPECT_EQ(*itr, primes[pos]);
    }

    // Post-increment
    pos = primes.size() - 1;
    for (auto itr = (--v1.end()); itr != v1.begin(); itr--, pos--)
    {
        EXPECT_EQ(*itr, primes[pos]);
    }
    
    // Pre-increment
    pos = primes.size() - 1;
    for (auto itr = (--v1.end()); itr != v1.begin(); --itr, pos--)
    {
        EXPECT_EQ(*itr, primes[pos]);
    }
}


TEST(Iterators, AccessOperators)
{
    {
        usu::vector<std::pair<int, int>> v1{ { 1, 1 }, { 2, 2 }, { 3, 3 }, { 4, 5 }, { 5, 7 }, { 6, 11 } };

        auto itr = v1.begin();
        EXPECT_EQ((*itr).first, 1);
        EXPECT_EQ((*itr).second, 1);

        EXPECT_EQ(itr->first, 1);
        EXPECT_EQ(itr->second, 1);

        itr++;
        itr++;
        itr++;

        EXPECT_EQ((*itr).first, 4);
        EXPECT_EQ((*itr).second, 5);

        EXPECT_EQ(itr->first, 4);
        EXPECT_EQ(itr->second, 5);
    }

    {
        using namespace std::string_literals;

        usu::vector<std::pair<int, std::string>> v2{
            { 1, "one"s },
            { 2, "two"s },
            { 3, "three"s },
            { 4, "five"s },
            { 5, "seven"s },
            { 6, "eleven"s }
        };

        auto itr = v2.begin();
        EXPECT_EQ((*itr).first, 1);
        EXPECT_EQ((*itr).second, "one"s);

        EXPECT_EQ(itr->first, 1);
        EXPECT_EQ(itr->second, "one"s);

        itr++;
        itr++;
        itr++;

        EXPECT_EQ((*itr).first, 4);
        EXPECT_EQ((*itr).second, "five"s);

        EXPECT_EQ(itr->first, 4);
        EXPECT_EQ(itr->second, "five"s);
    }
}

TEST(Modify, Add)
{
    std::vector<int> primes{ 1, 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41 };
    usu::vector<int> v1;
    usu::vector<int> v2{ 1, 2, 3, 5 };

    v1.add(1);
    v1.add(2);
    v1.add(3);
    v1.add(5);
    v1.add(7);
    v1.add(11);
    v1.add(13);
    v1.add(17);
    v1.add(19);
    v1.add(23);
    v1.add(29);
    v1.add(31);
    v1.add(37);
    v1.add(41);

    // Verifying .add works correctly combined with an initializer list
    v2.add(7);
    v2.add(11);
    v2.add(13);
    v2.add(17);
    v2.add(19);
    v2.add(23);
    v2.add(29);
    v2.add(31);
    v2.add(37);
    v2.add(41);

    EXPECT_EQ(v1.size(), primes.size());
    EXPECT_EQ(v2.size(), primes.size());

    for (std::size_t pos = 0; pos < primes.size(); pos++)
    {
        EXPECT_EQ(v1[pos], primes[pos]);
        EXPECT_EQ(v2[pos], primes[pos]);
    }
}

TEST(Modify, Insert)
{
    using namespace std::string_literals;

    std::vector<std::string> original{ "a"s, "b"s, "c"s, "d"s, "e"s, "f"s, "g"s };
    usu::vector<std::string> v1{ "a"s, "b"s, "c"s, "d"s, "e"s, "f"s, "g"s };

    v1.insert(0, "X"s);
    EXPECT_EQ(v1[0], "X"s);

    for (std::size_t pos = 0; pos < original.size(); pos++)
    {
        EXPECT_EQ(v1[pos + 1], original[pos]);
    }

    usu::vector<std::string> v2{ "a"s, "b"s, "c"s, "d"s, "e"s, "f"s, "g"s };
    v2.insert(1, "X"s);
    EXPECT_EQ(v2[0], "a"s);
    EXPECT_EQ(v2[1], "X"s);

    for (std::size_t pos = 2; pos < original.size(); pos++)
    {
        EXPECT_EQ(v2[pos + 1], original[pos]);
    }

    usu::vector<std::string> v3{ "a"s, "b"s, "c"s, "d"s, "e"s, "f"s, "g"s };
    v3.insert(7, "X"s); 
    EXPECT_EQ(v3[7], "X"s);

    for (std::size_t pos = 0; pos < original.size(); pos++)
    {
        EXPECT_EQ(v3[pos], original[pos]);
    }

    std::vector<std::string> original10{ "a"s, "b"s, "c"s, "d"s, "e"s, "f"s, "g"s, "h"s, "i"s, "j"s };
    usu::vector<std::string> v4{ "a"s, "b"s, "c"s, "d"s, "e"s, "f"s, "g"s, "h"s, "i"s, "j"s };

    v4.insert(0, "X"s);

    EXPECT_EQ(v4.size(), 11);
    EXPECT_EQ(v4[0], "X"s);

    for (std::size_t pos = 0; pos < original10.size(); pos++)
    {
        EXPECT_EQ(v4[pos + 1], original10[pos]);
    }

    // Make sure insert throws a range error when out of bounds
    try
    {
        usu::vector<int> v5{ 1, 2, 3, 5, 7, 11 };

        v5.insert(v5.size() + 1, 13);
        EXPECT_EQ(true, false);
    }
    catch (const std::range_error&)
    {
        EXPECT_EQ(true, true);
    }
}

TEST(Modify, Remove)
{
    std::vector<int> primes{ 1, 2, 3, 5, 7, 11 };
    usu::vector<int> v1{ 1, 2, 3, 5, 7, 11 };

    v1.remove(0);
    EXPECT_EQ(v1.size(), 5);

    for (std::size_t pos = 0; pos < v1.size(); pos++)
    {
        EXPECT_EQ(v1[pos], primes[pos + 1]);
    }

    usu::vector<int> v2{ 1, 2, 3, 5, 7, 11 };
    v2.remove(v2.size() - 1);
    EXPECT_EQ(v2.size(), 5);

    for (std::size_t pos = 0; pos < v2.size(); pos++)
    {
        EXPECT_EQ(v2[pos], primes[pos]);
    }

    usu::vector<int> v3{ 1, 2, 3, 5, 7, 11 };
    v3.remove(2);
    primes.erase(primes.begin() + 2);
    EXPECT_EQ(v3.size(), 5);

    for (std::size_t pos = 0; pos < v3.size(); pos++)
    {
        EXPECT_EQ(v3[pos], primes[pos]);
    }

    // Make sure remove throws a range error when out of bounds
    try
    {
        usu::vector<int> v4{ 1, 2, 3, 5, 7, 11 };

        v4.remove(v4.size() + 1);
        EXPECT_EQ(true, false);
    }
    catch (const std::range_error&)
    {
        EXPECT_EQ(true, true);
    }
}

TEST(Constructor, SizeCapacity)
{
    // 10 comes from a private constant in the usu::vector class: DEFAULT_INITIAL_CAPACITY
    usu::vector<int> v1;
    EXPECT_EQ(v1.size(), 0);
    EXPECT_EQ(v1.capacity(), 10);

    usu::vector<int> v2(2);
    EXPECT_EQ(v2.size(), 2);
    EXPECT_EQ(v2.capacity(), 10);

    // no resizing mehthods
    // // Default capacity increase is 2 * size
    // usu::vector<int> v3(100);
    // EXPECT_EQ(v3.size(), 100);
    // EXPECT_EQ(v3.capacity(), 200);
}