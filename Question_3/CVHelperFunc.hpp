/**
 * @file CVHelperFunc.hpp
 * @author Wong Chong Sheng (w.chongsheng@digipen.edu)
 * @brief Helper functions for OpenCV image processing, including mouse and button callbacks for user interaction.
 * @date 18-11-2026
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef CVHELPERFUNC_HPP
#define CVHELPERFUNC_HPP

#include <iostream>
#include <filesystem>
#include <chrono>

#include "../lib/energy.h"

// Data structure for storing image using OpenCV
struct OpenCVImageData
{
    std::string path;
    std::string name;
};

// Containers for image data from img folder
extern std::vector<OpenCVImageData> CVimages;

/**
 * @brief OpenCV trackbar callback. sets the number of seams to remove to the
 *      position of the bar.
 * 
 * @param pos 
 * @param userdata 
 */
void onTrackbar(int pos, void* userdata)
{
    *static_cast<int*>(userdata) = pos;
}

void loadImages()
{
    std::filesystem::path image_Folder = std::filesystem::current_path().parent_path() / "img";
    std::cout << "Image folder path: " << image_Folder << std::endl;
    if(std::filesystem::is_empty(image_Folder))
    {
        std::cerr << "Image folder is empty. Please add images to the folder: " << image_Folder << std::endl;
        exit(EXIT_FAILURE);
    }

    for (const auto& entry : std::filesystem::directory_iterator(image_Folder))
    {
        if(entry.is_regular_file() && cv::haveImageReader(entry.path().string()))
        {
            OpenCVImageData imageData;
            imageData.path = entry.path().string();
            imageData.name = entry.path().filename().string();
            CVimages.push_back(imageData);
        }
    }
};

#endif // CVHELPERFUNC_HPP