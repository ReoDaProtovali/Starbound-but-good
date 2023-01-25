#pragma once

#include <vector>
#include "Framework/Log.hpp"
#include <algorithm>
template<class T>
class Array2D {
public:
	Array2D() { width = 0; height = 0; data = std::vector<T>(); }
	Array2D(uint32_t p_width, uint32_t p_height) {
		data.resize(p_width * p_height, T());

		width = p_width;
		height = p_height;
	}
	Array2D(const Array2D<T>& other) {
		data = other.data;
		width = other.width;
		height = other.height;
	}
	Array2D(Array2D<T>&& other) noexcept {
		data = std::move(other.data);
		width = other.width;
		height = other.height;
	}
	Array2D<T> operator=(const Array2D<T>& other) {
		data = other.data;
		width = other.width;
		height = other.height;
		return *this;
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

	void append(T* p_data, size_t p_size) {
		if (p_size == 0) return;
		if (p_size % width != 0) {
			ERROR_LOG("Tried to append data of invalid size to existing 2d array. Make sure the width of what you're appending matches the destination.");
			return;
		}
		data.reserve(width * height);
		std::copy(&p_data[0], &p_data[p_size], std::back_inserter(data));
		LOG("2D Array appended from height " << height << " to height " << height + p_size / width);
		height += p_size / width;
	}

	void clear() {
		data.clear();
		data.shrink_to_fit();
	}
	size_t width;
	size_t height;
private:
	std::vector<T> data;
};
