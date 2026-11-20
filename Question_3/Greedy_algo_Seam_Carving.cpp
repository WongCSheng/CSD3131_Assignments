/**
 * @file Greedy_algo_Seam_Carving.cpp
 * @author Wong Chong Sheng (w.chongsheng@digipen.edu)
 * @brief Seam carving using greedy algorithm.
 *      This implementation uses OpenCV to compute the energy map and find seams in an image.
 *      The program reads an image, computes its energy map, finds the seam with the least energy,
 *      and removes it from the image. The process can be repeated to remove multiple seams.
 *      The user can specify the number of seams to remove and the orientation (vertical or horizontal).
 * @date 2026-11-17
 * 
 * @copyright Copyright (c) 2026
 * 
 */

// OpenCV includes
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

// Standard Library
#include <vector>
#include <string>

// Custom helper functions
#include "CVHelperFunc.hpp"
//#include "STBHelperFunc.hpp"

/**
 * @brief Computes the energy map of an image. Using OpenCV lib
 * 
 * @param image 
 * @return cv::Mat 
 */
cv::Mat computeEnergy(const cv::Mat& image)
{
    cv::Mat gray, gaus, gradX, gradY, energy;

    // Convert to grayscale
    cv::GaussianBlur(image, gaus, cv::Size(3, 3), 0);
    cv::cvtColor(gaus, gray, cv::COLOR_BGR2GRAY);

    // Compute gradients using Sobel operator
    cv::Sobel(gray, gradX, CV_64F, 1, 0);
    cv::Sobel(gray, gradY, CV_64F, 0, 1);

    // Add weight
    cv::magnitude(gradX, gradY, energy);

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
    
    if(orientation)  // Vertical seam
    {
        // Seam container
        std::vector<int> seam(rows);
        
        int bestY = 0;
        float minEnergy = energy.at<double>(0, 0);
        // Find the minimum energy pixel in the first row
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
            // Store energy of the pixel directly below the previous pixel
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

        return seam;
    }
    else // Horizontal seam
    {
        // Seam container
        std::vector<int> seam(cols);

        int bestX = 0;
        float minEnergy = energy.at<double>(0, 0);
        // Find the minimum energy pixel in the first column
        for(int r = 1; r < rows; ++r)
        {
            if (energy.at<double>(r, 0) < minEnergy)
            {
                minEnergy = energy.at<double>(r, 0);
                bestX = r;
            }
        }
        seam[0] = bestX;

        for (int j = 1; j < cols; ++j)
        {
            int prevX = seam[j - 1];
            int minX = prevX;
            // Store energy of the pixel directly to the right of the previous pixel
            double minEnergy = energy.at<double>(prevX, j);

            // Check the upper neighbor
            if (prevX > 0 && energy.at<double>(prevX - 1, j) < minEnergy)
            {
                minEnergy = energy.at<double>(prevX - 1, j);
                minX = prevX - 1;
            }

            // Check the lower neighbor
            if (prevX < rows - 1 && energy.at<double>(prevX + 1, j) < minEnergy)
            {
                minEnergy = energy.at<double>(prevX + 1, j);
                minX = prevX + 1;
            }

            seam[j] = minX;
        }

        return seam;
    }

    throw std::runtime_error("seams cannot be initialised");
}

/**
 * @brief Removes a seam from the image.
 * 
 * @param image 
 * @param seam 
 * @return cv::Mat 
 */
