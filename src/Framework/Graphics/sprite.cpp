#include "Framework/Graphics/Sprite.hpp"
Sprite::Sprite()
{

}

Sprite::Sprite(glm::vec3 p_position, Rect p_bounds)
{
	setPosition(p_position);

	bounds = p_bounds;
	// Default origin:
	setOrigin(bounds.getCenter());
	spriteMesh.addFloatAttrib(3); // Position
	spriteMesh.addFloatAttrib(2); // Texcoord
	
	// The four corner coordinates of the bounding rectangle. Note that the rectangle is in model space and not world space.
	glm::vec2 tl = bounds.getTL();
	glm::vec2 tr = bounds.getTR();
	glm::vec2 bl = bounds.getBL();
	glm::vec2 br = bounds.getBR();


	spriteMesh.pushVertices({
		tl.x, tl.y, 0.0f, 0.0f, 0.0f, // vertex 1
		tr.x, tr.y, 0.0f, 1.0f, 0.0f, // vertex 2
		bl.x, bl.y, 0.0f, 0.0f, 1.0f, // vertex 3
		bl.x, bl.y, 0.0f, 0.0f, 1.0f, // vertex 4
		tr.x, tr.y, 0.0f, 1.0f, 0.0f, // vertex 5
		br.x, br.y, 0.0f, 1.0f, 1.0f // vertex 6
		});
	spriteMesh.genVBO();
}

Sprite::~Sprite(){
	// Nasty errors if we don't unassign these
	attachedShader = nullptr;
	attachedTexture = nullptr;
}


void Sprite::attachShader(Shader& p_shader)
{
	attachedShader = &p_shader;
}

void Sprite::attachTexture(Texture& p_texture)
{
	attachedTexture = &p_texture;
}
void Sprite::attachShader(Shader* p_shader)
{
	attachedShader = p_shader;
}

void Sprite::attachTexture(Texture* p_texture)
{
	attachedTexture = p_texture;
}

void Sprite::draw(DrawSurface& p_target, DrawStates& p_drawStates) const
{
	glm::vec3 test = position;
	glm::vec2 test2 = origin;
	DrawStates newStates = DrawStates(p_drawStates);
	if (attachedShader) {
		newStates.attachShader(attachedShader);
	}
	if (attachedTexture) {
		newStates.attachTexture(attachedTexture);
	}
	// The sprite's transform is only in model space, so we don't want to set it, because that would override any camera transformations
	//newStates.setTransform(transform);
	p_target.draw(spriteMesh, GL_TRIANGLES, newStates);
}

void Sprite::setOriginRelative(OriginLoc p_origin)
{
	setOrigin(bounds, p_origin);
}
