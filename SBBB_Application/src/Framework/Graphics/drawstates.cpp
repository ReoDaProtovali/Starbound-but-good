#include "Framework/Graphics/DrawStates.hpp"
#include <initializer_list>
DrawStates::DrawStates(glm::mat4& p_transform)
{
	m_transform = p_transform;
}

DrawStates::DrawStates(const std::weak_ptr<Texture> p_texture)
{
	m_transform = glm::mat4(1.0f);
	if (m_texturePtrs.size() != 0) {
		m_texturePtrs[0] = p_texture;
		return;
	}
	m_texturePtrs.push_back(p_texture);
}

DrawStates::DrawStates(std::vector<std::weak_ptr<Texture>>& p_textures)
{
	m_transform = glm::mat4(1.0f);
	m_texturePtrs = p_textures;
}

DrawStates::DrawStates(std::weak_ptr<Shader> p_shader)
{
	m_transform = glm::mat4(1.0f);
	m_shaderPtr = p_shader;
}

DrawStates::DrawStates(BlendMode p_blendMode)
{
	m_transform = glm::mat4(1.0f);
	m_blendMode = p_blendMode;
}

DrawStates::DrawStates(glm::mat4& p_transform, std::weak_ptr<Texture> p_texture, std::weak_ptr<Shader> p_shader, BlendMode p_blendMode)
{
	m_shaderPtr = p_shader;
	m_transform = p_transform;
	m_blendMode = p_blendMode;

	if (m_texturePtrs.size() != 0) {
		m_texturePtrs[0] = p_texture;
		return;
	}
	m_texturePtrs.push_back(p_texture);
}
DrawStates::DrawStates(glm::mat4& p_transform, std::initializer_list<std::weak_ptr<Texture>> p_textures, std::weak_ptr<Shader> p_shader, BlendMode p_blendMode)
{
	m_transform = p_transform;
	m_blendMode = p_blendMode;
	m_texturePtrs = p_textures;
}

void DrawStates::setTransform(glm::mat4 p_transform)
{
	m_transform = p_transform;
}
void DrawStates::attachShader(std::weak_ptr<Shader> p_shader)
{
	m_shaderPtr = p_shader;
}
void DrawStates::attachTexture(std::weak_ptr<Texture> p_texture)
{
	if (m_texturePtrs.size() != 0) {
		m_texturePtrs[0] = p_texture;
		return;
	}
	m_texturePtrs.push_back(p_texture);
}
void DrawStates::addTexture(std::weak_ptr<Texture> p_texture)
{
	m_texturePtrs.push_back(p_texture);
}
void DrawStates::setTexture(size_t p_index, std::weak_ptr<Texture> p_texture)
{
	if (p_index >= m_texturePtrs.size()) throw new std::runtime_error("Attempted to set DrawState texture index out of range.");
	m_texturePtrs[p_index] = p_texture;
}
void DrawStates::setBlendMode(BlendMode p_blendMode)
{
	m_blendMode = p_blendMode;
};

