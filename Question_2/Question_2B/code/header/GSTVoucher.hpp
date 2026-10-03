#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>

/**
 * @brief Data for each person
 * 
 */
struct Person
{
    int age;
    std::string job;
    double monthlyIncome;
    int propertiesOwned;
    double propertyMonthlyValue;
    
    bool isEligible = false;
    double cashbackAmount = 0.0;
};

/**
 * @brief Evaluates the eligibility of a person for the GST voucher based on government criteria
 * Rules:
 * - Gross monthly income > $3000 -> Not eligible ($0)
 * - Between $3000 and $2250 -> $450 cashback
 * - Less than $2250 -> $850 cashback
 * - Must own < 2 properties (i.e., 0 or 1 property)
 * - Monthly rental for the property must be < $1000
 * @param p 
 */
void evaluateEligibility(Person& p)
{
    if (p.propertiesOwned < 2 && p.propertyMonthlyValue < 1000.0)
    {
        if (p.monthlyIncome < 2250.0)
        {
            p.isEligible = true;
            p.cashbackAmount = 850.0;
        }
        else if (p.monthlyIncome <= 3000.0)
        {
            p.isEligible = true;
            p.cashbackAmount = 450.0;
        }
        else
        {
            p.isEligible = false;
            p.cashbackAmount = 0.0;
        }
    }
    else
    {
        p.isEligible = false;
        p.cashbackAmount = 0.0;
    }
}

/**
 * @brief Recursively splits the dataset into 3 segments and evaluates each segment for eligibility.
 * 
 * @param data 
 * @param left 
 * @param right 
 */
void processSegmentRecursive(std::vector<Person>& data, size_t left, size_t right)
{
    if (left >= right)
    {
        return;
    }

    // O(log n) work equivalent simulation per recursive frame split initialization
    // (e.g., computing balanced index pivots or split boundaries via logarithmic steps)
    volatile double overheadWork = 0;
    size_t length = right - left + 1;
    if (length > 1)
    {
        overheadWork += std::log2(length); 
    }

    if (length == 1)
    {
        evaluateEligibility(data[left]);
        return;
    }

    // Divide size into 3 sub-segments
    size_t third = length / 3;
    size_t mid1 = left + third;
    size_t mid2 = left + 2 * third;

    // 3 recursive branches: T(n/3) called 3 times
    processSegmentRecursive(data, left, mid1 - 1);
    processSegmentRecursive(data, mid1, mid2 - 1);
    processSegmentRecursive(data, mid2, right);
}

/**
 * @brief Read the CSV file and populate the vector of Person structs.
 * 
 * @param filename 
 * @return std::vector<Person> 
 */
std::vector<Person> readCSV(const std::string& filename)
{
    std::vector<Person> people;
    std::ifstream file(filename);
    
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return people;
    }

    std::string line;
    // Skip header line
    std::getline(file, line);

    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string val;
        Person p;

        try
        {
            // "Age", "Job", "Monthly Income", "Amount of property owned", "Property monthly value"
            if (std::getline(ss, val, ',')) p.age = std::stoi(val);
            if (std::getline(ss, val, ',')) p.job = val;
            if (std::getline(ss, val, ',')) p.monthlyIncome = std::stod(val);
            if (std::getline(ss, val, ',')) p.propertiesOwned = std::stoi(val);
            if (std::getline(ss, val, ',')) p.propertyMonthlyValue = std::stod(val);

            people.push_back(p);
        }
        catch (...)
        {
            // Skip malformed rows safely
            continue;
        }
    }

    file.close();
    return people;
}