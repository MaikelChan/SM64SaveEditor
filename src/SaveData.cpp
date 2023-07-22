#include "SaveData.h"
#include <string>
#include <fstream>
#include <exception>
#include <cassert>

SaveData::SaveData()
{
	assert(sizeof(SaveData) == SAVE_DATA_SIZE);
}

SaveData* SaveData::Load(const std::string filePath)
{
	std::ifstream stream = std::ifstream(filePath, std::ios::binary);

	if (!stream || !stream.is_open())
	{
		throw std::runtime_error("There was an error trying to open open the file.");
		return nullptr;
	}

	stream.seekg(0, std::ios_base::end);
	size_t size = stream.tellg();

	if (size != SAVE_DATA_SIZE)
	{
		stream.close();
		throw std::runtime_error("The selected file is not a valid Super Mario 64 save file.");
		return nullptr;
	}

	SaveData* saveData = new SaveData();

	stream.seekg(0, std::ios_base::beg);
	stream.read((char*)saveData, SAVE_DATA_SIZE);
	stream.close();

	SaveData::Types type = saveData->GetType();

	if (type == SaveData::Types::NotValid)
	{
		delete saveData;
		throw std::runtime_error("The selected file is not a valid Super Mario 64 save file.");
		return nullptr;
	}

	return saveData;
}

void SaveData::Save(const std::string filePath, const SaveData* saveData)
{
	std::ofstream stream = std::ofstream(filePath, std::ios::binary);

	if (!stream || !stream.is_open())
	{
		throw std::runtime_error(std::string("Can't open file \"") + filePath + "\".");
	}

	stream.write((char*)saveData, SAVE_DATA_SIZE);
	stream.close();
}

SaveData::Types SaveData::GetType() const
{
	if ((saveSlots[0][0].Magic == SAVE_SLOT_MAGIC_LE || saveSlots[0][1].Magic == SAVE_SLOT_MAGIC_LE) &&
		(settings[0].Magic == SETTINGS_DATA_MAGIC_LE || settings[1].Magic == SETTINGS_DATA_MAGIC_LE))
	{
		return SaveData::Types::PC;
	}

	if ((saveSlots[0][0].Magic == SAVE_SLOT_MAGIC_BE || saveSlots[0][1].Magic == SAVE_SLOT_MAGIC_BE) &&
		(settings[0].Magic == SETTINGS_DATA_MAGIC_BE || settings[1].Magic == SETTINGS_DATA_MAGIC_BE))
	{
		return SaveData::Types::Nintendo64;
	}

	return SaveData::Types::NotValid;
}
