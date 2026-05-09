#include "SaveData.h"

#include <cassert>

#include "../Utils.h"

SaveData::SaveData()
{
	assert(sizeof(SaveData) == SAVE_DATA_SIZE);
}

void SaveData::EndianSwap()
{
	for (uint8_t s = 0; s < NUM_SAVE_SLOTS; s++)
	{
		for (uint8_t cp = 0; cp < NUM_COPIES; cp++)
		{
			saveSlots[s][cp].CapPos[0] = Utils::Swap16(saveSlots[s][cp].CapPos[0]);
			saveSlots[s][cp].CapPos[1] = Utils::Swap16(saveSlots[s][cp].CapPos[1]);
			saveSlots[s][cp].CapPos[2] = Utils::Swap16(saveSlots[s][cp].CapPos[2]);

			saveSlots[s][cp].Flags = Utils::Swap32(saveSlots[s][cp].Flags);

			saveSlots[s][cp].Magic = Utils::Swap16(saveSlots[s][cp].Magic);
			saveSlots[s][cp].Checksum = Utils::Swap16(saveSlots[s][cp].Checksum);
		}
	}

	for (uint8_t cp = 0; cp < NUM_COPIES; cp++)
	{
		for (uint8_t s = 0; s < NUM_SAVE_SLOTS; s++)
		{
			settings[cp].CoinScoreAges[s] = Utils::Swap32(settings[cp].CoinScoreAges[s]);
		}

		settings[cp].soundMode = Utils::Swap16(settings[cp].soundMode);
		settings[cp].language = Utils::Swap16(settings[cp].language);
		settings[cp].Magic = Utils::Swap16(settings[cp].Magic);
		settings[cp].Checksum = Utils::Swap16(settings[cp].Checksum);
	}
}
