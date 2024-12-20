// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

/**
 * [Internal]
 */

#if !defined(_EE) && !defined(__PSP__) && !defined(__PS3__) // Improve conditions
#include <engine/api.h>

#include <array>

#include "renderer.h"
#include <engine/lighting/lighting.h>
#include <engine/graphics/texture.h>

enum class BufferType
{
	Array_Buffer,
	Element_Array_Buffer,
};

enum class BufferMode
{
	Static,
	Dynamic,
};

class API RendererOpengl : public Renderer
{
public:
	RendererOpengl();
	RendererOpengl(const RendererOpengl& other) = delete;
	RendererOpengl& operator=(const RendererOpengl&) = delete;

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
	void SetFog(bool active) override;
	void SetFogValues(float start, float end, const Color& color) override;

	void DeleteSubMeshData(MeshData::SubMesh& subMesh) override;
	void UploadMeshData(MeshData& meshData) override;

	//Shader
	void UseShaderProgram(unsigned int programId) override;

	void Setlights(const LightsIndices& lightsIndices) override;

private:
	void ApplyTextureFilters(const Texture& texture);
	unsigned int CreateVertexArray();
	unsigned int CreateBuffer();
	void BindVertexArray(unsigned int bufferId);
	void DeleteBuffer(unsigned int bufferId);
	void DeleteVertexArray(unsigned int bufferId);

	int GetBufferTypeEnum(BufferType bufferType);
	// int GetBufferModeEnum(BufferMode bufferMode);
	int GetWrapModeEnum(WrapMode wrapMode);
	int maxLightCount = 8;
	void DisableAllLight();
	void SetLight(const int lightIndex, const Light& light, const Vector3& lightPosition, const Vector3& lightDirection) override;

	float fogStart = 0;
	float fogEnd = 10;
	Color fogColor;
	std::array<const Light*, MAX_LIGHT_COUNT> lastUpdatedLights;
	// int GetCullFaceEnum(CullFace face);
	// float GetAnisotropicValueEnum(Texture::AnisotropicLevel level);

	RenderingSettings lastSettings;
	unsigned int usedTexture = 0;
	unsigned int lastUsedColor = 0x00000000;
	unsigned int lastUsedColor2 = 0xFFFFFFFF;
	uint64_t lastShaderIdUsedColor = 0;
	// int GetDrawModeEnum(DrawMode drawMode);
};
#endif