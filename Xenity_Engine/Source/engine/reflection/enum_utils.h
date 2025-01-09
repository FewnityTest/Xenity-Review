// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <string>
#include <vector>
#include <map>
#include <cstdint>
#include <typeinfo>

#include <engine/api.h>

struct EnumValueName
{
	std::string name = "";
	int value = 0;
};

/**
* @brief Class to get the list of enum strings lists
* initialise enumStringsLists before calling main()
*/
class API EnumHelper
{
public:
	template<typename T>
	static std::string EnumAsString(T enumValue) 
	{
		const std::map<uint64_t, std::vector<EnumValueName>>& enumStringsLists = GetEnumStringsLists();
		const std::vector<EnumValueName>& enumStrings = enumStringsLists.at(static_cast<uint64_t>(typeid(T).hash_code()));
		for (auto& enumString : enumStrings)
		{
			if (enumString.value == static_cast<int>(enumValue))
			{
				return enumString.name;
			}
		}

		return "";
	}

	static std::map<uint64_t, std::vector<EnumValueName>>& GetEnumStringsLists()
	{
		static std::map<uint64_t, std::vector<EnumValueName>> enumStringsLists;
		return enumStringsLists;
	}
};

/**
* @brief Create a map with the int value as key and the second value as the enum name
* @param enumData The enum data as a string
*/
[[maybe_unused]] static std::vector<EnumValueName> ConvertEnumToVector(std::string enumData)
{
	size_t textSize = enumData.size();
	// Remove all spaces in the string
	for (size_t charIndex = 0; charIndex < textSize; charIndex++)
	{
		if (enumData[charIndex] == ' ')
		{
			enumData.erase(enumData.begin() + charIndex);
			charIndex--;
			textSize--;
		}
	}

	std::vector<EnumValueName> myVector;
	int lastEnumNameEndPos = 0;
	bool foundEgals = false;
	int currentValue = -1;
	for (int charIndex = 0; charIndex < textSize; charIndex++)
	{
		if (enumData[charIndex] == '=') // If the enum is defined with a number
		{
			foundEgals = true;
			int endPos = 0;

			// Find the end of the number
			for (int charIndex2 = charIndex; charIndex2 < textSize; charIndex2++)
			{
				if (enumData[charIndex2] == ',')
				{
					endPos = charIndex2;
					break;
				}
			}

			std::string stringValue;
			if (endPos == 0)
				stringValue = enumData.substr(charIndex + 1);
			else
				stringValue = enumData.substr(charIndex + 1, endPos - (charIndex + 1));

			// Convert the string to an int
			currentValue = std::stoi(stringValue);

			// Get the enum name and add it to the map
			EnumValueName enumValueName;
			enumValueName.value = currentValue;
			enumValueName.name = enumData.substr(lastEnumNameEndPos, charIndex - lastEnumNameEndPos);
			myVector.push_back(enumValueName);

			lastEnumNameEndPos = charIndex;
		}
		else if (enumData[charIndex] == ',') // If there is another enum
		{
			if (foundEgals)
			{
				foundEgals = false;
				lastEnumNameEndPos = charIndex + 1;
				continue;
			}

			// The enum value is not defined, use the last one and add one
			currentValue++;

			// Get the enum name and add it to the map
			EnumValueName enumValueName;
			enumValueName.value = currentValue;
			enumValueName.name = enumData.substr(lastEnumNameEndPos, charIndex - lastEnumNameEndPos);
			myVector.push_back(enumValueName);

			// Update indexes
			lastEnumNameEndPos = charIndex + 1;
			charIndex += 1;
		}
	}

	// Since the last enum does not have a ',' at the end, add the last enum value
	if (!foundEgals)
	{
		currentValue++;

		EnumValueName enumValueName;
		enumValueName.value = currentValue;
		enumValueName.name = enumData.substr(lastEnumNameEndPos);
		myVector.push_back(enumValueName);
	}
	return myVector;
}


/**
* @brief Register a enum's strings map into a static map that list's all other enum strings map
* @param newEnumStringsList The enum's strings map
*/
template<typename T>
static void* RegisterEnumStringsMap(const std::vector<EnumValueName>& newEnumStringsList)
{
	const uint64_t type = typeid(T).hash_code();
	std::map<uint64_t, std::vector<EnumValueName>>& enumStringsLists = EnumHelper::GetEnumStringsLists();
	enumStringsLists[type] = newEnumStringsList;

	return nullptr;
}

//TODO:
// Replace enum's strings map by a vector to have two enums string with the same int value
// Try to find another way to call RegisterEnumStringsMap without creating a void* variable
/**
* @brief Create an enum that can be used by the editor
*/
#define ENUM(name, ...) \
    enum class name { __VA_ARGS__ }; \
	static const void* name##Register = RegisterEnumStringsMap<name>(ConvertEnumToVector(#__VA_ARGS__));
