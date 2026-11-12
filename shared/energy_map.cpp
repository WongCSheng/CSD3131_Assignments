#include "energy_map.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../lib/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../lib/stb_image_write.h"

#include <iostream>

void energy_map::test() {
	std::cout << "abc\n";
}

bool energy_map::read_image(Image& image, const char* imagePath) {
    image.img_ptr = stbi_load(imagePath, &image.img_width, &image.img_height, &image.img_channels, 0);
    if (image.img_ptr == NULL) {
        printf("Error in loading the image\n");
        return false;
    }

    printf("Loaded image with a width of %dpx, a height of %dpx and %d channels\n", image.img_width, image.img_height, image.img_channels);
    return true;
}

void energy_map::free_image(Image& image) {
    if (image.img_ptr != nullptr) {
        stbi_image_free(image.img_ptr);
    }
}

void energy_map::write_image(const char* filename, int target_w, int target_h, int target_channels, unsigned char *bytes) {
    stbi_write_png(filename, target_w, target_h, target_channels, bytes, target_w * 3);
}


// placeholder fn DO NOT USE
unsigned char* energy_map::wn_test(unsigned char *img, int w, int h, int channels) {
    std::cout << w << ", " << h << ", " << channels << std::endl;
    size_t img_size = w * h * channels;
    unsigned char *energy_map = (unsigned char *)malloc(img_size);
    if (energy_map == NULL) {
        printf("Unable to allocate memory for the energy map.\n");
        return nullptr;
    }

    for (unsigned char *p = img, *pg = energy_map; p != img + img_size; p += channels, pg += channels) {
        *pg       = (uint8_t)fmin(0.393 * *p + 0.769 * *(p + 1) + 0.189 * *(p + 2), 255.0);         // red
        *(pg + 1) = (uint8_t)fmin(0.349 * *p + 0.686 * *(p + 1) + 0.168 * *(p + 2), 255.0);         // green
        *(pg + 2) = (uint8_t)fmin(0.272 * *p + 0.534 * *(p + 1) + 0.131 * *(p + 2), 255.0);         // blue        
        if (channels == 4) {
            *(pg + 3) = *(p + 3);
        }
    }

    return energy_map;
}