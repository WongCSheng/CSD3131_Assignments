#ifndef ENERGY_MAP
#define ENERGY_MAP

namespace energy_map {
	struct Image {
		unsigned char * img_ptr = nullptr;
		int img_width = 0;
		int img_height = 0;
		int img_channels = 0;
	};

	void test();

	bool read_image(Image& image, const char* imagePath);
	void free_image(Image& image);
	void write_image(const char* filename, int target_w, int target_h, int target_channels, unsigned char *bytes); // this assumes png

	unsigned char* wn_test(unsigned char *img, int w, int h, int channels); // DO NOT USE THIS THIS IS A PLACEHOLDER
}

#endif