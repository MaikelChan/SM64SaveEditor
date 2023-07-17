#include "SaveData.h"

SaveData::SaveData()
{

}

bool SaveData::Load(const char* filePath)
{
	std::ifstream stream = std::ifstream(filePath, std::ios::binary);

	if (!stream.is_open())
	{
		printf("Can't load file: %s", filePath);
		return false;
	}

	stream.seekg(0, std::ios_base::beg);

	for (int f = 0; f < NUM_SAVE_FILES; f++)
	{
		for (int c = 0; c < NUM_COPIES; c++)
		{
			size_t currentPosition = stream.tellg();
			bool valid = IsValid(stream, SAVE_FILE_SIZE, SAVE_FILE_MAGIC);
			stream.seekg(currentPosition, std::ios::beg);

			if (!valid)
			{
				if (c == 0)
				{
					stream.seekg(SAVE_FILE_SIZE, std::ios::cur);
					continue;
				}
				else
				{
					stream.close();
					printf("Save file %s is corrupt.", filePath);
					return false;
				}
			}

			stream.read((char*)&saveSlots[f].CapLevel, 1);
			stream.read((char*)&saveSlots[f].CapArea, 1);
			stream.read((char*)&saveSlots[f].CapPos[0], sizeof(uint16_t) * 3);

			uint32_t flags;
			stream.read((char*)&flags, sizeof(uint32_t));

			saveSlots[f].FileExists = (flags & (1 << 0)) != 0;
			saveSlots[f].HaveWingCap = (flags & (1 << 1)) != 0;
			saveSlots[f].HaveMetalCap = (flags & (1 << 2)) != 0;
			saveSlots[f].HaveVanishCap = (flags & (1 << 3)) != 0;
			saveSlots[f].HaveKey1 = (flags & (1 << 4)) != 0;
			saveSlots[f].HaveKey2 = (flags & (1 << 5)) != 0;
			saveSlots[f].BasementDoorUnlocked = (flags & (1 << 6)) != 0;
			saveSlots[f].UpstairsDoorUnlocked = (flags & (1 << 7)) != 0;
			saveSlots[f].DDDMovedBack = (flags & (1 << 8)) != 0;
			saveSlots[f].MoatDrained = (flags & (1 << 9)) != 0;
			saveSlots[f].PSSDoorUnlocked = (flags & (1 << 10)) != 0;
			saveSlots[f].WFDoorUnlocked = (flags & (1 << 11)) != 0;
			saveSlots[f].CCMDoorUnlocked = (flags & (1 << 12)) != 0;
			saveSlots[f].JRBDoorUnlocked = (flags & (1 << 13)) != 0;
			saveSlots[f].BITDWDoorUnlocked = (flags & (1 << 14)) != 0;
			saveSlots[f].BITSDoorUnlocked = (flags & (1 << 15)) != 0;
			saveSlots[f].CapOnGround = (flags & (1 << 16)) != 0;
			saveSlots[f].CapOnKlepto = (flags & (1 << 17)) != 0;
			saveSlots[f].CapOnUkiki = (flags & (1 << 18)) != 0;
			saveSlots[f].CapOnMrBlizzard = (flags & (1 << 19)) != 0;
			saveSlots[f].FiftyStarDoorUnlocked = (flags & (1 << 20)) != 0;

			for (int i = 0; i < COURSE_COUNT; i++)
			{
				uint8_t stars;
				stream.read((char*)&stars, 1);

				uint8_t cannon;
				stream.read((char*)&cannon, 1);

				bool cannonOpen = (cannon & 0x80) != 0;

				saveSlots[f].CourseStars[i].SetStars(stars);
				saveSlots[f].CourseStars[i].CannonOpen = cannonOpen;

				stream.seekg(-1, std::ios::cur);
			}

			stream.seekg(1, std::ios::cur);

			for (int i = 0; i < COURSE_STAGES_COUNT; i++)
			{
				stream.read((char*)&saveSlots[f].CourseStars[i].MaxCoins, 1);
			}

			if (c == 0)
			{
				stream.seekg(4 + SAVE_FILE_SIZE, std::ios::cur);
				break;
			}
			else
			{
				stream.seekg(4, std::ios::cur);
			}
		}
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

	uint16_t magic = static_cast<uint16_t>((m2 << 8) | m1);
	uint16_t checksum = static_cast<uint16_t>((c2 << 8) | c1);

	if (magic != magicToCheck) return false;
	if (checksum != CalculateChecksum(stream, size - 2)) return false;

	return true;
}