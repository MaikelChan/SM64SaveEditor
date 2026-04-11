#pragma once

#include <cstdint>
#include <string>

struct Utils
{
	inline static uint16_t Swap16(const uint16_t value)
	{
		return (value << 8) | (value >> 8);
	}

	inline static uint32_t Swap32(const uint32_t value)
	{
		return (value << 24) | ((value & 0xff00) << 8) | ((value & 0xff0000) >> 8) | (value >> 24);
	}

	inline static uint64_t Swap64(const uint64_t value)
	{
		return (value << 56) | ((value & 0xff00) << 40) | ((value & 0xff0000) << 24) | ((value & 0xff000000) << 8) | ((value & 0xff00000000) >> 8) | ((value & 0xff0000000000) >> 24) | ((value & 0xff000000000000) >> 40) | (value >> 56);
	}

	inline static std::string GetTimeString(const uint32_t value)
	{
		uint32_t h = value / 3600;
		uint32_t m = (value % 3600) / 60;
		uint32_t s = value % 60;

		return std::to_string(h) + "h " + std::to_string(m) + "m " + std::to_string(s) + "s";
	}
};