#include <iostream>
#include <vector>

// heapsort algorithm implementation for O(NlogN) algorithm demo
// build with  g++ Q2_linearithmic.cpp -o Q2lin
// then do ./Q2lin

void heapify(std::vector<int>& v, int n, int i)
{
    //set node at i, calc positions of left/right nodes
    int node = i;
    int left = 2*i +1;
    int right = 2*i + 2;

    //node should be largest
    //if num at left is larger
    // take note of left pos in node
    if (left < n && v[left] > v[node])
        node = left;

    //if num at right is larger
    // take note of right pos in node
    if (right < n && v[right] > v[node])
        node = right;
    
    //if node is not i, swap the value at node with i
    if (node != i)
    {
        std::swap(v[i], v[node]);
        heapify(v, n, node);
    }
}

void heapSort(std::vector<int>& v)
{
    int n = v.size();

    //build heap
    for (int i = (n/2 - 1); i >= 0; i--)
        heapify(v,n,i);

    // take element at pos 0, should be largest and move to end
    // then heapify reduced heap again
    for (int i= (n-1); i > 0; i--)
    {
        std::swap(v[0], v[i]);
        heapify(v, i, 0);
    }
}

int main()
{
    std::vector<int> v = {4,6,8,32,75,3,0,54,14};

    std::cout << "initial array: ";
    for (int i : v)
        std::cout << i << ","; 

    std::cout << std::endl;

    heapSort(v);

    std::cout << "final array: ";
    for (int i : v)
        std::cout << i << ","; 

    std::cout << std::endl;
}