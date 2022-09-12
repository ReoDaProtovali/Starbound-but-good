#pragma once
#include <util/ext/glm/vec2.hpp>
#include <util/ext/glm/vec3.hpp>
#include <util/ext/glm/vec4.hpp>
#include <util/ext/glm/mat4x4.hpp>
#include <util/ext/glm/gtc/matrix_transform.hpp>
#include <algorithm>


/// Camera wrapper for calculating the transformation matrix
/// Provides more intuitive control over view, provides
/// The visible frame for rendering within screen, etc.
class Camera
{
public:

	/** Default constructor
	*
	* Sets all the orientation vectors
	* Camera position is initialized to (0, 0, 1)
	* Scale is defaulted to 20 tiles
	* Looks towards the origin
	*
	*/
	Camera();

	/**
	* @overload Camera(glm::vec3 p_pos)
	*
	* @param p_pos - The initial position of the camera, same as default contructor otherwise
	*/
	Camera(glm::vec3 p_pos);

	/**
	* Makes the camera look directly at a given target.
	*
	* @param p_target - The global position the camera should center within view.
	*/
	void lookAt(glm::vec3 p_target);


	/// Face the camera in the negative z direction, keeping the position the same
	void lookForwards();

	/**
	* Modifies the aspect of the camera, which in most cases should be the aspect of the window.
	*
	* @param p_aspectRatio - Aspect ratio should be in the form of width / height.
	*/
	void setDimensions(float p_aspectRatio);

	/**
	* I got tired of constantly setting the dimensions to be that of the window every time I drew, so I'm keeping record of them within the camera now.
	*
	* @param p_pixelDimensions - A vec2 containing the camera dimensions in pixels.
	*/
	void setPixelDimensions(glm::vec2 p_pixelDimensions) { pixelDimensions = p_pixelDimensions; };
	/**
	* Generates a mat4 based on the camera and window's state.
	*
	* @param p_windowWidth, p_windowHeight - The camera's view window dimensions.
	* @returns glm::mat4 - To be used for position transformations, is equivalent to proj * view.
	*/
	glm::mat4 getTransform();

	/**
	* Used internally. Sets the camera's frame values, of course
	*
	* @param p_trX, p_trY - The top right x and y poition of the frame
	* @param p_width, p_height - The width and height of the camera's frame, starting at the top left and ending at the bottom right.
	*/
	void setFrame(float p_trX, float p_trY, float p_width, float p_height);

	/**
	* Getter for the camera's frame. Camera frame units are in tiles.
	*
	* @returns A vec4 representing the camera's frame. the x and y represent the top left corner coords, z and w represent the bottom right corner coords.
	*/
	const glm::vec4 getFrame();

	/**
	* Used to update the camera's frame value based on camera's pixel dimensions. Should be done at some point after a window resize.
	*
	*/
	void updateFrame();

	/**
	* Sets the tile scale for the camera.
	* Tile scale is a value representing the amount of tiles on the longest axis.
	* For example, if the tile scale is 20, and the window is wider than it is tall, the camera will display 20 tiles horizontally.
	*
	* @param p_tileScale - A numerical value representing the scale.
	*/
	void setTileScale(float p_tileScale);

	/**
	* Sets the global XY position for the camera. The Z position remains unchanged. Based around the center of the camera frame.
	*
	* @param p_globalPos - A vector containing the x and y coordinates to translate to (in tiles).
	*/
	void setGlobalPos(glm::vec2 p_globalPos);

	/**
	* Sets the global XY position for the camera. The Z position remains unchanged. Based around the center of the camera frame.
	*
	* @param p_globalX, p_globalY - float x and y positions to translate to (in tiles).
	*/
	void setGlobalPos(float p_globalX, float p_globalY);

	/// Enables manually changing the view matrix without it defaulting to lookForwards();
	void enableManualView() { manualView = true; }
	/// Ignores changes to the view matrix, and defaults to lookForwards();
	void disableManualView() { manualView = false; }
	/// Switches projection matrix from ortho to perspective, with an FOV of 90 by default. Should also use manual view when setting this, or else it's mostly useless.
	void enablePerspective() { perspective = true; }
	/// Disables perspective projection, and defaults to orthographic.
	void disablePerspective() { perspective = false; }

	/// Tile scale is a value representing the amount of tiles on the longest axis.
	float tileScale;
	/// Rotation of the camera on the z axis, currently unused.
	float zRotation = 0;
	glm::vec3 pos;
	glm::mat4 view;

private:
	/// used for lookAt and lookForwards, represents a position the camera is targeting.
	glm::vec3 target;
	glm::vec3 right; /* Used within constructor. The vector facing right relative to the camera's view. */
	glm::vec3 up; /* Used within constructor. The vector facing upwards relative to the camera's view */
	glm::vec3 forward; /* Used within constructor, represents the vector pointing straight out from the middle of the camera view. */
	glm::mat4 proj;
	/// Camera dimensions. Best normalized to the aspect ratio of the window. X is width, Y is height.
	glm::vec2 dimensions;
	glm::vec2 pixelDimensions;
	/// A vec4 representing the camera's frame. the x and y represent the top left corner coords, z and w represent the bottom right corner coords.
	glm::vec4 frame;
	/// A consntant value telling the camera what the vertical axis is. In this case, it's the y axis.
	const glm::vec3 upGuide = glm::vec3(0.0f, 1.0f, 0.0f); // y axis is up
	bool manualView = false;
	bool perspective = false;

};

