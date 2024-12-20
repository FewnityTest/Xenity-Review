// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include "menu.h"
#define FPS_HISTORY_SIZE 400
#define USED_MEMORY_HISTORY_SIZE 1000
#define USED_VIDE_MEMORY_HISTORY_SIZE 1000

class ClassicProfilerItem
{
public:
	ClassicProfilerItem(const std::string& _name) : name(_name) {}
	const std::string& name;
	uint32_t totalTime = 0;
	uint32_t callCountInFrame = 0;
};

class TimelineItem 
{
public:
	TimelineItem(const std::string& _name) : name(_name) {}
	const std::string& name;
	uint64_t start;
	uint64_t end;
	uint32_t level;
};

class ProfilerMenu : public Menu
{
public:
	void Init() override;
	void Draw() override;

private:
	/**
	* Read current fps and store it in the history
	*/
	void UpdateFpsCounter();

	void UpdateMemoryCounter();

	/**
	* Draw memory stats
	*/
	void DrawMemoryStats();

	/**
	* Draw all profiler benchmarks
	*/
	void DrawProfilerGraph();

	void CreateTimelineItems();

	float fpsAVG = 0;
	float nextFpsUpdate = 0;
	float lastFps = 0;
	float fpsHistory[FPS_HISTORY_SIZE] = { 0 };
	float usedMemoryHistory[USED_MEMORY_HISTORY_SIZE] = { 0 };
	float usedVideoMemoryHistory[USED_VIDE_MEMORY_HISTORY_SIZE] = { 0 };
	bool counterInitialised = false;
	std::vector<TimelineItem> timelineItems;
	std::vector<ClassicProfilerItem> classicProfilerItems;
	bool isPaused = false;
	uint64_t lastStartTime;
	uint64_t lastEndTime;
	uint32_t lastMaxLevel;
	uint32_t selectedProfilingRow = 0;
	uint32_t lastFrame = 0;
};

