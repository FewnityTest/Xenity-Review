// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gr�gory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "debug.h"

#ifdef __PSP__

#include <pspkernel.h>

/**
 * @brief Write text in the debug file
 */
void PspDebugPrint(std::string text)
{
	pspDebugScreenPrintf(text.c_str());
}

/**
 * @brief Init debug system (call once)
 *
 */
void PspDebugInit()
{
	pspDebugScreenInit(); // initialize the debug screen
}

#endif