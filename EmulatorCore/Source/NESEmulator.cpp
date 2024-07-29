#include "NESEmulator.h"

NesEmulator::NesEmulator() 
	: cpu(&bus), ppu(&bus), apu(&bus), io(&bus), mmc(new NMAP(bus)),
	bus(*this, cpu, ppu, io, apu, nullptr) {
	bus.mmc = mmc;
	Reset();
}

NesEmulator::~NesEmulator() {
	delete mmc;
}

void NesEmulator::Reset() {
	bus.Reset();
}

void NesEmulator::Run(int32_t cycles) {
	if (!m_romLoaded) return;
	m_cycleAligner += cycles;
	while (m_cycleAligner > 0) {
		uint32_t cpuClocks = cpu.Step();
		ppu.Step(cpuClocks);
		apu.Step(cpuClocks);
		m_cycleAligner -= cpuClocks;
	}
}

bool NesEmulator::LoadROM(const uint8_t* data, const uint32_t size) {
	bus.romInfo = ROMHeader(size, data);
	bus.romInfo.Print();

	delete mmc;
	mmc = CreateMapper(data + 0x10);
	bus.mmc = mmc;

	Reset();

	m_romLoaded = true;
	return m_romLoaded;
}

MMC* NesEmulator::CreateMapper(const uint8_t* data) {
	switch (bus.romInfo.mapper) {
	case 1:
		return new MMC1(bus, bus.romInfo, data);
	case 0:
		return new NMAP(bus, bus.romInfo, data);
	default:
		return new NMAP(bus);
	}
}