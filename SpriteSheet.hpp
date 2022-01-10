#pragma once
#include "glm/glm.hpp"
#include "GameConstants.hpp"
#include "Texture.hpp"
#include <string>
#include <stdexcept>

/// A class that provides a standard method to get texture coordinates based on any given sprite index.
/// Has different modes that coorespond to different kinds of spritesheets, ex BASIC for non-animated spritesheets, ANIMATION_ROWS for sheets organized by rows of frames.
/// NOTE:: should be replaced with a shader-based approach
class SpriteSheet
{
	public:
		///  Default constructor. 
		/// All values are uninitialized
		SpriteSheet()
			: sheetMode(SheetMode::BASIC),
			texture(Texture()),
			spriteDimensions(glm::ivec2()),
			currentSprite(NULL),
			spriteCount(NULL),
			currentAnimation(NULL),
			animationCount(NULL),
			frameCount(NULL) {};

		/** Initialized constructor. The current sprite, the current animation, animation count, and frame count should be set afterwards.
		* 
		* @param p_texture - The texture of the spritesheet, should be initialized
		* @param p_spriteDimensions - The integer width and height of a single sprite on the sheet, in pixels. Might add support for variable sprite dimensions later.
		* @param p_spriteCount - The maximum amount of sprites the spritesheet can hold. usually dependent on the sprite dimensions over the sprite area
		*/
		SpriteSheet(Texture p_texture, glm::ivec2 p_spriteDimensions, unsigned short p_spriteCount)
			: sheetMode(SheetMode::BASIC),
			texture(p_texture),
			spriteDimensions(p_spriteDimensions),
			currentSprite(NULL), 
			spriteCount(p_spriteCount), 
			currentAnimation(NULL), 
			animationCount(NULL), 
			frameCount(1) {};

		/// Copies the given texture into the spritesheet.
		void setTexture(Texture p_texture) { texture = p_texture; };
		/// Sets the spritesheet's max sprite count.
		void setSpriteCount(unsigned short p_spriteCount) { spriteCount = p_spriteCount; };
		/// Sets the current sprite by index, changes functionality based on the sheet's mode
		void setCurrentSprite(unsigned short p_spriteIndex);
		/// Sets the dimensions of a single sprite within the sheet
		void setSpriteDimensions(glm::ivec2 p_dim);
		/// Changes the mode of the spritesheet, which determines how to handle indexing and texture coordinate grabbing
		void setMode(SheetMode mode);
		/// Used mainly when the sheet is in animation mode, advances to the next frame of the animation, and wraps if it's past the end
		void nextFrame();

		/// Takes in a sprite index, and returns an x/y pixel position. The meaning of "index" changes depending on the sheet mode.
		glm::ivec2 indexToPos(unsigned short p_spriteIndex);
		/** Gets texture coordinates of a sprite within the sheet.
		* @param corner - The enum determining which corner of the sprite you want the texture coords for
		* @returns A vec2 containing the texture coordinates.
		*/
		glm::vec2 getTexCoords(Corner corner);
		/// Sets the current animation, in the case of animation rows, the row.
		void setAnimation(unsigned short p_animationID);
		/// Sets the amount of frames within the animation.
		void setFrameCount(unsigned short p_frameCount) { frameCount = p_frameCount; }

		/// The texture used by the spritesheet.
		Texture texture;

		unsigned short currentSprite;

	private:
		SheetMode sheetMode;
		unsigned short spriteCount;

		unsigned short currentAnimation;
		unsigned short frameCount;

		unsigned short animationCount;


		glm::ivec2 spriteDimensions;
};

