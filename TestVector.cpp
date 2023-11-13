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
    for (int i = 0; i < 10; ++i)  // fill the first bucket
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
    
    vec.insert(1,99);  // newVec: [0, 1, 99, 3, 4, 5, 6, 7, 8, 9] -- THIS FAILS
    EXPECT_EQ(vec.size(), 11);
    EXPECT_EQ(vec[0], 0);
    EXPECT_EQ(vec[1], 1);
    EXPECT_EQ(vec[2], 99);
    EXPECT_EQ(vec[3], 3);
}