#pragma once

#include <fstream>

#define NUM_SAVE_SLOTS 4
#define NUM_COPIES 2
#define COURSE_COUNT 24
#define COURSE_STAGES_COUNT 15

#define SETTINGS_DATA_MAGIC 0x4849
#define SETTINGS_DATA_SIZE 0x20

#define SAVE_FILE_MAGIC 0x4441
#define SAVE_FILE_SIZE 0x38

const char* const courseNames[]
{
	"Bob-Omb Battlefield",
	"Whomp's Fortress",
	"Jolly Roger Bay",
	"Cool, Cool Mountain",
	"Big Boo's Haunt",
	"Hazy Maze Cave",
	"Lethal Lava Land",
	"Shifting Sand Land",
	"Dire, Dire Docks",
	"Snowman's Land",
	"Wet-Dry World",
	"Tall, Tall Mountain",
	"Tiny-Huge Island",
	"Tick Tock Clock",
	"Rainbow Ride",
	// Extra courses
	"Bowser in the Dark World",
	"Bowser in the Fire Sea",
	"Bowser in the Sky",
	"Princess's Secret Slide",
	"Cavern of the Metal Cap",
	"Tower of the Wing Cap",
	"Vanish Cap Under the Moat",
	"Winged Mario over the Rainbow",
	"Secret Aquarium"
};

struct SettingsData
{
public:
	// Each save file has a 2 bit "age" for each course. The higher this value,
	// the older the high score is. This is used for tie-breaking when displaying
	// on the high score screen.
	uint32_t CoinScoreAges[NUM_SAVE_SLOTS] = {};
	uint16_t soundMode = 0;
	uint16_t language = 0;
};

struct CourseData
{
public:
	bool Star1 = false;
	bool Star2 = false;
	bool Star3 = false;
	bool Star4 = false;
	bool Star5 = false;
	bool Star6 = false;
	bool HundredCoinStar = false;
	bool CannonOpen = false;
	uint8_t MaxCoins = 0;

	void SetStars(const uint8_t stars)
	{
		Star1 = (stars & (1 << 0)) != 0;
		Star2 = (stars & (1 << 1)) != 0;
		Star3 = (stars & (1 << 2)) != 0;
		Star4 = (stars & (1 << 3)) != 0;
		Star5 = (stars & (1 << 4)) != 0;
		Star6 = (stars & (1 << 5)) != 0;
		HundredCoinStar = (stars & (1 << 6)) != 0;
	}
};

struct SaveSlot
{
public:
	uint8_t CapLevel = 0;
	uint8_t CapArea = 0;
	uint16_t CapPos[3] = {};

	bool FileExists = false;
	bool HaveWingCap = false;
	bool HaveMetalCap = false;
	bool HaveVanishCap = false;
	bool HaveKey1 = false;
	bool HaveKey2 = false;
	bool BasementDoorUnlocked = false;
	bool UpstairsDoorUnlocked = false;
	bool DDDMovedBack = false;
	bool MoatDrained = false;
	bool PSSDoorUnlocked = false;
	bool WFDoorUnlocked = false;
	bool CCMDoorUnlocked = false;
	bool JRBDoorUnlocked = false;
	bool BITDWDoorUnlocked = false;
	bool BITSDoorUnlocked = false;
	bool CapOnGround = false;
	bool CapOnKlepto = false;
	bool CapOnUkiki = false;
	bool CapOnMrBlizzard = false;
	bool FiftyStarDoorUnlocked = false;

	CourseData Courses[COURSE_COUNT] = {};
};

class SaveData
{
public:
	SaveSlot saveSlots[4] = {};
	SettingsData settings = {};

	SaveData();

	bool Load(const char* filePath);

private:
	uint16_t CalculateChecksum(std::ifstream& stream, const size_t size);
	bool IsValid(std::ifstream& stream, const size_t size, const uint16_t magicToCheck);
};
