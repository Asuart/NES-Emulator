#pragma once
#include "Globals.h"
#include "Bus.h"

class APU {
public:
	struct {
		union {
			struct {
				uint8_t volume : 4;
				uint8_t constantVolume : 1;
				uint8_t envelopeLoop : 1;
				uint8_t duty : 2;
			};
			uint8_t byte0;
		};
		union {
			struct {
				uint8_t shift : 3;
				uint8_t negate : 1;
				uint8_t period : 3;
				uint8_t enabled : 1;
			};
			uint8_t sweep;
		};
		uint8_t timerLow;
		union {
			struct {
				uint8_t timerHigh : 3;
				uint8_t lengthCounterLoad : 5;
			};
			uint8_t byte3;
		};
	} pulse1, pulse2;

	struct {
		union {
			struct {
				uint8_t linearCounterLoad : 7;
				uint8_t linearCounterControl : 1;
			};
			uint8_t byte0;
		};
		uint8_t unused;
		uint8_t timerLow;
		union {
			struct {
				uint8_t timerHigh : 3;
				uint8_t lengthCounterLoad : 5;
			};
			uint8_t byte3;
		};
	} triangle;

	struct {
		union {
			struct {
				uint8_t volume : 4;
				uint8_t constantVolume : 1;
				uint8_t envelopeLoop : 1;
				uint8_t unused0 : 2;
			};
			uint8_t byte0;
		};
		uint8_t unused;
		union {
			struct {
				uint8_t period : 4;
				uint8_t unused1 : 3;
				uint8_t loopNoise : 1;
			};
			uint8_t byte2;
		};
		union {
			struct {
				uint8_t unused2 : 3;
				uint8_t lengthCounterLoad : 5;
			};
			uint8_t byte3;
		};
	} noise;

	struct {
		union {
			struct {
				uint8_t frequency : 4;
				uint8_t unused0 : 2;
				uint8_t loop : 1;
				uint8_t irqEnable : 1;
			};
			uint8_t byte0;
		};
		union {
			struct {
				uint8_t loadCounter : 7;
				uint8_t unused1 : 1;
			};
			uint8_t byte1;
		};
		uint8_t sampleAddress;
		uint8_t sampleLength;
	} dmc;

	union {
		struct {
			uint8_t frameCounterUnused : 6;
			uint8_t irqInhibit : 1;
			uint8_t mode : 1;
		};
		uint8_t frameCounter;
	};

	APU(Bus& bus);

	void Reset();
	void Step(uint32_t cpuCycles);

	uint8_t Read(uint16_t address, bool fetch);
	void Write(uint16_t address, uint8_t value);

private:
	const double cpuClockRatio = 0.5;

	Bus& bus;
	uint64_t clock;
	double clockAligner;
	uint32_t clockAccumulator;

	bool frameInterrupt;
	bool dmcInterrupt;
	bool dmcActive;

	void ClockQuarterFrame();
	void ClockHalfFrame();
	void RequestIRQ();
};