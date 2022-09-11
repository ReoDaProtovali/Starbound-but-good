#ifndef DRAWSTATES_H
#define DRAWSTATES_H

#include <vector>
#include <util/ext/glm/mat4x4.hpp>
#include "Framework/Graphics/Shader.hpp"
#include "Framework/Graphics/Texture.hpp"
#include "Framework/Graphics/BlendMode.hpp"

// Pretty much contains everything you need to know about drawing apart from the mesh.
// Warning, members become invalid if the thing they point to get deleted. Make sure all pointers are assigned before drawing.
class DrawStates {
	DrawStates() :
		transform(nullptr),
		shader(nullptr)
	{};

	DrawStates(glm::mat4& p_transform);
	DrawStates(Texture& p_texture);
	DrawStates(std::vector<Texture*>& p_textures);
	DrawStates(Shader& p_shader);
	DrawStates(BlendMode blendMode);
	DrawStates(glm::mat4& p_transform, Texture& p_texture, Shader& p_shader, BlendMode p_blendMode = BlendMode());
	DrawStates(glm::mat4& p_transform, std::initializer_list<Texture*> p_textures, Shader& p_shader, BlendMode p_blendMode = BlendMode());

	// Just a function for convenience, so you don't have to worry about indexing the texure vector if you're only utilizing one texture as in most cases.
	void bindTexture(Texture& p_texture);
	bool checkIfInitialized() { return transformPointerSet && textureAdded && shaderPointerSet; };

private:
	// Pointers, because we want them to reflect the values of their outside variables so we don't need to update the draw states every time one changes.

	// The modelview matrix used the shader.
	glm::mat4* transform;
	bool transformPointerSet = false; // nullptr checking

	// Supports multiple textures for one shader. Should never be added to or removed from after initialization.
	std::vector<Texture*> textures;
	bool textureAdded = false; // nullptr checking

	Shader* shader;
	bool shaderPointerSet = false; // nullptr checking

	// Constructed with default settings.
	// Not a pointer, because it should only be a local copy.
	BlendMode blendMode;
};
#endif