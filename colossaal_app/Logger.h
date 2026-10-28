#pragma once
#include "link.h"
#include <filesystem>
#include <string>
#include <vector>


enum class DatabaseType
{
	SQLite = 0,
	MySQL = 1,
	FileDb = 2,
	// Add more database types as needed
};

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

	LoggerTag(int linkId, int tagId);
	LoggerTag LoggerTagFromLinkTag(const Link& link, int tagId);
	void copyFromLinkTag(const Link& link, int tagId);
};

class Logger
{
public:
	bool enabled;
	std::string name;
	std::filesystem::path dbPath;

	Logger(const std::string& name, int dbType, const std::vector<LoggerTag>& slowTags, const std::vector<LoggerTag>& fastTags);
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

