#include "../lib/energy.h"

#include <algorithm>

// https://avikdas.com/2019/05/14/real-world-dynamic-programming-seam-carving.html

// g++ -Wall -c ./lib/energy.o ./A2_Q1/q1.cpp -o ./lib/energy.o

using EnergyRow = std::vector<int>;
using EnergyMap = std::vector<EnergyRow>;

EnergyMap buildEnergyMap(const imgData& imd)
{
    int w = imd.getWidth();
    int h = imd.getHeight();

    EnergyMap em = EnergyMap(h, std::vector<int>(w));

    for (int i=0; i < h; i++)
    {
        for (int j=0; j < w; j++)
        {
            em[i][j] = generateEnergyValue(imd, i,j);
        }
    }

    return em;
}

struct seamPoint
{
    int energy;
    int prev_x;
};

std::vector<int> findSeam(const imgData& imd, const EnergyMap& em)
{

    // seam map for dp prt
    std::vector<std::vector<seamPoint>> storage;
    std::vector<seamPoint> prevER;
    for (int y=0; y < imd.getWidth(); y++)
    {
        prevER.push_back(seamPoint{em[0][y], -1});
    }

    for (int y=1; y < imd.getHeight(); y++)
    {
        std::vector<seamPoint> spr{};
        const EnergyRow& er = em[y];

        for (int x=0; x <imd.getWidth(); x++)
        {
            int energy = er[x];
            int x_left = std::max(0, x-1);
            int x_right = std::min(x+1, imd.getWidth()-1);

            int minE = er[x_left];
            int minpos = x_left;
            for (int i=x_left; i <= x_right; i++)
            {
                if (minE > prevER[i].energy)
                {
                    minE = prevER[i].energy;
                    minpos = i;
                }
            }

            minE += energy;
            spr.push_back(seamPoint{minE, minpos});
        }

        storage.push_back(prevER);
        prevER = spr;
    }

    storage.push_back(prevER);

    // find lowest seam
    std::vector<int> seampos;

    //find smallest in last row
    int pos = 0;
    std::vector<seamPoint> lastRow = storage[imd.getHeight()-1];
    for (int i=0; i<imd.getWidth(); i++)
    {
        if (lastRow[pos].energy > lastRow[i].energy)
        {
            pos = i;
        }
    }

    for (int i=imd.getHeight()-1; i >= 0; i--)
    {
        seamPoint sp = storage[i][pos];
        seampos.push_back(pos);
        pos = sp.prev_x;
    }

    std::reverse(seampos.begin(), seampos.end());
    return seampos;
}


int main()
{
    std::string imgpath = "./img/surfer.png";

    imgData imd{};
    if (!imd.parseImg(imgpath))
    {
        std::cout << "Exiting..." <<std::endl;
        return 0;
    }

    EnergyMap em = buildEnergyMap(imd);

    // for (const EnergyRow& er : em)
    // {
    //     for (const int i : er)
    //     {
    //         std::cout << i << ",";
    //     }
    //     std::cout <<std::endl;
    // }

    for (int i=0; i < 1024; i++)
    {
        std::vector<int> seam = findSeam(imd, em);

        // std::cout << "seam: ";
        // for (int i : seam)
        // {
        //     std::cout << i << ",";
        // }
        // std::cout << std::endl;

        imd.removeSeam(seam);
        std::cout << i << "/1024..." <<std::endl; 
    }

    std::string imgout = "./A2_Q1/outs.png";
    imd.writeImg(imgout);


    return 0;
}