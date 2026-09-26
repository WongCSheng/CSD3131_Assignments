#include <vector>
#include <string>
#include <iostream>

std::vector<std::string> permutations (const std::vector<char>& v, size_t pos)
{
    std::vector<std::string> perms{};

    if (v.empty())
    {
        return perms;
    }
    if (v.size() == 1)
    {
        perms.push_back(std::string{v[0]});
        return perms;
    }

    std::vector<char> remainder = v;
    remainder.erase(remainder.begin()+pos);

    for (size_t i=0; i < remainder.size(); i++)
    {
        std::vector<std::string> subperm;
        subperm = permutations(remainder, i);

        for (std::string sp : subperm)
        {
            perms.push_back(v[pos]+sp);
        }

    }
    return perms;
}

int main()
{
    std::vector<char> carr {'a','b','c','d'};
    std::vector<std::string> outputs{};

    for (size_t i=0; i < carr.size(); i++)
    {
        std::vector<std::string> perms = permutations(carr,i);
        outputs.reserve(outputs.size() + perms.size());
        outputs.insert(outputs.end(), perms.begin(), perms.end());
    }

    std::cout << "number of permutations: " << outputs.size() << std::endl;
    for (std::string s : outputs)
    {
        std::cout << s << " | ";
    }
    std::cout <<std::endl;


    return 0;
}

