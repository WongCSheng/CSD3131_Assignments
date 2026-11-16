#pragma once

#include <iostream>
#include <fstream>
#include <vector>

// g++ -Wall -c ./lib/energy.cpp -o ./lib/energy.o

struct RGBA 
{
    int R;
    int G;
    int B;
    int A;
};

class imgData
{
private:
    int w;
    int h;
    int ch;
    std::vector<unsigned char> data;

public:
    /**
     * @brief 
     *  returns data 
     */
    std::vector<unsigned char> getData() const
    { return data; }

    /**
     * @brief Get the Width object
     * 
     * @return int 
     */
    int getWidth() const { return w; }

    /**
     * @brief Get the Height object
     * 
     * @return int 
     */
    int getHeight() const { return h; }

    /**
     * @brief 
     *  gets rgba value of pixel
     * @param x 
     *  x position of pixel
     * @param y 
     *  y position of pixel
     * @return RGBA 
     *  rgba value at pixel x,y of image
     */
    RGBA getRGBA(int x, int y) const
    {
        RGBA ret{-1,-1,-1,-1};
    
        //check bounds
        if (x >= w || x < 0) return ret;
        if (y >= h || y < 0) return ret;
        ret.A = 1;

        size_t pos = (y*w + x)*ch;
        ret.R = static_cast<int>(data[pos]);
        ret.G = static_cast<int>(data[pos+1]);
        ret.B = static_cast<int>(data[pos+2]);
        if (ch == 4)
            ret.A = static_cast<int>(data[pos+3]);

        return ret;
    }

    /**
     * @brief 
     * parses image into data given a path
     * @param in path
     *   path to image to be parsed
     * @return
     *   true if success false if nobt
     */
    bool parseImg(const std::string& path); 

    /**
     * @brief 
     *  removes a seam from imgdata. t
     * @param seam
     *  vector of ints. assumes one int for each row
     *  int wil be the x position of pixel to be removed
     *  at the row
     */
    void removeSeam(const std::vector<int>& seam);

    /**
     * @brief 
     *  writes imgdata to specified path
     * @param path 
     *  output
     * @return
     *  true if success, false otherwise
     */
    bool writeImg(const std::string& path) const;
};


/**
 * @brief 
 *  Get Energy Value of a pixel
 * @param imd
 *  Image data
 * @param x
 *  xpos of pixel
 * @param y
 *  ypos of pixel
 * @return 
 *  energy value of pixel
 */
int generateEnergyValue(const imgData& imd, int x, int y);
