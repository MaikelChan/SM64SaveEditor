#pragma once

#include <fstream>

#define SAVE_DATA_SIZE 0x200

#define NUM_SAVE_SLOTS 4
#define NUM_COPIES 2
#define COURSE_COUNT 24
#define COURSE_STAGES_COUNT 15
#define COURSE_STAGES_COUNT_INTERNAL 38 // This is the actual game's number of levels, including unused ones.

#define SETTINGS_DATA_MAGIC_LE 0x4849
#define SETTINGS_DATA_MAGIC_BE 0x4948
#define SETTINGS_DATA_SIZE 0x20

#define SAVE_SLOT_MAGIC_LE 0x4441
#define SAVE_SLOT_MAGIC_BE 0x4144
#define SAVE_SLOT_SIZE 0x38

#define MAX_STARS_PER_LEVEL 7

#define SAVE_FLAG_FILE_EXISTS             (1 << 0)
#define SAVE_FLAG_HAVE_WING_CAP           (1 << 1)
#define SAVE_FLAG_HAVE_METAL_CAP          (1 << 2)
#define SAVE_FLAG_HAVE_VANISH_CAP         (1 << 3)
#define SAVE_FLAG_HAVE_KEY_1              (1 << 4)
#define SAVE_FLAG_HAVE_KEY_2              (1 << 5)
#define SAVE_FLAG_UNLOCKED_BASEMENT_DOOR  (1 << 6)
#define SAVE_FLAG_UNLOCKED_UPSTAIRS_DOOR  (1 << 7)
#define SAVE_FLAG_DDD_MOVED_BACK          (1 << 8)
#define SAVE_FLAG_MOAT_DRAINED            (1 << 9)
#define SAVE_FLAG_UNLOCKED_PSS_DOOR       (1 << 10)
#define SAVE_FLAG_UNLOCKED_WF_DOOR        (1 << 11)
#define SAVE_FLAG_UNLOCKED_CCM_DOOR       (1 << 12)
#define SAVE_FLAG_UNLOCKED_JRB_DOOR       (1 << 13)
#define SAVE_FLAG_UNLOCKED_BITDW_DOOR     (1 << 14)
#define SAVE_FLAG_UNLOCKED_BITFS_DOOR     (1 << 15)
#define SAVE_FLAG_CAP_ON_GROUND           (1 << 16)
#define SAVE_FLAG_CAP_ON_KLEPTO           (1 << 17)
#define SAVE_FLAG_CAP_ON_UKIKI            (1 << 18)
#define SAVE_FLAG_CAP_ON_MR_BLIZZARD      (1 << 19)
#define SAVE_FLAG_UNLOCKED_50_STAR_DOOR   (1 << 20)
#define SAVE_FLAG_COLLECTED_TOAD_STAR_1   (1 << 24)
#define SAVE_FLAG_COLLECTED_TOAD_STAR_2   (1 << 25)
#define SAVE_FLAG_COLLECTED_TOAD_STAR_3   (1 << 26)
#define SAVE_FLAG_COLLECTED_MIPS_STAR_1   (1 << 27)
#define SAVE_FLAG_COLLECTED_MIPS_STAR_2   (1 << 28)

#define SAVE_COURSE_FLAG_STAR_1           (1 << 0)
#define SAVE_COURSE_FLAG_STAR_2           (1 << 1)
#define SAVE_COURSE_FLAG_STAR_3           (1 << 2)
#define SAVE_COURSE_FLAG_STAR_4           (1 << 3)
#define SAVE_COURSE_FLAG_STAR_5           (1 << 4)
#define SAVE_COURSE_FLAG_STAR_6           (1 << 5)
#define SAVE_COURSE_FLAG_STAR_100_COIN    (1 << 6)
#define SAVE_COURSE_FLAG_STAR_CANNON_OPEN (1 << 7)

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

const uint8_t courseStarCount[]
{
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, // Main courses
	1, 1, 1, 2, 1, 1, 1, 1, 1                    // Extra courses
};

const bool courseHasCannon[]
{
	true, true, true, true, false, false, false, true, false, true, true, true, true, false, true, // Main courses
	false, false, false, false, false, false, false, true, false                                    // Extra courses
};

struct SettingsData
{
public:
	// Each save file has a 2 bit "age" for each course. The higher this value,
	// the older the high score is. This is used for tie-breaking when displaying
	// on the high score screen.
	uint32_t CoinScoreAges[NUM_SAVE_SLOTS];
	uint16_t soundMode;
	uint16_t language;
	uint8_t padding[8];
	uint16_t Magic;
	uint16_t Checksum;

	inline uint16_t CalculateChecksum() const
	{
		uint16_t checksum = 0;
		const uint8_t* p = reinterpret_cast<const uint8_t*>(&CoinScoreAges[0]);

		for (int i = 0; i < SETTINGS_DATA_SIZE - 2; i++)
		{
			checksum += *p++;
		}

		return checksum;
	}

	inline void UpdateChecksum()
	{
		Checksum = CalculateChecksum();
	}

	inline bool IsValid()
	{
		return Checksum == CalculateChecksum();
	}
};

struct SaveSlot
{
public:
	uint8_t CapLevel;
	uint8_t CapArea;
	uint16_t CapPos[3];
	uint32_t Flags;
	uint8_t CourseData[COURSE_COUNT];
	uint8_t unusedCourseData;
	uint8_t CourseCoinScores[COURSE_STAGES_COUNT];
	uint16_t Magic;
	uint16_t Checksum;

	inline uint16_t CalculateChecksum() const
	{
		uint16_t checksum = 0;
		const uint8_t* p = &CapLevel;

		for (int i = 0; i < SAVE_SLOT_SIZE - 2; i++)
		{
			checksum += *p++;
		}

		return checksum;
	}

	inline void UpdateChecksum()
	{
		Checksum = CalculateChecksum();
	}

	inline bool IsValid()
	{
		return Checksum == CalculateChecksum();
	}

	inline bool GetFlag(const uint32_t mask) const
	{
		return (Flags & mask) != 0;
	}

	inline void SetFlag(const uint32_t mask, const bool value)
	{
		if (value) Flags |= mask;
		else Flags &= ~mask;
	}

	inline bool GetCourseDataFlag(const uint8_t courseIndex, const uint8_t mask) const
	{
		return (CourseData[courseIndex] & mask) != 0;
	}

	inline void SetCourseDataFlag(const uint8_t courseIndex, const uint8_t mask, const bool value)
	{
		if (value) CourseData[courseIndex] |= mask;
		else CourseData[courseIndex] &= ~mask;
	}
};

class SaveData
{
public:
	SaveSlot saveSlots[NUM_SAVE_SLOTS][NUM_COPIES] = {};
	SettingsData settings[NUM_COPIES] = {};

	enum class Types { NotValid, PC, Nintendo64 };

	SaveData();

	static SaveData* Load(const char* filePath);
	static void Save(const char* filePath, const SaveData* saveData);

	Types GetType() const;
};
