#include "HmiWidgets.h"





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


NumericFieldWidget::NumericFieldWidget(int linkId, int tagId, ImFont* font, Link* links) : linkId(linkId), tagId(tagId), font(font), links(links)
{
}

bool NumericFieldWidget::draw(ImVec2 windowCursor, float posx, float posy, float width, float height)
{
	if (linkId < 0 || linkId > N_DEVICES || tagId < 0 || tagId > N_CHANNELS)
	{
		return false;
	}
	
	float px = windowCursor.x + posx;
	float py = windowCursor.y + posy;
	
	Tag tag = links[linkId].tags[tagId];
	
	switch (tag.value_type)
	{
	case CL_VALUE_REAL: this->text = std::format("{:.2f}", tag.tag_value.real_value); break;
	case CL_VALUE_INT: this->text = std::format("{}", tag.tag_value.int_value); break;
	case CL_VALUE_BOOL: this->text = tag.tag_value.bool_value ? std::format("ON") : std::format("OFF"); break;
	default: this->text = std::format("N/A"); break;
	}
	
	ImVec2 minPos = ImVec2(px, py);
	ImVec2 maxPos = ImVec2(px + width, py + height);
	
	if (links[linkId].is_error)
	{
		ImGui::GetWindowDrawList()->AddRectFilled(minPos, maxPos, IM_COL32(0, 0, 0, 100), 0.0f);

	}
	else if (tag.value_type == CL_VALUE_BOOL)
	{
		ImGui::GetWindowDrawList()->AddRectFilled(minPos, maxPos, tag.tag_value.bool_value ? IM_COL32(0, 200, 0, 255) : IM_COL32(255, 0, 0, 255), 0.0f);
	}
	else
	{
		ImGui::GetWindowDrawList()->AddRectFilled(minPos, maxPos, IM_COL32(0, 0, 0, 255), 0.0f);
	}
	
	ImGui::GetWindowDrawList()->AddRect(minPos, maxPos, IM_COL32(200, 200, 200, 255), 0.0f, 0, 5.0f);
	
	ImGui::PushFont(font);
	ImVec2 textSize = ImGui::CalcTextSize(this->text.c_str());
	ImGui::PopFont();
	
	ImVec2 textPos = ImVec2(px + (width - textSize.x) * 0.5f, py + (height - textSize.y) * 0.5f);
	ImGui::GetWindowDrawList()->AddText(this->font, this->font->FontSize, textPos, IM_COL32_WHITE, this->text.c_str());
	
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

HmiWidget::HmiWidget(TextWidget txtWidget, float posx, float posy, float width, float height, int id)
{
	this->widgetType = HmiWidgetType::Text;
	this->textConfig = txtWidget;
	this->id = id;
	this->posx = posx;
	this->posy = posy;
	
	// Calculate text size.
	this->width = width;
	this->height = height;

}

HmiWidget::HmiWidget(NumericFieldWidget numFieldWidget, float posx, float posy, float width, float height, int id)
{
	this->widgetType = HmiWidgetType::NumericField;
	this->numFieldConfig = numFieldWidget;
	this->id = id;
	this->height = height;
	this->width = width;
	this->posx = posx;
	this->posy = posy;
}


TextWidget::TextWidget(std::string text, int r, int g, int b, ImFont* font) : text(text), r(r), g(g), b(b), font(font)
{
}

bool TextWidget::draw(ImVec2 windowCursor, float posx, float posy)
{
	float px = windowCursor.x + posx;
	float py = windowCursor.y + posy;
	ImGui::GetWindowDrawList()->AddText(this->font, this->font->FontSize, ImVec2(px, py), IM_COL32(r, g, b, 255), this->text.c_str());
	return true;
}

bool HmiWidget::draw(ImVec2 windowCursor)
{
	switch (widgetType)
	{
	case HmiWidgetType::Image: imageConfig.draw(windowCursor, posx, posy, width, height); break;
	case HmiWidgetType::Text: textConfig.draw(windowCursor, posx, posy); break;
	case HmiWidgetType::NumericField: numFieldConfig.draw(windowCursor, posx, posy, width, height); break;
	}
	return true;
}
