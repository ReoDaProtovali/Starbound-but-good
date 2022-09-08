//#include "Framework/Framework.hpp"

//void Framework::Draw::drawSprite(glm::vec3 p_worldPos, glm::vec2 p_dimensions, Texture p_spriteTex, Camera& p_cam) {

	//std::vector<GLfloat> spriteVerts = { // Local coordinates, top left origin (actually, ends up bottom right in world space because Inverted y) (had to flip texture)
	//	0.0f, 0.0f, 2.0f,                     0.0f, 1.0f, // vertex 1 tl
	//	p_dimensions.x, 0.0f, 0.0f,           1.0f, 1.0f, // vertex 2 tr
	//	0.0f, p_dimensions.y, 0.0f,           0.0f, 0.0f, // vertex 3 bl
	//	0.0f, p_dimensions.y, 0.0f,           0.0f, 0.0f, // vertex 4 bl
	//	p_dimensions.x, 0.0f, 0.0f,           1.0f, 1.0f, // vertex 5 tr
	//	p_dimensions.x, p_dimensions.y, 0.0f, 1.0f, 0.0f // vertex 6 br
	//};
	//GenericMeshes.ImageMesh.subVBOData(0, spriteVerts.size(), spriteVerts.data());
	//glBindVertexArray(GenericMeshes.ImageMesh.VAO);

	//GenericShaders.ImageShader.use();

	//// Set the active texture unit to 0
	//// this was done with "imageShader.setTexUniform("imageTexture", 0);"
	//glActiveTexture(GL_TEXTURE0);
	//// Attach the actual texture to the main GL_TEXTURE_2D buffer on unit 0
	//glBindTexture(GL_TEXTURE_2D, p_spriteTex.glID);

	//// Matrix that transforms from local space to global space
	//glm::mat4 modelTransform = glm::mat4(1.0f); // Identity
	//modelTransform =
	//	glm::translate(modelTransform, glm::vec3(p_worldPos.x, p_worldPos.y, p_worldPos.z));

	//GenericShaders.ImageShader.setMat4Uniform("model", modelTransform);

	//// Matrix that transforms from global space, to view space, to clip space in one swoop
	//glm::mat4 finalTransform = p_cam.getTransformMat4();
	//GenericShaders.ImageShader.setMat4Uniform("transform", finalTransform);

	//glDrawArrays(GL_TRIANGLES, 0, GenericMeshes.ImageMesh.getTotalVBOSize());
	//glBindVertexArray(0);
//}