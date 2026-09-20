#include <vector>
#include <string>

int main()
{
    std::vector<char> carr {'a','b','c'};
    std::vector<std::string> outputs{};

}

std::vector<std::string> permutations (const std::vector<char>& v, int pos)
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

    for (int i=0; i < v.size(); i++)
    {
        std::string start{};
        start += v[i];

    }
}