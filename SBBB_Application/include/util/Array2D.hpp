#pragma once

#ifndef ARRAY2D_H
#define ARRAY2D_H

#include <vector>

template<class T>
class Array2D {
public:
	Array2D() { width = 0; height = 0; data = std::vector<T>(); }
	Array2D(size_t p_width, size_t p_height, T p_defaultValue) {
		data.reserve(p_width * p_height);
		for (int i = 0; i < p_width * p_height; i++) {
			data.push_back(p_defaultValue);
		}

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
	size_t width;
	size_t height;
private:
	std::vector<T> data;
	bool hasData = false;
};

#endif