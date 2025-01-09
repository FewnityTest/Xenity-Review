// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "scene_manager.h"

#if defined(EDITOR)
#include <editor/ui/editor_ui.h>
#include <editor/file_reference_finder.h>
#endif

#include <engine/file_system/file_system.h>
#include <engine/file_system/file.h>
#include <engine/class_registry/class_registry.h>
#include <engine/reflection/reflection_utils.h>
#include <engine/asset_management/project_manager.h>
#include <engine/game_elements/gameplay_manager.h>
#include <engine/game_elements/transform.h>
#include <engine/ui/window.h>
#include <engine/graphics/graphics.h>
#include <engine/graphics/camera.h>
#include <engine/physics/physics_manager.h>
#include <engine/tools/template_utils.h>
#include <engine/debug/debug.h>
#include <engine/missing_script.h>
#include "scene.h"
#include <engine/world_partitionner/world_partitionner.h>
#include <engine/debug/stack_debug_object.h>

using ordered_json = nlohmann::ordered_json;

std::shared_ptr<Scene> SceneManager::s_openedScene = nullptr;

ordered_json savedSceneData;
ordered_json savedSceneDataHotReloading;

bool SceneManager::s_sceneModified = false;

#if defined(EDITOR)

void SceneManager::SaveScene(SaveSceneType saveType)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	std::unordered_map<uint64_t, bool> usedIds;
	std::set<uint64_t> usedFilesIds;

	// Use ordered json to keep gameobject's order
	ordered_json j;

	j["Version"] = s_sceneVersion;

	// For each gameobject:
	for (const std::shared_ptr<GameObject>& go : GameplayManager::gameObjects)
	{
		const std::string gameObjectId = std::to_string(go->GetUniqueId());

		// Save GameObject's and Transform's values
		j["GameObjects"][gameObjectId]["Transform"]["Values"] = ReflectionUtils::ReflectiveToJson(*go->GetTransform().get());
		j["GameObjects"][gameObjectId]["Values"] = ReflectionUtils::ReflectiveToJson(*go.get());

		// Save GameObject's children ids
		std::vector<uint64_t> ids;
		const int childCount = go->GetChildrenCount();
		for (int childI = 0; childI < childCount; childI++)
		{
			const uint64_t id = go->GetChildren()[childI].lock()->GetUniqueId();
			if (usedIds[id])
			{
				Debug::PrintError("[SceneManager::SaveScene] GameObject Id already used by another Component/GameObject! Id: " + std::to_string(id), true);
			}
			usedIds[id] = true;
			ids.push_back(id);
		}
		j["GameObjects"][gameObjectId]["Children"] = ids;

		// Save components values
		for (const std::shared_ptr<Component>& component : go->m_components)
		{
			const uint64_t compId = component->GetUniqueId();
			const std::string compIdString = std::to_string(compId);
			if (usedIds[compId])
			{
				Debug::PrintError("[SceneManager::SaveScene] Component Id already used by another Component/GameObject! Id: " + compIdString, true);
			}
			usedIds[compId] = true;

			const ReflectiveData componentData = component->GetReflectiveData();

			const std::shared_ptr<MissingScript> missingScript = std::dynamic_pointer_cast<MissingScript>(component);
			// If the component is valide, save values
			if (!missingScript)
			{
				j["GameObjects"][gameObjectId]["Components"][compIdString]["Type"] = component->GetComponentName();
				j["GameObjects"][gameObjectId]["Components"][compIdString]["Values"] = ReflectionUtils::ReflectiveDataToJson(componentData);
				j["GameObjects"][gameObjectId]["Components"][compIdString]["Enabled"] = component->IsEnabled();
			}
			else
			{
				// Or save component raw values
				j["GameObjects"][gameObjectId]["Components"][compIdString] = missingScript->data;
			}

			// Get all files ids used by the component
			FileReferenceFinder::GetUsedFilesInReflectiveData(usedFilesIds, componentData);
		}
	}

	// Save lighting data
	j["Lighting"]["Values"] = ReflectionUtils::ReflectiveDataToJson(Graphics::s_settings.GetReflectiveData());

	// Add skybox file to the usedFile list
	if (Graphics::s_settings.skybox != nullptr)
	{
		usedFilesIds.insert(Graphics::s_settings.skybox->m_fileId);
	}

	// Save the usedFilesIds list
	j["UsedFiles"]["Values"] = usedFilesIds;

	if (saveType == SaveSceneType::SaveSceneForPlayState) // Save Scene in a temporary json to restore it after quitting play mode
	{
		savedSceneData = j;
	}
	else if (saveType == SaveSceneType::SaveSceneForHotReloading)// Save Scene in a temporary json to restore it after compiling the game
	{
		savedSceneDataHotReloading = j;
	}
	else
	{
		// Get scene path
		std::string path = "";
		if (SceneManager::s_openedScene)
		{
			path = SceneManager::s_openedScene->m_file->GetPath();
			XASSERT(!path.empty(), "[SceneManager::SaveScene] Scene path is empty");
		}
		else
		{
			path = EditorUI::SaveFileDialog("Save Scene", ProjectManager::GetAssetFolderPath());
		}

		// If there is no error, save the file
		if (!path.empty())
		{
			FileSystem::s_fileSystem->Delete(path);
			const std::shared_ptr<File> file = FileSystem::MakeFile(path);
			if (file->Open(FileMode::WriteCreateFile))
			{
				const std::string jsonData = j.dump(2);
				file->Write(jsonData);
				file->Close();
				ProjectManager::RefreshProjectDirectory();
				SetSceneModified(false);
			}
			else
			{
				Debug::PrintError("[SceneManager::SaveScene] Fail to save the scene file: " + file->GetPath(), true);
			}
		}
	}
}

