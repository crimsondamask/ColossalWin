#pragma once
#include "link.h"
#include <filesystem>
#include <string>
#include <vector>

class LoggerTag
{
public:
	int linkId;
	int tagId;
	std::string name;
	std::string description;
	std::string unit;
	bool enabled;
	int valueType;
	TagValue tagValue;

	LoggerTag(int linkId, int tagId, const std::string& name, const std::string& description, const std::string& unit, bool enabled, int valueType, const TagValue& tagValue)
		: linkId(linkId), tagId(tagId), name(name), description(description), unit(unit), enabled(enabled), valueType(valueType), tagValue(tagValue)
	{
	}
};

class Logger
{
public:
	bool enabled;
	std::string name;
	std::filesystem::path dbPath;

	Logger(const std::string& name, const std::filesystem::path& dbPath, int dbType, const std::vector<LoggerTag>& slowTags, const std::vector<LoggerTag>& fastTags);
	~Logger() = default;
	void addTagToFastLogger(const LoggerTag& tag);
	void removeTagFromFastLogger(const LoggerTag& tag);
	void logFastTags(const Link links[], int linkCount);
private:
	bool isError;
	std::string errMsg;
	int dbType;
	std::vector<LoggerTag> fastLogger;
	std::vector<LoggerTag> slowLogger;
};

