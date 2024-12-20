// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

/**
 * [Internal]
 */

#if defined(__PSP__)
#include <engine/api.h>

#include <array>

#include "renderer.h"
#include <engine/lighting/lighting.h>
#include <engine/graphics/texture.h>

class API RendererGU : public Renderer
{
public:
	RendererGU();
	RendererGU(const RendererGU& other) = delete;
	RendererGU& operator=(const RendererGU&) = delete;

	int Init() override;
	void Setup() override;
	void Stop() override;
	void NewFrame() override;
	void EndFrame() override;
	void SetViewport(int x, int y, int width, int height) override;
	void SetClearColor(const Color& color) override;
	void SetProjection2D(float projectionSize, float nearClippingPlane, float farClippingPlane) override;
	void SetProjection3D(float fov, float nearClippingPlane, float farClippingPlane, float aspect) override;
	void ResetView() override;
	void SetCameraPosition(const Camera& camera) override;
	void SetCameraPosition(const Vector3& position, const Vector3& rotation) override;
	void ResetTransform() override;
	void SetTransform(const Vector3& position, const Vector3& rotation, const Vector3& scale, bool resetTransform) override;
	void SetTransform(const glm::mat4& mat) override;
	void BindTexture(const Texture& texture) override;
	void DrawSubMesh(const MeshData::SubMesh& subMesh, const Material& material, RenderingSettings& settings) override;
	void DrawSubMesh(const MeshData::SubMesh& subMesh, const Material& material, const Texture& texture, RenderingSettings& settings) override;
	void DrawLine(const Vector3& a, const Vector3& bn, const Color& color, RenderingSettings& settings) override;
	unsigned int CreateNewTexture() override;
	void DeleteTexture(Texture& texture) override;
	void SetTextureData(const Texture& texture, unsigned int textureType, const unsigned char* buffer) override;
	void Clear() override;
	void SetFog(bool m_active) override;
	void SetFogValues(float start, float end, const Color& color) override;

	void DeleteSubMeshData(MeshData::SubMesh& subMesh) override;
	void UploadMeshData(MeshData& meshData) override;

	void Setlights(const LightsIndices& lightsIndices) override;

private:
	void ApplyTextureFilters(const Texture& texture);
	std::array<const Light*, MAX_LIGHT_COUNT> lastUpdatedLights;
	int maxLightCount = 4;
	void DisableAllLight();
	void SetLight(const int lightIndex, const Light& light, const Vector3& lightPosition, const Vector3& lightDirection) override;
	int GetWrapModeEnum(WrapMode wrapMode);
	float fogStart = 0;
	float fogEnd = 10;
	Color fogColor;
	//std::shared_ptr<Texture> usedTexture = nullptr;
	void* usedTexture = nullptr;
	RenderingSettings lastSettings;
	unsigned int lastUsedColor = 0x00000000;
	unsigned int lastUsedColor2 = 0xFFFFFFFF;
	// int GetCullFaceEnum(CullFace face);
	// float GetAnisotropicValueEnum(AnisotropicLevel level);

	// int GetDrawModeEnum(DrawMode drawMode);
};
#endif