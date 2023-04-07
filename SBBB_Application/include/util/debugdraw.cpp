#include "util/DebugDraw.hpp"

void DebugDraw::drawBoxImmediate(float p_x, float p_y, float p_w, float p_h, glm::vec3 p_col, DrawSurface& p_surface, Camera& p_camera) {
	static Mesh<float> s_Mesh{ NO_VAO_INIT };
	static bool firstRun = true;
	if (!s_Mesh.VAOInitialized) {
		s_Mesh.setStreamType(GL_DYNAMIC_DRAW);
		s_Mesh.addFloatAttrib(3);
	}

	s_Mesh.clean();

	// top side
	Rect localBounds = Rect(-0.5f, -0.5f, p_w + 1.f, 1.f);
	auto tl = localBounds.getTL();
	auto tr = localBounds.getTR();
	auto bl = localBounds.getBL();
	auto br = localBounds.getBR();

	s_Mesh.pushVertices({
		tl.x, -tl.y, 0.f,
		tr.x, -tr.y, 0.f,
		bl.x, -bl.y, 0.f,
		bl.x, -bl.y, 0.f,
		tr.x, -tr.y, 0.f,
		br.x, -br.y, 0.f
		});

	// left side
	localBounds = Rect(-0.5f, -0.5f, 1.f, p_h + 1.f);
	tl = localBounds.getTL();
	tr = localBounds.getTR();
	bl = localBounds.getBL();
	br = localBounds.getBR();

	s_Mesh.pushVertices({
		tl.x, -tl.y, 0.f,
		tr.x, -tr.y, 0.f,
		bl.x, -bl.y, 0.f,
		bl.x, -bl.y, 0.f,
		tr.x, -tr.y, 0.f,
		br.x, -br.y, 0.f
		});

	// right side
	localBounds = Rect(p_w - 0.5f, -0.5f, 1.f, p_h + 1.f);
	tl = localBounds.getTL();
	tr = localBounds.getTR();
	bl = localBounds.getBL();
	br = localBounds.getBR();

	s_Mesh.pushVertices({
		tl.x, -tl.y, 0.f,
		tr.x, -tr.y, 0.f,
		bl.x, -bl.y, 0.f,
		bl.x, -bl.y, 0.f,
		tr.x, -tr.y, 0.f,
		br.x, -br.y, 0.f
		});

	// bottom side
	localBounds = Rect(-0.5f, p_h - 0.5f, p_w + 1.f, 1.0f);
	tl = localBounds.getTL();
	tr = localBounds.getTR();
	bl = localBounds.getBL();
	br = localBounds.getBR();

	s_Mesh.pushVertices({
		tl.x, -tl.y, 0.f,
		tr.x, -tr.y, 0.f,
		bl.x, -bl.y, 0.f,
		bl.x, -bl.y, 0.f,
		tr.x, -tr.y, 0.f,
		br.x, -br.y, 0.f
		});

	if (firstRun) {
		s_Mesh.pushVBOToGPU();
	}
	else {
		s_Mesh.subCurrentVBOData();
	}

	GenericShaders& gs = GenericShaders::Get();
	DrawStates d;
	d.attachShader(&gs.solidColorShader);
	gs.solidColorShader.setVec3Uniform(1, p_col);
	auto currentTransform = glm::mat4(1);
	currentTransform = glm::translate(currentTransform, glm::vec3(p_x, p_y, 0));
	d.setTransform(p_camera.getTransform() * currentTransform);
	p_surface.draw(s_Mesh, GL_TRIANGLES, d);
	firstRun = false;
}
