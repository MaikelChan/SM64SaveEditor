#pragma once

#include <cstdint>

static uint16_t Swap16(const uint16_t value)
{
	return (value << 8) | (value >> 8);
}

static uint32_t Swap32(const uint32_t value)
{
	return (value << 24) | ((value & 0xff00) << 8) | ((value & 0xff0000) >> 8) | (value >> 24);
}