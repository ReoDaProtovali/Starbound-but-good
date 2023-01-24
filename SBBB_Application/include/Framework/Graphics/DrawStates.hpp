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
public:
	DrawStates() :
		m_transform(glm::mat4(1.0f))
	{};

	~DrawStates() {
	}

	DrawStates(glm::mat4& p_transform);
	DrawStates(Texture* p_texture);
	DrawStates(std::vector<Texture*>& p_textures);
	DrawStates(std::weak_ptr<Shader> p_shader);
	DrawStates(BlendMode blendMode);
	DrawStates(glm::mat4& p_transform, Texture* p_texture, std::weak_ptr<Shader> p_shader, BlendMode p_blendMode = BlendMode());
	DrawStates(glm::mat4& p_transform, std::initializer_list<Texture*> p_textures, std::weak_ptr<Shader> p_shader, BlendMode p_blendMode = BlendMode());

	void setTransform(glm::mat4 p_transform);
	// Sets the internal shader to use.
	void attachShader(std::weak_ptr<Shader> p_shader);

	void setBlendMode(BlendMode p_blendMode);

	// Just a function for convenience, so you don't have to worry about indexing the texure vector if you're only utilizing one texture as in most cases.
	// Reference overload
	void attachTexture(Texture* p_texture);

	void addTexture(Texture* p_texture);

	void setTexture(size_t p_index, Texture* p_texture);


	bool checkIfInit() { 
		for (auto texturePtr : m_texturePtrs) {
			if (texturePtr == nullptr) {
				LOG("Texture failed initialization check.");
				return false;
			}
		}
		if (!m_shaderPtr.lock()) {
			LOG("Shader failed initialization check.");
			return false;
		}
		return true;
	};

	// Pointers, because we want them to reflect the values of their outside variables so we don't need to update the draw states every time one changes.

	// The modelview matrix used the shader.
	glm::mat4 m_transform;

	// Supports multiple textures for one shader. Should never be added to or removed from after initialization.
	std::vector<Texture*> m_texturePtrs;

	std::weak_ptr<Shader> m_shaderPtr;

	// Constructed with default settings.
	// Not a pointer, because it should only be a local copy.
	BlendMode m_blendMode;
};
#endif