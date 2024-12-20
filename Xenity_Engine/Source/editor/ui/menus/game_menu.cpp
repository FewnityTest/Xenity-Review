// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "game_menu.h"

#include <imgui/imgui.h>

#include <editor/editor.h>

#include <engine/graphics/camera.h>
#include <engine/graphics/graphics.h>
#include <engine/inputs/input_system.h>

void GameMenu::Init()
{
}

void GameMenu::Draw()
{
	std::shared_ptr<Camera> camera = nullptr;
	Vector2Int frameBufferSize;
	if (startAvailableSize.x != 0 && startAvailableSize.y != 0)
	{
		const size_t cameraCount = Graphics::cameras.size();

		// Get game's camera
		for (size_t i = 0; i < cameraCount; i++)
		{
			std::shared_ptr<Camera> tempCam = Graphics::cameras[i].lock();
			if (!tempCam->IsEditor() && tempCam->IsEnabled() && tempCam->GetGameObject()->IsLocalActive())
			{
				camera = tempCam;
				if (needUpdateCamera)
				{
					camera->ChangeFrameBufferSize(Vector2Int(static_cast<int>(startAvailableSize.x), static_cast<int>(startAvailableSize.y)));
					needUpdateCamera = false;
				}
				frameBufferSize.x = camera->GetWidth();
				frameBufferSize.y = camera->GetHeight();
				break;
			}
		}
	}

	// Generate tab name
	std::string windowName = "Game";
	if (isLastFrameOpened)
	{
		if (camera)
		{
			windowName += " " + std::to_string(frameBufferSize.x) + "x" + std::to_string(frameBufferSize.y);
		}
		else
		{
			windowName += " (No camera)";
		}
	}
	windowName += "###Game" + std::to_string(id);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	const bool visible = ImGui::Begin(windowName.c_str(), &isActive, ImGuiWindowFlags_NoCollapse);
	isLastFrameOpened = visible;
	if (visible)
	{
		OnStartDrawing();

		if (InputSystem::GetKeyDown(KeyCode::MOUSE_RIGHT) && ImGui::IsWindowHovered())
		{
			ImGui::SetWindowFocus();
		}

		if (camera)
		{
			if ((isHovered || isFocused || lastSize != startAvailableSize || Editor::lastFocusedGameMenu.lock() == nullptr) && (startAvailableSize.x != 0 && startAvailableSize.y != 0))
			{
				Editor::lastFocusedGameMenu = shared_from_this();
				camera->ChangeFrameBufferSize(Vector2Int(static_cast<int>(startAvailableSize.x), static_cast<int>(startAvailableSize.y)));
				lastSize = startAvailableSize;
			}
			ImGui::Image((ImTextureID)camera->m_secondFramebufferTexture, ImVec2(startAvailableSize.x, startAvailableSize.y), ImVec2(0, 1), ImVec2(1, 0));
		}
		else
		{
			DrawNoCameraText();
		}

		CalculateWindowValues();
	}
	else
	{
		ResetWindowValues();
	}

	ImGui::End();
	ImGui::PopStyleVar();
}

void GameMenu::DrawNoCameraText()
{
	//Increase font size
	ImFont* font = ImGui::GetFont();
	font->Scale *= 2;
	ImGui::PushFont(font);

	//Draw text
	const std::string noCamText = "There is no camera";
	const ImVec2 textSize = ImGui::CalcTextSize(noCamText.c_str());
	const float offY = ImGui::GetCursorPosY();
	ImGui::SetCursorPos(ImVec2((startAvailableSize.x - textSize.x) / 2.0f, (startAvailableSize.y + offY) / 2.0f));
	ImGui::Text("%s", noCamText.c_str());
	ImGui::PopFont();

	//Reset font
	font->Scale /= 2.0f;
	ImGui::PushFont(font);
	ImGui::PopFont();
}
