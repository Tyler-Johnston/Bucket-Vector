#include "vector.hpp"

#include <iostream>
#include <string>

// template <typename T>
// requires usu::Vector<T>
// void report(T& vector)
// {
//     for (auto i = vector.begin(); i != vector.end(); i++)
//     {
//         std::cout << *i << ", ";
//     }
//     std::cout << std::endl;
// }

// void testBoolSpecialization()
// {
//     usu::vector<bool> vector;

//     // Adding 12 values because this will span one and a half bytes,
//     // giving a good exercise to the specialization.
//     for (auto i = 0; i < 12; i++)
//     {
//         vector.add(i % 2 == 0);
//     }

//     // Counted for loop
//     std::cout << "<bool> Specialization: Counted for loop\n";
//     for (std::size_t i = 0; i < vector.size(); i++)
//     {
//         std::cout << vector[i] << ", ";
//     }
//     std::cout << std::endl;

//     // Iterator for loop
//     std::cout << "<bool> Specialization: Iterator for loop\n";
//     for (auto i = vector.begin(); i != vector.end(); i++)
//     {
//         std::cout << *i << ", ";
//     }
//     std::cout << std::endl;

//     // Range-based for loop
//     std::cout << "<bool> Specialization: Ranged-based for loop\n";
//     for (auto&& value : vector)
//     {
//         std::cout << value << ", ";
//     }
//     std::cout << std::endl
//               << std::endl;

//     usu::vector<bool> vector1{ true, true, true, false, false, false, true, false, false, true };
//     usu::vector<bool> vector2{ true, true, true, false, false, false, true, false, false, true };
//     usu::vector<bool> vector3{ true, true, true, false, false, false, true, false, false, true };
//     usu::vector<bool> vector4{ true, true, true, false, false, false, true, false, false, true };
//     usu::vector<bool> vector5{ true, true, true, false, false, false, true, false, false, true };

//     std::cout << "original, insert(0, false), remove(0)" << std::endl;
//     report(vector1);
//     vector1.insert(0, false);
//     report(vector1);
//     vector1.remove(0);
//     report(vector1);
//     std::cout << std::endl;

//     std::cout << "original, insert(1, false), remove(1)" << std::endl;
//     report(vector2);
//     vector2.insert(1, false);
//     report(vector2);
//     vector2.remove(1);
//     report(vector2);
//     std::cout << std::endl;

//     std::cout << "original, insert(3, true), remove(3)" << std::endl;
//     report(vector3);
//     vector3.insert(3, true);
//     report(vector3);
//     vector3.remove(3);
//     report(vector3);
//     std::cout << std::endl;

//     std::cout << "original, insert(4, true), remove(4)" << std::endl;
//     report(vector4);
//     vector4.insert(4, true);
//     report(vector4);
//     vector4.remove(4);
//     report(vector4);
//     std::cout << std::endl;

//     std::cout << "original, insert(6, true), remove(6)" << std::endl;
//     report(vector5);
//     vector5.insert(6, true);
//     report(vector5);
//     vector5.remove(6);
//     report(vector5);
// }

// // ------------------------------------------------------------------
// //
// // Final project, a vector with a bool specialization
// //
// // ------------------------------------------------------------------
int main()
{
    std::cout << "this is the main func!" << std::endl;

    return 0;
}
