// colossal_cpp.cpp : This file contains the 'main' function. Program execution
// begins and ends there.
/// @file colossal.cpp

#include "mariadb/mysql.h"
#include "nlohmann/json.hpp"
#include <cmath>
#include <inttypes.h>
#include <stdlib.h>
#define _CRT_SECURE_NO_WARNINGS
#include "colossal.h"
// #include "curl/curl.h"
// #include "curl/easy.h"
#include "imgui/GLFW/glfw3.h"
#include "imgui/IconsFontAwesome4.h"
#include "imgui/imgui.h"
#include <fstream>
#include <iostream>
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/implot/implot.h"
#include "jansson/jansson.h"
#include "link.h"
#include "nfd.h"
#include "snap7/snap7.h"
#include <GL/gl.h>
#include <cstddef>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>
#include <time.h>
#include <filesystem>
#if defined(_WIN32)
#include <wincrypt.h>
#include <windows.h>
#endif

#if defined(_WIN32)
#define LOG_MARIADB_PORT 3333
#else
#define LOG_MARIADB_PORT 0
#endif

#define STB_IMAGE_IMPLEMENTATION
#define OPENSSL_API_1_0

#include "stb_image.h"

#define POSTDATA_BUF_STRLEN 2048
#define TAGSDATA_BUF_STRLEN 1024
#define TAGDATA_BUF_STRLEN 128

double* plot_time_data = (double*)malloc(1 * sizeof(double));
double* plot_value_data = (double*)malloc(1 * sizeof(double));

static int polling_thread(void* arg);

static bool load_config(Link links[])
{
	json_t* root;

	json_error_t* json_error = NULL;

	root = json_load_file("config.json", 0, json_error);

	if (!root)
	{
		return false;
	}

	if (!json_is_array(root))
	{
		json_decref(root);
		return false;
	}

	size_t array_size = json_array_size(root);

	if (array_size != N_DEVICES)
	{
		json_decref(root);
		return false;
	}

	for (size_t i = 0; i < array_size; i++)
	{
		json_t* link_json, * link_name_json, * link_tk_json, * protocol_json, * link_config_json, * mb_tcp_config_json,
			* ip_json, * url_json, * token_json, * tags_json, * logging_json, * tcp_port_json, * mb_serial_config_json,
			* serial_port_json, * serial_slave_json, * serial_baudrate_json, * serial_parity_json, * eip_config_json,
			* eip_ip_json, * s7_config_json, * s7_ip_json, * s7_rack_json, * s7_slot_json;

		tags_json = {};
		link_json = json_array_get(root, i);

		char link_name_buf[32];
#if defined(_WIN32)
		sprintf_s(link_name_buf, "LINK_%d", i);
#else
		snprintf(link_name_buf, sizeof(link_name_buf), "LINK_%zu", i);
#endif

		char link_tk_buf[32];
		snprintf(link_tk_buf, sizeof(link_tk_buf), "LK%zu:", i);

		MbTcpConfig mb_tcp_config;

#if defined(_WIN32)
		sprintf_s(mb_tcp_config.ip, "127.0.0.1");
#else
		snprintf(mb_tcp_config.ip, sizeof(mb_tcp_config.ip), "127.0.0.1");
#endif
		mb_tcp_config.port = 5502;

		MbSerialConfig mb_serial_config;

#if defined(_WIN32)
		sprintf_s(mb_serial_config.com_port, "COM3");
#else
		snprintf(mb_serial_config.com_port, sizeof(mb_serial_config.com_port), "COM3");
#endif

		mb_serial_config.baudrate = BR_9600;
		mb_serial_config.parity = CL_SERIAL_PARITY_NONE;

		S7Config s7_config;

#if defined(_WIN32)
		sprintf_s(s7_config.ip, "192.168.0.1");
#else
		snprintf(s7_config.ip, sizeof(s7_config.ip), "192.168.0.1");
#endif

		s7_config.rack = 0;
		s7_config.slot = 2;

		EipConfig eip_config;

#if defined(_WIN32)
		sprintf_s(eip_config.ip, "192.168.1.10");
#else
		snprintf(eip_config.ip, sizeof(eip_config.ip), "192.168.1.10");
#endif

#if defined(_WIN32)
		sprintf_s(eip_config.path, "1.0");
#else
		snprintf(eip_config.path, sizeof(eip_config.path), "1.0");
#endif

		LinkConfig link_config;
		link_config.mb_tcp_config = mb_tcp_config;
		link_config.mb_serial_config = mb_serial_config;
		link_config.s7_config = s7_config;
		link_config.eip_config = eip_config;

		Link link = {};
		link = *cl_new_link(link_name_buf, link_tk_buf, i, MB_TCP, link_config, N_CHANNELS, false);
		links[i] = link;

		if (!json_is_object(link_json))
		{
			json_decref(root);
			return false;
		}

		link_name_json = json_object_get(link_json, "name");

		if (!json_is_string(link_name_json))
		{
			json_decref(root);
			return false;
		}

#if defined(_WIN32)
		sprintf_s(links[i].name, "%s", json_string_value(link_name_json));
#else
		snprintf(links[i].name, sizeof(links[i].name), "%s", json_string_value(link_name_json));
#endif

		link_tk_json = json_object_get(link_json, "tk");

		if (!json_is_string(link_tk_json))
		{
			json_decref(root);
			return false;
		}

		snprintf(links[i].tk, sizeof(links[i].tk), "%s", json_string_value(link_tk_json));

		protocol_json = json_object_get(link_json, "protocol");

		if (!json_is_integer(protocol_json))
		{
			json_decref(root);
			return false;
		}

		links[i].protocol = json_integer_value(protocol_json);

		url_json = json_object_get(link_json, "url");

		if (!json_is_string(url_json))

		{
			json_decref(root);
			return false;
		}

#if defined(_WIN32)
#else
#endif

		token_json = json_object_get(link_json, "token");

		if (!json_is_string(token_json))

		{
			json_decref(root);
			return false;
		}

		logging_json = json_object_get(link_json, "logging");

		if (!json_is_integer(logging_json))
		{
			json_decref(root);
			return false;
		}

		links[i].logging_type = json_integer_value(logging_json);

		link_config_json = json_object_get(link_json, "link_config");

		if (!json_is_object(link_config_json))
		{
			json_decref(root);
			return false;
		}

		mb_tcp_config_json = json_object_get(link_config_json, "mb_tcp_config");

		if (!json_is_object(mb_tcp_config_json))
		{
			json_decref(root);
			return false;
		}

		ip_json = json_object_get(mb_tcp_config_json, "ip");

		if (!json_is_string(ip_json))
		{
			json_decref(root);
			return false;
		}

		sprintf(links[i].link_config.mb_tcp_config.ip, "%s", json_string_value(ip_json));

		tcp_port_json = json_object_get(mb_tcp_config_json, "port");

		if (!json_is_integer(tcp_port_json))
		{
			json_decref(root);
			return false;
		}

		links[i].link_config.mb_tcp_config.port = json_integer_value(tcp_port_json);

		mb_serial_config_json = json_object_get(link_config_json, "mb_serial_config");

		if (!json_is_object(mb_serial_config_json))
		{
			json_decref(root);
			return false;
		}

		serial_port_json = json_object_get(mb_serial_config_json, "serial_port");

		if (!json_is_string(serial_port_json))
		{
			json_decref(root);
			return false;
		}
		serial_slave_json = json_object_get(mb_serial_config_json, "slave");

		if (!json_is_integer(serial_slave_json))
		{
			json_decref(root);
			return false;
		}

		// links[i].link_config.mb_serial_config.slave = json_integer_value(serial_slave_json);

		serial_baudrate_json = json_object_get(mb_serial_config_json, "baudrate");

		if (!json_is_integer(serial_baudrate_json))
		{
			json_decref(root);
			return false;
		}

		links[i].link_config.mb_serial_config.baudrate = json_integer_value(serial_baudrate_json);

		serial_parity_json = json_object_get(mb_serial_config_json, "parity");

		if (!json_is_string(serial_parity_json))
		{
			json_decref(root);
			return false;
		}

		if (json_string_length(serial_parity_json) > 0)
		{
			links[i].link_config.mb_serial_config.parity = json_string_value(serial_parity_json)[0];
		}
		else
		{
			json_decref(root);
			return false;
		}

		eip_config_json = json_object_get(link_config_json, "eip_config");

		if (!json_is_object(eip_config_json))
		{
			json_decref(root);
			return false;
		}

		eip_ip_json = json_object_get(eip_config_json, "ip");

		if (!json_is_string(eip_ip_json))
		{
			json_decref(root);
			return false;
		}

		sprintf(links[i].link_config.eip_config.ip, "%s", json_string_value(eip_ip_json));

		s7_config_json = json_object_get(link_config_json, "s7_config");

		if (!json_is_object(s7_config_json))
		{
			json_decref(root);
			return false;
		}

		s7_ip_json = json_object_get(s7_config_json, "ip");

		if (!json_is_string(s7_ip_json))
		{
			json_decref(root);
			return false;
		}

		sprintf(links[i].link_config.s7_config.ip, "%s", json_string_value(s7_ip_json));

		s7_rack_json = json_object_get(s7_config_json, "rack");

		if (!json_is_integer(s7_rack_json))
		{
			json_decref(root);
			return false;
		}

		links[i].link_config.s7_config.rack = json_integer_value(s7_rack_json);

		s7_slot_json = json_object_get(s7_config_json, "slot");

		if (!json_is_integer(s7_slot_json))
		{
			json_decref(root);
			return false;
		}

		links[i].link_config.s7_config.slot = json_integer_value(s7_slot_json);

		tags_json = json_object_get(link_json, "tags");

		if (!json_is_array(tags_json))
		{
			json_decref(root);
			return false;
		}

		if (json_array_size(tags_json) >= N_CHANNELS)
		{
			json_decref(root);
			return false;
		}

		for (size_t j = 0; j < json_array_size(tags_json); j++)
		{
			json_t* tag_json, * tag_name_json, * tag_description_json, * tag_unit_json, * tag_enabled_json,
				* tag_logged_json, * value_type_json, * tag_address_json, * mb_addr_json, * eip_addr_json,
				* eip_tag_name_json, * s7_addr_json, * s7_db, * s7_start, * s7_start_bit;

			tag_json = json_array_get(tags_json, j);

			if (!json_is_object(tag_json))
			{
				json_decref(root);
				return false;
			}

			tag_name_json = json_object_get(tag_json, "name");

			if (!json_is_string(tag_name_json))
			{
				json_decref(root);
				return false;
			}

			sprintf(links[i].tags[j].name, "%s", json_string_value(tag_name_json));

			tag_description_json = json_object_get(tag_json, "description");

			if (!json_is_string(tag_description_json))
			{
				json_decref(root);
				return false;
			}
			sprintf(links[i].tags[j].description, "%s", json_string_value(tag_description_json));

			tag_unit_json = json_object_get(tag_json, "unit");

			if (!json_is_string(tag_unit_json))
			{
				json_decref(root);
				return false;
			}

			sprintf(links[i].tags[j].unit, "%s", json_string_value(tag_unit_json));

			tag_enabled_json = json_object_get(tag_json, "enabled");

			if (!json_is_integer(tag_enabled_json))
			{
				json_decref(root);
				return false;
			}

			links[i].tags[j].enabled = (bool)json_integer_value(tag_enabled_json);
			tag_logged_json = json_object_get(tag_json, "logged");

			if (!json_is_integer(tag_logged_json))
			{
				json_decref(root);
				return false;
			}

			links[i].tags[j].logged = (bool)json_integer_value(tag_logged_json);
			value_type_json = json_object_get(tag_json, "value_type");

			if (!json_is_integer(value_type_json))
			{
				json_decref(root);
				return false;
			}

			links[i].tags[j].value_type = json_integer_value(value_type_json);

			tag_address_json = json_object_get(tag_json, "tag_address");

			if (!json_is_object(tag_address_json))
			{
				json_decref(root);
				return false;
			}

			mb_addr_json = json_object_get(tag_address_json, "mb_address");

			if (!json_is_integer(mb_addr_json))
			{
				json_decref(root);
				return false;
			}

			links[i].tags[j].tag_addr.mb_addr.reg = json_integer_value(mb_addr_json);

			eip_addr_json = json_object_get(tag_address_json, "ab_address");

			if (!json_is_object(eip_addr_json))
			{
				json_decref(root);
				return false;
			}
			eip_tag_name_json = json_object_get(eip_addr_json, "tag");

			if (!json_is_string(eip_tag_name_json))
			{
				json_decref(root);
				return false;
			}
			sprintf(links[i].tags[j].tag_addr.eip_tag_addr.tag_name, "%s", json_string_value(eip_tag_name_json));

			s7_addr_json = json_object_get(tag_address_json, "s7_address");

			if (!json_is_object(s7_addr_json))
			{
				json_decref(root);
				return false;
			}

			s7_db = json_object_get(s7_addr_json, "db");

			if (!json_is_integer(s7_db))
			{
				json_decref(root);
				return false;
			}

			links[i].tags[j].tag_addr.s7_tag_addr.db_number = json_integer_value(s7_db);

			s7_start = json_object_get(s7_addr_json, "start");

			if (!json_is_integer(s7_start))
			{
				json_decref(root);
				return false;
			}

			links[i].tags[j].tag_addr.s7_tag_addr.start = json_integer_value(s7_start);

			s7_start_bit = json_object_get(s7_addr_json, "start_bit");

			if (!json_is_integer(s7_start_bit))
			{
				json_decref(root);
				return false;
			}

			links[i].tags[j].tag_addr.s7_tag_addr.start_bit = json_integer_value(s7_start_bit);
		}
	}

	return true;
}

