#include "ClAlarms.h"


ClAlarm::ClAlarm(int id, const std::string& name, const std::string& description,
	int valueType, int linkId, int tagId, const TagValue& thresholdValue, ClAlarmType alarmType)
	: id(id), name(name), description(description), valueType(valueType), linkId(linkId), tagId(tagId), thresholdValue(thresholdValue), alarmType(alarmType),
	active(false), acknowledged(false), enabled(true), logged(false), activeTimestamp(0), acknowledgedTimestamp(0), alarmOn(false) {

}

void ClAlarm::processAlarm(const TagValue& currentValue, bool isLinkError) {
	if (!enabled) {
		return;
	}

	if (isLinkError) {
		return;
	}

	switch (alarmType) {
	case ClAlarmType::Equal:
		switch (valueType) {
		case VALUE_REAL:
			active = (currentValue.real_value == thresholdValue.real_value);

			break;
		case VALUE_INT:
			active = (currentValue.int_value == thresholdValue.int_value);
			break;
		case VALUE_BOOL:
			active = (currentValue.bool_value == thresholdValue.bool_value);
			break;
		}
		break;
	case ClAlarmType::Low:
		switch (valueType) {
		case VALUE_REAL:
			active = (currentValue.real_value < thresholdValue.real_value);
			break;
		case VALUE_INT:
			active = (currentValue.int_value < thresholdValue.int_value);
			break;
		case VALUE_BOOL:
			active = (currentValue.bool_value < thresholdValue.bool_value);
			break;
		}
		break;
	case ClAlarmType::High:
		switch (valueType) {
		case VALUE_REAL:
			active = (currentValue.real_value > thresholdValue.real_value);
			break;
		case VALUE_INT:
			active = (currentValue.int_value > thresholdValue.int_value);
			break;
		case VALUE_BOOL:
			active = (currentValue.bool_value > thresholdValue.bool_value);
			break;
		}
		break;
	case ClAlarmType::HighHigh:
		switch (valueType) {
		case VALUE_REAL:
			active = (currentValue.real_value > thresholdValue.real_value);
			break;
		case VALUE_INT:
			active = (currentValue.int_value > thresholdValue.int_value);
			break;
		case VALUE_BOOL:
			active = (currentValue.bool_value > thresholdValue.bool_value);
			break;
		}
		break;
	case ClAlarmType::LowLow:
		switch (valueType) {
		case VALUE_REAL:
			active = (currentValue.real_value < thresholdValue.real_value);
			break;
		case VALUE_INT:
			active = (currentValue.int_value < thresholdValue.int_value);
			break;
		case VALUE_BOOL:
			active = (currentValue.bool_value < thresholdValue.bool_value);
			break;
		}
		break;
	case ClAlarmType::BitTrue:
		switch (valueType) {
		case VALUE_BOOL:
			active = currentValue.bool_value;
			break;
		default:
			active = false;
			break;
		}
		break;
	case ClAlarmType::BitFalse:
		switch (valueType) {
		case VALUE_BOOL:
			active = !currentValue.bool_value;
			break;
		default:
			active = false;
			break;
		}
		break;
	}

	if (active && !alarmOn) {
		activeTimestamp = static_cast<int>(time(nullptr));
		alarmOn = true;
		acknowledged = false;
	}

	if (alarmOn && acknowledged)
	{
		acknowledgedTimestamp = static_cast<int>(time(nullptr));
	}

	if (!active && alarmOn && acknowledged) {
		alarmOn = false;
	}
	std::cout << "Alarm ID: " << id << ", Name: " << name << ", Active: " << (active ? "Yes" : "No") << std::endl;

}

bool ClAlarm::acknowledge() {
	acknowledged = true;
	return acknowledged;
}

ClAlarms::ClAlarms(const Link* const links) : links(links) {}

void ClAlarms::addAlarm(const ClAlarm& alarm) {
	alarms.push_back(alarm);
}

void ClAlarms::removeAlarm(int id) {
	alarms.erase(std::remove_if(alarms.begin(), alarms.end(),
		[id](const ClAlarm& alarm) { return alarm.id == id; }), alarms.end());
}

void ClAlarms::processAlarms() {
	for (auto& alarm : alarms) {
		if (links != nullptr && alarm.linkId >= 0 && alarm.linkId < N_DEVICES) {
			const Link& link = links[alarm.linkId];
			if (alarm.tagId >= 0 && alarm.tagId < N_CHANNELS) {
				const Tag& tag = link.tags[alarm.tagId];
				alarm.processAlarm(tag.tag_value, link.is_error);
			}
		}
	}
}

bool ClAlarms::loadJsonAlarms(const std::string& filePath) {
	std::ifstream file(filePath);
	if (!file.is_open()) {
		std::cerr << "Failed to open file: " << filePath << std::endl;
		return false;
	}

	nlohmann::json jsonAlarms;
	try {
		file >> jsonAlarms;
	}
	catch (const nlohmann::json::parse_error& e) {
		std::cerr << "Failed to parse JSON: " << e.what() << std::endl;
		return false;
	}

	for (const auto& jsonAlarm : jsonAlarms) {
		int id = jsonAlarm.value("id", -1);
		std::string name = jsonAlarm.value("name", "");
		std::string description = jsonAlarm.value("description", "");
		int valueType = jsonAlarm.value("valueType", 0);
		int linkId = jsonAlarm.value("linkId", -1);
		int tagId = jsonAlarm.value("tagId", -1);
		TagValue thresholdValue;

		switch (valueType) {
		case VALUE_REAL:
			thresholdValue.real_value = jsonAlarm.value("thresholdValue", 0.0);
			break;
		case VALUE_INT:
			thresholdValue.int_value = jsonAlarm.value("thresholdValue", 0);
			break;
		case VALUE_BOOL:
			thresholdValue.bool_value = jsonAlarm.value("thresholdValue", false);
			break;
		}

		ClAlarmType alarmType = static_cast<ClAlarmType>(jsonAlarm.value("alarmType", 0));

		ClAlarm alarm(id, name, description, valueType, linkId, tagId, thresholdValue, alarmType);
		addAlarm(alarm);
	}

	return true;
}

int ClAlarms::activeAlarmsCount() const {
	return std::count_if(alarms.begin(), alarms.end(), [](const ClAlarm& alarm) {
		return alarm.isActive();
		});
}

int ClAlarms::acknowledgedAlarmsCount() const {
	return std::count_if(alarms.begin(), alarms.end(), [](const ClAlarm& alarm) {
		return alarm.isAcknowledged();
		});
}

void ClAlarms::acknowledgeAll() {
	for (auto& alarm : alarms) {
		alarm.acknowledge();
	}
}