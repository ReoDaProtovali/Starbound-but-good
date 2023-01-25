#pragma once
#include <vector>
#include <iostream>
#include <stdexcept>
#include <filesystem>
#include <map>
#include <unordered_map>
#include <memory>
#include <string>
#include <cctype>
#include <util/ext/json.hpp>
#include "Framework/Graphics/Pixmap.hpp"

#include "Framework/Graphics/Texture.hpp"

struct TileInfo {
	std::string id = "";
	std::string name = "";
	std::string genericDescription = "";
	std::vector<std::string> categories{};
	std::string rarity = "";
	std::string imageFile = "";
	size_t imageIndex = 0;
	bool isAnimated = false;
	bool emissive = false;
	std::vector<float> lightingColor{};
	uint16_t maxHP = 0;
	uint16_t variationCount = 0;
	float lightAbsorption = 0;
};

class ResourceLoader
{
public:
	/// Default constructor. Does nothing.
	ResourceLoader();
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
	* @returns A pointer to the texture, with relevant information. Not owned by the caller.
	*/
	Texture* getTexture(TextureID p_ID, bool& p_success); // friendlier, success bool is passed by reference

	Texture* getTileSheetTexture();

	void loadAllTileSets();

	void loadTileSet(std::filesystem::path p_tileSetPath);

	void loadDirTiles(std::string p_namespace, std::filesystem::path p_tileInfoPath, std::filesystem::path p_imagePath, std::filesystem::path p_parentPath);

private:
	/// A standard library map that stores pairs of textures and their respective IDs.
	std::map<TextureID, Texture> textures;
	std::unordered_map<std::string, size_t> tileInfoIndexDict;
	std::vector<TileInfo> tileInfoCache;
	Pixmap tileSheetPixmap;
	Texture tileSheetTexture;
};

