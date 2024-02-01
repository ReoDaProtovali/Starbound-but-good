#ifndef FRAMEWORK_CONSTANTS_H
#define FRAMEWORK_CONSTANTS_H

// runs once when things initialize to help trace where the program fails
//#define LOADLOGGING_ENABLED
// logs whenever many things are deleted. used to catch memory leaks
//#define DELETELOGGING_ENABLED
// logs EVERY buffer made by opengl, only for extreme debugging use
//#define GLGENLOGGING_ENABLED 

#include "Log.hpp"

// Texture needs access to this at the moment which is unfortunate
//enum class TextureID {
//	NO_TEXTURE,
//	STONE_TEXTURE,
//	DIRT_TEXTURE,
//	TILE_TEXTURE,
//	BOMB_TEXTURE,
//	REOHANG_TEXTURE,
//	TESTSPRITESHEET_TEXTURE,
//	ME_TEXTURE,
//	CAMERA_FRAME_TEXTURE,
//	CAMERA_FRAME_TEXTURE2,
//	REO_TEST,
//	TILESHEET_TEXTURE
//};


#endif