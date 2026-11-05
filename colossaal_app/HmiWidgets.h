#pragma once
#include <vector>
#include <string>

#include "stb_image.h"
#include "imgui/GLFW/glfw3.h"
#include <filesystem>
#include <fstream>
#include "../colossal_cpp/link.h"

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
class NumericFieldWidget
{
public:
	NumericFieldWidget() = default;
	NumericFieldWidget(int linkId, int tagId, ImFont* font, Link* links);
	bool draw(ImVec2 windowCursor, float posx, float posy, float width, float height);
	
	std::string text;
	int linkId;
	int tagId;
	ImFont* font;
	Link* links;
};
class TextWidget
{
public:
	TextWidget() = default;
	TextWidget(std::string text, int r, int g, int b, ImFont* font);
	bool draw(ImVec2 windowCursor, float posx, float posy);
	
	std::string text;
	int r;
	int g;
	int b;
	ImFont* font;
};

class HmiWidget
{
public:
	HmiWidget(ImageWidget imgWidget, float posx, float posy, float width, float height, int id);
	HmiWidget(TextWidget txtWidget, float posx, float posy, float width, float height, int id);
	HmiWidget(NumericFieldWidget numFieldWidget, float posx, float posy, float width, float height, int id);
	//HmiWidget(NumericFieldWidget numFieldWidget, float posx, float posy, float width, float height, int id);
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
	TextWidget textConfig;
	NumericFieldWidget numFieldConfig;

};

bool loadTextureFromMemory(const void* data, size_t data_size, GLuint* out_texture, int* out_width, int* out_height);
bool loadTextureDataFromFile(std::filesystem::path filePath, GLuint* outTexture, int* outWidth, int* outHeight);