#endif

void SceneManager::ReloadScene()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	LoadScene(s_openedScene);
}

void SceneManager::RestoreScene()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	LoadScene(savedSceneData);
}

void SceneManager::RestoreSceneHotReloading()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	LoadScene(savedSceneDataHotReloading);
}

void SceneManager::SetSceneModified(bool value)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	if (GameplayManager::GetGameState() == GameState::Stopped)
	{
		s_sceneModified = value;
		Window::UpdateWindowTitle();
	}
}

bool SceneManager::OnQuit()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	bool cancel = false;
#if defined(EDITOR)
	if (GameplayManager::GetGameState() != GameState::Stopped)
	{
		const DialogResult result = EditorUI::OpenDialog("You are in play mode", "Do you want to stop the game?", DialogType::Dialog_Type_YES_NO_CANCEL);
		if (result == DialogResult::Dialog_YES)
		{
			GameplayManager::SetGameState(GameState::Stopped, true);
		}
		cancel = true;
	}
	else
	{
		if (s_sceneModified)
		{
			// Ask if the user wants to save the scene or not if the quit the scene
			const DialogResult result = EditorUI::OpenDialog("The Scene Has Been Modified", "Do you want to save?", DialogType::Dialog_Type_YES_NO_CANCEL);
			if (result == DialogResult::Dialog_YES)
			{
				SaveScene(SaveSceneType::SaveSceneToFile);
			}
			else if (result == DialogResult::Dialog_CANCEL)
			{
				cancel = true;
			}
		}
	}

#endif
	return cancel;
}

void SceneManager::LoadScene(const ordered_json& jsonData)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	// Automaticaly start the game if built in engine mode
#if !defined(EDITOR)
	GameplayManager::SetGameState(GameState::Starting, true);
#else
	if (GameplayManager::GetGameState() == GameState::Playing)
	{
		GameplayManager::SetGameState(GameState::Starting, true);
	}
