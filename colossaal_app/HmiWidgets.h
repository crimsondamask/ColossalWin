#pragma once
#include <vector>
#include <string>

#include "stb_image.h"
#include "imgui/GLFW/glfw3.h"
#include "imgui/imgui.h"
#include <filesystem>
#include <fstream>

enum class HmiWidgetType
{
	Image = 0,
	NumericField = 1,
	StatusIndicator,
	Text,
	PushButton,
	InvisibleButton,
};

class ImageWidget
{
public:
	ImageWidget() = default;
	ImageWidget(std::string path);
	bool setTexture();
	bool draw(ImVec2 windowCursor, float posx, float posy, float width, float height);
	
	std::string imgWidgetPath;
	GLuint texture;
	int imgWidth;
	int imgHeight;
};

class TextWidget
{
public:
	TextWidget(std::string text, float r, float g, float b);
	
	std::string text;
	float r;
	float g;
	float b;
};

class HmiWidget
{
public:
	HmiWidget(ImageWidget imgWidget, float posx, float posy, float width, float height, int id);
	bool draw(ImVec2 windowCursor);
private:
	int id;
	HmiWidgetType widgetType;
	float posx;
	float posy;
	float width;
	float height;
	std::string label;
	ImageWidget imageConfig;
	//TextWidget textConfig;

};

bool loadTextureFromMemory(const void* data, size_t data_size, GLuint* out_texture, int* out_width, int* out_height);
bool loadTextureDataFromFile(std::filesystem::path filePath, GLuint* outTexture, int* outWidth, int* outHeight);

