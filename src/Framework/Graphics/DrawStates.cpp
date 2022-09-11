#include "Framework\Graphics\DrawStates.hpp"
#include <initializer_list>
DrawStates::DrawStates(glm::mat4& p_transform)
{
	shader = nullptr;
	transform = nullptr;
	transform = &p_transform;
	transformPointerSet = true;
}

DrawStates::DrawStates(Texture& p_texture)
{
	shader = nullptr;
	transform = nullptr;
	if (textures.size() != 0) {
		textures[0] = &p_texture;
		textureAdded = true;
		return;
	}
	textures.push_back(&p_texture);
	textureAdded = true;
}

DrawStates::DrawStates(std::vector<Texture*>& p_textures)
{
	shader = nullptr;
	transform = nullptr;
	textures = p_textures;
	textureAdded = true;
}

DrawStates::DrawStates(Shader& p_shader)
{
	shader = nullptr;
	transform = nullptr;
	shader = &p_shader;
	shaderPointerSet = true;
}

DrawStates::DrawStates(BlendMode p_blendMode)
{
	shader = nullptr;
	transform = nullptr;
	blendMode = p_blendMode;
}

DrawStates::DrawStates(glm::mat4& p_transform, Texture& p_texture, Shader& p_shader, BlendMode p_blendMode)
{
	shader = &p_shader;
	transform = &p_transform;
	blendMode = p_blendMode;
	shaderPointerSet = true;
	transformPointerSet = true;

	if (textures.size() != 0) {
		textures[0] = &p_texture;
		textureAdded = true;
		return;
	}
	textures.push_back(&p_texture);
	textureAdded = true;
}
DrawStates::DrawStates(glm::mat4& p_transform, std::initializer_list<Texture*> p_textures, Shader& p_shader, BlendMode p_blendMode) 
{
	shader = &p_shader;
	transform = &p_transform;
	blendMode = p_blendMode;
	shaderPointerSet = true;
	transformPointerSet = true;
	textures = p_textures;
	textureAdded = true;
};

void DrawStates::bindTexture(Texture& p_texture) 
{
	if (textures.size() != 0) {
		textures[0] = &p_texture;
		textureAdded = true;
		return;
	}
	textures.push_back(&p_texture);
	textureAdded = true;
}