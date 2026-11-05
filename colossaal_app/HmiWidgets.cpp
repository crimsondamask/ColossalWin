#include "HmiWidgets.h"


ImageWidget::ImageWidget(std::string path) : imgWidgetPath(path), texture(0), imgHeight(0), imgWidth(0)
{

}

bool ImageWidget::setTexture()
{
	if (!loadTextureDataFromFile(std::filesystem::path(imgWidgetPath), &texture, &imgWidth, &imgHeight))
	{
		return false;
	}

	return true;
}

bool ImageWidget::draw(ImVec2 windowCursor, float posx, float posy, float width, float height)
{
	float px = windowCursor.x + posx;
	float py = windowCursor.y + posy;

	ImVec2 minPos = ImVec2(px, py);
	ImVec2 maxPos = ImVec2(px + width, py + height);

	ImGui::GetWindowDrawList()->AddImage((ImTextureID)(intptr_t)texture, minPos, maxPos);

	return true;
}

TextWidget::TextWidget(std::string text, float r, float g, float b) : text(text), r(r), g(g), b(b)
{
}


bool loadTextureFromMemory(const void* data, size_t data_size, GLuint* out_texture, int* out_width, int* out_height)
{
	int image_width = 0;
	int image_height = 0;

	unsigned char* image_data =
		stbi_load_from_memory((const unsigned char*)data, (int)data_size, &image_width, &image_height, NULL, 4);

	if (image_data == NULL)
	{
		return false;
	}

	GLuint image_texture;
	glGenTextures(1, &image_texture);
	glBindTexture(GL_TEXTURE_2D, image_texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	stbi_image_free(image_data);

	*out_texture = image_texture;
	*out_width = image_width;
	*out_height = image_height;

	return true;
}


bool loadTextureDataFromFile(std::filesystem::path filePath, GLuint* outTexture, int* outWidth, int* outHeight)
{
	std::uintmax_t fileSize = std::filesystem::file_size(filePath);
	std::ifstream file(filePath, std::ios::binary);
	if (!file)
	{
		return false;
	}

	std::vector<char> buffer(fileSize);
	if (file.read(buffer.data(), fileSize))
	{
		bool ret = loadTextureFromMemory(buffer.data(), fileSize, outTexture, outWidth, outHeight);
		return ret;
	}
	else
	{
		return false;
	}

}

HmiWidget::HmiWidget(ImageWidget imgWidget, float posx, float posy, float width, float height, int id)
{
	this->widgetType = HmiWidgetType::Image;
	this->imageConfig = imgWidget;
	this->id = id;
	this->height = height;
	this->width = width;
	this->posx = posx;
	this->posy = posy;
}

bool HmiWidget::draw(ImVec2 windowCursor)
{
	switch (widgetType)
	{
	case HmiWidgetType::Image: imageConfig.draw(windowCursor, posx, posy, width, height); break;
	}
	return true;
}
