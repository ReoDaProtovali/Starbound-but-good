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
	m_spriteMesh.addFloatAttrib(3); // Position
	m_spriteMesh.addFloatAttrib(2); // Texcoord
	
	// The four corner coordinates of the bounding rectangle. Note that the rectangle is in model space and not world space.
	glm::vec2 tl = bounds.getTL();
	glm::vec2 tr = bounds.getTR();
	glm::vec2 bl = bounds.getBL();
	glm::vec2 br = bounds.getBR();


	m_spriteMesh.pushVertices({
		tl.x, tl.y, 0.0f, 0.0f, 0.0f, // vertex 1
		tr.x, tr.y, 0.0f, 1.0f, 0.0f, // vertex 2
		bl.x, bl.y, 0.0f, 0.0f, 1.0f, // vertex 3
		bl.x, bl.y, 0.0f, 0.0f, 1.0f, // vertex 4
		tr.x, tr.y, 0.0f, 1.0f, 0.0f, // vertex 5
		br.x, br.y, 0.0f, 1.0f, 1.0f // vertex 6
		});
	m_spriteMesh.genVBO();
}


void Sprite::attachShader(std::shared_ptr<Shader> p_shader)
{

	m_attachedShader = p_shader;
}

void Sprite::attachTexture(std::shared_ptr<Texture> p_texture)
{
	m_attachedTexture = p_texture;
}



void Sprite::draw(DrawSurface& p_target, DrawStates& p_drawStates)
{
	DrawStates newStates = DrawStates(p_drawStates);
	if (m_attachedShader != nullptr) {
		newStates.attachShader(m_attachedShader);
	}
	if (m_attachedTexture != nullptr) {
		newStates.attachTexture(m_attachedTexture);
	}

	// Multiply it with the existing state transform, just in case it's relative to another coordinate system. Model space to world space.
	newStates.setTransform(p_drawStates.m_transform * getObjectTransform());

	p_target.draw(m_spriteMesh, GL_TRIANGLES, newStates);
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
	m_spriteMesh.subVBOData(0, 30, newVBOData.data());
}
