#include "binary_mesh_loader.h"

#include <iostream>
#include <cstring>

#include <engine/graphics/3d_graphics/mesh_data.h>
#include <engine/file_system/file.h>
#include <engine/file_system/file_system.h>
#include <engine/asset_management/project_manager.h>
#include <engine/debug/debug.h>
#include <engine/debug/stack_debug_object.h>
#include <engine/tools/endian_utils.h>

#if defined(__PSP__)
#include <pspkernel.h>
#endif

/**
* Vertex descriptor - 4 bytes
* Sub mesh count - 4 bytes
* ------ For one sub mesh
* vertice_count - 4 bytes
* index_count - 4 bytes
* vertexMemSize - 4 bytes
* indexMemSize - 4 bytes
* vertice data - vertexMemSize bytes
* indices data - indexMemSize bytes
* ------
*/

bool BinaryMeshLoader::LoadMesh(MeshData& mesh)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	unsigned char* fileData = ProjectManager::fileDataBase.GetBitFile().ReadBinary(mesh.m_filePosition, mesh.m_fileSize);
	unsigned char* fileDataOriginalPtr = fileData;

	VertexElements vertexDescriptor = *reinterpret_cast<VertexElements*>(fileData);
	fileData += sizeof(VertexElements);

	uint32_t subMeshCount = *reinterpret_cast<uint32_t*>(fileData);

#if defined(__PS3__)
	vertexDescriptor = EndianUtils::SwapEndian(vertexDescriptor);
	subMeshCount = EndianUtils::SwapEndian(subMeshCount);
#endif // defined(__PS3__)

	fileData += sizeof(uint32_t);

#if defined(__PSP__)
	mesh.m_hasIndices = false; // Disable indices on psp, this will improve performances
#else // !defined(__PSP__)
	mesh.m_hasIndices = true;
#endif // !defined(__PSP__)
	mesh.m_hasColor = false;

	mesh.SetVertexDescriptor(vertexDescriptor);

	// Read all sub meshes
	for (uint32_t i = 0; i < subMeshCount; i++)
	{
		uint32_t vertice_count = 0;
		uint32_t index_count = 0;
		uint32_t vertexMemSize = 0;
		uint32_t indexMemSize = 0;

		// Use memcpy to avoid alignment issues
		memcpy(&vertice_count, fileData, sizeof(uint32_t));
		fileData += sizeof(uint32_t);
		memcpy(&index_count, fileData, sizeof(uint32_t));
		fileData += sizeof(uint32_t);
		memcpy(&vertexMemSize, fileData, sizeof(uint32_t));
		fileData += sizeof(uint32_t);
		memcpy(&indexMemSize, fileData, sizeof(uint32_t));
		fileData += sizeof(uint32_t);

#if defined(__PS3__)
		vertice_count = EndianUtils::SwapEndian(vertice_count);
		index_count = EndianUtils::SwapEndian(index_count);
		vertexMemSize = EndianUtils::SwapEndian(vertexMemSize);
		indexMemSize = EndianUtils::SwapEndian(indexMemSize);
#endif // defined(__PS3__)

		mesh.AllocSubMesh(vertice_count, index_count);
		std::unique_ptr<MeshData::SubMesh>& subMesh = mesh.m_subMeshes[mesh.m_subMeshCount - 1];

		// Copy vertices data
		memcpy(subMesh->data, fileData, vertexMemSize);
		fileData += vertexMemSize;

#if defined(__PS3__)
		for (int vertexIndex = 0; vertexIndex < vertice_count; vertexIndex++)
		{
			if ((uint32_t)vertexDescriptor & (uint32_t)VertexElements::NORMAL_32_BITS)
			{
				if ((uint32_t)vertexDescriptor & (uint32_t)VertexElements::UV_32_BITS)
				{
					VertexNormalsNoColor& vertex = ((VertexNormalsNoColor*)subMesh->data)[vertexIndex];
					vertex.x = EndianUtils::SwapEndian(vertex.x);
					vertex.y = EndianUtils::SwapEndian(vertex.y);
					vertex.z = EndianUtils::SwapEndian(vertex.z);

					vertex.u = EndianUtils::SwapEndian(vertex.u);
					vertex.v = EndianUtils::SwapEndian(vertex.v);

					vertex.normX = EndianUtils::SwapEndian(vertex.normX);
					vertex.normY = EndianUtils::SwapEndian(vertex.normY);
					vertex.normZ = EndianUtils::SwapEndian(vertex.normZ);
				}
				else 
				{
					VertexNormalsNoColorNoUv& vertex = ((VertexNormalsNoColorNoUv*)subMesh->data)[vertexIndex];
					vertex.x = EndianUtils::SwapEndian(vertex.x);
					vertex.y = EndianUtils::SwapEndian(vertex.y);
					vertex.z = EndianUtils::SwapEndian(vertex.z);

					vertex.normX = EndianUtils::SwapEndian(vertex.normX);
					vertex.normY = EndianUtils::SwapEndian(vertex.normY);
					vertex.normZ = EndianUtils::SwapEndian(vertex.normZ);
				}
			}
			else 
			{
				if ((uint32_t)vertexDescriptor & (uint32_t)VertexElements::UV_32_BITS)
				{
					VertexNoColor& vertex = ((VertexNoColor*)subMesh->data)[vertexIndex];
					vertex.x = EndianUtils::SwapEndian(vertex.x);
					vertex.y = EndianUtils::SwapEndian(vertex.y);
					vertex.z = EndianUtils::SwapEndian(vertex.z);

					vertex.y = EndianUtils::SwapEndian(vertex.y);
					vertex.v = EndianUtils::SwapEndian(vertex.v);
				}
				else
				{
					VertexNoColorNoUv& vertex = ((VertexNoColorNoUv*)subMesh->data)[vertexIndex];
					vertex.x = EndianUtils::SwapEndian(vertex.x);
					vertex.y = EndianUtils::SwapEndian(vertex.y);
					vertex.z = EndianUtils::SwapEndian(vertex.z);
				}
			}
		}
#endif // defined(__PS3__)

		// Copy indices data
		if(mesh.m_hasIndices)
		{
			memcpy(subMesh->indices, fileData, indexMemSize);
			
#if defined(__PS3__)
			for (int indexIndex = 0; indexIndex < index_count; indexIndex++)
			{
				if (subMesh->isShortIndices)
				{
					unsigned short& index = ((unsigned short*)subMesh->indices)[indexIndex];
					index = EndianUtils::SwapEndian(index);
				}
				else 
				{
					unsigned int& index = ((unsigned int*)subMesh->indices)[indexIndex];
					index = EndianUtils::SwapEndian(index);
				}
			}
#endif // defined(__PS3__)
		}
		fileData += indexMemSize;
	}

#if defined(__PSP__)
	sceKernelDcacheWritebackInvalidateAll(); // Very important
#endif // defined(__PSP__)

	delete[] fileDataOriginalPtr;

	return true;
}