#endif

	ClearScene();

	std::vector<std::shared_ptr<Component>> allComponents;

	if (jsonData.contains("GameObjects"))
	{
		// Create all GameObjects and Components
		for (const auto& gameObjectKV : jsonData["GameObjects"].items())
		{
			const std::shared_ptr<GameObject> newGameObject = CreateGameObject();
			// Set gameobject id
			const uint64_t id = std::stoull(gameObjectKV.key());
			newGameObject->SetUniqueId(id);

			// Fill gameobjet's values from json
			ReflectionUtils::JsonToReflective(gameObjectKV.value(), *newGameObject.get());

			// Create components if there is any
			if (gameObjectKV.value().contains("Components"))
			{
				for (auto& componentKV : gameObjectKV.value()["Components"].items())
				{
					const std::string componentName = componentKV.value()["Type"];
					std::shared_ptr<Component> comp = ClassRegistry::AddComponentFromName(componentName, *newGameObject);

					// Get and set component id
					const uint64_t compId = std::stoull(componentKV.key());

					if (comp)
					{
						// Enable or disable component
						if (componentKV.value().contains("Enabled"))
						{
							const bool isEnabled = componentKV.value()["Enabled"];
							comp->SetIsEnabled(isEnabled);
						}
					}
#if defined(EDITOR)
					else
					{
						// If the component is missing (the class doesn't exist anymore or the game is not compiled
						// Create a missing script and copy component data to avoid data loss
						const std::shared_ptr<MissingScript> missingScript = std::make_shared<MissingScript>();
						missingScript->data = componentKV.value();
						newGameObject->AddExistingComponent(missingScript);
						comp = missingScript;
					}
#endif
					if (comp)
					{
						allComponents.push_back(comp);
						comp->SetUniqueId(compId);
					}
					else 
					{
#if defined(EDITOR)
						XASSERT(false, "[SceneManager::LoadScene] Missing script not created!");
#endif
					}
				}
			}
		}

		// Set gameobjects parents
		for (auto& kv : jsonData["GameObjects"].items())
		{
			const std::shared_ptr<GameObject> parentGameObject = FindGameObjectById(std::stoull(kv.key()));
			// Check if the parent has children
			if (parentGameObject && kv.value().contains("Children"))
			{
				// For each child, set his parent
				for (const auto& kv2 : kv.value()["Children"].items())
				{
					const std::shared_ptr<GameObject> goChild = FindGameObjectById(kv2.value());
					if (goChild)
					{
						goChild->SetParent(parentGameObject);
					}
				}
			}
		}

		// Bind Components values
		for (auto& kv : jsonData["GameObjects"].items())
		{
			const std::shared_ptr<GameObject> go = FindGameObjectById(std::stoull(kv.key()));
			if (go)
			{
				// Update transform
				const std::shared_ptr<Transform> transform = go->GetTransform();
				ReflectionUtils::JsonToReflective(kv.value()["Transform"], *transform.get());
				//transform->SetRotation(Quaternion::Euler(transform->GetLocalEulerAngles().x, transform->GetLocalEulerAngles().y, transform->GetLocalEulerAngles().z));
				transform->m_isTransformationMatrixDirty = true;
				transform->UpdateLocalRotation();
				transform->UpdateWorldValues();

				// If the gameobject has components
				if (kv.value().contains("Components"))
				{
					const int componentCount = go->GetComponentCount();
					// Find the component with the Id
					for (const auto& kv2 : kv.value()["Components"].items())
					{
						for (int compI = 0; compI < componentCount; compI++)
						{
							const std::shared_ptr<Component>& component = go->m_components[compI];
							if (component->GetUniqueId() == std::stoull(kv2.key()))
							{
								// Fill values
								ReflectionUtils::JsonToReflective(kv2.value(), *component.get());
								break;
							}
						}
					}
				}
			}
		}

		// Call Awake on Components
		if (GameplayManager::GetGameState() == GameState::Starting)
		{
			std::vector<std::shared_ptr<Component>> orderedComponentsToInit;
			const size_t componentsCount = allComponents.size();
			int componentsToInitCount = 0;

			// Find uninitiated components and order them
			for (size_t i = 0; i < componentsCount; i++)
			{
				if (auto& componentToCheck = allComponents[i])
				{
					if (!componentToCheck->m_initiated)
					{
						bool placeFound = false;
						for (size_t componentToInitIndex = 0; componentToInitIndex < componentsToInitCount; componentToInitIndex++)
						{
							// Check if the checked has a higher priority (lower value) than the component in the list
							if (componentToCheck->m_updatePriority <= orderedComponentsToInit[componentToInitIndex]->m_updatePriority)
							{
								orderedComponentsToInit.insert(orderedComponentsToInit.begin() + componentToInitIndex, componentToCheck);
								placeFound = true;
								break;
							}
						}
						// if the priority is lower than all components's priorities in the list, add it the end of the list
						if (!placeFound)
						{
							orderedComponentsToInit.push_back(componentToCheck);
						}
						componentsToInitCount++;
					}
				}
			}

			// Call components Awake() function
			for (int i = 0; i < componentsToInitCount; i++)
			{
				const std::shared_ptr<Component>& componentToInit = orderedComponentsToInit[i];
				if (!componentToInit->m_isAwakeCalled && componentToInit->GetGameObject()->IsLocalActive() && componentToInit->IsEnabled())
				{
					componentToInit->Awake();
					componentToInit->m_isAwakeCalled = true;
				}
			}
		}
	}

	// Load lighting values
	if (jsonData.contains("Lighting"))
	{
		ReflectionUtils::JsonToReflectiveData(jsonData["Lighting"], Graphics::s_settings.GetReflectiveData());
		Graphics::OnLightingSettingsReflectionUpdate();
	}

	// Automaticaly set the game in play mode if built in engine mode
