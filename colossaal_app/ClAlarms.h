#pragma once
#include "../colossal_cpp/link.h"
#include "../colossal_cpp/nlohmann/json.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

enum class ClAlarmType
{
	Equal = 0,
	Low = 1,
	High = 2,
	HighHigh = 3,
	LowLow = 4,
	BitTrue = 5,
	BitFalse = 6
};

class ClAlarm
{
public:
	int id;
	std::string name;
	std::string description;
	bool enabled;
	bool logged;
	int valueType;
	int linkId;
	int tagId;
	TagValue thresholdValue;
	ClAlarmType alarmType;

	ClAlarm(int id, const std::string& name, const std::string& description,
		int valueType, int linkId, int tagId, const TagValue& thresholdValue, ClAlarmType alarmType);
	~ClAlarm() = default;
	void processAlarm(const TagValue& currentValue, bool isLinkError);
	bool isActive() const { return active; }
	bool isAcknowledged() const { return acknowledged; }
	bool acknowledge();

private:
	bool active;
	bool acknowledged;
};
class ClAlarms
{
public:
	std::vector<ClAlarm> alarms;
	const Link* const links;

	ClAlarms(const Link* const links);

	void addAlarm(const ClAlarm& alarm);
	void removeAlarm(int id);
	void processAlarms();
	bool loadJsonAlarms(const std::string& filePath);
};

