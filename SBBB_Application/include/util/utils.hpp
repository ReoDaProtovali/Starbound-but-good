#pragma once


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
#include <GameConstants.hpp>

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
	// wraps float values to the range [0, r] 
	inline float fwrapUnsigned(float x, float r) {
		float aR = fabs(r); // The wrapping point should always be positive or else weird things happen
		// Piecewise fmod transformation function
		return x < 0 ? (fmodf(x, aR) + r) : fmodf(x, aR);
	}
	inline unsigned int modUnsigned(int x, unsigned int d) {
		if (x >= 0) {
			return x % d;
		}
		else {
			return d - 1 - abs(x % (int)d);
		}
	}
	inline int gridFloor(int x, unsigned int d) {
		return (x < 0) ? x / (int)d - 1 : x / (int)d;
	}
	// scuffed as hell
	inline glm::ivec4 frameToChunkCoords(glm::vec4 p_frame, int p_chunkSizeMultiplier = 1) {
		int x1 = utils::gridFloor((int)p_frame.x / p_chunkSizeMultiplier, CHUNKSIZE);
		int y1 = utils::gridFloor((int)p_frame.y / p_chunkSizeMultiplier, CHUNKSIZE) + 1;
		int x2 = utils::gridFloor((int)p_frame.z / p_chunkSizeMultiplier, CHUNKSIZE) + 1;
		int y2 = utils::gridFloor((int)p_frame.w / p_chunkSizeMultiplier, CHUNKSIZE) + 2;
		return glm::ivec4(x1, y1, x2, y2);
	}
	inline uint8_t* toRGBAUnsignedCharArray(float* floats, size_t outBytes) {
		uint8_t* out = (uint8_t*)malloc(outBytes);
		for (int i = 0; i < outBytes; i += 4) {
			out[i + 0] = (uint8_t)(floats[i + 0] * 255.f);
			out[i + 1] = (uint8_t)(floats[i + 1] * 255.f);
			out[i + 2] = (uint8_t)(floats[i + 2] * 255.f);
			out[i + 3] = (uint8_t)(floats[i + 3] * 255.f);
		}
		return out;
	}
	inline uint8_t* divideRes(uint16_t n, uint16_t width, size_t totalBytes, const uint8_t* p_data) {
		uint8_t* out = (uint8_t*)malloc(totalBytes / n);
		for (int y = 0; y < (totalBytes / (width * 4)) / n; y++) {
			for (int x = 0; x < width * 4; x += n * 4) {
				out[y * ((width * 4) / n) + x / n + 0] = p_data[y * n * width * 4 + x + 0];
				out[y * ((width * 4) / n) + x / n + 1] = p_data[y * n * width * 4 + x + 1];
				out[y * ((width * 4) / n) + x / n + 2] = p_data[y * n * width * 4 + x + 2];
				out[y * ((width * 4) / n) + x / n + 3] = p_data[y * n * width * 4 + x + 3];
			}
		}
		return out;
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
