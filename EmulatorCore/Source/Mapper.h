#pragma once
#include "Globals.h"
#include "ROMHeader.h"

class Mapper {
protected:
	Mapper() = default;

public:
	virtual void Reset();
	virtual uint8_t Read(uint16_t address, bool fetch = false) const = 0;
	virtual void Write(uint16_t address, uint8_t value) = 0;
};

class NMAP : public Mapper {
	std::array<uint8_t, 0xbfe0> rom = std::array<uint8_t, 0xbfe0>();

public:
	NMAP() = default;
	NMAP(const ROMHeader& header, const uint8_t* data);

	uint8_t Read(uint16_t address, bool fetch = false) const override;
	void Write(uint16_t address, uint8_t value) override;
};