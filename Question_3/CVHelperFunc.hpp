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
#include <numeric>
#include <chrono>

#include "../lib/energy.h"

// Forward Declaration
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
    std::filesystem::path image_Folder;
    if(std::filesystem::current_path().string().find("build") != std::string::npos)
    {
        image_Folder = std::filesystem::current_path().parent_path().parent_path() / "img";
    }
    else
    {
        image_Folder = std::filesystem::current_path().parent_path() / "img";
    }
    std::cout << "Image folder path: " << image_Folder << std::endl;

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

/**
 * @brief Draws the seam thats to be removed
 * 
 * @param image 
 * @param seams 
 * @param orientation 
 * @param color 
 */
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

/**
 * @brief Calculate and generate the images that is do be seam carved.
 *      Removes the need to do on the spot calculations when the trackbar is moved, thus
 *      reducing laggy-ness.
 * 
 * @param images 
 * @param img 
 * @param orientation 
 */
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

/**
 * @brief For headless executable when running in main.cpp
 * 
 * @param argv 
 * @return int 
 */
int runHeadless(char* argv[])
{
    cv::Mat image = cv::imread(argv[1]);
    if (image.empty()) { std::cerr << "Failed to load " << argv[1] << "\n"; return 1; }

    int seams = std::stoi(argv[2]);
    std::string out = argv[3];
    bool vertical = std::stoi(argv[4]) != 0;
    std::filesystem::create_directories(out);

    // Only deals with vertical to be in line with q1
    cv::Mat work = vertical ? image.clone() : cv::Mat(image.t());

    cv::Mat energy8;
    cv::normalize(computeEnergy(work), energy8, 0, 255, cv::NORM_MINMAX, CV_8U);

    cv::Mat seamVis = work.clone();
    std::vector<std::vector<int>> origin(work.rows, std::vector<int>(work.cols));
    for (auto& row : origin) std::iota(row.begin(), row.end(), 0);

    auto t0 = std::chrono::steady_clock::now();
    for (int i = 0; i < seams && work.cols > 1; ++i)
    {
        std::vector<int> seam = FindGreedySeam(computeEnergy(work), true);
        for (int r = 0; r < work.rows; ++r)
        {
            seamVis.at<cv::Vec3b>(r, origin[r][seam[r]]) = cv::Vec3b(0, 255, 0);
            origin[r].erase(origin[r].begin() + seam[r]);
        }
        work = removeSeam(work, seam, true);
    }
    double ms = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - t0).count();

    if (!vertical) { work = work.t(); seamVis = seamVis.t(); energy8 = energy8.t(); }

    cv::imwrite(out + "/out.png", work);
    cv::imwrite(out + "/gradient.png", energy8);
    cv::imwrite(out + "/seams.png", seamVis);
    std::cout << "Total time: " << ms << " msec" << std::endl;
    return 0;
}

#endif // CVHELPERFUNC_HPP