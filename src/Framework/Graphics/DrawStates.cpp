#include "Framework\Graphics\DrawStates.hpp"
#include <initializer_list>
DrawStates::DrawStates(glm::mat4& p_transform)
{
	transform = p_transform;
}

DrawStates::DrawStates(const std::weak_ptr<Texture> p_texture)
{
	transform = glm::mat4(1.0f);
	if (textures.size() != 0) {
		textures[0] = p_texture;
		textureAdded = true;
		return;
	}
	textures.push_back(p_texture);
	textureAdded = true;
}

DrawStates::DrawStates(std::vector<std::weak_ptr<Texture>>& p_textures)
{
	transform = glm::mat4(1.0f);
	textures = p_textures;
	textureAdded = true;
}

DrawStates::DrawStates(std::weak_ptr<Shader> p_shader)
{
	transform = glm::mat4(1.0f);
	shader = p_shader;
	shaderPointerSet = true;
}

DrawStates::DrawStates(BlendMode p_blendMode)
{
	transform = glm::mat4(1.0f);
	blendMode = p_blendMode;
}

DrawStates::DrawStates(glm::mat4& p_transform, std::weak_ptr<Texture> p_texture, std::weak_ptr<Shader> p_shader, BlendMode p_blendMode)
{
	shader = p_shader;
	transform = p_transform;
	blendMode = p_blendMode;
	shaderPointerSet = true;

	if (textures.size() != 0) {
		textures[0] = p_texture;
		textureAdded = true;
		return;
	}
	textures.push_back(p_texture);
	textureAdded = true;
}
DrawStates::DrawStates(glm::mat4& p_transform, std::initializer_list<std::weak_ptr<Texture>> p_textures, std::weak_ptr<Shader> p_shader, BlendMode p_blendMode)
{
	transform = p_transform;
	blendMode = p_blendMode;
	textures = p_textures;
	textureAdded = true;
}

void DrawStates::setTransform(glm::mat4 p_transform)
{
	transform = p_transform;
}
void DrawStates::attachShader(std::weak_ptr<Shader> p_shader)
{
	shader = p_shader;
	shaderPointerSet = true;
}
void DrawStates::attachTexture(std::weak_ptr<Texture> p_texture)
{
	if (textures.size() != 0) {
		textures[0] = p_texture;
		textureAdded = true;
		return;
	}
	textures.push_back(p_texture);
	textureAdded = true;
}
void DrawStates::addTexture(std::weak_ptr<Texture> p_texture)
{
	textures.push_back(p_texture);
}
void DrawStates::setTexture(size_t p_index, std::weak_ptr<Texture> p_texture)
{
	if (p_index >= textures.size()) throw new std::runtime_error("Attempted to set DrawState texture index out of range.");
	textures[p_index] = p_texture;
}
void DrawStates::setBlendMode(BlendMode p_blendMode)
{
	blendMode = p_blendMode;
};

