#include "vector.hpp"

// changed it from <format> to this due to compiler issues on my end, temporarily
#include <fmt/format.h>
#include <iostream>
#include <string>

template <typename T>
void report(T& vector)
{
    std::cout << fmt::format("size    : {}\ncapacity: {}\nvalues  : ", vector.size(), vector.capacity());

    for (auto i = vector.begin(); i != vector.end(); i++)
    {
        std::cout << *i << ", ";
    }
    std::cout << std::endl;
}

template <typename T>
void demonstrateConstruction(usu::vector<T>& v1, usu::vector<T>& v2, usu::vector<T>& v3)
{
    std::cout << "\nDemonstrating Construction" << std::endl;

    std::cout << "\n-- v1 --\n";
    report(v1);

    std::cout << "\n-- v2 --\n";
    report(v2);

    std::cout << "\n-- v3 --\n";
    report(v3);
}

template <typename T>
void demonstrateAddInsertRemove(usu::vector<T>& v, const T& addValue, const T& insertValue1, const T& insertValue2, const T& insertValue3)
{
    std::cout << "\nDemonstrating Add/Insert/Remove" << std::endl;
    
    v.add(addValue);
    std::cout << "\n-- add --\n";
    report(v);

    v.insert(0, insertValue1);
    std::cout << "\n-- insert at 0 --\n";
    report(v);

    v.insert(4, insertValue2);
    std::cout << "\n-- insert at 4 --\n";
    report(v);

    v.insert(9, insertValue3);
    std::cout << "\n-- insert at 9 --\n";
    report(v);

    v.remove(0);
    std::cout << "\n-- remove at 0 --\n";
    report(v);

    v.remove(3);
    std::cout << "\n-- remove at 3 --\n";
    report(v);

    v.remove(7);
    std::cout << "\n-- remove at 7 --\n";
    report(v);
}

template <typename T, typename E>
void demonstrateCapacity(usu::vector<T>& v1, usu::vector<E>& v2)
{
    std::cout << "\nDemonstrate Capacity" << std::endl;
    // Insert until new capacity is required
    v1.insert(0, 29);
    v1.insert(0, 31);
    v1.insert(0, 47);
    std::cout << "\n-- maxed capacity (v1) --\n";
    report(v1);
    v1.insert(0, 41);
    std::cout << "\n-- updated capacity (v1) --\n";
    report(v1);

    std::cout << "\n-- initial capacity (v2) --\n";
    report(v2);
    v2.add("twenty-six");
    v2.add("thirty");
    v2.add("thirty-three");
    std::cout << "\n-- maxed capacity (v2) --\n";
    report(v2);
    v2.add("thirty-nine");
    std::cout << "\n-- updated capacity (v2) --\n";
    report(v2);
}

template <typename T>
void demonstrateIteration(usu::vector<T>& v)
{
    std::cout << "\nDemonstrate Iteration\n" << std::endl;

    std::cout << "Postfix: ";
    for (auto i{ v.begin() }; i != v.end(); i++)
    {
        std::cout << *i << ", ";
    }
    std::cout << std::endl;

    std::cout << "Prefix: ";
    for (auto i{ v.begin() }; i != v.end(); ++i)
    {
        std::cout << *i << ", ";
    }
    std::cout << std::endl;

    std::cout << "Decrement: \n";
    auto j{ v.begin() };
    ++j;
    ++j;
    std::cout << *j << std::endl;
    --j;
    std::cout << *j << std::endl;
    j--;
    std::cout << *j << std::endl;

    std::cout << "For-Each iteration: ";
    for (auto&& value : v)
    {
        std::cout << value << ", ";
    }
    std::cout << std::endl;
}

template <typename T>
void demonstrateMapping(usu::vector<T>& v, std::function<void(T&)> func)
{
    std::cout << "\nDemonstrate Mapping" << std::endl;

    std::cout << "\n-- initial values --\n";
    report(v);
    std::cout << "\n-- applying lambda -- " << std::endl;
    v.map(func);
    report(v);
}

int main()
{
    usu::vector<int> v1;
    usu::vector<int> v2(20);
    usu::vector v3{ 1, 2, 3, 5, 7, 11 };
    usu::vector<std::string> v4{ "one", "two", "three", "five", "seven", "eleven" };
    usu::vector v5{ 1.1, 2.2, 3.3, 5.5, 7.7, 11.1 };

    demonstrateConstruction(v1, v2, v3);

    // test adding/inserting/removing on int, string, and float types
    demonstrateAddInsertRemove(v3, 23, 13, 17, 19);
    demonstrateAddInsertRemove(v4, std::string("twenty-three"), std::string("thirteen"), std::string("seventeen"), std::string("nineteen"));
    demonstrateAddInsertRemove(v5, 23.3, 13.3, 17.7, 19.9);

    // test capacity on int and string type
    demonstrateCapacity(v3, v4);

    // test post/pre-fix, decrement, and for-each iteration
    demonstrateIteration(v3);
    demonstrateIteration(v4);

    // test lambda function mapping over elements in the vector for int and string types
    demonstrateMapping<int>(v3, [](int& x) { x = x * x; });
    demonstrateMapping<std::string>(v4, [](std::string& str) { str += " - modified"; });



    
    return 0;
}
