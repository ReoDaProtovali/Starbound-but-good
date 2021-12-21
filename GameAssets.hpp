#pragma once
#ifndef GAMEASSETS_H
#define GAMEASSETS_H

#include "ResourceLoader.hpp"

namespace rl {
	inline bool loadRes(ResourceLoader& p_res) {
		bool success = true;
		success = p_res.load("res/tiles/teststone.png", TextureID::STONE_TEXTURE) &&
			p_res.load("res/tiles/testdirt.png", TextureID::DIRT_TEXTURE) &&
			p_res.load("res/tiles/tile1.png", TextureID::TILE_TEXTURE) &&
			p_res.load("res/reohangsmall.png", TextureID::REOHANG_TEXTURE) &&
			p_res.load("res/tiles/spritesheet.png", TextureID::TILESHEET_TEXTURE) &&
			p_res.load("res/tiles/testsheet.png", TextureID::TESTSPRITESHEET_TEXTURE);
		if (!success) {
			throw std::invalid_argument("One or more file not found");
		}
		return success;
	}
}

#endif GAMEASSETS_H