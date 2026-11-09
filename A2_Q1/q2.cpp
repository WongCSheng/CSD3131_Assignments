#include "../lib/energy.h"

using EnergyMap = std::vector<std::vector<int>>;

EnergyMap buildEnergyMap(const imgData& imd)
{
    int w = imd.getWidth();
    int h = imd.getHeight();

    EnergyMap em = EnergyMap(h*w);

    for (int i=0; i < h; i++)
    {
        for (int j=0; j < w; j++)
        {
            em[i][j] = generateEnergyValue(imd, i,j);
        }
    }

    return em;
}


int main()
{
    std::string imgpath = "./img/BroadwayTowerSeamCarvingA.png";

    imgData imd{};
    if (!imd.parseImg(imgpath))
    {
        std::cout << "Exiting..." <<std::endl;
        return 0;
    }

    EnergyMap em = buildEnergyMap(imd);


    return 0;
}