static bool loadTextureFromMemory(const void* data, size_t data_size, GLuint* out_texture, int* out_width, int* out_height)
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


static bool loadTextureDataFromFile(std::filesystem::path filePath, GLuint* outTexture, int* outWidth, int* outHeight)
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

static void ui_tag_display(size_t link_count, Link links[], HmiDisplay hmi_display[], int id, Buffer buf[],
	bool* menu_state, ImFont* display_font, ImFont* regular_font, ImFont* icons)
{

	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));

	int tag_id = hmi_display[id].tag_id;
	int link_id = hmi_display[id].link_id;
	Buffer buffer = buf[link_id];
	char tag_display_title[32];
#if defined(_WIN32)
	sprintf_s(tag_display_title, "Disp:%d", id);
#else
	snprintf(tag_display_title, sizeof(tag_display_title), "Disp:%d", id);
#endif

	ImGui::PushStyleColor(ImGuiCol_TabUnfocused, hmi_display[id].plot_color);
	ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(0, 0, 0, 255));
	if (ImGui::Begin(tag_display_title, menu_state))
	{
		ImGui::PopStyleColor();

		ImGui::PopStyleColor();
		ImGuiStyle style = ImGui::GetStyle();

		ImGui::StyleColorsDark();
		// ImFont *icons_font = icons;

		ImGui::PushFont(display_font);

		char tag_display_name[32];
		snprintf(tag_display_name, sizeof(tag_display_name), "%s", links[link_id].tags[tag_id].name);

		ImGui::BeginGroup();

		// ImGui::Text("%s", tag_display_name);

		// ImGui::PushStyleColor(ImGuiCol_Header, hmi_display[id].plot_color);
		ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0, 0, 0, 255));
		ImGui::PushStyleColor(ImGuiCol_Text, hmi_display[id].plot_color);
		if (ImGui::CollapsingHeader(tag_display_name, ImGuiTreeNodeFlags_Bullet))
		{
			ImGui::PopStyleColor();
			char const* combo_array[N_CHANNELS] = {};

			for (int i = 0; i < N_CHANNELS; i++)
			{
				combo_array[i] = links[link_id].tags[i].name;
			}
			ImGui::Combo("Tag", &hmi_display[id].tag_id, combo_array, IM_ARRAYSIZE(combo_array));
			ImGui::ColorEdit4("Color", (float*)&hmi_display[id].plot_color);

			ImGui::Checkbox("Show Trend", &hmi_display[id].show_plot);
		}
		else
		{
			ImGui::PopStyleColor();
		}

		ImGui::PopStyleColor();

		ImColor value_color = {};
		value_color.Value.x = hmi_display[id].plot_color.x;
		value_color.Value.y = hmi_display[id].plot_color.y;
		value_color.Value.z = hmi_display[id].plot_color.z;
		value_color.Value.w = hmi_display[id].plot_color.w;

		ImGui::PushStyleColor(ImGuiCol_Text, hmi_display[id].plot_color);

		switch (links[link_id].tags[tag_id].value_type)
		{
		case VALUE_REAL:
			ImGui::Text("%0.3f", links[link_id].tags[tag_id].tag_value.real_value);
			break;
		case VALUE_INT:
			ImGui::Text("%d", links[link_id].tags[tag_id].tag_value.int_value);
			break;
		case VALUE_BOOL:
			ImGui::Text("%d", links[link_id].tags[tag_id].tag_value.bool_value);
			break;
		default:
			ImGui::Text("%0.3f", links[link_id].tags[tag_id].tag_value.real_value);
			break;
		}

		ImGui::PopStyleColor();

		ImGui::Text("%s", links[link_id].tags[tag_id].unit);

		ImGui::EndGroup();

		ImGui::PopFont();

		ImGui::PushFont(regular_font);
		ImGui::PopFont();

		ImVec2 win_size = ImGui::GetWindowSize();
		ImVec2 plot_size = {};
		plot_size.x = 0.95 * win_size.x;
		plot_size.y = 0.95f * (win_size.y - 150.0f);

		int plot_line_flags = ImPlotLineFlags_None;
		int plot_flags = ImPlotLineFlags_None | ImPlotFlags_Crosshairs | ImPlotFlags_NoLegend | ImPlotFlags_NoTitle;

		if (hmi_display[id].show_plot)
		{
			if (ImPlot::BeginPlot("Tag Plot", plot_size, plot_flags))
			{
				double* tag_time_data = (double*)malloc(buffer.tip * sizeof(double));
				double* tag_value_data = (double*)malloc(buffer.tip * sizeof(double));

				for (size_t i = 0; i < buffer.tip; i++)
				{
					Tag tag = buffer.link[i].tags[tag_id];
					double time = (double)buffer.link[i].timestamp;
					tag_time_data[i] = time;

					switch (tag.value_type)
					{
					case VALUE_REAL:
						tag_value_data[i] = tag.tag_value.real_value;
						break;

					case VALUE_INT:
						tag_value_data[i] = (double)tag.tag_value.int_value;
						break;
					case VALUE_BOOL:
						tag_value_data[i] = (double)tag.tag_value.bool_value;
						break;
					default:
						tag_value_data[i] = tag.tag_value.real_value;
						break;
					}
				}

				double x_axis_min = 0;
				double x_axis_max = 60;

				if (buffer.tip > 1)
				{
					x_axis_min = (double)(buffer.link[buffer.tip - 1].timestamp > TAG_DISPLAY_PERIOD
						? buffer.link[buffer.tip - 1].timestamp - TAG_DISPLAY_PERIOD
						: 0);
					x_axis_max = (double)(buffer.link[buffer.tip - 1].timestamp + 10);
				}

				ImPlot::SetupAxisLimits(ImAxis_X1, x_axis_min, x_axis_max, ImPlotCond_Always);

				double y_axis_min = 0;
				double y_axis_max = 100;

				int buf_count = (buffer.tip < TAG_DISPLAY_PERIOD) ? (buffer.tip - 1) : TAG_DISPLAY_PERIOD;

				if (buffer.tip > 1)
				{

					float max = 0.0f;

					for (int i = 0; i < buf_count; i++)
					{
						switch (buffer.link[buffer.tip - 1].tags[tag_id].value_type)
						{
						case VALUE_REAL:
							if (buffer.link[buffer.tip - buf_count + i].tags[tag_id].tag_value.real_value > max)
							{

								max = buffer.link[buffer.tip - buf_count + i].tags[tag_id].tag_value.real_value;
							}
							break;
						case VALUE_INT:
							if ((float)buffer.link[buffer.tip - buf_count + i].tags[tag_id].tag_value.int_value > max)
							{

								max = (float)buffer.link[buffer.tip - buf_count + i].tags[tag_id].tag_value.int_value;
							}
							break;
						case VALUE_BOOL:
							if ((float)buffer.link[buffer.tip - buf_count + i].tags[tag_id].tag_value.bool_value > max)
							{

								max = (float)buffer.link[buffer.tip - buf_count + i].tags[tag_id].tag_value.bool_value;
							}
							break;
						default:
							if (buffer.link[buffer.tip - buf_count].tags[tag_id].tag_value.real_value > max)
							{

								max = buffer.link[buffer.tip - buf_count].tags[tag_id].tag_value.real_value;
							}
							break;
						}
					}

					y_axis_max = max + (0.4 * max);
				}

				ImPlot::SetupAxisLimits(ImAxis_Y1, y_axis_min, y_axis_max, ImPlotCond_Always);
				ImPlot::GetStyle().Colormap = ImPlotColormap_Cool;
				ImPlot::GetStyle().Colors[ImPlotCol_FrameBg] = ImVec4(0, 0, 0, 255);
				ImPlot::GetStyle().Colors[ImPlotCol_Line] = hmi_display[id].plot_color;
				ImPlot::GetStyle().Colors[ImPlotCol_Crosshairs] = ImVec4(254, 254, 254, 250);
				ImPlot::GetStyle().Colors[ImPlotCol_PlotBorder] = ImVec4(0, 0, 0, 255);
				ImPlot::GetStyle().UseLocalTime = true;
				ImPlot::GetStyle().Use24HourClock = true;
				ImPlot::GetStyle().LineWeight = 2.0f;
				ImPlot::GetStyle().UseISO8601 = true;

				ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Time);
				ImPlot::SetupAxis(ImAxis_X1, NULL,
					ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoTickMarks |
					ImPlotAxisFlags_NoGridLines | ImPlotAxisFlags_NoDecorations);
				ImPlot::SetupAxis(ImAxis_Y1, NULL, ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_NoGridLines);

				ImPlot::PlotLine("Tag_Plot", tag_time_data, tag_value_data, buffer.tip - 1, plot_line_flags, 0,
					sizeof(double));

				ImPlot::EndPlot();

				free(tag_time_data);
				free(tag_value_data);
			}
		}

		ImGui::StyleColorsLight();
		ImGui::GetStyle() = style;
	}
	else
	{
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
	}
	ImGui::PopStyleColor();
	ImGui::End();
}

