// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gr�gory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "callbacks.h"

#if defined(__PSP__)

int exit_callback(int arg1, int arg2, void* common)
{
	sceKernelExitGame();
	return 0;
}

int CallbackThread(SceSize args, void* argp)
{
	int cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
	sceKernelRegisterExitCallback(cbid);
	sceKernelSleepThreadCB();

	return 0;
}

int SetupCallbacks(void)
{
	int thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);
	if (thid >= 0)
	{
		sceKernelStartThread(thid, 0, 0);
	}
	return thid;
}
#endif