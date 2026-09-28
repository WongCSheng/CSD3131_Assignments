/**
 * @file Question2BShowcase.cpp
 * @author Wong Chong Sheng (w.chongsheng@digipen.edu)
 * @brief Program to showcase the implementation of Question 2B.
 *      The following program shows the usage of the substitution method, recursive method, and master method
 *      which satisfies the time complexity T(n) = 3T(n/3) + lg(n), to solve a complex problem.
 * @date 23-09-2026
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include <iostream>
#include <stdlib.h>
#include <chrono>

#include "recursiveFunc.h"

/*
* From T(n) = 3·T(n/3) + lg(n), we can tell that the recursive method splits into 3 subproblems of size n/3,
* and the cost of combining the results is lg(n).
*/

/**
 * @brief An example of a real life scenerio where T(n) = 3·T(n/3) + lg(n), would be 
 * 
 */

void SubstitutionMethod()
{
    std::cout << "Substitution Method choosen.\nEnter number of recursive calls: ";
    int n;
    std::cin >> n;
    auto start_time = std::chrono::high_resolution_clock::now();
    recursiveExample(n);
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    std::cout << "Time taken: " << duration.count() << " microseconds" << std::endl;
}

/*
* From T(n) = 3·T(n/3) + lg(n), we can tell that the recursive method splits into 3 subproblems of size n/3,
* and the cost of combining the results is lg(n).
*/

void RecursiveMethod()
{
    std::cout << "Recursive Method choosen" << std::endl;
}

/*
 * To understand from the Master Method perspective, we have T(n) = 3·T(n/3) + lg(n), we can see that a = 3, b = 3, and f(n) = lg(n).
 */

void MasterMethod()
{
    std::cout << "Master Method choosen" << std::endl;
}

int main()
{    
    int selectedMethod = 0;
    
    std::cout << "Enter which method you want to view:\n1. Substitution Method\n2. Recursive Method\n3. Master Method\n4. Exit\n";

    while(std::cin >> selectedMethod)
    {
        // Exit the program if the user inputs 4
        if(selectedMethod == 4)
        {
            std::cout << "Exiting program." << std::endl;
            break;
        }

        switch(selectedMethod)
        {
            case 1:
                SubstitutionMethod();
                break;
            case 2:
                RecursiveMethod();
                break;
            case 3:
                MasterMethod();
                break;
            default:
                std::cout << "Invalid input. Please try again." << std::endl;
        }

        selectedMethod = 0;
    
        std::cout << "Enter which method you want to view:\n1. Substitution Method\n2. Recursive Method\n3. Master Method\n4. Exit\n";
    }

    return 0;
}