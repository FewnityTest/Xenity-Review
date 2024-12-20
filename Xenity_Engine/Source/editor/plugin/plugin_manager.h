// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#if defined(_WIN32) || defined(_WIN64)

/**
 * [Internal]
 */
#include <string>
#include <vector>
#include <memory>

#include "plugin.h"

class PluginManager
{
public:
	static void Init();
	static void Stop();

	/*template <typename T>
	static void Register()
	{
		static_assert( 
			std::is_base_of<Plugin, T>::value, 
			"Called PluginManager::Register with a non-Plugin type!" 
		);

		Register( new T() );
	}*/
	static void Register( Plugin* plugin );

private:
	static std::vector<std::unique_ptr<Plugin>> plugins;
};

#endif