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
#include <util/ext/glm/glm.hpp>
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
	void logVector(T p_vec) { // careful what you toss in here, must be cout-able lol
		for (int i = 0; i < p_vec.size(); i++) {
			std::cout << p_vec[i] << ", ";
		}
		std::cout << "\n";
	}
	template <typename T>
	T clamp(T v, T p_min, T p_max) {  // make sure whatever you're passing in has a > operator
		return std::min(std::max(v, p_min), p_max);
	}
	inline bool pointCollidingRect(glm::vec2 p_pointPos, glm::vec2 p_rectPos, glm::vec2 p_rectDim) {
		glm::vec2 relativePos = p_rectPos - p_pointPos;
		//printf("%f, %f \n", relativePos.x, relativePos.y);
		if ((relativePos.x <= p_rectDim.x) && (relativePos.y <= p_rectDim.y) && (relativePos.x > 0.0f) && (relativePos.y > 0.0f)) {
			return true;
		};
		return false;
	}
}
// A utility class that can provide fps readouts, and also just measure times.
struct fpsGauge {
	fpsGauge() { elapsed = 0; }
	int start = SDL_GetTicks();
	int elapsed;
	std::vector<float> frametimeBuffer;

	void startStopwatch() {
		start = SDL_GetTicks();
	}
	void stopStopwatch() {
		elapsed = SDL_GetTicks() - start;

	}
	float getSecondsElapsed() {
		return (float)elapsed / 1000.0f;
	}
	/** Adds a new frametime to the gauge's buffer, removes any old ones.
	* 
	* @param maxBufferLength - The maximum length of the buffer before it starts removing old values
	*/
	void update(int maxBufferLength) {
		stopStopwatch();
		startStopwatch();
		frametimeBuffer.push_back(getSecondsElapsed());
		if (frametimeBuffer.size() > (size_t)maxBufferLength) { // quarter second fps display buffer
			frametimeBuffer.erase(frametimeBuffer.begin());
		}
	}

};

// just a really basic timer, nothing fancy
struct Timer {
	std::chrono::time_point<std::chrono::high_resolution_clock> start;
	Timer() {
		start = std::chrono::high_resolution_clock::now();
	}
	~Timer() {
		std::chrono::duration<double, std::milli> duration = std::chrono::high_resolution_clock::now() - start;
		std::cout << "The simple timer took " << duration.count() << "ms!" << std::endl;
	}
};
#endif UTILS_H