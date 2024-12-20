// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

/**
 * [Internal]
 */

#if defined(_EE2)
#include <engine/api.h>

#include "renderer.h"
#include <engine/lighting/lighting.h>
#include <gsKit.h>
#include <draw3d.h>
#include <math3d.h>

class API RendererGsKit : public Renderer
{
public:
	RendererGsKit();
	RendererGsKit(const RendererGsKit& other) = delete;
	RendererGsKit& operator=(const RendererGsKit&) = delete;

	int Init() override;
	void Setup() override;
	void Stop() override;
	void NewFrame() override;
	void EndFrame() override;
	void SetViewport(int x, int y, int width, int height) override;
	void SetClearColor(const Color &color) override;
	void SetProjection2D(float projectionSize, float nearClippingPlane, float farClippingPlane) override;
	void SetProjection3D(float fov, float nearClippingPlane, float farClippingPlane, float aspect) override;
	void ResetView() override;
	void SetCameraPosition(const std::shared_ptr<Camera> &camera) override;
	void ResetTransform() override;
	void SetTransform(const Vector3 &position, const Vector3 &rotation, const Vector3 &scale, bool resetTransform) override;
	void SetTransform(const glm::mat4 &mat) override;
	void BindTexture(const std::shared_ptr<Texture> &texture) override;
	void DrawMeshData(const std::shared_ptr<MeshData> &meshData, const std::vector<std::shared_ptr<Texture>> &textures, RenderingSettings &settings) override;
	void DrawLine(const Vector3 &a, const Vector3 &bn, const Color &color, RenderingSettings &settings) override;
	unsigned int CreateNewTexture() override;
	void DeleteTexture(Texture *texture) override;
	void SetTextureData(const std::shared_ptr<Texture> &texture, unsigned int textureType, const unsigned char *buffer) override;
	void Clear() override;
	void SetFog(bool m_active) override;
	void SetFogValues(float start, float end, const Color &color) override;

	void DeleteSubMeshData(MeshData::SubMesh *subMesh) override;
	void UploadMeshData(const std::shared_ptr<MeshData> &meshData) override;

	void Setlights(const std::shared_ptr<Camera> &camera) override;
	GSGLOBAL *gsGlobal = nullptr;
	MATRIX world_view;
	MATRIX view_screen;

private:
	void ApplyTextureFilters(const std::shared_ptr<Texture> &texture);

	int maxLightCount = 8;
	void DisableAllLight();
	void SetLight(int lightIndex, const Vector3 &lightPosition, float intensity, Color color, LightType type, float attenuation) override;

	float fogStart = 0;
	float fogEnd = 10;
	Color fogColor;
	// int GetCullFaceEnum(CullFace face);
	// float GetAnisotropicValueEnum(AnisotropicLevel level);

	// int GetDrawModeEnum(DrawMode drawMode);
};
#endif