#include "SaveData.h"
#include <string>
#include <exception>

SaveData::SaveData()
{

}

SaveData* SaveData::Load(const char* filePath)
{
	std::ifstream stream = std::ifstream(filePath, std::ios::binary);

	if (!stream || !stream.is_open())
	{
		throw std::runtime_error(std::string("Can't open file \"") + filePath + "\".");
		return nullptr;
	}

	stream.seekg(0, std::ios_base::end);
	size_t size = stream.tellg();

	if (size != SAVE_DATA_SIZE)
	{
		stream.close();
		throw std::runtime_error(std::string("File \"") + filePath + "\" is not a valid Super Mario 64 save file.");
		return nullptr;
	}

	if (sizeof(SaveData) != SAVE_DATA_SIZE)
	{
		stream.close();
		throw std::runtime_error(std::string("Size of SaveData struct is not ") + std::to_string(SAVE_DATA_SIZE) + " but " + std::to_string(sizeof(SaveData)) + ". This shouldn't be happening.");
		return nullptr;
	}

	SaveData* saveData = new SaveData();

	stream.seekg(0, std::ios_base::beg);
	stream.read((char*)saveData, SAVE_DATA_SIZE);

	stream.close();
	return saveData;
}

void SaveData::Save(const char* filePath, const SaveData* saveData)
{
	std::ofstream stream = std::ofstream(filePath, std::ios::binary);

	if (!stream || !stream.is_open())
	{
		throw std::runtime_error(std::string("Can't open file \"") + filePath + "\".");
	}

	stream.write((char*)saveData, SAVE_DATA_SIZE);
	stream.close();
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