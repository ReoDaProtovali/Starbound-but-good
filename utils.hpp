#pragma once
#ifndef UTILS_H
#define UTILS_H

#include <SDL.h>
#include <string>
#include <iostream>
#include <fstream>
#include <iostream>
#include <chrono>
#include <ctime>
#include <algorithm>
#include <vector>
#include "glm/vec2.hpp"
#include <numeric>
constexpr auto SIXTY_TIMES_PER_SECOND = 0.01666666666f;
constexpr auto ONE_TIME_PER_SECOND = 0.001f;
namespace utils {
	inline float hireTimeInSeconds() {
		float t = (float)SDL_GetTicks();
		t *= 0.001f;
		return t;
	}
	inline std::string readFile(const char* filePath) {
		std::string content;
		std::ifstream fileStream(filePath, std::ios::in);

		if (!fileStream.is_open()) {
			std::cerr << "Could not read file " << filePath << ". File does not exist." << std::endl;
			return "";
		}

		std::string line = "";
		while (!fileStream.eof()) {
			std::getline(fileStream, line);
			content.append(line + "\n");
		}

		fileStream.close();
		return content;
	}
	inline glm::vec2 lerp(glm::vec2 v1, glm::vec2 v2, float t) {
		glm::vec2 offset = v1 - v2;
		return v1 + offset * t;
	}

	inline float averageVector(std::vector<float>& p_vec) {
		float total = 0.0f;
		for (int i = 0; i < p_vec.size(); i++) {
			total += p_vec[i];
		}
		return total / p_vec.size();
	}
	template <typename T>
	void logVector(T p_vec) {
		for (int i = 0; i < p_vec.size(); i++) {
			std::cout << p_vec[i] << ", ";
		}
		std::cout << std::endl;
	}
	template <typename T>
	T clamp(T v, T p_min, T p_max) {
		return std::min(std::max(v, p_min), p_max);
	}
}
struct fpsGauge {
	fpsGauge() {}
	int start = SDL_GetTicks();
	int elapsed;

	void startStopwatch() {
		start = SDL_GetTicks();
	}
	void stopStopwatch() {
		elapsed = SDL_GetTicks() - start;

	}
	float getSecondsElapsed() {
		return (float)elapsed / 1000.0f;
	}
};

#endif UTILS_H