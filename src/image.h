#pragma once

// Image storage and saving

#include "common.h"
#include <string>


namespace giulia {


	// An RGB pixel
	struct pixel {
		
		unsigned char r;
		unsigned char g;
		unsigned char b;

		pixel() : r(0), g(0), b(0) {}

		pixel(unsigned char r, unsigned char g, unsigned char b)
			: r(r), g(g), b(b) {}

		inline pixel operator+(pixel other) {
			return pixel(r + other.r, g + other.g, b + other.b);
		}

		inline pixel operator-(pixel other) {
			return pixel(r - other.r, g - other.g, b - other.b);
		}

		inline pixel operator*(real_t scalar) {
			return pixel(r * scalar, g * scalar, b * scalar);
		}

		inline pixel operator/(real_t scalar) {
			return pixel(r / scalar, g / scalar, b / scalar);
		}
	};


	inline pixel lerp(pixel P1, pixel P2, real_t interp) {
		return (P1 + (P2 - P1) * interp);
	}


	struct image {

		public:

			image(unsigned int w, unsigned int h) : width(w), height(h) {

				if(data != nullptr)
					delete[] data;

				data = new pixel[w * h];
			}

			inline ~image() {
				// if(data != nullptr)
				// 	delete[] data;
			}

			inline pixel* get_data() const {
				return data;
			}

			inline pixel get_pixel(unsigned int i) const {
				return data[i];
			}

			inline pixel get_pixel(unsigned int i, unsigned int j) const {
				return data[width * j + i];
			}

			inline unsigned int get_width() const {
				return width;
			}


			inline unsigned int get_height() const {
				return height;
			}

			inline pixel& operator[](unsigned int i) {
				return data[i];
			}


		private:
			unsigned int width {1024};
			unsigned int height {1024};
			pixel* data {nullptr};

	};


	using draw_function = pixel(*)(real_t, real_t, global_state&);


	// Save an image to file in the BMP format
	// Returns 0 on success
	int save_image(std::string filename, unsigned int width, unsigned int height, pixel* data);

	// Save an image to file in the BMP format
	// Returns 0 on success
	int save_image(std::string filename, image img);

	// Pixel intensity
	real_t intensity(pixel p);

	// Apply contrast
	pixel contrast(pixel p, real_t value);

	// Supersampling Anti-aliasing with grid points
	pixel supersampling(
		real_t x, real_t y, global_state& state,
		draw_function draw, unsigned int order = 2, real_t stepsize = 0);

}