static void ui_about_window(bool* menu_state)
{

	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
	char about_window_title[32];
	sprintf_s(about_window_title, "About Colossal");
	if (ImGui::Begin(about_window_title, menu_state))
	{
		ImGui::PopStyleColor();
		ImGui::Text("Colossal V1.3.1 by Abdelkader Madoui.");
	}
	else
	{
		ImGui::PopStyleColor();
	}
	ImGui::End();
}

static void ui_tag_window(size_t link_count, Link links[], Link ui_link_buffers[], bool* menu_state,
	ConfigUpdate config_update[], int selected_link_index, int* selected_tag_index,
	int config_edit_flags[])
{
	Link* ui_buffer = &ui_link_buffers[selected_link_index];
	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
	char tag_window_title[32];
#if defined(_WIN32)
	sprintf_s(tag_window_title, "Properties");
#else
	snprintf(tag_window_title, sizeof(tag_window_title), "Properties");
#endif

	if (ImGui::Begin(tag_window_title, menu_state))
	{
		ImGui::PopStyleColor();

		if (ImGui::Button("Back"))
		{
			if (!(*selected_tag_index == 0))
			{
				(*selected_tag_index)--;
			}
		}
		ImGui::SameLine(0.0f, 20.0f);
		if (ImGui::Button("Next"))
		{
			if (!(*selected_tag_index == (N_CHANNELS - 1)))
			{
				(*selected_tag_index)++;
			}
		}
		ImGui::BeginDisabled(!ui_buffer->tags[*selected_tag_index].enabled);

		ImGui::Text("%s:%s Details", links[selected_link_index].name,
			links[selected_link_index].tags[*selected_tag_index].name);

		if (ImGui::InputText("Tag", ui_buffer->tags[*selected_tag_index].name,
			IM_ARRAYSIZE(ui_buffer->tags[*selected_tag_index].name), ImGuiInputTextFlags_CharsNoBlank))
		{
			config_edit_flags[selected_link_index] |= CONFIG_EDIT_CHANNEL_CONFIG;
		}

		if (ImGui::InputText("Description", ui_buffer->tags[*selected_tag_index].description,
			IM_ARRAYSIZE(ui_buffer->tags[*selected_tag_index].description)))
		{
			config_edit_flags[selected_link_index] |= CONFIG_EDIT_CHANNEL_CONFIG;
		}
		if (ImGui::InputText("Unit", ui_buffer->tags[*selected_tag_index].unit,
			IM_ARRAYSIZE(ui_buffer->tags[*selected_tag_index].unit)))
		{
			config_edit_flags[selected_link_index] |= CONFIG_EDIT_CHANNEL_CONFIG;
		}

		// Show the tag value options and address depending on the protocol.
		switch (ui_buffer->protocol)
		{
		case MB_SERIAL: {
			// MB_SERIAL and MB_TCP use the same protocol.
			// We let it leak into the next case
		}
		case MB_TCP: {

			const char* function_type[] = { "HOLDING", "INPUT", "COIL" };
			if (ImGui::Combo("Function Type", &ui_buffer->tags[*selected_tag_index].tag_addr.mb_addr.mb_function,
				function_type, IM_ARRAYSIZE(function_type)))
			{
				config_edit_flags[selected_link_index] |= CONFIG_EDIT_CHANNEL_CONFIG;
			}
			const char* value_types[] = { "INT", "REAL - Uses 2 registers", "COIL" };
			if (ImGui::Combo("Value Type", &ui_buffer->tags[*selected_tag_index].value_type, value_types,
				IM_ARRAYSIZE(value_types)))
			{
				config_edit_flags[selected_link_index] |= CONFIG_EDIT_CHANNEL_CONFIG;
			}
			if (ImGui::InputInt("Slave", &ui_buffer->tags[*selected_tag_index].tag_addr.mb_addr.slave))
			{
				config_edit_flags[selected_link_index] |= CONFIG_EDIT_CHANNEL_CONFIG;
			}
			if (ImGui::InputInt("Address", &ui_buffer->tags[*selected_tag_index].tag_addr.mb_addr.reg))
			{
				config_edit_flags[selected_link_index] |= CONFIG_EDIT_CHANNEL_CONFIG;
			}
			break;
		}
		case SIEMENS_S7: {

			const char* value_types[] = { "INT (16bit)", "REAL (32bit)", "BIT" };
			if (ImGui::Combo("Value Type", &ui_buffer->tags[*selected_tag_index].value_type, value_types,
				IM_ARRAYSIZE(value_types)))
			{
				config_edit_flags[selected_link_index] |= CONFIG_EDIT_CHANNEL_CONFIG;
			}
			if (ImGui::InputInt("DB", &ui_buffer->tags[*selected_tag_index].tag_addr.s7_tag_addr.db_number))
			{
				config_edit_flags[selected_link_index] |= CONFIG_EDIT_CHANNEL_CONFIG;
			}
			if (ImGui::InputInt("Offset", &ui_buffer->tags[*selected_tag_index].tag_addr.s7_tag_addr.start))
			{
				config_edit_flags[selected_link_index] |= CONFIG_EDIT_CHANNEL_CONFIG;
			}
			if (ImGui::InputInt("Bit", &ui_buffer->tags[*selected_tag_index].tag_addr.s7_tag_addr.start_bit))
			{
				config_edit_flags[selected_link_index] |= CONFIG_EDIT_CHANNEL_CONFIG;
			}
			break;
		}
		case EIP: {

			const char* value_types[] = { "INT (16bit)", "REAL (32bit)", "BIT" };
			if (ImGui::Combo("Value Type", &ui_buffer->tags[*selected_tag_index].value_type, value_types,
				IM_ARRAYSIZE(value_types)))
			{
				config_edit_flags[selected_link_index] |= CONFIG_EDIT_CHANNEL_CONFIG;
			}
			if (ImGui::InputText("PLC Tag", ui_buffer->tags[*selected_tag_index].tag_addr.eip_tag_addr.tag_name,
				IM_ARRAYSIZE(ui_buffer->tags[*selected_tag_index].tag_addr.eip_tag_addr.tag_name),
				ImGuiInputTextFlags_CharsNoBlank))
			{
#if defined(_WIN32)
				sprintf_s(ui_buffer->tags[*selected_tag_index].tag_addr.eip_tag_addr.eip_path, EIP_TAG_TEMPLATE,
					ui_buffer->link_config.eip_config.ip,
					ui_buffer->tags[*selected_tag_index].tag_addr.eip_tag_addr.tag_name);
#else
				snprintf(ui_buffer->tags[*selected_tag_index].tag_addr.eip_tag_addr.eip_path,
					sizeof(ui_buffer->tags[*selected_tag_index].tag_addr.eip_tag_addr.eip_path), EIP_TAG_TEMPLATE,
					ui_buffer->link_config.eip_config.ip,
					ui_buffer->tags[*selected_tag_index].tag_addr.eip_tag_addr.tag_name);
#endif
				config_edit_flags[selected_link_index] |= CONFIG_EDIT_CHANNEL_CONFIG;
			}

			ImGui::InputText("EIP Path",
				links[selected_link_index].tags[*selected_tag_index].tag_addr.eip_tag_addr.eip_path,
				IM_ARRAYSIZE(ui_buffer->tags[*selected_tag_index].tag_addr.eip_tag_addr.tag_name),
				ImGuiInputTextFlags_ReadOnly);
			break;
		}

		default: {
			break;
		}
		}
		ImGui::EndDisabled();

		if (ImGui::Checkbox("Enabled", &ui_buffer->tags[*selected_tag_index].enabled))
		{
			config_edit_flags[selected_link_index] |= CONFIG_EDIT_CHANNEL_CONFIG;
		}

		ImGui::SameLine();
		if (ImGui::Checkbox("Logged", &ui_buffer->tags[*selected_tag_index].logged))
		{
			config_edit_flags[selected_link_index] |= CONFIG_EDIT_CHANNEL_CONFIG;
		}

		switch (ui_buffer->tags[*selected_tag_index].value_type)
		{
		case VALUE_REAL:
			if (ImGui::InputFloat("Value to Write", &ui_buffer->tags[*selected_tag_index].value_to_write.real_value))
			{
				config_edit_flags[selected_link_index] |= CONFIG_EDIT_CHANNEL_CONFIG;
			}
			break;
		case VALUE_INT:
			if (ImGui::InputInt("Value to Write", &ui_buffer->tags[*selected_tag_index].value_to_write.int_value))
			{
				config_edit_flags[selected_link_index] |= CONFIG_EDIT_CHANNEL_CONFIG;
			}
			break;
		case VALUE_BOOL:
			if (ImGui::Checkbox("Value to Write", &ui_buffer->tags[*selected_tag_index].value_to_write.bool_value))
			{
				config_edit_flags[selected_link_index] |= CONFIG_EDIT_CHANNEL_CONFIG;
			}
			break;
		default:
			if (ImGui::InputFloat("Value to Write", &ui_buffer->tags[*selected_tag_index].value_to_write.real_value))
			{
				config_edit_flags[selected_link_index] |= CONFIG_EDIT_CHANNEL_CONFIG;
			}
			break;
		}

		if (ImGui::Button("Write"))
		{

			if (config_update_put(&config_update[selected_link_index], ui_buffer, false, true, *selected_tag_index))
			{

				// Reset the config change indication flags.
			}
		}
	}
	else
	{

		ImGui::PopStyleColor();
	}
	ImGui::End();
}

