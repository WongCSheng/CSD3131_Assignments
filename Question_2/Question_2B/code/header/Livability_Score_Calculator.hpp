#include <iostream>
#include <vector>

/**
 * NON-RECURSIVE WORK: O(log n)
 * Simulates querying a URA zoning database to calculate an infrastructure overhead.
 * By using a binary search pattern (halving the search space), 
 * it executes in strictly logarithmic time relative to 'n'.
 */
int queryURA_ZoningDatabase(int n) {
    if (n <= 0) return 0;
    
    int left = 1;
    int right = n;
    int infrastructure_overhead = 0;
    
    // Binary search pattern guarantees O(log n) complexity
    while (left <= right) {
        int mid = left + (right - left) / 2;
        infrastructure_overhead += 5; // Simulate score calculation at this level
        left = mid + 1;               // Force worst-case traversal to ensure exactly log(n) steps
    }
    
    return infrastructure_overhead;
}

/**
 * RECURSIVE ALGORITHM: T(n) = 3T(n/3) + log(n)
 * Evaluates the Livability Score of Singapore's living spaces.
 */
int calculateLivabilityScore(const std::vector<int>& land_parcels, int start, int end) {
    int n = end - start;
    
    // 1. BASE CASE: O(1) time
    // If we have fewer than 3 land parcels, we can't divide them into North/West/East.
    if (n < 3) {
        int score = 0;
        for (int i = start; i < end; ++i) {
            score += land_parcels[i];
        }
        return score;
    }
    
    // 2. DIVIDE: O(1) time
    // We strictly use indices to prevent O(n) array copying.
    // We divide the current dataset 'n' into 3 equal thirds.
    int third = n / 3;
    int boundary_NW = start + third;         // Boundary between North and West
    int boundary_WE = start + 2 * third;     // Boundary between West and East
    
    // 3. CONQUER: 3 * T(n/3) time
    // Recursively process the 3 geographic living spaces
    int north_score = calculateLivabilityScore(land_parcels, start, boundary_NW);
    int west_score  = calculateLivabilityScore(land_parcels, boundary_NW, boundary_WE);
    int east_score  = calculateLivabilityScore(land_parcels, boundary_WE, end);
    
    // 4. NON-RECURSIVE WORK: O(log n) time
    // Fetch the zoning overhead for the current land size 'n'
    int zoning_overhead = queryURA_ZoningDatabase(n);
    
    // 5. COMBINE: O(1) time
    // Total livability is the sum of the regions plus the zoning overhead
    return north_score + west_score + east_score + zoning_overhead;
}

void tempFunc()
{
    // Simulated URA data for Singapore land parcels (Livability points per parcel)
    // We use a size of 27 (3^3) to demonstrate the T(n/3) division perfectly.
    std::vector<int> singapore_land_spaces = {
        80, 85, 90, 75, 70, 88, 92, 78, 81,  // North Region Parcels
        65, 60, 55, 85, 89, 90, 75, 72, 68,  // West Region Parcels
        95, 92, 88, 85, 80, 91, 89, 94, 90   // East Region Parcels
    };
    
    int total_parcels = singapore_land_spaces.size();
    
    std::cout << "===========================================" << std::endl;
    std::cout << " URA Urban Livability Analysis (Singapore) " << std::endl;
    std::cout << "===========================================" << std::endl;
    std::cout << "Total Land Parcels to process: " << total_parcels << "\n" << std::endl;
    
    // Execute the algorithm
    int final_score = calculateLivabilityScore(singapore_land_spaces, 0, total_parcels);
    
    std::cout << "Algorithm Executed Successfully!" << std::endl;
    std::cout << "Overall Singapore Livability Score: " << final_score << std::endl;
    std::cout << "===========================================" << std::endl;
}