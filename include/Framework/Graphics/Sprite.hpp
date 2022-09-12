#ifndef SPRITE_H
#define SPRITE_H
#include "Framework/Graphics/TransformObject.hpp"
#include "Framework/Graphics/DrawObject.hpp"
#include "Framework/Graphics/Mesh.hpp"
#include "Framework/Graphics/Shader.hpp"
#include "util/Rect.hpp"
#include "Framework/Graphics/Texture.hpp"


class DrawSurface;
// A sprite containing its own transform and mesh.
class Sprite : public TransformObject, public DrawObject {
public:
	Sprite();
	Sprite(glm::vec3 p_position, Rect p_bounds);
	~Sprite();

	// Shader must have the appropriate vertex attribute format
	void attachShader(Shader& p_shader);
	void attachTexture(Texture& p_texture);
	// Shader must have the appropriate vertex attribute format
	void attachShader(Shader* p_shader);
	void attachTexture(Texture* p_texture);

	void draw(DrawSurface& p_target, DrawStates& p_drawStates) const override;

	// We don't want to override the setorigin class of the inherited class, so we name it something different
	void setOriginRelative(OriginLoc p_origin);
	Rect bounds;
private:
	Mesh<GLfloat> spriteMesh;

	// Optional attachments directly to the sprite, so you don't have to worry about setting your drawStates properly before draw().
	Shader* attachedShader = nullptr;
	Texture* attachedTexture = nullptr;

};

#endif