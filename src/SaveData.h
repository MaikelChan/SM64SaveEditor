#pragma once

#include <cstdint>
#include <string>

#define SAVE_DATA_SIZE 0x200

#define NUM_SAVE_SLOTS 4
#define NUM_COPIES 2
#define COURSE_COUNT 24
#define COURSE_STAGES_COUNT 15
#define COURSE_STAGES_COUNT_INTERNAL 38 // This is the actual game's number of levels, including unused ones.

#define SAVE_SLOT_MAGIC_LE 0x4441
#define SAVE_SLOT_MAGIC_BE 0x4144
#define SAVE_SLOT_SIZE 0x38

#define SETTINGS_DATA_MAGIC_LE 0x4849
#define SETTINGS_DATA_MAGIC_BE 0x4948
#define SETTINGS_DATA_SIZE 0x20

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

const char* const courseNames[COURSE_COUNT]
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

const char* const courseStarNames[COURSE_COUNT][MAX_STARS_PER_LEVEL]
{
	{   // Bob-omb Battlefield
		"Big Bob-omb on the Summit",
		"Footrace with Koopa the Quick",
		"Shoot to the Island in the Sky",
		"Find the 8 Red Coins",
		"Mario Wings to the Sky",
		"Behind Chain Chomp's Gate",
		u8"100 Coin \u0101"
	},
	{   // Whomp's Fortress
		"Chip off Whomp's Block",
		"To the Top of the Fortress",
		"Shoot into the Wild Blue",
		"Red Coins on the Floating Isle",
		"Fall onto the Caged Island",
		"Blast Away the Wall",
		u8"100 Coin \u0101"
	},
	{   // Jolly Roger Bay
		"Plunder in the Sunken Ship",
		"Can the Eel Come out and Play ?",
		"Treasure of the Ocean Cave",
		"Red Coins on the Ship Afloat",
		"Blast to the Stone Pillar",
		"Through the Jet Stream",
		u8"100 Coin \u0101"
	},
	{   // Cool, Cool Mountain
		"Slip Slidin' Away",
		"Li'l Penguin Lost",
		"Big Penguin Race",
		"Frosty Slide for 8 Red Coins",
		"Snowman's Lost his Head",
		"Wall Kicks will Work",
		u8"100 Coin \u0101"
	},
	{   // Big Boo's Haunt
		"Go on a Ghost Hunt",
		"Ride Big Boo's Merry - Go - Round",
		"Secret of the Haunted Books",
		"Seek the 8 Red Coins",
		"Big Boo's Balcony",
		"Eye to Eye in the Secret Room",
		u8"100 Coin \u0101"
	},
	{	// Hazy Maze Cave
		"Swimming Beast in the Cavern",
		"Elevate for 8 Red Coins",
		"Metal - Head Mario Can Move!",
		"Navigating the Toxic Maze",
		"A - Maze - ing Emergency Exit",
		"Watch for the Rolling Rocks",
		u8"100 Coin \u0101"
	},
	{   // Lethal Lava Land
		"Boil the Big Bully",
		"Bully the Bullies",
		"8 - Coin Puzzle with 15 Pieces",
		"Red - Hot Log Rolling",
		"Hot - Foot - It into the Volcano",
		"Elevator Tour in the Volcano",
		u8"100 Coin \u0101"
	},
	{	// Shifting Sand Land
		"In the Talons of the Big Bird",
		"Shining Atop the Pyramid",
		"Inside the Ancient Pyramid",
		"Stand Tall on the Four Pillars",
		"Free Flying for 8 Red Coins",
		"Pyramid Puzzle",
		u8"100 Coin \u0101"
	},
	{   // Dire, Dire Docks
		"Board Bowser's Sub",
		"Chests in the Current",
		"Pole - Jumping for Red Coins",
		"Through the Jet Stream",
		"The Manta Ray's Reward",
		"Collect the Caps",
		u8"100 Coin \u0101"
	},
	{   // Snowman's Land
		"Snowman's Big Head",
		"Chill with the Bully",
		"In the Deep Freeze",
		"Whirl from the Freezing Pond",
		"Shell Shreddin' for 8 Red Coins",
		"Into the Igloo",
		u8"100 Coin \u0101"
	},
	{	// Wet - Dry World
		"Shocking Arrow Lifts!",
		"Top O' The Town",
		"Secrets in the Shallows & Sky",
		"Express Elevators–Hurry Up!",
		"Go to Town for Red Coins",
		"Quick Race through Downtown",
		u8"100 Coin \u0101"
	},
	{	// Tall, Tall Mountain
		"Scale the Mountain",
		"Mystery of the Monkey Cage",
		"Scary ‘Shrooms, Red Coins",
		"Mysterious Mountainside",
		"Breathtaking View from Bridge",
		"Blast to the Lonely Mushroom",
		u8"100 Coin \u0101"
	},
	{	// Tiny - Huge Island
		"Pluck the Piranha Flower",
		"The Tip Top of the Huge Island",
		"Rematch with Koopa the Quick",
		"Five Itty Bitty Secrets",
		"Wiggler's Red Coins",
		"Make Wiggler Squirm",
		u8"100 Coin \u0101"
	},
	{   // Tick Tock Clock
		"Roll into the Cage",
		"The Pit and the Pendulums",
		"Get a Hand",
		"Stomp on the Thwomp",
		"Timed Jumps on Moving Bars",
		"Stop Time for Red Coins",
		u8"100 Coin \u0101"
	},
	{   // Rainbow Ride
		"Cruiser Crossing the Rainbow",
		"The Big House in the Sky",
		"Coins Amassed in a Maze",
		"Swingin' in the Breeze",
		"Tricky Triangles!",
		"Somewhere over the Rainbow",
		u8"100 Coin \u0101"
	},
	// Bowser in the Dark World
	{ "Defeat Bowser 1", "", "", "", "", "", "" },
	// Bowser in the Fire Sea
	{ "Defeat Bowser 2", "", "", "", "", "", "" },
	// Bowser in the Sky
	{ "Defeat Bowser 3", "", "", "", "", "", "" },
	// Princess's Secret Slide
	{ "Complete the Secret Slide", "Complete the Secret Slide in under 21s", "", "", "", "", "" },
	// Cavern of the Metal Cap
	{ "Get the 8 red coins", "", "", "", "", "", "" },
	// Tower of the Wing Cap
	{ "Get the 8 red coins", "", "", "", "", "", "" },
	// Vanish Cap Under the Moat
	{ "Get the 8 red coins", "", "", "", "", "", "" },
	// Winged Mario over the Rainbow
	{ "Get the 8 red coins", "", "", "", "", "", "" },
	// Secret Aquarium
	{ "Get the 8 red coins", "", "", "", "", "", "" }
};

