#define STB_IMAGE_IMPLEMENTATION
#include "./stb_image.h"

#include <iostream>
#include <fstream>
#include <vector>

typedef struct RGBA 
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
     * parses image into data given a path
     * @param in path
     *   path to image to be parsed
     * @return
     *   true if success false if nobt
     */
    bool parseImg(std::string path);

    /**
     * @brief 
     *  returns data 
     */
    std::vector<unsigned char> getData()
    { return data; }

    /**
     * @brief Get the Width object
     * 
     * @return int 
     */
    int getWidth()  { return w; }

    /**
     * @brief Get the Height object
     * 
     * @return int 
     */
    int getHeight() { return h; }

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
    RGBA getRGBA(int x, int y)
    {
        RGBA ret{-1,-1,-1,-1};
    
        //check bounds
        if (x > w || x < 0) return ret;
        if (y > h || y < 0) return ret;
        ret.A = 0;

        size_t pos = (y*w + x)*ch;
        ret.R = static_cast<int>(data[pos]);
        ret.G = static_cast<int>(data[pos+1]);
        ret.B = static_cast<int>(data[pos+2]);
        if (ch == 4)
            ret.A = static_cast<int>(data[pos+3]);

        return ret;
    }
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
int generateEnergyValue(imgData& imd, int x, int y);