cv::Mat removeSeam(const cv::Mat& image, const std::vector<int>& seam, bool orientation)
{
    int rows = image.rows;
    int cols = image.cols;

    if(orientation) // Vertical seams
    {
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
    else // Horizontal seams
    {
        cv::Mat output(rows - 1, cols, image.type());

        for(int i = 0; i < cols; ++i)
        {
            int j = seam[i];

            // Copy the pixels before the seam
            if(j > 0)
            {
                image.col(i).rowRange(0, j).copyTo(output.col(i).rowRange(0, j));
            }

            // Copy the pixels after the seam
            if(j < rows - 1)
            {
                image.col(i).rowRange(j + 1, rows).copyTo(output.col(i).rowRange(j, rows - 1));
            }
        }

        return output;
    }

    throw std::runtime_error("Cannot find seams to remove");
}

/**
 * @brief Carves out a specified number of seams from the image.
 * 
 * @param image 
 * @param numSeams 
 * @return cv::Mat 
 */
cv::Mat benchmarkSeamCarving(const cv::Mat& image, int numSeams, bool orientation)
{
    cv::Mat carvedImage = image.clone();

    for (int i = 0; i < numSeams; ++i)
    {
        cv::Mat energy = computeEnergy(carvedImage);
        std::vector<int> seam = FindGreedySeam(energy, orientation);
        carvedImage = removeSeam(carvedImage, seam, orientation);
    }

    return carvedImage;
}

// Container of images from img folder
std::vector<OpenCVImageData> CVimages;
std::vector<imgData> stbImages;

// Container of removed seams
std::vector<cv::Mat> cachedImg(0);

int main(int argc, char* argv[])
{
    if(argc >= 5)
    {
        return runHeadless(argv);
    }

    cv::String displayWindow = "Image Viewer"; // Display window name

    loadImages();

    int seamsToRemove = 0; // number of pixels to carve
    bool orientation = true; // true for vertical seam, false for horizontal seam

    char entered_key = 0;
    std::cout << "Enter key: 'e' to load images, 'b' to benchmark greedy algorithm, 'q' to exit" << std::endl;
    while (std::cin >> entered_key)
    {
        // To quit program
        if (entered_key == 'q' || entered_key == 'Q')
        {
            std::cout << "Exiting program." << std::endl;
            break;
        }

        // To benchmark time taken for greedy algorithm
        if(entered_key == 'b' || entered_key == 'B')
        {
            int iter = 1;
            for(const auto&[path, name] : CVimages)
            {
                std::cout << iter++ << ". " << name << '\n';
            }
            std::cout << "Select which image to load: ";
            int selectedImage = 1;
            std::cin >> selectedImage;

            // Create a window and show image
            cv::namedWindow(displayWindow, cv::WindowFlags::WINDOW_NORMAL);
            cv::Mat image = cv::imread(CVimages[selectedImage - 1].path);
            cv::imshow(displayWindow, image);

            std::cout << "Selected orientation (1 for vertical, 0 for horizontal): ";
            std::cin >> orientation;

            int maximumAllowedSeams = orientation ? (image.cols / 2) : (image.rows / 2); // Limit to half the size of the original image
            std::cout << "Enter number of seams to remove (Maximum number is " << maximumAllowedSeams << "): ";
            std::cin >> seamsToRemove;

            // Benchmark Greedy
            auto startGreedy = std::chrono::high_resolution_clock::now();
            cv::Mat greedyResult = benchmarkSeamCarving(image, seamsToRemove, orientation);
            auto endGreedy = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> greedyTime = endGreedy - startGreedy;

            std::cout << "Greedy Execution Time: " << greedyTime.count() << " ms\n";
            std::cout << "Showing carved image using Greedy algorithm..." << std::endl;

            cv::imshow("Greedy Carved Image", greedyResult);
            cv::imwrite("output/edited_" + CVimages[selectedImage - 1].name, greedyResult);

            std::cout << "Images saved as carved_greedy.jpg and carved_dp.jpg\n";
        }

        // View image
        if(entered_key == 'e' || entered_key == 'E')
        {
            int iter = 1;
            for(const auto&[path, name] : CVimages)
            {
                std::cout << iter++ << ". " << name << '\n';
            }
            std::cout << "Select which image to load: ";
            int selectedImage = 0;
            std::cin >> selectedImage;

            // Load the selected image
            if (selectedImage > 0 && selectedImage <= CVimages.size())
            {
                cv::Mat image = cv::imread(CVimages[selectedImage - 1].path);
                if (image.empty())
                {
                    std::cerr << "Failed to load image: " << CVimages[selectedImage - 1].path << std::endl;
                    break;
                }

                std::cout << "Selected orientation (1 for vertical, 0 for horizontal): ";
                std::cin >> orientation;

                // Set the maximum number of seams to remove based on orientation
                int maxSeams = orientation ? (image.cols / 3 * 2) : (image.rows / 3 * 2);
                std::cout << "Maximum amount of allowed seams: " << maxSeams / 2 << std::endl;
                int prevSeamsToRemove = -1;

                // Create a window and show image
                cv::namedWindow(displayWindow, cv::WindowFlags::WINDOW_AUTOSIZE);

                // Trackbar window
                cv::String trackbarWindow = "Seam control";
                cv::namedWindow(trackbarWindow, cv::WindowFlags::WINDOW_GUI_EXPANDED);
                        
                // Trackbar settings
                cv::String trackbarName = "Seams to remove";
                cv::createTrackbar(trackbarName, trackbarWindow, nullptr, 500, onTrackbar, &seamsToRemove);
                cv::setTrackbarMax(trackbarName, trackbarWindow, maxSeams);
                cv::setTrackbarPos(trackbarName, trackbarWindow, 0);

                cv::Mat newImage = image.clone();
                cachedImg.push_back(image.clone());

                // Pre-compute all seams carving
                precomputeImages(cachedImg, newImage, orientation);
                std::cout << "Drag slider in window. Press 's' to save, 'q' to return.\n";

                while(true)
                {
                    if(seamsToRemove != prevSeamsToRemove) // Remove seams
                    {
                        cv::imshow(displayWindow, cachedImg[seamsToRemove]);
                        prevSeamsToRemove = seamsToRemove;
                    }

                    // Wait 10ms for use to press a key
                    entered_key = cv::waitKey(10);
                    if (entered_key == 'q' || entered_key == 'Q')
                    {
                        cachedImg.clear();
                        cv::destroyWindow(displayWindow);
                        cv::destroyWindow(trackbarWindow);
                        break;
                    }
                    // Exit if the user closes the window
                    if (cv::getWindowProperty(displayWindow, cv::WND_PROP_VISIBLE) < 1
                        || cv::getWindowProperty(trackbarWindow, cv::WND_PROP_VISIBLE) < 1)
                    {
                        return 0;
                    }
                }
            }
        }
        
        entered_key = -1; // Reset entered_key to avoid reloading the same image
        std::cout << "Enter key: 'e' to load images, 'b' to benchmark greedy algorithm, 'q' to exit" << std::endl;
    }

    cv::destroyAllWindows();

    return 0;
}