// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

/**
 * [Internal]
 */

#if defined(_EE)
#include <string>
#include <vector>

#include <engine/api.h>
#include "file.h"


class Directory;
class File;

class API FilePS2 : public File
{
public:
	FilePS2() = delete;
	explicit FilePS2(const std::string& _path);
	FilePS2(const FilePS2& other) = delete;
	FilePS2& operator=(const FilePS2&) = delete;

	~FilePS2();

	/**
	* @brief Open the file
	* @param fileMode The mode to open the file
	* @return True if the file is opened successfully
	*/
	bool Open(FileMode fileMode) override;

	/**
	* @brief Close the file
	*/
	void Close() override;

	/**
	* @brief Check if the file exists
	* @return True if the file exists
	*/
	bool CheckIfExist() override;

	/**
	* @brief Read all the content of the file as a string
	*/
	std::string ReadAll() override;

	/**
	* @brief Read all the content of the file as a binary
	* @param size Output: The size of the binary
	* @return The binary data
	*/
	unsigned char* ReadAllBinary(int& size) override;

	/**
	* @brief Write string data to the file
	* @param data The data to write
	*/
	void Write(const std::string& data) override;

protected:
	int m_fileId;
};

#endif