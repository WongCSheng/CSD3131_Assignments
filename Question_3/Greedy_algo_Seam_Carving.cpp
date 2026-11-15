#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>

#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>

/**
 * @brief Computes the energy map of an image.
 * 
 * @param image 
 * @return cv::Mat 
 */
cv::Mat computeEnergy(const cv::Mat& image)
{
    cv::Mat gray, gradX, gradY, absgradX, absgradY, energy;

    // Convert to grayscale
    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(gray, gray, cv::Size(3, 3), 0);

    // Compute gradients using Sobel operator
    cv::Sobel(gray, gradX, CV_64F, 1, 0);
    cv::Sobel(gray, gradY, CV_64F, 0, 1);

    // Get the absolute values of the gradients
    cv::convertScaleAbs(gradX, absgradX);
    cv::convertScaleAbs(gradY, absgradY);

    // Combine the gradients to get the energy map
    cv::addWeighted(absgradX, 0.5, absgradY, 0.5, 0, energy);

    // Convert to double for further processing
    energy.convertTo(energy, CV_64F);

    return energy;
}

/**
 * @brief Using the greedy algorithm to find the seam.
 * Find and use the first minimum energy pixel in the first row, then for each subsequent row,
 * choose the pixel with the minimum energy among the three possible pixels
 * (directly below, below-left, and below-right) to form a seam.
 * 
 * @param energy 
 * @return std::vector<int> 
 */
std::vector<int> FindGreedySeam(const cv::Mat& energy, bool orientation)
{
    int rows = energy.rows;
    int cols = energy.cols;

    // Seam container
    std::vector<int> seam(rows);
    
    if(orientation)  // Vertical seam
    {
        int bestY = 0;
        float minEnergy = energy.at<double>(0, 0);
        for(int c = 1; c < cols; ++c)
        {
            if (energy.at<double>(0, c) < minEnergy)
            {
                minEnergy = energy.at<double>(0, c);
                bestY = c;
            }
        }
        seam[0] = bestY;

        for (int i = 1; i < rows; ++i)
        {
            int prevY = seam[i - 1];
            int minY = prevY;
            double minEnergy = energy.at<double>(i, prevY);

            // Check the left neighbor
            if (prevY > 0 && energy.at<double>(i, prevY - 1) < minEnergy)
            {
                minEnergy = energy.at<double>(i, prevY - 1);
                minY = prevY - 1;
            }

            // Check the right neighbor
            if (prevY < cols - 1 && energy.at<double>(i, prevY + 1) < minEnergy)
            {
                minEnergy = energy.at<double>(i, prevY + 1);
                minY = prevY + 1;
            }

            seam[i] = minY;
        }
    }
    else // Horizontal seam
    {

    }

    return seam;
}

/**
 * @brief Removes a seam from the image.
 * 
 * @param image 
 * @param seam 
 * @return cv::Mat 
 */
cv::Mat removeSeam(const cv::Mat& image, const std::vector<int>& seam)
{
    int rows = image.rows;
    int cols = image.cols;

    cv::Mat output(rows, cols - 1, image.type());

    for (int i = 0; i < rows; ++i)
    {
        int j = seam[i];

        // Copy the pixels before the seam
        if (j > 0)
        {
            image.row(i).colRange(0, j).copyTo(output.row(i).colRange(0, j));
        }

        // Copy the pixels after the seam
        if (j < cols - 1)
        {
            image.row(i).colRange(j + 1, cols).copyTo(output.row(i).colRange(j, cols - 1));
        }
    }

    return output;
}

/**
 * @brief Carves out a specified number of seams from the image.
 * 
 * @param image 
 * @param numSeams 
 * @return cv::Mat 
 */
cv::Mat carveWidth(const cv::Mat& image, int numSeams, bool orientation)
{
    cv::Mat carvedImage = image.clone();

    for (int i = 0; i < numSeams; ++i)
    {
        cv::Mat energy = computeEnergy(carvedImage);
        std::vector<int> seam = FindGreedySeam(energy, orientation);
        carvedImage = removeSeam(carvedImage, seam);
    }

    return carvedImage;
}

int main(int argc, char** argv)
{
    std::string imagePath = (argc > 1) ? argv[1] : "input.jpg";
    cv::Mat image = cv::imread(imagePath);

    if (image.empty()) {
        std::cerr << "Could not open or find the image: " << imagePath << std::endl;
        return -1;
    }


    int seamsToRemove = 1; // number of pixels to carve
    bool orientation = true; // true for vertical seam, false for horizontal seam

    std::cout << "Selected orientation (1 for vertical, 0 for horizontal): ";
    std::cin >> orientation;
    std::cout << "\nNumber of seams to remove: ";
    std::cin >> seamsToRemove;

    // Benchmark Greedy
    auto startGreedy = std::chrono::high_resolution_clock::now();
    cv::Mat greedyResult = carveWidth(image, seamsToRemove, orientation);
    auto endGreedy = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> greedyTime = endGreedy - startGreedy;

    std::cout << "Greedy Execution Time: " << greedyTime.count() << " ms\n";

    cv::imwrite("carved_greedy.jpg", greedyResult);

    std::cout << "Images saved as carved_greedy.jpg and carved_dp.jpg" << std::endl;
    return 0;
}