#include <iostream>
#include <vector>

// heaps algorithm implementation for O(N!) algorithm demo
// build with  g++ Q2_factorial.cpp -o Q2fac
// then do ./Q2fac

void printPerms(const std::vector<int>& v)
{
    for (int i : v) std::cout << i;
    std::cout << std::endl;
}

void heapPerm(std::vector<int>& v, int size, int n)
{
    //trace calls
    std::cout << "F({";
    for (int i : v) std::cout << i;
    std::cout << "}, " << size << "," << n << ")" << std::endl;

    // if size is 1, 
    // print permutation
    if (size == 1) {
        std::cout << "Permutation obtained: ";
        for (int i : v) std::cout << i;
        std::cout << std::endl;
        return;
    }


    for (int i =0; i < size; i++)
    {
        heapPerm(v, size - 1, n);

        // if size is odd,
        // swap first and last elements
        if (size % 2 == 1)
            std::swap(v[0], v[size-1]);

        // if size is even,
        // swap at pos i and last
        else
            std::swap(v[i], v[size-1]);
    }
}

int main()
{
    std::vector<int> v = { 1, 2, 3, 4 };
    int n = v.size();
    heapPerm(v, n, n);
    return 0;
}