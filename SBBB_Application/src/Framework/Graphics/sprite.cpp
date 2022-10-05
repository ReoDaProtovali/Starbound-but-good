#include "Framework/Graphics/Sprite.hpp"

Sprite::Sprite()
{

}

Sprite::Sprite(glm::vec3 p_position, Rect p_bounds)
{
	LOAD_LOG("Created Sprite at x: " << p_position.x << " y: " << p_position.y);
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


void Sprite::attachShader(std::shared_ptr<Shader> p_shader)
{

	attachedShader = p_shader;
}

void Sprite::attachTexture(std::shared_ptr<Texture> p_texture)
{
	attachedTexture = p_texture;
}



void Sprite::draw(DrawSurface& p_target, DrawStates& p_drawStates)
{
	DrawStates newStates = DrawStates(p_drawStates);
	if (attachedShader != nullptr) {
		newStates.attachShader(attachedShader);
	}
	if (attachedTexture != nullptr) {
		newStates.attachTexture(attachedTexture);
	}

	// Multiply it with the existing state transform, just in case it's relative to another coordinate system. Model space to world space.
	newStates.setTransform(p_drawStates.transform * getObjectTransform());

	p_target.draw(spriteMesh, GL_TRIANGLES, newStates);
}

void Sprite::setOriginRelative(OriginLoc p_origin)
{
	setOrigin(bounds, p_origin);
}

void Sprite::setBounds(Rect p_bounds)
{
	if (bounds == p_bounds) return;
	bounds = p_bounds;
	glm::vec2 tl = bounds.getTL();
	glm::vec2 tr = bounds.getTR();
	glm::vec2 bl = bounds.getBL();
	glm::vec2 br = bounds.getBR();

	std::vector<GLfloat> newVBOData = {
		tl.x, tl.y, 0.0f, 0.0f, 0.0f, // vertex 1
		tr.x, tr.y, 0.0f, 1.0f, 0.0f, // vertex 2
		bl.x, bl.y, 0.0f, 0.0f, 1.0f, // vertex 3
		bl.x, bl.y, 0.0f, 0.0f, 1.0f, // vertex 4
		tr.x, tr.y, 0.0f, 1.0f, 0.0f, // vertex 5
		br.x, br.y, 0.0f, 1.0f, 1.0f // vertex 6
	};
	spriteMesh.subVBOData(0, 30, newVBOData.data());
}
