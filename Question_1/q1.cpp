#include "../lib/energy.h"

#include <algorithm>
#include <string>
#include <cmath>
#include <chrono>

// https://avikdas.com/2019/05/14/real-world-dynamic-programming-seam-carving.html
// g++ -Wall -c ./lib/energy.o ./A2_Q1/q1.cpp -o ./lib/energy.o

//type declarations
using EnergyRow = std::vector<int>;
using EnergyMap = std::vector<EnergyRow>;

// builds energy map
EnergyMap buildEnergyMap(const imgData& imd, int& max)
{
    int w = imd.getWidth();
    int h = imd.getHeight();

    EnergyMap em = EnergyMap(h, std::vector<int>(w));

    for (int i=0; i < h; i++)
    {
        for (int j=0; j < w; j++)
        {
            em[i][j] = generateEnergyValue(imd, j,i);
            //Get the max energy value
            if (max < std::abs(em[i][j])) {
                max = std::abs(em[i][j]);
            }
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

// from le min
void energyMapOutput(const imgData& imd, int max, const EnergyMap& em, const std::string& path, imgData& gradient) {
    int w = imd.getWidth();
    int h = imd.getHeight();

    std::vector<std::vector<double>> norm(h, std::vector<double>(w));
    //norm.reserve(h);

    for (int i = 0; i < h; i++)
    {
        norm[i].reserve(w);
        for (int j = 0; j < w; j++)
        {
            norm[i][j] =(std::fabs(static_cast<double>(em[i][j])) / static_cast<double>(max));
            //pow is needed for gamma correction (to make the faint edges more obvious)
            norm[i][j] = std::pow(norm[i][j], 0.3);
            //Multiply by 255 (0 - 255) color
            norm[i][j] *= 255.0;
            
        }
    
    }
    //Create the data for imgdata with the gradient values
    std::vector<unsigned char> gradient_data;

    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            //Get the value
            int val = norm[i][j];
            //clamp it between 0 to 255
            if (val > 255) {
                val = 255;
            }
            else if (val < 0) {
                val = 0;
            }
            //Set rgb to value for greyscale
            gradient_data.push_back(val);
            gradient_data.push_back(val);
            gradient_data.push_back(val);
            //Make alpha value to be 255 no matter
            if (imd.getCh() == 4) { gradient_data.push_back(255); } // alpha channel is stored 0-255 like the rest
        }
    }
    //Create an imgdata
    //imgData gradient;
    //Set the values based on imd and the gradient data
    gradient.createMap(imd.getWidth(), imd.getHeight(), imd.getCh(), gradient_data);
    //Write out as image
    gradient.writeImg(path);

}

// seam visualizer from le min
void addSeamsToAll(std::vector<std::vector<int>>& allseams, int h, std::vector<int>& seams) {
    for (int i = 0; i < h; i++) {
        allseams[i].push_back(seams[i]);
    }
}
std::vector<int> getOriginal(std::vector<int>& seams, int w, int h, std::vector<std::vector<int>>& origin_map) {
    std::vector<int> tmpVec;
    for (int i = 0; i < h; i++) {
        int c = seams[i];
        int originalc = origin_map[i][c];
        tmpVec.push_back(originalc);
    }
    return tmpVec;
}
void shrinkOriginMap(std::vector<std::vector<int>>& origin_map, std::vector<int>& seams, int w, int h) {
    for (int i = 0; i < h; i++) {
        int c = seams[i];
        origin_map[i].erase(origin_map[i].begin() + c);
    }
}
void seamOutput(const imgData& imd, const std::string& path, std::vector<std::vector<int>>& allseams) {
    int w = imd.getWidth();
    int h = imd.getHeight();

    std::vector<unsigned char> greyscale_data = imd.getData();

    std::vector<unsigned char> gradient_data;

    for (int i = 0; i < h; i++)
    {
        std::vector<int> seampos = allseams[i]; // get the seam col data
        for (int j = 0; j < w; j++)
        {
            //if value is not inside the vector, just get the greyscale value as it is not a seam
            if (std::find(seampos.begin(), seampos.end(), j) == seampos.end()) {
                size_t pos = (i * w + j) * imd.getCh();
                gradient_data.push_back(greyscale_data[pos]);
                gradient_data.push_back(greyscale_data[pos +1]);
                gradient_data.push_back(greyscale_data[pos +2]);
                
                if (imd.getCh() == 4) { gradient_data.push_back(greyscale_data[pos+3]); } 
                continue;
            }
            
            //Set rgb to value for greyscale
            gradient_data.push_back(255);
            gradient_data.push_back(0);
            gradient_data.push_back(0);
            //Make alpha value to be 255 no matter
            if (imd.getCh() == 4) { gradient_data.push_back(255); } // alpha channel is stored 0-255 like the rest
        }
    }
    //Create an imgdata
    imgData gradient;
    //Set the values based on imd and the gradient data
    gradient.createMap(imd.getWidth(), imd.getHeight(), imd.getCh(), gradient_data);
    //Write out as image
    gradient.writeImg(path);
}

// main takes 3 args, first is path to image, 2nd is num of seams, 3 is output folder path
int main(int argc, char *argv[])
{
    if (argc < 4) {
        std::cout << "Not enough inputs." << std::endl;
        std::cout << "Format: <img/path.png> <no of seams> <output/folder>" << std::endl;
        return 0;
    }

    //prep image
    std::string imgpath = argv[1];
    imgData imd{};
    if (!imd.parseImg(imgpath))
    {
        std::cout << "Exiting..." <<std::endl;
        return 0;
    }

    std::string outPath = std::string{argv[3]} + "/out.png";
    std::string gradientPath = std::string{argv[3]} + "/gradient.png";
    std::string seamPath = std::string{argv[3]} + "/seams.png";

    int max =0;
    EnergyMap em = buildEnergyMap(imd, max);
    imgData seamimd{};

    // energy map is output in the original orientation
    energyMapOutput(imd, max, em, gradientPath, seamimd);

    //Create a origin map to store the original column indexes
    std::vector<std::vector<int>> origin_map{};
    origin_map.resize(imd.getHeight());
    
    //Create all seams to store all the seams for the output
    std::vector<std::vector<int>> allseams{};
    allseams.resize(imd.getHeight());
    for (int r = 0; r < imd.getHeight(); r++) {
        origin_map[r].resize(imd.getWidth());
        for (int c = 0; c < imd.getWidth(); c++) {
            origin_map[r][c] = c;
        }
            
    }

    //start seam carving
    double totalduration{};

    for (int i=0; i < std::stoi(argv[2]); i++)
    {
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

        std::vector<int> seam = findSeam(imd, em);

        //le min code
        std::vector<int> originalco = getOriginal(seam, imd.getWidth(), imd.getHeight(), origin_map);
        addSeamsToAll(allseams, imd.getHeight(), originalco);
        shrinkOriginMap(origin_map, seam, imd.getWidth(), imd.getHeight());

        imd.removeSeam(seam);
        em = buildEnergyMap(imd, max);

        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        double duration = std::chrono::duration<double, std::milli>(end - start).count();

        totalduration += duration;
        std::cout << "Seam " << i <<" (" <<duration <<" msec)" << std::endl;
    }

    std::cout << "Total time: " << totalduration << " msec" << std::endl;

    //create seam image
    seamOutput(seamimd, seamPath, allseams);

    //create output image
    imd.writeImg(outPath);


    return 0;
}