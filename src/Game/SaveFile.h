#pragma once

#include <filesystem>
#include <fstream>

class SaveData;

enum class SaveFileTypes { NotValid, BigEndian, LittleEndian };

class SaveFile
{
private:
	SaveData* saveData;

	std::filesystem::path filePath;
	std::string fileName;
	SaveFileTypes fileType;

public:
	SaveFile();
	~SaveFile();

	std::string Read(std::ifstream& stream, const size_t streamSize);
	void Write(std::ofstream& stream);

	inline SaveData* GetSaveData() const { return saveData; }

	inline std::filesystem::path GetFilePath() const { return filePath; }
	inline void SetFilePath(std::filesystem::path _filePath) { filePath = _filePath; fileName = _filePath.filename().u8string(); }
	inline std::string GetFileName() const { return fileName; }
	inline SaveFileTypes GetFileType() const { return fileType; }

private:
	static SaveFileTypes CalculateType(SaveData* saveData);
};
