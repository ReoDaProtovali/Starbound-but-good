#pragma once
#ifndef UTILS_H
#define UTILS_H

#include <SDL.h>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "glm/vec2.hpp"
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
}

#endif UTILS_H