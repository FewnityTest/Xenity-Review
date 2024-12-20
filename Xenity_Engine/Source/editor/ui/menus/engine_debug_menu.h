// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include "menu.h"

class EngineDebugMenu : public Menu
{
public:
	void Init() override;
	void Draw() override;

private:
	/**
	* Draw files reference count list
	*/
	void DrawFilesList();
};