static void ui_plot_window(size_t link_count, Link links[], int selected_link, int selected_tag,
	PlotDataBuffer plot_buffers[N_DEVICES][N_CHANNELS], bool open)
{

	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
	char plot_window_title[32];
	sprintf_s(plot_window_title, "Plot Data");

	if (ImGui::Begin(plot_window_title, &open))
	{

		ImVec2 win_size = ImGui::GetWindowSize();
		ImVec2 plot_size = {};
		plot_size.x = 0.95 * win_size.x;
		plot_size.y = 0.95f * (win_size.y - 150.0f);

		int plot_line_flags = ImPlotLineFlags_None;
		int plot_flags = ImPlotLineFlags_None | ImPlotFlags_Crosshairs | ImPlotFlags_NoLegend | ImPlotFlags_NoTitle;

		ImGui::PopStyleColor();
		if (ImPlot::BeginPlot("Tag Plot", plot_size, plot_flags))
		{

			double* tag_time_data = plot_buffers[selected_link][selected_tag].time_data();
			double* tag_data = plot_buffers[selected_link][selected_tag].tag_data();

			int tag_data_size = plot_buffers[selected_link][selected_tag].tag_data_size();

			double x_axis_min = 0;
			double x_axis_max = 60;

			if (tag_data_size != 0 && tag_data[tag_data_size - 1] > 1000000.0)
			{
				x_axis_min = tag_time_data[tag_data_size - 1] - 60.0;
				x_axis_max = tag_time_data[tag_data_size - 1];
			}

			ImPlot::SetupAxisLimits(ImAxis_X1, x_axis_min, x_axis_max, ImPlotCond_Once);

			ImPlot::GetStyle().UseLocalTime = true;
			ImPlot::GetStyle().Use24HourClock = true;
			ImPlot::GetStyle().LineWeight = 3.0f;
			ImPlot::GetStyle().UseISO8601 = true;

			ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Time);
			// ImPlot::SetupAxis(ImAxis_X1, NULL, ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_NoGridLines);
			// ImPlot::SetupAxis(ImAxis_Y1, NULL, ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_NoGridLines);

			ImPlot::PlotLine("Tag_Plot", tag_time_data, tag_data, tag_data_size, plot_line_flags, 0, sizeof(double));

			ImPlot::EndPlot();
		}
	}
	else
	{
		ImGui::PopStyleColor();
	}
	ImGui::End();
}

static void ui_evals_window(size_t link_count, Link links[], size_t eval_count, ClEval evals[],
	ClEval ui_eval_buffers[], bool* menu_state, int* selected_eval_index,
	bool* eval_config_change)
{

	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
	char evals_window_title_buf[32];

	snprintf(evals_window_title_buf, sizeof(evals_window_title_buf), "Calculations");

	if (ImGui::Begin(evals_window_title_buf, menu_state))
	{
		ImGui::PopStyleColor();

		char evals_header_title_buf[32];

		if (*eval_config_change)
		{
			snprintf(evals_header_title_buf, sizeof(evals_header_title_buf), "Configuration *");
		}
		else
		{
			snprintf(evals_header_title_buf, sizeof(evals_header_title_buf), "Configuration");
		}

		if (ImGui::CollapsingHeader(evals_header_title_buf))
		{

			if (ImGui::InputText("Name", ui_eval_buffers[*selected_eval_index].name,
				IM_ARRAYSIZE(ui_eval_buffers[*selected_eval_index].name)))
			{
				*eval_config_change = true;
			}

			if (ImGui::Checkbox("Enabled", &ui_eval_buffers[*selected_eval_index].enabled))
			{
				*eval_config_change = true;
			}

			const char* eval_type_arr[4] = { "EXPRESSION EVALUATION", "RANDOM GENERATOR", "ISO5167 RATE CALC",
											"SCRIPT EVALUATION" };

			if (ImGui::Combo("Type", &ui_eval_buffers[*selected_eval_index].eval_type, eval_type_arr,
				IM_ARRAYSIZE(eval_type_arr)))
			{
				*eval_config_change = true;
			}

			switch (ui_eval_buffers[*selected_eval_index].eval_type)
			{
			case CL_RAND_EVAL:
				if (ImGui::InputInt("MIN", (int*)&ui_eval_buffers[*selected_eval_index].eval_config.rand_config.min))
				{
					*eval_config_change = true;
				}
				if (ImGui::InputInt("MAX", (int*)&ui_eval_buffers[*selected_eval_index].eval_config.rand_config.max))
				{
					*eval_config_change = true;
				}
				break;
			case CL_EXPR_EVAL:
				if (ImGui::InputText("Expression", ui_eval_buffers[*selected_eval_index].eval_config.expr_config.expr,
					CL_EXPR_LEN - 1))
				{
					*eval_config_change = true;
				}
				break;
			case CL_ISO5167_EVAL:
				break;
			case CL_LUA_EVAL:
				if (ImGui::InputTextMultiline("Script",
					ui_eval_buffers[*selected_eval_index].eval_config.lua_config.script,
					CL_EXPR_LEN - 1, ImVec2(0.0f, 400.0f)))
				{
					*eval_config_change = true;
				}
				break;
			default:
				break;
			}

			const char* eval_val_type[3] = { "INT", "REAL", "BOOL" };

			if (ImGui::Combo("Value Type", &ui_eval_buffers[*selected_eval_index].value_type, eval_val_type,
				IM_ARRAYSIZE(eval_val_type)))
			{
				*eval_config_change = true;
			}
		}

		if (ImGui::Button("Reconfigure"))
		{
			for (int i = 0; i < N_EVALS; i++)
			{
				evals[i] = ui_eval_buffers[i];
			}

			*eval_config_change = false;
		}
		ImVec2 outer_size = ImVec2(0.0f, 400.0f);
		if (ImGui::BeginTable("Tag Data", 5,
			ImGuiTableFlags_Resizable | ImGuiTableFlags_Borders | ImGuiTableFlags_ScrollY |
			ImGuiTableFlags_ScrollX | ImGuiTableFlags_RowBg,
			outer_size))

		{

			ImGui::TableSetupColumn("NAME");
			ImGui::TableSetupColumn("IDENT");
			ImGui::TableSetupColumn("VALUE");
			ImGui::TableSetupColumn("TYPE");
			ImGui::TableSetupColumn("DETAILS");
			ImGui::TableSetupScrollFreeze(0, 1);
			ImGui::TableHeadersRow();
			for (int i = 0; i < N_EVALS; i++)
			{
				ImGui::PushID(i);
				char selectable_label[32];
				bool set_selected = false;

				snprintf(selectable_label, sizeof(selectable_label), "%s", evals[i].name);

				ImGui::TableNextRow();
				ImGui::TableNextColumn();

				if (*selected_eval_index == i)
				{
					set_selected = true;
				}
				else
				{
					set_selected = false;
				}
				if (ImGui::Selectable(selectable_label, set_selected, ImGuiSelectableFlags_SpanAllColumns))
				{
					*selected_eval_index = i;
				}
				ImGui::TableNextColumn();
				ImGui::Text("%s", evals[i].tk);
				ImGui::TableNextColumn();
				ImGui::BeginDisabled(!evals[i].enabled);

				if (!evals[i].enabled || evals[i].is_error)
				{
					ImGui::Text("ERROR: %s", evals[i].err_msg);
				}
				else
				{
					switch (evals[i].value_type)
					{
					case VALUE_REAL:
						ImGui::Text("%0.3f", evals[i].result.real_value);
						break;
					case VALUE_INT:
						ImGui::Text("%d", evals[i].result.int_value);
						break;
					case VALUE_BOOL:
						ImGui::Text("%d", evals[i].result.bool_value);
						break;
					}
				}
				ImGui::TableNextColumn();

				switch (evals[i].eval_type)
				{
				case CL_EXPR_EVAL:
					ImGui::Text("EXPRESSION");
					break;
				case CL_RAND_EVAL:
					ImGui::Text("RANDOM");
					break;
				case CL_ISO5167_EVAL:
					ImGui::Text("RATE CALC");
					break;
				default:
					ImGui::Text("EXPRESSION");
					break;
				}
				ImGui::TableNextColumn();
				switch (evals[i].eval_type)
				{
				case CL_EXPR_EVAL:
					ImGui::Text("%s", evals[i].eval_config.expr_config.expr);
					break;
				case CL_RAND_EVAL:
					ImGui::Text("min=%ld,max=%ld", evals[i].eval_config.rand_config.min,
						evals[i].eval_config.rand_config.max);
					break;
				case CL_ISO5167_EVAL:
					ImGui::Text("RATE CALC");
					break;
				default:
					ImGui::Text("%s", evals[i].eval_config.expr_config.expr);
					break;
				}
				ImGui::EndDisabled();
				ImGui::PopID();
			}
			ImGui::EndTable();
		}
	}
	else
	{
		ImGui::PopStyleColor();
	}
	ImGui::End();
}

