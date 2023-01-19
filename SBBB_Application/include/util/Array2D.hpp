#pragma once

#ifndef ARRAY2D_H
#define ARRAY2D_H

#include <vector>

template<class T>
class Array2D {
public:
	Array2D() { width = 0; height = 0; data = std::vector<T>(); }
	Array2D(uint32_t p_width, uint32_t p_height) {
		data.resize(p_width * p_height, T());

		width = p_width;
		height = p_height;
	}
	
	T& operator()(int x, int y) {
		return data[((y * width) + x)];
	}
	T operator()(int x, int y) const {
		return data[((y * width) + x)];
	}
	void fill(T p_fillValue) {
		for (size_t i = 0; i < width * height; i++) {
			data[i] = p_fillValue;
		}
	}
	std::vector<T>& getData() {
		return data;
	}

	void resize(uint32_t p_width, uint32_t p_height) {
		data.resize(p_width * p_height, T());

		width = p_width;
		height = p_height;
	}
	void clear() {
		data.clear();
		data.shrink_to_fit();
		hasData = false;
	}
	size_t width;
	size_t height;
private:
	std::vector<T> data;
	bool hasData = false;
};

#endif