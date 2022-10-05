#ifndef PIXMAP_H
#define PIXMAP_H
#include <util/ext/glm/glm.hpp>
#include <iostream>
#include "util/Array2D.hpp"

/// A class that allows for simple modification of image data using the CPU.
class Pixmap
{
public:
	/** Default constructor. 
	* Width and height of 0px, uninitialized data array.
	*/
	Pixmap();
	/** Constructor with initialized width and height. 
	* By default, every pixel is set to black.
	* @param p_width, p_height - The width and height of the pixmap.
	*/
	Pixmap(unsigned int p_width, unsigned int p_height);

	unsigned int width = 0;
	unsigned int height = 0;

	/// Returns an RGBA vec4 based on a given x and y position.
	/// Attempting to get a pixel outside of the image's dimensions returns an uninitialized vec4.
	glm::vec4 getPixel(unsigned int p_x, unsigned int p_y);
	/// Sets a pixel at a given x and y based on the provided RGBA vec4.
	/// Attempting to set a pixel outside of the image's dimensions does nothing.
	void setPixel(unsigned int p_x, unsigned int p_y, glm::vec4 p_color);
	/// Sets the color of every pixel to opaque black.
	void clear();
	/// Fills the image with the supplied color.
	void fill(glm::vec4 p_color);
	/// @returns A pointer to the Pixmap data on the heap. Used by OpenGL textures.
	glm::vec4* getData();
	/// A function for testing purposes, prints every row and column of the pixel data to the console. Can lag.
	void logPixmap();

private:
	/// An array of pixels, where each pixel is an RGBA vec4, and every value is between 0 and 1.
	Array2D<glm::vec4> pixels;
};

#endif