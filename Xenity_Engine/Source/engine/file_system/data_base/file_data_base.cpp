#include "file_data_base.h"

#include <engine/file_system/file_system.h>
#include <engine/reflection/reflection_utils.h>
#include <engine/debug/stack_debug_object.h>
#include <set>

using ordered_json = nlohmann::ordered_json;

ReflectiveData FileDataBaseEntry::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, p, "p", true);
	Reflective::AddVariable(reflectedVariables, id, "id", true);
	Reflective::AddVariable(reflectedVariables, po, "po", true);
	Reflective::AddVariable(reflectedVariables, s, "s", true);
	Reflective::AddVariable(reflectedVariables, mpo, "mpo", true);
	Reflective::AddVariable(reflectedVariables, ms, "ms", true);
	Reflective::AddVariable(reflectedVariables, t, "t", true);
	return reflectedVariables;
}

ReflectiveData FileDataBase::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, m_fileList, "fl", true);
	return reflectedVariables;
}

void FileDataBase::AddFile(FileDataBaseEntry* file)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	m_fileList.push_back(file);
}

void FileDataBase::Clear()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	const size_t listSize = m_fileList.size();
	for (size_t i = 0; i < listSize; i++)
	{
		delete m_fileList[i];
	}
	m_fileList.clear();
}

void FileDataBase::SaveToFile(const std::string& path)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	FileSystem::s_fileSystem->Delete(path);

	const std::shared_ptr<File> file = FileSystem::MakeFile(path);
	const bool openResult = file->Open(FileMode::WriteCreateFile);
	XASSERT(openResult, "Failed to create data base file" + path);
	if (openResult)
	{
		ordered_json j;
		j["Values"] = ReflectionUtils::ReflectiveDataToJson(GetReflectiveData());

		std::vector<uint8_t> binaryFileDataBase;
		ordered_json::to_msgpack(j, binaryFileDataBase);

		file->Write(binaryFileDataBase.data(), binaryFileDataBase.size());
		file->Close();
	}
}

void FileDataBase::LoadFromFile(const std::string& path)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	Clear();

	const std::shared_ptr<File> file = FileSystem::MakeFile(path);
	const bool openResult = file->Open(FileMode::ReadOnly);

	XASSERT(openResult, "Data base file not found");

	if (openResult)
	{
		// Read json data
		size_t dataSize = 0;
		unsigned char* data = file->ReadAllBinary(dataSize);
		file->Close();

		XASSERT(dataSize != 0, "Failed to read data base file");

		// Json library wants a vector of uint8_t
		std::vector<uint8_t> binaryFileDataBase;
		binaryFileDataBase.resize(dataSize);
		memcpy(binaryFileDataBase.data(), data, dataSize);
		free(data);

		const ordered_json j = ordered_json::from_msgpack(binaryFileDataBase);

		ReflectionUtils::JsonToReflectiveData(j, GetReflectiveData());
	}
}

IntegrityState FileDataBase::CheckIntegrity()
{
	int state = static_cast<int>(IntegrityState::Integrity_Ok);
	size_t currentPos = 0;
	std::set<uint64_t> idSet;
	for (auto entry : m_fileList)
	{
		if (entry->p == "")
		{
			state |= static_cast<int>(IntegrityState::Integrity_Has_Empty_Path);
		}

		// Check if the id is unique
		if (idSet.find(entry->id) != idSet.end())
		{
			state |= static_cast<int>(IntegrityState::Integrity_Error_Non_Unique_Ids);
		}
		idSet.insert(entry->id);

		// Code and header should not be in the file data base
		if (entry->t == FileType::File_Other || entry->t == FileType::File_Code || entry->t == FileType::File_Header)
		{
			state |= static_cast<int>(IntegrityState::Integrity_Has_Wrong_Type_Files);
		}

		// Check if the position of the file is correct
		// Audio is not included in the binary file
		if (entry->t != FileType::File_Audio)
		{
			if (entry->po != currentPos)
			{
				state |= static_cast<int>(IntegrityState::Integrity_Wrong_File_Position);
			}
			if (entry->s == 0)
			{
				state |= static_cast<int>(IntegrityState::Integrity_Wrong_File_Size);
			}
			currentPos += entry->s;
		}

		// Check if the position of the meta file is correct
		if (entry->mpo != currentPos)
		{
			state |= static_cast<int>(IntegrityState::Integrity_Wrong_Meta_File_Position);
		}
		if (entry->ms == 0)
		{
			state |= static_cast<int>(IntegrityState::Integrity_Wrong_Meta_File_Size);
		}
		currentPos += entry->ms;
	}

	return static_cast<IntegrityState>(state);
}