//#if !defined(EDITOR)
	if (GameplayManager::GetGameState() == GameState::Starting)
	{
		GameplayManager::SetGameState(GameState::Playing, true);
	}
//#endif
}

void SceneManager::LoadScene(const std::shared_ptr<Scene>& scene)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	XASSERT(scene != nullptr, "[SceneManager::LoadScene] scene is nullptr");

	const bool canceled = OnQuit();
	if (canceled)
		return;

	Debug::Print("Loading scene...", true);

	// Get scene file and read all data
	bool openResult = true;
#if defined(EDITOR)
	openResult = scene->m_file->Open(FileMode::ReadOnly);
#endif
	if (openResult)
	{
		std::string jsonString;
#if defined(EDITOR)
		jsonString = scene->m_file->ReadAll();
		scene->m_file->Close();
#else
		unsigned char* binData = ProjectManager::fileDataBase.GetBitFile().ReadBinary(scene->m_filePosition, scene->m_fileSize);
		jsonString = std::string(reinterpret_cast<const char*>(binData), scene->m_fileSize);
		free(binData);
#endif
		XASSERT(!jsonString.empty(), "[SceneManager::LoadScene] jsonString is empty");

		try
		{
			ordered_json data;
			if (!jsonString.empty())
			{
				data = ordered_json::parse(jsonString);
			}
			LoadScene(data);
			s_openedScene = scene;
			SetSceneModified(false);
		}
		catch (const std::exception& e)
		{
			CreateEmptyScene();
#if defined(EDITOR)
			EditorUI::OpenDialog("Error", "Error while loading the scene. The file is probably corrupted.", DialogType::Dialog_Type_OK);
#endif
			Debug::PrintError("[SceneManager::LoadScene] Scene file error: " + std::string(e.what()), true);
			return;
		}
	}
}

void SceneManager::ClearScene()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	WorldPartitionner::ClearWorld();
	Graphics::DeleteAllDrawables();
	Graphics::usedCamera.reset();
	size_t cameraCount = Graphics::cameras.size();
	for (size_t i = 0; i < cameraCount; i++)
	{
		if (Graphics::cameras[i].expired() || !Graphics::cameras[i].lock()->IsEditor())
		{
			Graphics::cameras.erase(Graphics::cameras.begin() + i);
			i--;
			cameraCount--;
		}
	}

	PhysicsManager::Clear();
	GameplayManager::orderedComponents.clear();
	GameplayManager::gameObjectsToDestroy.clear();
	GameplayManager::componentsToDestroy.clear();
	GameplayManager::gameObjects.clear();
	GameplayManager::componentsCount = 0;
	GameplayManager::gameObjectCount = 0;
#if defined(EDITOR)
	Editor::SetSelectedGameObject(nullptr);
#endif
	Window::UpdateWindowTitle();
}

void SceneManager::CreateEmptyScene()
{
	s_openedScene.reset();
	SceneManager::ClearScene();
}
