#include "energy.h"

// https://avikdas.com/2019/05/14/real-world-dynamic-programming-seam-carving.html

bool imgData::parseImg(const std::string& path) 
{
    // check if exists
    std::ifstream f(path.c_str());

    if (!f.good())
    {
        std::cout << "Failed to read file" << std::endl;
        return false;
    }

    unsigned char* d = stbi_load(path.data(), &w, &h, &ch, 0);
    if (!d)
    {
        std::cout << "Failed to load image" << std::endl;
    }

    size_t imgsize = w * h * ch;
    data = std::vector<unsigned char>(d, d+imgsize);

    stbi_image_free(d);
    return true;
}

void imgData::removeSeam(const std::vector<int>& seam)
{
    std::vector<unsigned char> newdata((w-1)*h*ch);

    for (int i =0; i < h; i++)
    {
        int seampos = seam[i]; // get position of seam for this row
        for (int j =0; j < w; j++)
        {
            // skip if seam
            if (j == seampos) continue;

            // copy to new vector
            size_t pos = (i*w +j)*ch;
            newdata.push_back(data[pos]);
            newdata.push_back(data[pos+1]);
            newdata.push_back(data[pos+2]);
            if (ch == 4) { newdata.push_back(data[pos+3]); }
        }
    }

    w--;
    data = newdata;
}

bool imgData::writeImg(const std::string& path) const
{
    int stride = w*ch;

    if (stbi_write_png(path.data(), w, h, ch, data.data(), stride))
        return true;
    
    return false;
}

int generateEnergyValue(const imgData& imd, int x, int y)
{
    RGBA center = imd.getRGBA(x, y);
    RGBA left = imd.getRGBA(x-1, y);
    RGBA right = imd.getRGBA(x+1, y);
    RGBA up = imd.getRGBA(x, y-1);
    RGBA down = imd.getRGBA(x, y+1);

    const RGBA* xL = (left.A != -1)? &left : &center;
    const RGBA* xR = (right.A != -1)? &right : &center;

    int dxr = xL->R - xR->R;
    int dxg = xL->G - xR->G;
    int dxb = xL->B - xR->B;
    int dx = dxr*dxr + dxg*dxg + dxb*dxb;
    
    const RGBA* yU = (up.A != -1)? &up : &center;
    const RGBA* yD = (down.A != -1)? &down : &center;

    int dyr = yU->R - yD->R;
    int dyg = yU->G - yD->G;
    int dyb = yU->B - yD->B;
    int dy = dyr*dyr + dyg*dyg + dyb*dyb;

    return dx+dy;
}