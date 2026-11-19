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

cv::Mat computeEnergy(const cv::Mat& image);
std::vector<int> FindGreedySeam(const cv::Mat& energy, bool orientation);
cv::Mat removeSeam(const cv::Mat& image, const std::vector<int>& seam, bool orientation);

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

/**
 * @brief Load images from the img folder
 * 
 */
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
}

void drawSeams(cv::Mat& image, const std::vector<int>& seams, bool orientation, cv::Vec3b color = cv::Vec3b(0, 255, 0))
{
    int rows = image.rows;
    int cols = image.cols;

    if(orientation) // Vertical
    {
        for(int i = 0; i < rows; ++i)
        {
            image.at<cv::Vec3b>(i, seams[i]) = color;
        }
    }
    else // Horizontal
    {
        for(int i = 0; i < cols; ++i)
        {
            image.at<cv::Vec3b>(seams[i], i) = color;
        }
    }
}

void precomputeImages(std::vector<cv::Mat>& images, const cv::Mat& img, bool orientation = true)
{
    int maxSeams = orientation ? img.cols / 3 * 2 : img.rows / 3 * 2;
    cv::Mat carvedImg = img.clone();

    for(int i = 1; i <= maxSeams; ++i)
    {
        cv::Mat energy = computeEnergy(carvedImg);
        std::vector<int> seam = FindGreedySeam(energy, orientation);
        drawSeams(carvedImg, seam, orientation);
        images.push_back(carvedImg.clone());
        carvedImg = removeSeam(carvedImg, seam, orientation);
    }
}

#endif // CVHELPERFUNC_HPP