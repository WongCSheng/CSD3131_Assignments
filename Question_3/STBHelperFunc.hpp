

#ifndef STBHELPERFUNC_HPP
#define STBHELPERFUNC_HPP

#include <iostream>
#include <filesystem>

#include "../lib/energy.h"
#include "../shared/energy_map.h"

// Container of images from img folder using stb_image data structure
extern std::vector<imgData> stbImages;

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
        if(entry.is_regular_file())
        {
            imgData imageData;
            if(imageData.parseImg(entry.path().string()))
            {
                stbImages.push_back(imageData);
                std::cout << iter << ". " << entry.path().filename().string() << std::endl;
                iter++;
            }
            else
            {
                std::cerr << "Failed to read image: " << entry.path().string() << std::endl;
            }
        }
    }
}

#endif