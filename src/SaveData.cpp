#include "SaveData.h"

SaveData::SaveData()
{

}

bool SaveData::Load(const char* filePath)
{
	std::ifstream stream = std::ifstream(filePath, std::ios::binary);

	if (!stream.is_open())
	{
		printf("Can't load file: %s\n", filePath);
		return false;
	}

	stream.seekg(0, std::ios_base::beg);

	for (int s = 0; s < NUM_SAVE_SLOTS; s++)
	{
		for (int cp = 0; cp < NUM_COPIES; cp++)
		{
			size_t currentPosition = stream.tellg();
			bool valid = IsValid(stream, SAVE_FILE_SIZE, SAVE_FILE_MAGIC);
			stream.seekg(currentPosition, std::ios::beg);

			if (valid)
			{
				stream.read((char*)&saveSlots[s].CapLevel, 1);
				stream.read((char*)&saveSlots[s].CapArea, 1);
				stream.read((char*)&saveSlots[s].CapPos[0], sizeof(uint16_t) * 3);

				uint32_t flags;
				stream.read((char*)&flags, sizeof(uint32_t));

				saveSlots[s].FileExists = (flags & (1 << 0)) != 0;
				saveSlots[s].HaveWingCap = (flags & (1 << 1)) != 0;
				saveSlots[s].HaveMetalCap = (flags & (1 << 2)) != 0;
				saveSlots[s].HaveVanishCap = (flags & (1 << 3)) != 0;
				saveSlots[s].HaveKey1 = (flags & (1 << 4)) != 0;
				saveSlots[s].HaveKey2 = (flags & (1 << 5)) != 0;
				saveSlots[s].BasementDoorUnlocked = (flags & (1 << 6)) != 0;
				saveSlots[s].UpstairsDoorUnlocked = (flags & (1 << 7)) != 0;
				saveSlots[s].DDDMovedBack = (flags & (1 << 8)) != 0;
				saveSlots[s].MoatDrained = (flags & (1 << 9)) != 0;
				saveSlots[s].PSSDoorUnlocked = (flags & (1 << 10)) != 0;
				saveSlots[s].WFDoorUnlocked = (flags & (1 << 11)) != 0;
				saveSlots[s].CCMDoorUnlocked = (flags & (1 << 12)) != 0;
				saveSlots[s].JRBDoorUnlocked = (flags & (1 << 13)) != 0;
				saveSlots[s].BITDWDoorUnlocked = (flags & (1 << 14)) != 0;
				saveSlots[s].BITSDoorUnlocked = (flags & (1 << 15)) != 0;
				saveSlots[s].CapOnGround = (flags & (1 << 16)) != 0;
				saveSlots[s].CapOnKlepto = (flags & (1 << 17)) != 0;
				saveSlots[s].CapOnUkiki = (flags & (1 << 18)) != 0;
				saveSlots[s].CapOnMrBlizzard = (flags & (1 << 19)) != 0;
				saveSlots[s].FiftyStarDoorUnlocked = (flags & (1 << 20)) != 0;

				for (int cc = 0; cc < COURSE_COUNT; cc++)
				{
					uint8_t stars;
					stream.read((char*)&stars, 1);

					uint8_t cannon;
					stream.read((char*)&cannon, 1);

					bool cannonOpen = (cannon & 0x80) != 0;

					saveSlots[s].Courses[cc].SetStars(stars);
					saveSlots[s].Courses[cc].CannonOpen = cannonOpen;

					stream.seekg(-1, std::ios::cur);
				}

				stream.seekg(1, std::ios::cur);

				for (int cc = 0; cc < COURSE_STAGES_COUNT; cc++)
				{
					stream.read((char*)&saveSlots[s].Courses[cc].MaxCoins, 1);
				}

				if (cp == 0)
				{
					stream.seekg(4 + SAVE_FILE_SIZE, std::ios::cur);
					break;
				}
				else
				{
					stream.seekg(4, std::ios::cur);
				}

				break;
			}

			if (cp == 0)
			{
				stream.seekg(SAVE_FILE_SIZE, std::ios::cur);
				continue;
			}

			stream.close();
			printf("Save slot %i in file %s is corrupted.\n", s, filePath);
			return false;
		}
	}

	for (int cp = 0; cp < NUM_COPIES; cp++)
	{
		size_t currentPosition = stream.tellg();
		bool valid = IsValid(stream, SETTINGS_DATA_SIZE, SETTINGS_DATA_MAGIC);
		stream.seekg(currentPosition, std::ios::beg);

		if (valid)
		{
			for (int s = 0; s < NUM_SAVE_SLOTS; s++)
			{
				stream.read((char*)&settings.CoinScoreAges[s], 4);
			}

			stream.read((char*)&settings.soundMode, 2);
			stream.read((char*)&settings.language, 2);

			break;
		}

		if (cp == 0)
		{
			stream.seekg(SETTINGS_DATA_SIZE, std::ios::cur);
			continue;
		}

		stream.close();
		printf("Menu data in file %s is corrupted.\n", filePath);
		return false;
	}

	stream.close();
	return true;
}

uint16_t SaveData::CalculateChecksum(std::ifstream& stream, const size_t size)
{
	uint16_t chksum = 0;

	for (long d = 0; d < size; d++)
	{
		uint8_t byte;
		stream.read((char*)&byte, 1);
		chksum += byte;
	}

	return chksum;
}

bool SaveData::IsValid(std::ifstream& stream, const size_t size, const uint16_t magicToCheck)
{
	uint8_t m1, m2, c1, c2;

	size_t position = stream.tellg();
	stream.seekg(position + size - 4, std::ios_base::beg);

	stream.read((char*)&m1, 1);
	stream.read((char*)&m2, 1);
	stream.read((char*)&c1, 1);
	stream.read((char*)&c2, 1);

	stream.seekg(position, std::ios_base::beg);

	uint16_t magic = static_cast<uint16_t>((m2 << 8) | m1);
	uint16_t checksum = static_cast<uint16_t>((c2 << 8) | c1);

	if (magic != magicToCheck) return false;
	if (checksum != CalculateChecksum(stream, size - 2)) return false;

	return true;
}