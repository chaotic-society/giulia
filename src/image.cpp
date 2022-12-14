#include "image.h"

#define THEORETICA_LONG_DOUBLE_PREC
#include "theoretica/core/real_analysis.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

using namespace theoretica;
namespace th = theoretica;
using namespace giulia;


pixel* giulia::image::get_data() const {
	return (pixel*) &(data[0]);
}

pixel giulia::image::get_pixel(unsigned int i) const {
	return data[i];
}

pixel giulia::image::get_pixel(unsigned int i, unsigned int j) const {
	return data[width * j + i];
}

unsigned int giulia::image::get_width() const {
	return width;
}


unsigned int giulia::image::get_height() const {
	return height;
}

unsigned int giulia::image::get_size() const {
	return width * height;
}

pixel& giulia::image::operator[](unsigned int i) {
	return data[i];
}


int giulia::image::save(const std::string& filename) {
	
	int res = stbi_write_bmp(filename.c_str(), width, height, 3, (void*) get_data());
	return res ? 0 : -1;
}


void apply(image& img, std::function<pixel(pixel)> f) {

	for (size_t i = 0; i < img.get_size(); ++i)
		img[i] = f(img[i]);

}


void apply(image& img, std::vector<std::function<pixel(pixel)>> functions) {

	for (size_t i = 0; i < img.get_size(); ++i)
		for (int j = 0; j < functions.size(); ++j)
			img[i] = functions[j](img[i]);

}


void giulia::overwrite(image& img, real_t x, real_t y, pixel c) {

	const size_t w = img.get_width();
	const size_t h = img.get_height();
	const size_t size = h * w;
	const real_t aspect_ratio = w / (real_t) h;

	unsigned int i = (unsigned int) (w * x) + w * (unsigned int) (h * (1 - y));
	img[i] = c;
}


void giulia::negative(image& img) {

	for (size_t i = 0; i < img.get_size(); ++i) {
		const pixel p = img[i];
		img[i] = pixel(255 - p.r, 255 - p.g, 255 - p.b);
	}

}


void giulia::grayscale(image& img) {

	for (size_t i = 0; i < img.get_size(); ++i) {
		const pixel p = img[i];
		const unsigned char g = intensity(p) / SQRT3;
		img[i] = pixel(g, g, g);
	}

}


void giulia::gamma_correction(image& img, real_t gamma, real_t c) {

	for (size_t i = 0; i < img.get_size(); ++i) {
		const pixel p = img[i];
		img[i] = pixel(c * th::powf(p.r, gamma), c * th::powf(p.g, gamma), c * th::powf(p.b, gamma));
	}

}


void giulia::contrast(image& img, real_t a, unsigned char s) {

	for (size_t i = 0; i < img.get_size(); ++i) {
		const pixel p = img[i];
		img[i] = pixel(
			clamp(a * clamp((unsigned int) p.r - s, 0, 255) + s, 0, 255),
			clamp(a * clamp((unsigned int) p.g - s, 0, 255) + s, 0, 255),
			clamp(a * clamp((unsigned int) p.b - s, 0, 255) + s, 0, 255));
	}

}


void contrast_threshold(image& img, real_t t) {

	for (size_t i = 0; i < img.get_size(); ++i) {
		const pixel p = img[i];

		if(intensity(p) >= t)
			img[i] = pixel(255, 255, 255);
		else
			img[i] = pixel(0, 0, 0);
	}

}


void giulia::contrast_stretch(image& img,
		unsigned char min_in, unsigned char max_in,
		unsigned char min_out, unsigned char max_out) {

	unsigned char diff_in = max_in - min_in;
	unsigned char diff_out = max_out - min_out;

	for (size_t i = 0; i < img.get_size(); ++i) {
		const pixel p = img[i];
		img[i] = pixel(
			clamp(((real) p.r - min_in) * diff_out / diff_in + min_out, 0, 255),
			clamp(((real) p.g - min_in) * diff_out / diff_in + min_out, 0, 255),
			clamp(((real) p.b - min_in) * diff_out / diff_in + min_out, 0, 255));
	}

}


void giulia::log(image& img, real_t c) {

	for (size_t i = 0; i < img.get_size(); ++i) {
		const pixel p = img[i];
		img[i] = pixel(
			clamp(c * th::ln(real(p.r) + 1), 0, 255), 
			clamp(c * th::ln(real(p.g) + 1), 0, 255), 
			clamp(c * th::ln(real(p.b) + 1), 0, 255));
	}

}


pixel giulia::lerp(pixel P1, pixel P2, real_t interp) {
	return (P1 + (P2 - P1) * interp);
}


real_t giulia::intensity(pixel p) {
	return th::sqrt(square(p.r) + square(p.g) + square(p.b));
}


pixel giulia::contrast(pixel p, real_t value) {
	return p * th::powf(intensity(p), value);
}


pixel giulia::supersampling(
		real_t x, real_t y, global_state& state, draw_function draw, unsigned int order, real_t stepsize) {

	if(order == 1)
		return draw(x, y, state);

	if(order % 2 != 0)
		return pixel(0, 0, 0);

	if(stepsize == 0)
		stepsize = 0.25 / state["width"];

	// Grid pattern
	const real_t x1 = x + (stepsize);
	const real_t x2 = x + (stepsize * 3);
	const real_t y1 = y + (stepsize);
	const real_t y2 = y + (stepsize * 3);

	pixel p1 = supersampling(x1, y1, state, draw, order / 2, stepsize / 2.0);
	pixel p2 = supersampling(x1, y2, state, draw, order / 2, stepsize / 2.0);
	pixel p3 = supersampling(x2, y1, state, draw, order / 2, stepsize / 2.0);
	pixel p4 = supersampling(x2, y2, state, draw, order / 2, stepsize / 2.0);

	unsigned int r = (((unsigned int) p1.r) + p2.r + p3.r + p4.r) / 4;
	unsigned int g = (((unsigned int) p1.g) + p2.g + p3.g + p4.g) / 4;
	unsigned int b = (((unsigned int) p1.b) + p2.b + p3.b + p4.b) / 4;

	return pixel(r, g, b);
}
