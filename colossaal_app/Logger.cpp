#include "Logger.h"
#include "sqlite3.h"


Logger::Logger(const std::string& name, const std::filesystem::path& dbPath, int dbType, const std::vector<LoggerTag>& slowTags, const std::vector<LoggerTag>& fastTags)
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
	for (auto& tag : fastLogger)
	{
		if (tag.linkId < 0 || tag.linkId >= linkCount)
			continue;
		const Link& link = links[tag.linkId];
		if (!link.active || !link.tags[tag.tagId].enabled)
			continue;
		int tagId = tag.tagId;
		int linkId = tag.linkId;
		// Update the tag information from the link before logging
		tag.copyFromLinkTag(links[linkId], tagId);
		// Log the tag value to the database
		// This is a placeholder for actual logging logic
	}
}

LoggerTag::LoggerTag(int linkId, int tagId)
	: linkId(linkId), tagId(tagId) 
{
	this->name = std::string("");
	this->enabled = false;
	this->description = std::string("");
	this->unit = std::string("");
	this->valueType = 0;
	tagValue = {};
}


LoggerTag LoggerTag::LoggerTagFromLinkTag(const Link& link, int tagId)
{
	const auto& linkTag = link.tags[tagId];
	LoggerTag tag = LoggerTag(link.id, tagId);
	tag.name = linkTag.name;
	tag.description = linkTag.description;
	tag.unit = linkTag.unit;
	tag.enabled = linkTag.enabled;
	tag.valueType = linkTag.value_type;
	tag.tagValue = linkTag.tag_value;
	return tag;
}

void LoggerTag::copyFromLinkTag(const Link& link, int tagId)
{
	const auto& linkTag = link.tags[tagId];

	this->linkId = link.id;
	this->tagId = tagId;
	this->name = linkTag.name;
	this->description = linkTag.description;
	this->unit = linkTag.unit;
	this->enabled = linkTag.enabled;
	this->valueType = linkTag.value_type;
	this->tagValue = linkTag.tag_value;
}