#include "../lib/energy.h"

#include <algorithm>

// https://avikdas.com/2019/05/14/real-world-dynamic-programming-seam-carving.html
// g++ -Wall -c ./lib/energy.o ./A2_Q1/q1.cpp -o ./lib/energy.o

//type declarations
using EnergyRow = std::vector<int>;
using EnergyMap = std::vector<EnergyRow>;

// builds energy map
EnergyMap buildEnergyMap(const imgData& imd)
{
    int w = imd.getWidth();
    int h = imd.getHeight();

    EnergyMap em = EnergyMap(h, std::vector<int>(w));

    for (int i=0; i < h; i++)
    {
        for (int j=0; j < w; j++)
        {
            em[i][j] = generateEnergyValue(imd, j,i);
        }
    }

    return em;
}

// struct for backtracking when using dynamic programming
struct seamPoint
{
    int energy;
    int prev_x;
};

std::vector<int> findSeam(const imgData& imd, const EnergyMap& em)
{

    // seam map for dp prt
    std::vector<std::vector<seamPoint>> storage;

    //set preious row as first row 
    //since first row doesnt need to calculate for previous row's val
    std::vector<seamPoint> prevER;
    for (int x=0; x < imd.getWidth(); x++)
    {
        prevER.push_back(seamPoint{em[0][x], -1});
    }

    //start from 2nd row, minimize value
    for (int y=1; y < imd.getHeight(); y++)
    {
        std::vector<seamPoint> spr{};
        const EnergyRow& er = em[y];

        //get minimum energy for every pixel in row
        for (int x=0; x <imd.getWidth(); x++)
        {
            int energy = er[x];
            int x_left = std::max(0, x-1);
            int x_right = std::min(x+1, imd.getWidth()-1);

            int minE = prevER[x_left].energy;
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

        //push back previus row into storage
        storage.push_back(prevER);
        //set current row as prevous row
        prevER = spr;
    }

    //push back last row into storage
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

    //get seam points
    for (int i=imd.getHeight()-1; i >= 0; i--)
    {
        seamPoint sp = storage[i][pos];
        seampos.push_back(pos);
        pos = sp.prev_x;
    }

    //seam is reversed, so put it properly
    std::reverse(seampos.begin(), seampos.end());
    return seampos;
}

// main takes 2 args, first is path to image, 2nd is num of seams
int main(int argc, char *argv[])
{
    //prep image
    std::string imgpath = argv[1];
    imgData imd{};
    if (!imd.parseImg(imgpath))
    {
        std::cout << "Exiting..." <<std::endl;
        return 0;
    }

    EnergyMap em = buildEnergyMap(imd);
    // for (EnergyRow er : em)
    // {
    //     for (int i: er)
    //     {
    //         std::cout << i << ",";
    //     }
    //     std::cout << std::endl;
    // }

    //start seam carving
    for (int i=0; i < std::stoi(argv[2]); i++)
    {
        std::vector<int> seam = findSeam(imd, em);

        std::cout << "seam: " << std::endl;
        // for (int i : seam)
        // {
        //     std::cout << i << ",";

        // }
        std::cout << std::endl;
        imd.removeSeam(seam);
        em = buildEnergyMap(imd);
    }

    //create output image
    std::string imgout = "./A2_Q1/outs.png";
    imd.writeImg(imgout);


    return 0;
}