static void ui_links_window(size_t link_count, Link links[], Link ui_link_buffers[], ConfigUpdate config_update[],
	bool* menu_state, int* selected_link_index, int* selected_tag_index,
	int config_edit_flags[])

{
	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));

	char links_window_title_buf[32];
#if defined(_WIN32)
	sprintf_s(links_window_title_buf, "Links");
#else
	snprintf(links_window_title_buf, sizeof(links_window_title_buf), "Links");
#endif
	if (ImGui::Begin(links_window_title_buf, menu_state))
	{
		ImGui::PopStyleColor();
		for (int i = 0; i < link_count; i++)
		{

			ImGui::PushID(i);

			Link* link = &links[i];
			// Used to hold UI data and persist it across frames.
			// The use of pointers here is important as we don't want
			// to just copy the buffer. We want to mutate the buffer state
			// outside of the event loop.
			Link* ui_buffer = &ui_link_buffers[i];

			// A little hack to show an asterics when the config is edited.
			char collapsing_header_title[2048];
			if (config_edit_flags[i])
			{

				if (link->protocol == SIEMENS_S7)
				{

#if defined(_WIN32)
					sprintf_s(collapsing_header_title, "%.2lf (us) %s %s %s Config *", link->elapsed_time * 1000000.0,
						link->name, link->link_config.s7_config.cpu_info.ModuleTypeName,
						link->link_config.s7_config.cpu_info.SerialNumber);
#else
					snprintf(collapsing_header_title, sizeof(collapsing_header_title), "%s %s %s Config *", link->name,
						link->link_config.s7_config.cpu_info.ModuleTypeName,
						link->link_config.s7_config.cpu_info.SerialNumber);
#endif
				}
				else
				{

#if defined(_WIN32)
					sprintf_s(collapsing_header_title, "%.2lf (us) %s Config *", link->elapsed_time * 1000000.0,
						link->name);
#else
					snprintf(collapsing_header_title, sizeof(collapsing_header_title), "%s Config *", link->name);
#endif
				}
			}
			else
			{
				if (link->protocol == SIEMENS_S7)
				{

#if defined(_WIN32)
					sprintf_s(collapsing_header_title, "%.2lf (us) %s %s %s Config", link->elapsed_time * 1000000.0,
						link->name, link->link_config.s7_config.cpu_info.ModuleTypeName,
						link->link_config.s7_config.cpu_info.ModuleName);
#else
					snprintf(collapsing_header_title, sizeof(collapsing_header_title), "%s %s %s Config", link->name,
						link->link_config.s7_config.cpu_info.ModuleTypeName,
						link->link_config.s7_config.cpu_info.ModuleName);
#endif
				}
				else
				{
#if defined(_WIN32)
					sprintf_s(collapsing_header_title, "%.2lf (us) %s Config", link->elapsed_time * 1000000.0,
						link->name);
#else
					snprintf(collapsing_header_title, sizeof(collapsing_header_title), "%s Config", link->name);
#endif
				}
			}

			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
			if (ImGui::CollapsingHeader(collapsing_header_title, ImGuiTreeNodeFlags_Bullet))
			{
				ImGui::PopStyleColor();
				if (ImGui::InputText("Name", ui_buffer->name, IM_ARRAYSIZE(ui_link_buffers->name),
					ImGuiInputTextFlags_CharsNoBlank)

					)
				{
					config_edit_flags[i] |= CONFIG_EDIT_DEVICE_CONFIG;
				}

				const char* link_types[] = { "MODBUS TCP", "MODBUS SERIAL", "ALLEN BRADLEY EIP",
											"SIEMENS S7", "OPCUA",         "EVAL" };

				if (ImGui::Combo("Link Protocol", &ui_buffer->protocol, link_types, IM_ARRAYSIZE(link_types)))
				{
					config_edit_flags[i] |= CONFIG_EDIT_DEVICE_CONFIG;

					for (int tag_i = 0; tag_i < ui_buffer->tag_count; tag_i++)
					{
						ui_buffer->tags[tag_i].protocol = ui_buffer->protocol;
					}
				}

				if (ImGui::Checkbox("Active", &ui_buffer->active))
				{
					config_edit_flags[i] |= CONFIG_EDIT_DEVICE_CONFIG;
				}

				if (ImGui::InputInt("Poll Delay (ms)", &ui_buffer->poll_delay, 10))
				{
					config_edit_flags[i] |= CONFIG_EDIT_DEVICE_CONFIG;
				}

				switch (ui_buffer->protocol)
				{
				case MB_TCP: {

					if (ImGui::InputText("IP Address", ui_buffer->link_config.mb_tcp_config.ip,
						IM_ARRAYSIZE(ui_buffer->link_config.mb_tcp_config.ip)))
					{
						config_edit_flags[i] |= CONFIG_EDIT_DEVICE_CONFIG;
					}

					if (ImGui::InputInt("Port", &ui_buffer->link_config.mb_tcp_config.port))
					{
						config_edit_flags[i] |= CONFIG_EDIT_DEVICE_CONFIG;
					}
					if (ImGui::Checkbox("Swap contiguous float registers",
						&ui_buffer->link_config.mb_tcp_config.low_first))
					{
						config_edit_flags[i] |= CONFIG_EDIT_DEVICE_CONFIG;
					}
					break;
				}
				case MB_SERIAL: {

					if (ImGui::InputText("Serial Port", ui_buffer->link_config.mb_serial_config.com_port,
						IM_ARRAYSIZE(ui_buffer->link_config.mb_serial_config.com_port)))
					{
						config_edit_flags[i] |= CONFIG_EDIT_DEVICE_CONFIG;
					}

					const char* baudrates[] = { "9600", "19200", "38400", "115200" };
					static int baudrate = 0;

					switch (ui_buffer->link_config.mb_serial_config.baudrate)
					{
					case 9600:
						baudrate = 0;
						break;
					case 19200:
						baudrate = 1;
						break;
					case 38400:
						baudrate = 2;
						break;
					case 115200:
						baudrate = 3;
						break;
					default:
						baudrate = 0;
						break;
					}
					if (ImGui::Combo("Baudrate", &baudrate, baudrates, IM_ARRAYSIZE(baudrates)))
					{
						config_edit_flags[i] |= CONFIG_EDIT_DEVICE_CONFIG;
						switch (baudrate)
						{
						case 0:
							ui_buffer->link_config.mb_serial_config.baudrate = BR_9600;
							break;
						case 1:
							ui_buffer->link_config.mb_serial_config.baudrate = BR_19200;
							break;
						case 2:
							ui_buffer->link_config.mb_serial_config.baudrate = BR_38400;
							break;
						case 3:
							ui_buffer->link_config.mb_serial_config.baudrate = BR_115200;
							break;

						default:
							ui_buffer->link_config.mb_serial_config.baudrate = BR_9600;
							break;
						}
					}

					const char* parities[] = { "NONE", "EVEN", "ODD" };

					static int parity = 0;

					switch (ui_buffer->link_config.mb_serial_config.parity)
					{
					case 'N':
						parity = 0;
						break;
					case 'E':
						parity = 1;
						break;
					case 'O':
						parity = 2;
						break;
					default:
						parity = 0;
						break;
					}
					if (ImGui::Combo("Parity", &parity, parities, IM_ARRAYSIZE(parities)))
					{
						config_edit_flags[i] |= CONFIG_EDIT_DEVICE_CONFIG;
						switch (parity)
						{
						case 0:
							ui_buffer->link_config.mb_serial_config.parity = 'N';
							break;
						case 1:
							ui_buffer->link_config.mb_serial_config.parity = 'E';
							break;
						case 2:
							ui_buffer->link_config.mb_serial_config.parity = 'O';
							break;

						default:
							ui_buffer->link_config.mb_serial_config.parity = 'N';
							break;
						}
					}
					if (ImGui::Checkbox("Swap contiguous float registers",
						&ui_buffer->link_config.mb_serial_config.low_first))
					{
						config_edit_flags[i] |= CONFIG_EDIT_DEVICE_CONFIG;
					}
					break;
				}
				case SIEMENS_S7: {
					if (ImGui::InputText("IP Address", ui_buffer->link_config.s7_config.ip,
						IM_ARRAYSIZE(ui_buffer->link_config.s7_config.ip)))
					{
						config_edit_flags[i] |= CONFIG_EDIT_DEVICE_CONFIG;
					}

					if (ImGui::InputInt("Rack", &ui_buffer->link_config.s7_config.rack))
					{
						config_edit_flags[i] |= CONFIG_EDIT_DEVICE_CONFIG;
					}
					if (ImGui::InputInt("Slot", &ui_buffer->link_config.s7_config.slot))
					{
						config_edit_flags[i] |= CONFIG_EDIT_DEVICE_CONFIG;
					}
					break;
				}
				case EIP: {
					if (ImGui::InputText("IP Address", ui_buffer->link_config.eip_config.ip,
						IM_ARRAYSIZE(ui_buffer->link_config.eip_config.ip)))
					{
						config_edit_flags[i] |= CONFIG_EDIT_DEVICE_CONFIG;
					}
					break;
				}
				case OPCUA: {
					break;
				}
				case EVAL:
					ImGui::Text("EVALUATION LINK");
					break;
				default:
					break;
				}
			}
			else
			{

				ImGui::PopStyleColor();
			}
			// Button to send config update to the threads
			char reconfig_button_buf[32];
#if defined(_WIN32)
			sprintf_s(reconfig_button_buf, "Reconfigure");
#else
			snprintf(reconfig_button_buf, sizeof(reconfig_button_buf), "Reconfigure");
#endif

			if (ImGui::Button(reconfig_button_buf))
			{
				link = ui_buffer;
				if (config_update_put(&config_update[i], link, false, false, 0))
				{
					// Reset the config change indication flags.
					config_edit_flags[i] = 0;
				}
			}

			ImGui::SameLine();

			if (ImGui::Button("Reconnect"))
			{
				link = ui_buffer;
				if (config_update_put(&config_update[i], link, true, false, 0))
				{
					// Reset the config change indication flags.
					config_edit_flags[i] = 0;
				}
			}

			if (link->is_error)
			{
				ImGui::Text("Link ID: %d. ERROR: %s", link->id, link->err_msg);
			}
			ImVec2 outer_size = ImVec2(0.0f, 400.0f);
			if (ImGui::BeginTable("Tag Data", 7,
				ImGuiTableFlags_Resizable | ImGuiTableFlags_Borders | ImGuiTableFlags_ScrollY |
				ImGuiTableFlags_ScrollX | ImGuiTableFlags_RowBg,
				outer_size))

			{

				ImGui::TableSetupColumn("TAG");
				ImGui::TableSetupColumn("IDENT");
				ImGui::TableSetupColumn("VALUE");
				ImGui::TableSetupColumn("UNIT");
				ImGui::TableSetupColumn("TYPE");
				ImGui::TableSetupColumn("ADDRESS");
				ImGui::TableSetupColumn("DESC");
				ImGui::TableSetupScrollFreeze(0, 1);
				ImGui::TableHeadersRow();
				for (int j = 0; j < link->tag_count; j++)
				{
					char selectable_label[32];
					bool set_selected = false;
#if defined(_WIN32)
					sprintf_s(selectable_label, "%s", link->tags[j].name);
#else
					snprintf(selectable_label, sizeof(selectable_label), "%s", link->tags[j].name);
#endif
					ImGui::TableNextRow();
					ImGui::TableNextColumn();

					if (*selected_link_index == i && *selected_tag_index == j)
					{
						set_selected = true;
					}
					else
					{
						set_selected = false;
					}
					if (ImGui::Selectable(selectable_label, set_selected, ImGuiSelectableFlags_SpanAllColumns))
					{
						*selected_link_index = i;
						*selected_tag_index = j;
					}
					// ImGui::Text("CH%d", device->channels[j].id);
					ImGui::TableNextColumn();
					ImGui::Text("%s", link->tags[j].tk);
					ImGui::TableNextColumn();
					ImGui::BeginDisabled(!link->tags[j].enabled);

					if (!link->tags[j].enabled || link->tags[j].is_error)
					{
						ImGui::Text("ERROR: %s", link->tags[j].err_msg);
					}
					else
					{
						switch (link->tags[j].value_type)
						{
						case VALUE_REAL:
							ImGui::Text("%0.3f", link->tags[j].tag_value.real_value);
							break;
						case VALUE_INT:
							ImGui::Text("%d", link->tags[j].tag_value.int_value);
							break;
						case VALUE_BOOL:
							ImGui::Text("%d", link->tags[j].tag_value.bool_value);
							break;
						}
					}
					ImGui::TableNextColumn();
					ImGui::Text("%s", link->tags[j].unit);
					ImGui::TableNextColumn();

					switch (link->tags[j].value_type)
					{
					case VALUE_REAL:
						ImGui::Text("REAL");
						break;
					case VALUE_INT:
						ImGui::Text("INT");
						break;
					case VALUE_BOOL:
						ImGui::Text("BOOL");
						break;
					default:
						ImGui::Text("INT");
						break;
					}
					ImGui::TableNextColumn();
					switch (link->tags[j].protocol)
					{
					case MB_TCP:
					case MB_SERIAL:
						ImGui::Text("%d", link->tags[j].tag_addr.mb_addr.reg);
						break;
					case EIP:
						ImGui::Text("%s", link->tags[j].tag_addr.eip_tag_addr.tag_name);
						break;
					case SIEMENS_S7:
						ImGui::Text("DB%d:%d.%d", link->tags[j].tag_addr.s7_tag_addr.db_number,
							link->tags[j].tag_addr.s7_tag_addr.start,
							link->tags[j].tag_addr.s7_tag_addr.start_bit);
						break;

					default:
						ImGui::Text("%d", link->tags[j].tag_addr.mb_addr.reg);
						break;
					}
					ImGui::TableNextColumn();
					ImGui::Text("%s", link->tags[j].description);
					ImGui::EndDisabled();
				}
				ImGui::EndTable();
			}
			ImGui::PopID();
		}
	}
	else
	{

		ImGui::PopStyleColor();
	}
	ImGui::End();
}

