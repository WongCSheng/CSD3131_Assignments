//#include "../shared/energy_map.h"
#include "../lib/energy.h"

// https://solarianprogrammer.com/2019/06/10/c-programming-reading-writing-images-stb_image-libraries/
// https://github.com/arrij46/Gradient-Edge-Detection/blob/main/Serialq1.cpp

#include <iostream>
#include <tuple>

//type declarations
using EnergyRow = std::vector<int>;
using EnergyMap = std::vector<EnergyRow>;

// y/x convention
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

// ---------- Graph Cut method ----------
// Node ID calculation based on x, y coords + width
int id(int i, int j, int W) { return i * W + j; }

// Reversion of Node ID, going back to a pixel
std::tuple<int, int> reverse_id(int u, int W) {
    // int i = u / W;   // row
    // int j = u % W;   // column
    return std::tuple<int, int>{
        u / W, u % W
    };
}

struct Edge {
    int to;        // the id of the node where it points
    long long cap; // remaining capacity
};

std::vector<Edge> edges;               // all edges, reverse stored right after each one
std::vector<std::vector<int>> adj;     // adj[u] = indices into edges struct for edges leaving u

// s and t are node IDs. By default they take the values W*H and W*H+1 since these two values are out
// of range for all possible node IDs in the graph. Setting them to 0 and 1 would technically be fine too
// but since that would shift up all the IDs in the image by 2, I would rather not confuse myself with the
// shifting and instead always leave them consistently at W*H and W*H+1
int s;                                 // source, a node connected by an inf-cap edge to first col's px in every row
int t;                                 // sink, a node w an inf-cap edge from last col's px in every row

// Build the Edges and Adjacency maps here based on the EnergyMap
void build_edge_adj(const imgData& imd, const EnergyMap& em) {
    // clear the globals here
    edges.clear();
    adj.clear();
}

struct seamPoint
{
    int energy;
    int prev_x;
};

std::vector<int> findSeam(const imgData& imd, const EnergyMap& em)
{
    // how to perform graph cut for seam finding?


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
// ---------- Graph Cut method ----------

// main takes 2 args, first is path to image, 2nd is number of seams, 3rd is output path
int main(int argc, char *argv[])
{
	std::cout << "hello world\n";
    //energy_map::test();
    //test_stb_included();

    // Step 1: Read image with stb_image
    // energy_map::Image image;
    // if (!energy_map::read_image(image, "../img/BroadwayTowerSeamCarvingA.png")) {
    //     exit(1);
    // }
    std::string imgpath = argv[1];
    imgData imd{};
    if (!imd.parseImg(imgpath))
    {
        std::cout << "Exiting..." <<std::endl;
        return 0;
    }

    // Step 2: Apply the energy filter to it
    EnergyMap em = buildEnergyMap(imd);

    // Step 3: Perform Graph Cut & Discard the removed seams
    for (int i=0; i < std::stoi(argv[2]); i++)
    {
        // build the edge and adjacency lists
        build_edge_adj(imd, em);

        // compute s and t
        // they can have arbitrary node IDs, but since 0 and 1 are taken, 
        // they take the first 2 out of bounds IDs (being W*H and W*H+1)
        s = imd.getWidth() * imd.getHeight();
        t = s + 1;

        // max flow, bfs, seam extraction
        std::vector<int> seam = findSeam(imd, em);

        // remove seam from image
        imd.removeSeam(seam);

        // rebuild the energy map for the next iteration
        em = buildEnergyMap(imd);
    }

    // Step 4: Output Image
    // where to output, target width, target height, target channels, bytes, 
    // if (test_img != nullptr) {
    //     energy_map::write_image("abc.png", image.img_width, image.img_height, image.img_channels, test_img);
    // }

    // free(test_img);
    // free_image(image);
    std::string imgout = argv[3];
    imd.writeImg(imgout);
}