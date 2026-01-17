#include "SaveData.h"

#include <cassert>
#include <exception>
#include <fstream>
#include <string>

#include "../Utils.h"
#include "SaveEditorUI.h"

SaveData::SaveData()
{
	assert(sizeof(SaveData) == SAVE_DATA_SIZE);
}

SaveData::Types SaveData::Load(const std::filesystem::path filePath, SaveData* saveData)
{
	std::ifstream stream = std::ifstream(filePath, std::ios::binary);

	if (!stream || !stream.is_open())
	{
		throw std::runtime_error("There was an error trying to open the file.");
	}

	stream.seekg(0, std::ios_base::end);
	size_t size = stream.tellg();

	if (size != SAVE_DATA_SIZE)
	{
		stream.close();
		throw std::runtime_error("The selected file is not a valid Super Mario 64 save file.");
	}

	stream.seekg(0, std::ios_base::beg);
	stream.read((char*)saveData, SAVE_DATA_SIZE);
	stream.close();

	SaveData::Types type = saveData->CalculateType();

	if (type == SaveData::Types::NotValid)
	{
		throw std::runtime_error("The selected file is not a valid Super Mario 64 save file.");
	}
	else if (type == SaveData::Types::BigEndian)
	{
		saveData->EndianSwap();
	}

	saveData->CheckValidityAndFix();

	return type;
}

void SaveData::Save(const std::filesystem::path filePath, const SaveData* saveData, const SaveData::Types type)
{
	std::ofstream stream = std::ofstream(filePath, std::ios::binary);

	if (!stream || !stream.is_open())
	{
		throw std::runtime_error(std::string("Can't save file \"") + filePath.u8string() + "\".");
	}

	SaveData saveDataCopy = SaveData(*saveData);
	saveDataCopy.FillMagicAndBackupData();
	if (type == SaveData::Types::BigEndian) saveDataCopy.EndianSwap();

	stream.write((char*)&saveDataCopy, SAVE_DATA_SIZE);
	stream.close();
}

SaveData::Types SaveData::CalculateType() const
{
	if ((saveSlots[0][0].Magic == SAVE_SLOT_MAGIC_LE || saveSlots[0][1].Magic == SAVE_SLOT_MAGIC_LE) &&
		(settings[0].Magic == SETTINGS_DATA_MAGIC_LE || settings[1].Magic == SETTINGS_DATA_MAGIC_LE))
	{
		return SaveData::Types::LittleEndian;
	}

	if ((saveSlots[0][0].Magic == SAVE_SLOT_MAGIC_BE || saveSlots[0][1].Magic == SAVE_SLOT_MAGIC_BE) &&
		(settings[0].Magic == SETTINGS_DATA_MAGIC_BE || settings[1].Magic == SETTINGS_DATA_MAGIC_BE))
	{
		return SaveData::Types::BigEndian;
	}

	return SaveData::Types::NotValid;
}

void SaveData::CheckValidityAndFix()
{
	std::string message;

	for (int s = 0; s < NUM_SAVE_SLOTS; s++)
	{
		if (saveSlots[s][0].IsValid()) continue;

		if (saveSlots[s][1].IsValid())
		{
			memcpy(&saveSlots[s][0], &saveSlots[s][1], SAVE_SLOT_SIZE);
			message += std::string("Save slot \"") + tabNames[s] + "\" is corrupted, but valid data has been restored from the backup data.\n\n";
		}
		else
		{
			saveSlots[s][0].UpdateChecksum();
			message += std::string("Save slot \"") + tabNames[s] + "\" is corrupted along with its backup. Data might be completely wrong.\n\n";
		}
	}

	if (!settings[0].IsValid())
	{
		if (settings[1].IsValid())
		{
			memcpy(&settings[0], &settings[1], SETTINGS_DATA_SIZE);
			message += "Settings data is corrupted, but valid data has been restored from the backup data.\n\n";
		}
		else
		{
			settings[0].UpdateChecksum();
			message += "Settings data is corrupted along with its backup. Data might be completely wrong.\n\n";
		}
	}

	if (!message.empty())
	{
		//popupDialogUi.SetMessage(MessageTypes::Warning, "Load warnings", message);
		//popupDialogUi.SetIsVisible(true);
	}
}

void SaveData::FillMagicAndBackupData()
{
	for (int s = 0; s < NUM_SAVE_SLOTS; s++)
	{
		saveSlots[s][0].Magic = SAVE_SLOT_MAGIC_LE;
		memcpy(&saveSlots[s][1], &saveSlots[s][0], SAVE_SLOT_SIZE);
	}

	settings[0].Magic = SETTINGS_DATA_MAGIC_LE;
	memcpy(&settings[1], &settings[0], SETTINGS_DATA_SIZE);
}

void SaveData::EndianSwap()
{
	for (int s = 0; s < NUM_SAVE_SLOTS; s++)
	{
		for (int cp = 0; cp < NUM_COPIES; cp++)
		{
			saveSlots[s][cp].CapPos[0] = Swap16(saveSlots[s][cp].CapPos[0]);
			saveSlots[s][cp].CapPos[1] = Swap16(saveSlots[s][cp].CapPos[1]);
			saveSlots[s][cp].CapPos[2] = Swap16(saveSlots[s][cp].CapPos[2]);

			saveSlots[s][cp].Flags = Swap32(saveSlots[s][cp].Flags);

			saveSlots[s][cp].Magic = Swap16(saveSlots[s][cp].Magic);
			saveSlots[s][cp].Checksum = Swap16(saveSlots[s][cp].Checksum);
		}
	}

	for (int cp = 0; cp < NUM_COPIES; cp++)
	{
		for (int s = 0; s < NUM_SAVE_SLOTS; s++)
		{
			settings[cp].CoinScoreAges[s] = Swap32(settings[cp].CoinScoreAges[s]);
		}

		settings[cp].soundMode = Swap16(settings[cp].soundMode);
		settings[cp].language = Swap16(settings[cp].language);
		settings[cp].Magic = Swap16(settings[cp].Magic);
		settings[cp].Checksum = Swap16(settings[cp].Checksum);
	}
}
