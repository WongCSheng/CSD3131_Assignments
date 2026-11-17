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
    cv::Mat image;
};

struct mouse
{
    int x = 0, y = 0;
};

// Containers for image data from img folder
extern std::vector<OpenCVImageData> CVimages;
extern mouse mouseData;

/**
 * @brief Mouse callback function for opencv window.
 * 
 */
cv::MouseCallback mouseCallback = [](int event, int x, int y, int flags, void* userdata)
{
    if (event == cv::EVENT_LBUTTONDOWN)
    {
        // Mouse button pressed, do something...
    }
};

void loadImages()
{
    std::filesystem::path image_Folder = std::filesystem::current_path().parent_path() / "img";
    std::cout << "Image folder path: " << image_Folder << std::endl;
    short iter = 1;
    if(std::filesystem::is_empty(image_Folder))
    {
        std::cerr << "Image folder is empty. Please add images to the folder: " << image_Folder << std::endl;
        exit(EXIT_FAILURE);
    }

    int selectedImageIndex = -1;
    for (const auto& entry : std::filesystem::directory_iterator(image_Folder))
    {
        if(entry.is_regular_file() && cv::haveImageReader(entry.path().string()))
        {
            OpenCVImageData imageData;
            imageData.path = entry.path().string();
            CVimages.push_back(imageData);
            std::cout << iter << ". " << entry.path().filename().string() << std::endl;
            iter++;
        }
    }
    std::cout << "Input which image to use." << std::endl;
    std::cin >> selectedImageIndex;

    while(selectedImageIndex < 1 || selectedImageIndex > CVimages.size())
    {
        std::cerr << "Invalid selection. Please select a number between 1 and " << CVimages.size() << std::endl;
        std::cin >> selectedImageIndex;
    }

    OpenCVImageData imageData;
    imageData.path = CVimages[selectedImageIndex - 1].path;
    CVimages.push_back(imageData);
};

#endif // CVHELPERFUNC_HPP