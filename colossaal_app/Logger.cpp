#include "Logger.h"

Logger::Logger(const std::string& name, const std::filesystem::path& dbPath, int dbType, const std::vector<LoggerTag>& slowTags,const std::vector<LoggerTag>& fastTags )
	: name(name), dbPath(dbPath), dbType(dbType), fastLogger(slowTags), slowLogger(fastTags)
{
	enabled = true;
	isError = false;
}

void Logger::addTagToFastLogger(const LoggerTag& tag)
{
	fastLogger.push_back(tag);
}

void Logger::removeTagFromFastLogger(const LoggerTag& tag)
{
	auto it = std::remove_if(fastLogger.begin(), fastLogger.end(), [&](const LoggerTag& t) {
		return t.linkId == tag.linkId && t.tagId == tag.tagId;
	});
	fastLogger.erase(it, fastLogger.end());
}

void Logger::logFastTags(const Link links[], int linkCount)
{
	if (!enabled)
		return;
	for (const auto& tag : fastLogger)
	{
		if (tag.linkId < 0 || tag.linkId >= linkCount)
			continue;
		const Link& link = links[tag.linkId];
		if (!link.active || !link.tags[tag.tagId].enabled)
			continue;
		// Log the tag value to the database
		// This is a placeholder for actual logging logic
	}
}
