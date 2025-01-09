// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "../unit_test_manager.h"

#include <engine/debug/debug.h>
#include <engine/class_registry/class_registry.h>
#include <engine/tools/gameplay_utility.h>

#include <engine/lighting/lighting.h>
#include <engine/graphics/camera.h>
#include <engine/graphics/ui/text_renderer.h>
#include <engine/particle_system/particle_system.h>
#include <engine/graphics/2d_graphics/billboard_renderer.h>
#include <engine/audio/audio_source.h>
#include <engine/graphics/2d_graphics/line_renderer.h>
#include <engine/graphics/2d_graphics/sprite_renderer.h>
#include <engine/graphics/2d_graphics/tile_map.h>
#include <engine/graphics/3d_graphics/mesh_renderer.h>
#include <engine/graphics/ui/text_mesh.h>
#include <engine/graphics/ui/canvas.h>
#include <engine/game_elements/rect_transform.h>
#include <engine/physics/rigidbody.h>
#include <engine/physics/box_collider.h>
#include <engine/physics/sphere_collider.h>
#include <engine/graphics/3d_graphics/lod.h>
#include <engine/test_component.h>
#include <engine/missing_script.h>
#include <engine/game_elements/gameplay_manager.h>

template <typename T>
void ClassRegistryAddComponentFromNameTest::TestAddComponent(std::shared_ptr<GameObject>& newGameObject, bool& result, std::string& errorOut, const std::string& componentName)
{
	std::shared_ptr<Component> addedComponent = ClassRegistry::AddComponentFromName(componentName, *newGameObject);
	if (!newGameObject->GetComponent<T>() || !std::dynamic_pointer_cast<T>(addedComponent) || (!Compare(newGameObject->GetComponent<T>(), std::dynamic_pointer_cast<T>(addedComponent))))
	{
		errorOut += "Failed to add "+ componentName + " component\n";
		result = false;
	}
}

TestResult ClassRegistryAddComponentFromNameTest::Start(std::string& errorOut)
{
	BEGIN_TEST();

	ClassRegistry::Reset();
	ClassRegistry::RegisterEngineComponents();
	ClassRegistry::RegisterEngineFileClasses();
	std::shared_ptr<GameObject> newGameObject = CreateGameObject();

	{
		TestAddComponent<Light>(newGameObject, result, errorOut, "Light");
		TestAddComponent<Camera>(newGameObject, result, errorOut, "Camera");
		TestAddComponent<TextRenderer>(newGameObject, result, errorOut, "TextRenderer");
		TestAddComponent<Canvas>(newGameObject, result, errorOut, "Canvas");
		TestAddComponent<RectTransform>(newGameObject, result, errorOut, "RectTransform");
		TestAddComponent<TextMesh>(newGameObject, result, errorOut, "TextMesh");
		TestAddComponent<MeshRenderer>(newGameObject, result, errorOut, "MeshRenderer");

		if (!Compare(newGameObject->GetComponentCount(), 7)) 
		{
			errorOut += "Failed to add all components\n";
			result = false;
		}
	}

	Destroy(newGameObject);
	GameplayManager::RemoveDestroyedGameObjects();
	newGameObject.reset();

	END_TEST();
}

TestResult ClassRegistryGetComponentNamesTest::Start(std::string& errorOut)
{
	BEGIN_TEST();

	ClassRegistry::Reset();

	std::vector<std::string> names = ClassRegistry::GetComponentNames();
	EXPECT_EQUALS(names.size(), static_cast<size_t>(0), "Failed to clear component names");

	ClassRegistry::RegisterEngineComponents();
	ClassRegistry::RegisterEngineFileClasses();

	names = ClassRegistry::GetComponentNames();
	for (const std::string& name : names)
	{
		EXPECT_NOT_EQUALS(name, "", "Failed to get component names (empty name)");
	}

	// Try to find a basic components
	EXPECT_NOT_EQUALS(std::find(names.begin(), names.end(), "Light"), names.end(), "Failed to find Light component name");
	EXPECT_NOT_EQUALS(std::find(names.begin(), names.end(), "Camera"), names.end(), "Failed to find Camera component name");

	EXPECT_NOT_EQUALS(names.size(), static_cast<size_t>(0), "Failed to get component names (empty list)");
	
	END_TEST();
}