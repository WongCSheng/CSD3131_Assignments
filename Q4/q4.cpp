#include "../shared/energy_map.h"

// https://solarianprogrammer.com/2019/06/10/c-programming-reading-writing-images-stb_image-libraries/

#include <iostream>

//void test_stb_included() {
//    const int w = 256, h = 256;
//    unsigned char data[w * h * 3];
//    for (int y = 0; y < h; y++)
//        for (int x = 0; x < w; x++) {
//            data[(y * w + x) * 3 + 0] = x;  // red increases left to right
//            data[(y * w + x) * 3 + 1] = y;  // green increases top to bottom
//            data[(y * w + x) * 3 + 2] = 128;
//        }
//    stbi_write_png("test.png", w, h, 3, data, w * 3);
//}

int main(void) {
	std::cout << "hello world\n";
    //energy_map::test();
    //test_stb_included();

    // Step 1: Read image with stb_image
    energy_map::Image image;
    if (!energy_map::read_image(image, "../img/BroadwayTowerSeamCarvingA.png")) {
        exit(1);
    }

    // Step 2: Apply the energy filter to it
    unsigned char* test_img = energy_map::wn_test(image.img_ptr, image.img_width, image.img_height, image.img_channels);

    // Step 3: Perform Graph Cut
    
    // Step 4: Discard the removed seams

    // Step 5: Output Image
    // where to output, target width, target height, target channels, bytes, 
    if (test_img != nullptr) {
        energy_map::write_image("abc.png", image.img_width, image.img_height, image.img_channels, test_img);
    }

    free(test_img);
    free_image(image);
}