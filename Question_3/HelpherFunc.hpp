/**
 * @file HelpherFunc.hpp
 * @author Wong Chong Sheng (w.chongsheng@digipen.edu)
 * @brief Extra functions to assist in file management and user input.
 * @date 17-11-2026
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#pragma once

#include <iostream>
#include <filesystem>
#include <chrono>

Imagefile image = {"", "", 0};

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

cv::ButtonCallback buttonCallback = [](int state, void* userdata)
{
    std::vector<std::string> imageFiles;
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
        if(entry.is_regular_file())
        {
            imageFiles.push_back(entry.path().string());
            std::cout << iter << ". " << entry.path().filename().string() << std::endl;
            iter++;
        }
    }
    std::cout << "Input which image to use." << std::endl;
    std::cin >> selectedImageIndex;

    while(selectedImageIndex < 1 || selectedImageIndex > imageFiles.size())
    {
        std::cerr << "Invalid selection. Please select a number between 1 and " << imageFiles.size() << std::endl;
        std::cin >> selectedImageIndex;
    }

    image.path = imageFiles[selectedImageIndex - 1];
};