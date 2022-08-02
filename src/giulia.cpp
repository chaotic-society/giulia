
#include "common.h"
#include "image.h"
#include "fractals.h"

#include <fstream>
#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace giulia;


// Setup rendering variables
void setup(global_state& state) {

	state["scale.x"] = 3.5;
	state["scale.y"] = 3.5;

	state["translation.x"] = 0;
	state["translation.y"] = 0;
}


// Draw a pixel at the specified x and y
pixel draw(real_t x, real_t y, global_state& state) {

	// Coordinate transformation
	x = (x - state["translation.x"]) * state["scale.x"];
	y = (y - state["translation.y"]) * state["scale.y"];

	return draw_julia(x, y, -0.76, 0.1482, 2500);
}


int main(int argc, char const *argv[]) {

	// Changing seed
	int seed = time(nullptr);
	srand(seed);

	// Image width and height
	unsigned int width = 1024;
	unsigned int height = 1024;

	// Set width and height from terminal
	if(argc >= 4) {
		width = std::atoi(argv[2]);
		height = std::atoi(argv[3]);
	}

	// Aspect ratio of the image
	real_t aspect_ratio = width / (real_t) height;

	// Total size of the image
	unsigned int size = width * height;

	// Output file name
	std::string filename = "giulia.bmp";

	// Global state variables
	global_state state;
	state["seed"] = seed;
	state["width"] = width;
	state["height"] = height;
	state["aspect_ratio"] = aspect_ratio;
	state["iteration"] = 0;

	// Read file name from terminal
	if(argc >= 2)
		filename = std::string(argv[1]);

	// Image data
	image img = image(width, height);

	// Setup global state before rendering
	setup(state);

	std::cout << "Rendering image ..." << std::endl;

	// Render the image
	for (int i = 0; i < size; ++i) {

		state["iteration"] += 1;
		
		// Convert index to pixel location
		real_t x = ((i % width) / (real_t) (width - 1)) - 0.5;
		real_t y = ((((size - i) / (real_t) width) / (real_t) height) - 0.5) / aspect_ratio;

		// The origin corresponds to the center of the image

		// Draw pixel
		img[i] = draw(x, y, state);
	}

	// Save the result to file
	std::cout << "Saving image as " << filename << " ..." << std::endl;
	int res = save_image(filename, img);

	if(res)	std::cout << "Failed writing to file" << res << std::endl;
	else	std::cout << "Successfully saved image" << std::endl;

	return res;
}
