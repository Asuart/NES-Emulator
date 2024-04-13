#include "NESEmulator.h"

NesEmulator::NesEmulator() {
	bus = new Bus(this);
	mmc = new MMC(bus);
	bus->mmc = mmc;
	io = new IO(bus);
	bus->io = io;
	ppu = new PPU(bus);
	bus->ppu = ppu;
	cpu = new CPU(bus);
	bus->cpu = cpu;
}

NesEmulator::~NesEmulator() {
	delete bus;
	delete cpu;
	delete mmc;
	delete ppu;
	delete io;
}

void NesEmulator::Reset() {
	bus->Reset();
}

void NesEmulator::Run(int32_t instructionsCount) {
	if (!romLoaded) return;
	for (int32_t i = 0; i < instructionsCount; i++) {
		uint64_t startClock = cpu->clock;
		cpu->Step();
		ppuClockAligner +=  (cpu->clock - startClock) * ppu->cpuClockRatio;
		for (; ppuClockAligner >= 1.0; ppuClockAligner--) {
			ppu->Step();
		}
	}
}

bool NesEmulator::LoadROM(const uint8_t* data, const uint32_t size) {
	romLoaded = bus->LoadROM(data, size);
	return romLoaded;
}