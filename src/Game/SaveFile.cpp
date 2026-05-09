#include "SaveFile.h"

#include "../Utils.h"
#include "SaveData.h"

SaveFile::SaveFile()
{
	saveData = nullptr;

	filePath.clear();
	fileName.clear();
	fileType = SaveFileTypes::NotValid;
}

SaveFile::~SaveFile()
{
	if (saveData != nullptr)
	{
		delete saveData;
		saveData = nullptr;
	}
}

std::string SaveFile::Read(std::ifstream& stream, const size_t streamSize)
{
	SaveData* newSaveData = new SaveData();
	stream.read((char*)newSaveData, SAVE_DATA_SIZE);

	// Check what type of file is and initialize

	fileType = CalculateType(newSaveData);

	switch (fileType)
	{
		case SaveFileTypes::NotValid:
		{
			return "The selected file is not a valid Super Mario 64 save file.";
		}

		case SaveFileTypes::BigEndian:
		{
			newSaveData->EndianSwap();
			break;
		}

		case SaveFileTypes::LittleEndian:
		{
			break;
		}

		default:
		{
			throw std::runtime_error("SaveFile type " + std::to_string((int)fileType) + " not implemented.");
		}
	}

	// It's a valid file type, but check if there are data inconsistencies that should trigger a warning

	std::string warningMessages;

	for (int s = 0; s < NUM_SAVE_SLOTS; s++)
	{
		if (newSaveData->saveSlots[s][0].IsValid()) continue;

		if (newSaveData->saveSlots[s][1].IsValid())
		{
			std::copy(&newSaveData->saveSlots[s][1], &newSaveData->saveSlots[s][1] + 1, &newSaveData->saveSlots[s][0]);
			warningMessages += std::string("Save slot \"") + std::to_string(s) + "\" is corrupted, but valid data has been restored from the backup data.\n\n";
		}
		else
		{
			newSaveData->saveSlots[s][0].UpdateChecksum();
			warningMessages += std::string("Save slot \"") + std::to_string(s) + "\" is corrupted along with its backup. Data might be completely wrong.\n\n";
		}
	}

	if (!newSaveData->settings[0].IsValid())
	{
		if (newSaveData->settings[1].IsValid())
		{
			std::copy(&newSaveData->settings[1], &newSaveData->settings[1] + 1, &newSaveData->settings[0]);
			warningMessages += "Settings data is corrupted, but valid data has been restored from the backup data.\n\n";
		}
		else
		{
			newSaveData->settings[0].UpdateChecksum();
			warningMessages += "Settings data is corrupted along with its backup. Data might be completely wrong.\n\n";
		}
	}

	// Assign references to the loaded data before finishing

	saveData = newSaveData;

	return warningMessages;
}

void SaveFile::Write(std::ofstream& stream)
{
	// Refresh all checksums

	for (int s = 0; s < NUM_SAVE_SLOTS; s++)
	{
		saveData->saveSlots[s][0].Magic = SAVE_SLOT_MAGIC_LE;
		std::copy(&saveData->saveSlots[s][0], &saveData->saveSlots[s][0] + 1, &saveData->saveSlots[s][1]);
	}

	saveData->settings[0].Magic = SETTINGS_DATA_MAGIC_LE;
	std::copy(&saveData->settings[0], &saveData->settings[0] + 1, &saveData->settings[1]);

	// Do the saving

	if (fileType == SaveFileTypes::BigEndian) saveData->EndianSwap();

	stream.write((char*)saveData, SAVE_DATA_SIZE);

	// Keep the data in memory in little endian

	if (fileType == SaveFileTypes::BigEndian) saveData->EndianSwap();
}

SaveFileTypes SaveFile::CalculateType(SaveData* saveData)
{
	if (saveData == nullptr) return SaveFileTypes::NotValid;

	bool hasLeSlot = (saveData->saveSlots[0][0].Magic == SAVE_SLOT_MAGIC_LE || saveData->saveSlots[0][1].Magic == SAVE_SLOT_MAGIC_LE);
	bool hasLeSettings = (saveData->settings[0].Magic == SETTINGS_DATA_MAGIC_LE || saveData->settings[1].Magic == SETTINGS_DATA_MAGIC_LE);

	bool hasBeSlot = (saveData->saveSlots[0][0].Magic == SAVE_SLOT_MAGIC_BE || saveData->saveSlots[0][1].Magic == SAVE_SLOT_MAGIC_BE);
	bool hasBeSettings = (saveData->settings[0].Magic == SETTINGS_DATA_MAGIC_BE || saveData->settings[1].Magic == SETTINGS_DATA_MAGIC_BE);

	bool hasNoSettings = (saveData->settings[0].Magic == 0 && saveData->settings[1].Magic == 0);

	if (hasLeSlot && (hasLeSettings || hasNoSettings))
	{
		return SaveFileTypes::LittleEndian;
	}

	if (hasBeSlot && (hasBeSettings || hasNoSettings))
	{
		return SaveFileTypes::BigEndian;
	}

	return SaveFileTypes::NotValid;
}
