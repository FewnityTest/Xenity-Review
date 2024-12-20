// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <memory>

#include "menu.h"

class Texture;

class MainBarMenu : public Menu
{
public:
	void Init() override;
	void Draw() override;

	/**
	* Get bar height
	*/
	float GetHeight() const
	{
		return height;
	}

private:
	/**
	* Add a component to the selected gameobject
	*/
	template<typename T>
	void AddComponentToSelectedGameObject();

	/**
	* Create a gameobject with a component
	*/
	template <typename T>
	std::shared_ptr<T> CreateGameObjectWithComponent(const std::string& gameObjectName);

	/**
	* Draw an image as a button
	* @return is clicked
	*/
	bool DrawImageButton(const bool enabled, const Texture& texture);

	float height = 0;
};