static void glfw_error_callback(int error, const char* description);

struct CurlMemoryStruct
{
	char* memory;
	size_t size;
};

// static size_t WriteMemoryCallback(void *contents, size_t size, size_t
// nmemb,
//                                   void *userp) {
//     size_t                   realsize = size * nmemb;
//     struct CurlMemoryStruct *mem      = (struct CurlMemoryStruct *)userp;

//     char *ptr = (char *)realloc(mem->memory, mem->size + realsize + 1);
//     if ( ptr == NULL ) {
//         return 0;
//     }
//     mem->memory = ptr;
//     memcpy(&(mem->memory[mem->size]), contents, realsize);
//     mem->size              += realsize;
//     mem->memory[mem->size]  = 0;
//     return realsize;
// }

//
//int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
int main(int, char**)
{

	glfwSetErrorCallback(glfw_error_callback);

	if (!glfwInit())
	{
		return EXIT_FAILURE;
	}

	// Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
	// GL ES 2.0 + GLSL 100 (WebGL 1.0)
	const char* glsl_version = "#version 100";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(IMGUI_IMPL_OPENGL_ES3)
	// GL ES 3.0 + GLSL 300 es (WebGL 2.0)
	const char* glsl_version = "#version 300 es";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
	// GL 3.2 + GLSL 150
	const char* glsl_version = "#version 150";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE,
		GLFW_OPENGL_CORE_PROFILE);            // 3.2+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required on Mac
#else
	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif

	// Create window with graphics context.

	// Font scaling depending on monitor resolution
	//

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	// float font_scale_factor = 1.0;
	// int monitor_xscale, monitor_yscale;

	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

	// glfwWindowHint(GLFW_DECORATED, true);
	// monitor_xscale = mode->width;
	// monitor_yscale = mode->height;

	// printf("%d, %d\n", monitor_xscale, monitor_yscale);
	GLFWwindow* window = glfwCreateWindow(mode->width, mode->height - 60, "Colossal V1.3", nullptr, nullptr);

	if (window == nullptr)
	{
		return EXIT_FAILURE;
	}

	int width, height, channels;

	unsigned char* pixels = stbi_load("icon.png", &width, &height, &channels, 4);

	if (pixels)
	{
		GLFWimage images[1];
		images[0].width = width;
		images[0].height = height;
		images[0].pixels = pixels;

		glfwSetWindowIcon(window, 1, images);

		stbi_image_free(pixels);
	}
	// Create GLFW context.
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable Vsync.

	// Setup dear imgui context.
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImPlot::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	(void)io;

	// IO configuration flags.
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
#if defined(_WIN32)
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
#endif

	// Light mode style
	ImGui::StyleColorsLight();

	ImGuiStyle& style = ImGui::GetStyle();

	// GUI Customization
	if (1)
	{
		// Because I am sick of rounded corners.
		style.WindowRounding = 0.0f;
		style.ChildRounding = 0.0f;
		style.FrameRounding = 0.0f;
		style.TabRounding = 0.0f;
		style.PopupRounding = 0.0f;
		style.ScrollbarRounding = 0.0f;

		style.Colors[ImGuiCol_TitleBg].x = 0.7f;
		style.Colors[ImGuiCol_TitleBg].y = 0.7f;
		style.Colors[ImGuiCol_TitleBg].z = 0.7f;
		style.Colors[ImGuiCol_TitleBg].w = 1.0f;

		style.Colors[ImGuiCol_Button].x = 0.8f;
		style.Colors[ImGuiCol_Button].y = 0.8f;
		style.Colors[ImGuiCol_Button].z = 0.8f;
		style.Colors[ImGuiCol_Button].w = 1.0f;

		style.Colors[ImGuiCol_TableHeaderBg].x = 0.8f;
		style.Colors[ImGuiCol_TableHeaderBg].y = 0.8f;
		style.Colors[ImGuiCol_TableHeaderBg].z = 0.8f;
		style.Colors[ImGuiCol_TableHeaderBg].w = 1.0f;

		style.Colors[ImGuiCol_TitleBg].x = 0.7f;
		style.Colors[ImGuiCol_TitleBg].y = 0.7f;
		style.Colors[ImGuiCol_TitleBg].z = 0.7f;
		style.Colors[ImGuiCol_TitleBg].w = 1.0f;

		style.Colors[ImGuiCol_TabDimmed].x = 0.7f;
		style.Colors[ImGuiCol_TabDimmed].y = 0.7f;
		style.Colors[ImGuiCol_TabDimmed].z = 0.7f;
		style.Colors[ImGuiCol_TabDimmed].w = 1.0f;

		style.Colors[ImGuiCol_TabDimmedSelected].x = 0.7f;
		style.Colors[ImGuiCol_TabDimmedSelected].y = 0.7f;
		style.Colors[ImGuiCol_TabDimmedSelected].z = 0.7f;
		style.Colors[ImGuiCol_TabDimmedSelected].w = 1.0f;

		style.Colors[ImGuiCol_TabUnfocused].x = 0.7f;
		style.Colors[ImGuiCol_TabUnfocused].y = 0.7f;
		style.Colors[ImGuiCol_TabUnfocused].z = 0.7f;
		style.Colors[ImGuiCol_TabUnfocused].w = 1.0f;

		style.Colors[ImGuiCol_TitleBgActive].x = 0.14f;
		style.Colors[ImGuiCol_TitleBgActive].y = 0.28f;
		style.Colors[ImGuiCol_TitleBgActive].z = 0.56f;
		style.Colors[ImGuiCol_TitleBgActive].w = 1.0f;

		style.Colors[ImGuiCol_HeaderHovered].x = 0.36f;
		style.Colors[ImGuiCol_HeaderHovered].y = 0.52;
		style.Colors[ImGuiCol_HeaderHovered].z = 0.84f;
		style.Colors[ImGuiCol_HeaderHovered].w = 1.0f;

		style.Colors[ImGuiCol_Header].x = 0.36f;
		style.Colors[ImGuiCol_Header].y = 0.52;
		style.Colors[ImGuiCol_Header].z = 0.84f;
		style.Colors[ImGuiCol_Header].w = 1.0f;

		style.Colors[ImGuiCol_TabHovered].x = 0.14f;
		style.Colors[ImGuiCol_TabHovered].y = 0.28f;
		style.Colors[ImGuiCol_TabHovered].z = 0.56f;
		style.Colors[ImGuiCol_TabHovered].w = 1.0f;

		style.Colors[ImGuiCol_TabSelectedOverline].x = 0.14f;
		style.Colors[ImGuiCol_TabSelectedOverline].y = 0.28f;
		style.Colors[ImGuiCol_TabSelectedOverline].z = 0.56f;
		style.Colors[ImGuiCol_TabSelectedOverline].w = 1.0f;

		style.Colors[ImGuiCol_TabSelected].x = 0.14f;
		style.Colors[ImGuiCol_TabSelected].y = 0.28f;
		style.Colors[ImGuiCol_TabSelected].z = 0.56f;
		style.Colors[ImGuiCol_TabSelected].w = 1.0f;
	}

	ImPlotStyle implot_style = ImPlot::GetStyle();

	// Setup renderer

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// Load font.
#if defined(_WIN32)
	ImFont* regular_font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 20.0);
	// ImFont *regular_font = io.Fonts->AddFontFromFileTTF("./plex.ttf", 18.0);
	// Font to use for the HMI display of tags.
	ImFont* display_font = io.Fonts->AddFontFromFileTTF("./plex.ttf", 38.0);
