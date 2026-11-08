#define STB_IMAGE_IMPLEMENTATION
#include "../lib/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../lib/stb_image_write.h"

#include "../shared/energy_map.h"

// https://solarianprogrammer.com/2019/06/10/c-programming-reading-writing-images-stb_image-libraries/

#include <iostream>

void test_stb_included() {
    const int w = 256, h = 256;
    unsigned char data[w * h * 3];
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++) {
            data[(y * w + x) * 3 + 0] = x;  // red increases left to right
            data[(y * w + x) * 3 + 1] = y;  // green increases top to bottom
            data[(y * w + x) * 3 + 2] = 128;
        }
    stbi_write_png("test.png", w, h, 3, data, w * 3);
}

int main(void) {
	std::cout << "hello world\n";
    //energy_map::test();
    //test_stb_included();

    // Step 1: Read image with stb_image
    int width, height, channels;
    unsigned char *img = stbi_load("../img/BroadwayTowerSeamCarvingA.png", &width, &height, &channels, 0);
    if (img == NULL) {
        printf("Error in loading the image\n");
        exit(1);
    }
    printf("Loaded image with a width of %dpx, a height of %dpx and %d channels\n", width, height, channels);

    // Step 2: Apply the energy filter to it
    size_t img_size = width * height * channels;
    unsigned char *sepia_img = (unsigned char *)malloc(img_size);
    if (sepia_img == NULL) {
        printf("Unable to allocate memory for the sepia image.\n");
        exit(1);
    }

    for (unsigned char *p = img, *pg = sepia_img; p != img + img_size; p += channels, pg += channels) {
        *pg       = (uint8_t)fmin(0.393 * *p + 0.769 * *(p + 1) + 0.189 * *(p + 2), 255.0);         // red
        *(pg + 1) = (uint8_t)fmin(0.349 * *p + 0.686 * *(p + 1) + 0.168 * *(p + 2), 255.0);         // green
        *(pg + 2) = (uint8_t)fmin(0.272 * *p + 0.534 * *(p + 1) + 0.131 * *(p + 2), 255.0);         // blue        
        if (channels == 4) {
            *(pg + 3) = *(p + 3);
        }
    }
    stbi_write_png("abc_sepia.png", width, height, channels, sepia_img, width * 3);

    // Step 3: Perform Graph Cut
    
    // Step 4: Discard the removed seams

    // Step 5: Output Image
    // where to output, target width, target height, target channels, bytes, 
    stbi_write_png("abc.png", width, height, channels, img, width * 3);

    stbi_image_free(img);
    free(sepia_img);
}