const bool courseHasCannon[]
{
	true, true, true, true, false, false, false, true, false, true, true, true, true, false, true, // Main courses
	false, false, false, false, false, false, false, true, false                                   // Extra courses
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

	inline uint8_t GetTotalStars()
	{
		uint8_t count = 0;

		for (int c = 0; c < COURSE_COUNT; c++)
		{
			for (int st = 0; st < courseStarCount[c]; st++)
			{
				if ((CourseData[c] & (1 << st)) != 0) count++;
			}
		}

		if ((Flags & SAVE_FLAG_COLLECTED_TOAD_STAR_1) != 0) count++;
		if ((Flags & SAVE_FLAG_COLLECTED_TOAD_STAR_2) != 0) count++;
		if ((Flags & SAVE_FLAG_COLLECTED_TOAD_STAR_3) != 0) count++;
		if ((Flags & SAVE_FLAG_COLLECTED_MIPS_STAR_1) != 0) count++;
		if ((Flags & SAVE_FLAG_COLLECTED_MIPS_STAR_2) != 0) count++;

		return count;
	}
};

class SaveData
{
public:
	SaveSlot saveSlots[NUM_SAVE_SLOTS][NUM_COPIES] = {};
	SettingsData settings[NUM_COPIES] = {};

	enum class Types { NotValid, PC, Nintendo64 };

	SaveData();

	static SaveData* Load(const std::string filePath);
	static void Save(const std::string filePath, const SaveData* saveData);

	Types GetType() const;
};
