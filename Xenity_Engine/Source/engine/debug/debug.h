// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once
#include <string>
#include <memory>
#include <vector>

#include <engine/api.h>
#include <engine/event_system/event_system.h>

class Socket;
class File;

enum class DebugType 
{
	Log,
	Warning,
	Error,
};

struct DebugHistory
{
	std::string message;
	DebugType type = DebugType::Log;
	int count = 0;
};

/**
 * @brief Used to print text in a console/file or remotely to a server
 */
class API Debug
{
public:

	/**
	* @brief Print a text
	* @param text Text to print
	* @param hideInEditorConsole If true, the text will not be printed in the editor's console
	*/
	static void Print(const std::string& text, bool hideInEditorConsole = false);

	/**
	* @brief Print an error
	* @param text Text to print
	* @param hideInEditorConsole If true, the text will not be printed in the editor's console
	*/
	static void PrintError(const std::string& text, bool hideInEditorConsole = false);

	/**
	* @brief Print a warning
	* @param text Text to print
	* @param hideInEditorConsole If true, the text will not be printed in the editor's console
	*/
	static void PrintWarning(const std::string& text, bool hideInEditorConsole = false);

	/**
	* @brief Get the event when a debug message is printed
	*/
	static Event<>& GetOnDebugLogEvent()
	{
		return s_onDebugLogEvent;
	}


private:
	friend class BottomBarMenu;
	friend class Engine;
	friend class ConsoleMenu;
	friend class NetworkManager;
	friend class Compiler;

	static size_t s_lastDebugMessageHistoryIndex;

	/**
	* @brief [Internal] Init debug system
	*/
	static int Init();

	/**
	* @brief [Internal] Send all profiler data to the debug server
	*/
	static void SendProfilerDataToServer();

	/**
	* @brief [Internal] Connect the game to an online debug console
	*/
	static void ConnectToOnlineConsole();

	/**
	* @brief [Internal] Get all the debug text into a string
	*/
	static const std::string& GetDebugString()
	{
		return s_debugText;
	}

	/**
	* @brief [Internal] Clear all logs (the file is untouched)
	*/
	static void ClearDebugLogs();


	// [Internal] Lower is higher speed
	static float s_sendProfilerDelay;
	static std::vector<DebugHistory> s_debugMessageHistory;

	/**
	* @brief Add a message in the history
	* @param message Message to add
	* @param messageType Type of the message
	*/
	static void AddMessageInHistory(const std::string& message, DebugType messageType);

	/**
	* @brief Send text via the socket to the online debug console
	* @param text Text to send
	*/
	static void PrintInOnlineConsole(const std::string& text);

	/**
	* @brief Print text to the cmd
	* @param text Text to print
	*/
	static void PrintInConsole(const std::string& text);

	/**
	* @brief Write text to the debug file
	* @param text Text to write
	*/
	static void PrintInFile(const std::string& text);

	static Event<> s_onDebugLogEvent;
	static std::string s_debugText;
	static std::shared_ptr<Socket> s_socket;
	static float s_sendProfilerCooldown;
	static std::shared_ptr<File> s_file;
};