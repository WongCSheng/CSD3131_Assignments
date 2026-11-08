#define STB_IMAGE_IMPLEMENTATION
#include "../lib/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../lib/stb_image_write.h"

#include <iostream>

int main(void) {
	std::cout << "hello world\n";

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