#else

	ImFont* regular_font = io.Fonts->AddFontFromFileTTF("./plex.ttf", 16.0);
	ImFont* display_font = io.Fonts->AddFontFromFileTTF("./plex.ttf", 32.0);
#endif
	// ==================================================
	// float baseFontSize = 13.0f; // 13.0f is the size of the default font.
	// Change to the font size you use. float iconFontSize =
	//     baseFontSize * 2.0f /
	//     3.0f; // FontAwesome fonts need to have their sizes reduced
	//     by 2.0f/3.0f in order to align correctly

	// // merge in icons from Font Awesome
	static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
	float baseFontSize = 16.0f;                      // 13.0f is the size of the default font.
	// Change to the font size you use.
	float iconFontSize = baseFontSize * 2.6f / 3.0f; // FontAwesome fonts need to have their sizes
	// reduced by 2.0f/3.0f in order to align correctly
	ImFontConfig icons_config;
	icons_config.MergeMode = true;
	icons_config.PixelSnapH = true;
	icons_config.GlyphMinAdvanceX = iconFontSize;
	ImFont* icons = io.Fonts->AddFontFromFileTTF("./fontawesome.ttf", iconFontSize, &icons_config, icons_ranges);
	// // use FONT_ICON_FILE_NAME_FAR if you want regular instead of solid

	// ==================================================
	// Our state:
	//
	// Thread handles.
	thrd_t th[N_DEVICES];
	// Plot buffers
	PlotDataBuffer plot_buffers[N_DEVICES][N_CHANNELS];
	// Mutexes to use
	mtx_t mtxes[N_DEVICES];
	// Ring buffer for each device.
	Buffer buf[N_DEVICES];
	// Config update buffer
	ConfigUpdate config_update[N_DEVICES];
	// Arguments to pass to each thread.
	ThreadArg thread_arg[N_DEVICES];
	// Device list
	Link links[N_DEVICES];
	// Config file links
	Link config_links[N_DEVICES];
	// UI buffers to hold the GUI data
	Link ui_link_buffers[N_DEVICES];
	// Evaluations array
	ClEval evals[N_EVALS];
	ClEval ui_eval_buffer[N_EVALS];
	// Data type to hold the result of a fetch query from the DB.
	RecordQuery records = {};
	unsigned long last_buf_get = 0;

	records.rows = (RecordRow*)malloc(sizeof(RecordRow));
	snprintf(records.start, sizeof(records.start), "2025/09/23 12:00:00");
	snprintf(records.end, sizeof(records.end), "2025/09/23 14:00:00");
	snprintf(records.error_msg, sizeof(records.error_msg), "");
	snprintf(records.cols, sizeof(records.cols), "TAG_0, TAG_1");
	records.num_fields = 6;
	records.fields = NULL;

	// Tag HMI display config
	HmiDisplay hmi_display[N_TAG_DISPLAYS];
	UiMenuState menu_state = {};
	int config_edit_flags[N_DEVICES] = {};
	bool eval_config_change = false;
	// We use this so we don't lock the mutex each frame.
	int logger_selected_link = 0;
	int selected_link_index = 0;
	int selected_tag_index = 0;
	int selected_eval_index = 0;

	bool is_config_loaded = load_config(config_links);

	menu_state.devices_menu = true;
	menu_state.tag_menu = true;

	// Initialize each buffer for 10 products.
	// Can only keep 10 products at a time.
	// This is enough as the main thread will
	// keep consuming the products.
	// If the products are not consumed and the buffer is full,
	// the polling thread will stop polling the device.
	// This will probably change once we implement the logging
	// functionality.
	// TODO
	// =======================================================================
	// Add the ability to check for config files in the file system and
	// create Links accordingly.
	for (int i = 0; i < N_DEVICES; i++)
	{
		// Initialize all the devices.
		// This is needed to allocate the required memory for channels
		// so that the GUI can access them.
		char link_name_buf[32];
		sprintf(link_name_buf, "LINK_%d", i);

		char link_tk_buf[32];
		sprintf(link_tk_buf, "LK%d:", i);

		MbTcpConfig mb_tcp_config;
		sprintf(mb_tcp_config.ip, "127.0.0.1");
		mb_tcp_config.port = 5502;
		mb_tcp_config.low_first = false;

		MbSerialConfig mb_serial_config;
		sprintf(mb_serial_config.com_port, "COM3");
		mb_serial_config.baudrate = BR_9600;
		mb_serial_config.parity = CL_SERIAL_PARITY_NONE;
		mb_serial_config.low_first = false;

		S7Config s7_config;
		sprintf(s7_config.ip, "192.168.0.1");
		s7_config.rack = 0;
		s7_config.slot = 2;

		EipConfig eip_config;
		sprintf(eip_config.ip, "192.168.1.10");
		sprintf(eip_config.path, "1.0");

		LinkConfig link_config;
		link_config.mb_tcp_config = mb_tcp_config;
		link_config.mb_serial_config = mb_serial_config;
		link_config.s7_config = s7_config;
		link_config.eip_config = eip_config;

		Link link = {};
		Link thread_link = {};

		link = *cl_new_link(link_name_buf, link_tk_buf, i, MB_TCP, link_config, N_CHANNELS, false);
		thread_link = *cl_new_link(link_name_buf, link_tk_buf, i, MB_TCP, link_config, N_CHANNELS, false);

		if (is_config_loaded)
		{
			links[i] = config_links[i];
			// Link data copy used as a buffer for the UI widgets to write
			// to.
			ui_link_buffers[i] = config_links[i];
		}
		else
		{
			links[i] = link;
			ui_link_buffers[i] = link;
		}

		// Initializes the mutexes
		mtx_init(&mtxes[i], mtx_plain);
		// Initialize the buffers
		buf_init(&buf[i], &mtxes[i], PLOT_BUFFER_SIZE);
		// and the config update so we can send updates to the threads.
		config_update_init(&config_update[i]);

		// Spawn the threads.
		thread_arg[i].id = i + 1;
		thread_arg[i].buf_ptr = &buf[i];
		thread_arg[i].config_update_ptr = &config_update[i];

		// the thread argument holds the firstly created link.
		// This is used as the initial values for the thread to try and
		// poll...etc
		thread_arg[i].link = thread_link;

		if (thrd_create(&th[i], polling_thread, (void*)&thread_arg[i]) != thrd_success)
		{
			fprintf(stderr, "Could not spawn thread.\n");
			return EXIT_FAILURE;
		}

		// We don't have to wait for the thread to finish.
		// And we want the OS to clean everything after we quit.
		thrd_detach(th[i]);

		LoggingThreadArg logging_arg = {};

		snprintf(logging_arg.error_msg, sizeof(logging_arg.error_msg), "Logging not started.");

		logging_arg.id = i;
		logging_arg.is_error = true;
		logging_arg.mtx = mtxes[i];
		logging_arg.links = links;

		// if (thrd_create(&logging_th[i], logging_thread, (void *)&logging_arg) != thrd_success)
		// {

		//     fprintf(stderr, "Could not spawn logging thread.\n");
		//     return EXIT_FAILURE;
		// }
		// thrd_detach(logging_th[i]);
	}

	for (int i = 0; i < N_EVALS; i++)
	{
		cl_eval_init(&evals[i]);
		cl_eval_init(&ui_eval_buffer[i]);

		snprintf((&evals[i])->tk, sizeof((&evals[i])->tk), "EV%d", i);
		snprintf((&ui_eval_buffer[i])->tk, sizeof((&ui_eval_buffer[i])->tk), "EV%d", i);
	}

	for (int i = 0; i < N_TAG_DISPLAYS; i++)
	{
		hmi_display[i].tag_id = i;
		hmi_display[i].link_id = 0;
		hmi_display[i].show_plot = false;
		hmi_display[i].plot_color = ImVec4(0, 255, 0, 255);
	}

	int imageWidth = 0;
	int imageHeight = 0;
	GLuint imageTexture = 0;
	//bool ret = load_texture_from_file("colossal.png", &image_texture, &image_width, &image_height);
	// C++ implementation.
	// Upgrade
	bool ret = loadTextureDataFromFile(std::filesystem::path("colossal.png"), &imageTexture, &imageWidth, &imageHeight);

	if (!ret)
	{
		// TODO:
		// Better error handling.
		return EXIT_FAILURE;
	}

	// The main loop
	while (!glfwWindowShouldClose(window))
	{
		// Main event loop.
		// Poll and handle events.
		glfwWaitEventsTimeout(0.5);

		if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
		{
			ImGui_ImplGlfw_Sleep(10);
			// continue;
		}

		// Start the Imgui frame.
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// const ImGuiViewport *viewport = ImGui::GetMainViewport();
		//
		ImGuiID dock_id = ImGui::GetID("MyDockSpace");
		ImGuiDockNodeFlags dock_flags = 0;
		dock_flags |= ImGuiDockNodeFlags_PassthruCentralNode;
		ImGui::DockSpaceOverViewport(dock_id, ImGui::GetMainViewport(), dock_flags);

		unsigned long cur_timestamp = (unsigned long)time(nullptr);
		// Consume the data in the buffer each N_FRAMES...
		if ((cur_timestamp - last_buf_get) > 1)
		{
			for (size_t i = 0; i < N_DEVICES; i++)
			{
				// Get the device data from the threads buffers and put it
				// in the links[] for display
				if (buf_get(&buf[i], &links[i], 1))
				{
					// TODO
					// do something
				}
				// Get the last value from the buffer.
				// This is a work around because buf_get won't get the last
				// value if the buffer is full.
				if (buf_peek_last(&buf[i], &links[i]))
				{
					// do something
					// Push latest data to plot buffers.
					// This could be improved.
					for (int j = 0; j < N_CHANNELS; j++)
					{
						if (links[i].tags[j].enabled && links[i].timestamp > 0 && !links[i].is_error)
						{
							plot_buffers[i][j].push_value((double)links[i].tags[j].tag_value.real_value,
								(double)cur_timestamp);
						}
						else
						{
							plot_buffers[i][j].push_value((double)NAN, (double)cur_timestamp);
						}
					}
				}
			}
			for (int i = 0; i < N_EVALS; i++)
			{
				cl_evaluate(&evals[i], evals, links);
			}
			last_buf_get = cur_timestamp;
		}

		if (ImGui::BeginMainMenuBar())
		{
			// Load our images
			imageWidth = ImGui::GetContentRegionAvail().x;
			imageHeight = ImGui::GetContentRegionAvail().y;

			ImGui::Image((ImTextureID)(intptr_t)imageTexture, ImVec2(110.0f, imageHeight + 8.0f));

			if (ImGui::BeginMenu("File"))
			{
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit"))
			{

				ImGui::MenuItem("Tag Properties", NULL, &menu_state.tag_menu);
				ImGui::MenuItem("Calculations", NULL, &menu_state.evals_menu);
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Devices"))
			{
				ImGui::MenuItem("Device Details", NULL, &menu_state.devices_menu);
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Data"))
			{
				// ImGui::MenuItem("Data Export", NULL, &menu_state.tag_export);
				// ImGui::MenuItem("Tag Plot", NULL, &menu_state.plot_menu);
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("View"))
			{
				ImGui::MenuItem("Tag Displays", NULL, &menu_state.tag_displays);
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Help"))
			{
				ImGui::MenuItem("About", NULL, &menu_state.about);
				ImGui::EndMenu();
			}
			ImGui::SameLine(ImGui::GetWindowWidth() - 70.0);

			ImGui::EndMainMenuBar();
		}
		//

		// if (menu_state.tag_displays)
		// {
		//     for (int i = 0; i < N_TAG_DISPLAYS; i++)
		//     {
		//         ui_tag_display(N_DEVICES, links, hmi_display, i, buf, &menu_state.tag_displays, display_font,
		//                        regular_font, icons);
		//     }
		// }

		// Tag options window. Tag selection is done through the links
		// window table.
		//
		if (menu_state.about)
		{
			ui_about_window(&menu_state.about);
		}

		if (menu_state.tag_menu)
		{
			ui_tag_window(N_DEVICES, links, ui_link_buffers, &menu_state.tag_menu, config_update, selected_link_index,
				&selected_tag_index, config_edit_flags);
		}
		// Window containing the different links configs and their
		// associated tags values.
		if (menu_state.devices_menu)
		{
			ui_links_window(N_DEVICES, links, ui_link_buffers, config_update, &menu_state.devices_menu,
				&selected_link_index, &selected_tag_index, config_edit_flags);
		}

		if (menu_state.evals_menu)
		{
			ui_evals_window(N_DEVICES, links, N_EVALS, evals, ui_eval_buffer, &menu_state.evals_menu,
				&selected_eval_index, &eval_config_change);
		}

		if (true)
		{
			ui_plot_window(N_DEVICES, links, selected_link_index, selected_tag_index, plot_buffers, true);
		}

		ImGui::Render();

		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(0.5, 0.5, 0.5, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}

		glfwMakeContextCurrent(window);
		glfwSwapBuffers(window);
	}
	printf("Closing\n");
	// Cleanup

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImPlot::DestroyContext();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);

	glfwTerminate();

	return EXIT_SUCCESS;
}

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "GLFW error %d: %s\n", error, description);
}

