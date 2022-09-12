#include "Framework\Graphics\DrawStates.hpp"
#include <initializer_list>
DrawStates::~DrawStates()
{
	shader = nullptr;
	for (int i = 0; i < textures.size(); i++) {
		textures[i] = nullptr;
	}
}
DrawStates::DrawStates(glm::mat4& p_transform)
{
	shader = nullptr;
	transform = p_transform;
}

DrawStates::DrawStates(Texture& p_texture)
{
	shader = nullptr;
	transform = glm::mat4(1.0f);
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
	transform = glm::mat4(1.0f);
	textures = p_textures;
	textureAdded = true;
}

DrawStates::DrawStates(Shader& p_shader)
{
	shader = nullptr;
	transform = glm::mat4(1.0f);
	shader = &p_shader;
	shaderPointerSet = true;
}

DrawStates::DrawStates(BlendMode p_blendMode)
{
	shader = nullptr;
	transform = glm::mat4(1.0f);
	blendMode = p_blendMode;
}

DrawStates::DrawStates(glm::mat4& p_transform, Texture& p_texture, Shader& p_shader, BlendMode p_blendMode)
{
	shader = &p_shader;
	transform = p_transform;
	blendMode = p_blendMode;
	shaderPointerSet = true;

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
	transform = p_transform;
	blendMode = p_blendMode;
	shaderPointerSet = true;
	textures = p_textures;
	textureAdded = true;
}

void DrawStates::setTransform(glm::mat4 p_transform)
{
	transform = p_transform;
}
void DrawStates::attachShader(Shader* p_shader)
{
	shader = p_shader;
	shaderPointerSet = true;
}
void DrawStates::attachShader(Shader& p_shader)
{
	shader = &p_shader;
	shaderPointerSet = true;
}
void DrawStates::attachTexture(Texture* p_texture)
{
	if (textures.size() != 0) {
		textures[0] = p_texture;
		textureAdded = true;
		return;
	}
	textures.push_back(p_texture);
	textureAdded = true;
}
void DrawStates::attachTexture(Texture& p_texture)
{
	if (textures.size() != 0) {
		textures[0] = &p_texture;
		textureAdded = true;
		return;
	}
	textures.push_back(&p_texture);
	textureAdded = true;
}
void DrawStates::setBlendMode(BlendMode p_blendMode)
{
	blendMode = p_blendMode;
};

