#pragma once

#ifndef ARRAY2D_H
#define ARRAY2D_H

template<class T>
class Array2D {
public:
	Array2D() { width = 0; height = 0; data = nullptr; }
	Array2D(int p_width, int p_height) {
		data = new T[p_width * p_height];
		width = p_width;
		height = p_height;
	}
	Array2D(int p_width, int p_height, T p_defaultValue) {
		data = new T[p_width * p_height];
		for (int i = 0; i < p_width * p_height; i++) {
			data[i] = p_defaultValue;
		}
		width = p_width;
		height = p_height;
	}
	
	T& operator()(int x, int y) {
		return data[(y * width) + x];
	}
	T operator()(int x, int y) const {
		return data[(y * width) + x];
	}
	void fill(T p_fillValue) {
		for (int i = 0; i < width * height; i++) {
			data[i] = p_fillValue;
		}
	}
	T* getData() {
		return data;
	}
	//~Array2D() {
	//	delete[] data;
	//}
	unsigned int width;
	unsigned int height;
private:
	T* data;
};

#endif