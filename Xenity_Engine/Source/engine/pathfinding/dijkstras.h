// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <string>

#include <engine/api.h>

class API Dijkstras
{
public:
	Dijkstras() = delete;
	Dijkstras(int size);


	int* dist = nullptr;
	int* path = nullptr;
	int* usedPath = nullptr;
	int** nodes = nullptr;
	int squareSize = 4;
	int pathCount = 0;
	void processDijkstras(int source, int destination);
	void printParentPath(int parent[], int i, std::string& str);
	void fillPath(int parent[], int path[], int i, int pathIndex);
	int GetMin(int dist[], bool visited[]);

private:
};