/// Spawn a thread to keep polling the device and does not block main.
int polling_thread(void* arg)
{
	if (!arg)
	{
		fprintf(stderr, "Error: Null pointer passed to the thread function.\n");
		return EXIT_FAILURE;
	}

	ThreadArg* arg_ptr = (ThreadArg*)arg;
	Buffer* buf_ptr = arg_ptr->buf_ptr;
	// Get a first copy of the link data.
	// Note that the tags field is an array and it is not passed by value
	//
	Link link = arg_ptr->link;
	ConfigUpdate* config_update_ptr = arg_ptr->config_update_ptr;
	unsigned long timestamp = (unsigned long)time(nullptr);

	bool reconnect_flag = false;

	for (;;)
	{
#if defined(_WIN32)
		// Sleep for 2 seconds before attempting to reconnect.
		Sleep(2000);
#else
		sleep(2);
#endif
		timestamp = (unsigned long)time(nullptr);

		int tag_to_write_id = 0;
		bool tag_write_pending = false;
		// Update the links (from the GUI) and reconnect.
		if (config_update_get(config_update_ptr, &link, &reconnect_flag, &tag_write_pending, &tag_to_write_id))
		{
			printf("Config updated: Device %d\n", arg_ptr->id);
		}

		if (!link.active)
		{
			continue;
		}

		if (cl_connect_link(&link) == -1)
		{

			// Hack to get Windows error message.
#if defined(_WIN32)
			wchar_t* s = NULL;
			FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL, WSAGetLastError(), MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_UK), (LPWSTR)&s, 0, NULL);
#endif

			link.timestamp = timestamp;
			link.elapsed_time = 0.0;
			link.is_error = true;
			sprintf_s(link.err_msg, "Error while trying to connect to device: %ls", s);

			// Update the tags errors
			for (int i = 0; i < N_CHANNELS; i++)
			{
				link.tags[i].is_error = true;
				snprintf(link.tags[i].err_msg, sizeof(link.tags[i].err_msg), "The link is disconnected.");
			}
			// Make sure to put the data in the buffer so that main can get
			// the error message.
			if (buf_put(buf_ptr, link))
			{
				// printf("Producer N. %d produced data. timestamp: %lu\n",
				// id, timestamp);
			}

			fprintf(stderr, "Failed to connect to device: %d. %s", link.id, link.err_msg);
			continue; // Restart
		}

		reconnect_flag = false;
		link.is_error = false;

		while (!reconnect_flag) // Loop until error
		{
			int tag_to_write_id = 0;
			bool tag_write_pending = false;
			// Check if there is a configuration update and if we need to
			// reconnect the device.
			if (config_update_get(config_update_ptr, &link, &reconnect_flag, &tag_write_pending, &tag_to_write_id))
			{
				if (reconnect_flag)
					break;
			}

			if (tag_write_pending && tag_to_write_id < N_CHANNELS)
			{
				link.tags[tag_to_write_id].write_flag = true;
			}
			timestamp = (unsigned long)time(nullptr);
			link.timestamp = timestamp;

			LARGE_INTEGER frequency;
			LARGE_INTEGER start;
			LARGE_INTEGER end;
			double elapsed_time;

			QueryPerformanceFrequency(&frequency);
			QueryPerformanceCounter(&start);

			link.is_error = false;
			for (int i = 0; i < link.tag_count; i++)
			{

				if (!link.tags[i].enabled)
				{
					// Skip the channel if disabled
					continue;
				}

				if (link.tags[i].write_flag)
				{
					if (cl_write_tag(&link, i) == -1)
					{
						link.is_error = true;
						if (buf_put(buf_ptr, link))
						{
							// printf("Producer N. %d produced data. timestamp:
							// %lu\n", id, timestamp);
						}
					}
					else
					{
						link.tags[i].write_flag = false;
						link.tags[i].is_error = false;
						link.is_error = false;
					}
				}
				// Read the tag.
				if (cl_read_tag(&link, i) == -1)
				{
					link.is_error = true;
					reconnect_flag = true;
					// indicate that an error happened.
					// note that each tag holds its own error flag. So, this
					// is redundant.
					if (buf_put(buf_ptr, link))
					{
						// printf("Producer N. %d produced data. timestamp:
						// %lu\n", id, timestamp);
					}

					// break;
				}
			}

			QueryPerformanceCounter(&end);

			elapsed_time = (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart;
			link.elapsed_time = elapsed_time;

			if (buf_put(buf_ptr, link))
			{
				// printf("Producer N. %d produced data. timestamp:
				// %lu\n", id, timestamp);
			}

#if defined(_WIN32)
			Sleep(link.poll_delay);
#else
			sleep(link.poll_delay / 1000);
#endif

			if (reconnect_flag) // In case of an error, break out of the
				// loop and reconnect.
			{
				printf("Reconnect requested\n");
				// close and free any resources.
				if (cl_diconnect_link(&link) == -1)
				{
					// unimplemented.
				}
				break;
			}
		}
	}
	// TODO
	// Should clean the links and tags.
	return EXIT_SUCCESS;
}
