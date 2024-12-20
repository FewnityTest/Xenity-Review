// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <string>

#include "menu.h"

class EngineSettingsMenu : public Menu
{
public:
	void Init() override;
	void Draw() override;
private:
	bool DrawSelectFolderButton(std::string& path);
	bool DrawCompilerOptions();
};

