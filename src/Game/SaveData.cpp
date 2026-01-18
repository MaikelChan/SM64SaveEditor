#include "SaveData.h"

#include <cassert>

#include "../Utils.h"

SaveData::SaveData()
{
	assert(sizeof(SaveData) == SAVE_DATA_SIZE);
}

SaveData::InitializationResult SaveData::CheckAndInitialize()
{
	SaveData::Types type = CalculateType();

	if (type == SaveData::Types::NotValid)
	{
		return { SaveData::Types::NotValid, "The selected file is not a valid Super Mario 64 save file." };
	}
	else if (type == SaveData::Types::BigEndian)
	{
		EndianSwap();
	}

	std::string warnings = CheckValidityAndFix();

	return { type, warnings };
}

void SaveData::PrepareForSaving(const Types type)
{
	for (int s = 0; s < NUM_SAVE_SLOTS; s++)
	{
		saveSlots[s][0].Magic = SAVE_SLOT_MAGIC_LE;
		std::copy(&saveSlots[s][0], &saveSlots[s][0] + 1, &saveSlots[s][1]);
	}

	settings[0].Magic = SETTINGS_DATA_MAGIC_LE;
	std::copy(&settings[0], &settings[0] + 1, &settings[1]);

	if (type == SaveData::Types::BigEndian) EndianSwap();
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

std::string SaveData::CheckValidityAndFix()
{
	std::string message;

	for (int s = 0; s < NUM_SAVE_SLOTS; s++)
	{
		if (saveSlots[s][0].IsValid()) continue;

		if (saveSlots[s][1].IsValid())
		{
			std::copy(&saveSlots[s][1], &saveSlots[s][1] + 1, &saveSlots[s][0]);
			message += std::string("Save slot \"") + std::to_string(s) + "\" is corrupted, but valid data has been restored from the backup data.\n\n";
		}
		else
		{
			saveSlots[s][0].UpdateChecksum();
			message += std::string("Save slot \"") + std::to_string(s) + "\" is corrupted along with its backup. Data might be completely wrong.\n\n";
		}
	}

	if (!settings[0].IsValid())
	{
		if (settings[1].IsValid())
		{
			std::copy(&settings[1], &settings[1] + 1, &settings[0]);
			message += "Settings data is corrupted, but valid data has been restored from the backup data.\n\n";
		}
		else
		{
			settings[0].UpdateChecksum();
			message += "Settings data is corrupted along with its backup. Data might be completely wrong.\n\n";
		}
	}

	return message;
}
