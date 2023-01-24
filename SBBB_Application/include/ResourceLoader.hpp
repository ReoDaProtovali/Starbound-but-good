#pragma once
#include <vector>
#include <iostream>
#include <stdexcept>
#include "Framework/Graphics/Texture.hpp"
#include <map>
#include <memory>

/// Handles loading PNG images from files and stores them internally as textures. Allows for retrieval of them later via TextureID.
class ResourceLoader
{
public:
	/// Default constructor. Does nothing.
	ResourceLoader() {};
	// Deletes all managed textures
	~ResourceLoader();
	/** Loads a texture into the texture pool.
	* @param p_filepath - The filepath to the texture. In standard directory notation, folders separated by "/"
	* @param p_assignedID - The enum TextureID you want the image file to be associated with, for retrieval later.
	* @returns A boolean representing if the file was found or not.
	*/
	bool loadTexID(const char* p_filepath, TextureID p_assignedID);
	/** Get a texture from the resource loader's texture pool. Throws an exception if the TextureID has no associated texture.
	* @param p_ID - The TextureID associated with the Texture you want to access.
	*/
	Texture* getTexture(TextureID p_ID);
	/** Get a texture from the resource loader's texture pool.
	* @param p_ID - The TextureID associated with the Texture you want to access.
	* @param success - a reference to a boolean, sets it to false if the given ID has no associated texture, otherwise sets it to true.
	*/
	Texture* getTexture(TextureID p_ID, bool& p_success); // friendlier, success bool is passed by reference

private:
	/// A standard library map that stores pairs of textures and their respective IDs.
	std::map<TextureID, Texture> textures;
};

