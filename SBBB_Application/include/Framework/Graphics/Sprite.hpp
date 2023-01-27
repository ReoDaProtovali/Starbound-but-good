#ifndef SPRITE_H
#define SPRITE_H
#include "Framework/Graphics/TransformObject.hpp"
#include "Framework/Graphics/DrawSurface.hpp"
#include "Framework/Graphics/DrawStates.hpp"
#include "Framework/Graphics/Mesh.hpp"
#include "Framework/Graphics/Shader.hpp"
#include "Framework/Graphics/Texture.hpp"
#include "util/Rect.hpp"


//class DrawSurface;
// A sprite containing its own transform and mesh.
class Sprite : public TransformObject {
public:
	Sprite();
	Sprite(glm::vec3 p_position, Rect p_bounds);

	~Sprite() {
		DELETE_LOG("Sprite Destroyed");
	}
	// Shader must have the appropriate vertex attribute format
	void attachShader(Shader* p_shader);
	void attachTexture(Texture* p_texture);

	void draw(DrawSurface& p_target, DrawStates& p_drawStates);

	// We don't want to override the setorigin class of the inherited class, so we name it something different
	void setOriginRelative(OriginLoc p_origin);
	void setBounds(Rect p_bounds);
	Rect bounds;
private:
	Mesh<GLfloat> m_spriteMesh;
	// Optional attachments directly to the sprite, so you don't have to worry about setting your drawStates properly before draw().
	Shader* m_attachedShader;
	Texture* m_attachedTexture;

};

#endif