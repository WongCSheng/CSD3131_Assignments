#include <iostream>
#include <vector>

/**
 * @brief Calling 3 recursive functions with n/3 as the argument until the base case is reached.
 * T(n) = 3T(n/3)
 * 
 * @param n 
 */
void recursiveExample(int n)
{
    if (n <= 1)
    {
        std::cout << "Base case reached with n = " << n << std::endl;
        return;
    }

    for(int i = 0; i < n; ++i)
    {
        std::cout << "log(n) time" << std::endl;
    }
    std::cout << "Recursive call with n = " << n << std::endl;
    recursiveExample(n / 3);
    recursiveExample(n / 3);
    recursiveExample(n / 3);
}