#include "APU.h"

APU::APU(Bus* bus)
	: bus(bus) {
	Reset();
}

void APU::Reset() {
	clock = 0;
	clockAccumulator = 0;
	clockAligner = 0;
	frameInterrupt = false;
	dmcInterrupt = false;
	dmcActive = false;
}

void APU::Step(uint32_t cpuCycles) {
	clockAligner += cpuCycles * cpuClockRatio;
	while (clockAligner >= 1.0) {
		clockAligner -= 1.0;
		clock++;
		clockAccumulator++;
		if (!mode) {
			switch (clockAccumulator) {
			case 3728:
				ClockQuarterFrame();
				break;
			case 7456:
				ClockQuarterFrame();
				ClockHalfFrame();
				break;
			case 11185:
				ClockQuarterFrame();
				break;
			case 14914:
				ClockQuarterFrame();
				ClockHalfFrame();
				if (!irqInhibit) RequestIRQ();
				break;
			case 14915:
				clockAccumulator = 0;
				if (!irqInhibit) RequestIRQ();
				break;
			}
		}
		else {
			switch (clockAccumulator) {
			case 3728:
				ClockQuarterFrame();
				break;
			case 7456:
				ClockQuarterFrame();
				ClockHalfFrame();
				break;
			case 11185:
				ClockQuarterFrame();
				break;
			case 18640:
				ClockQuarterFrame();
				ClockHalfFrame();
				break;
			case 18641:
				clockAccumulator = 0;
				break;
			}
		}
	}
}

uint8_t APU::Read(uint16_t address, bool fetch) {
	uint8_t result = 0;
	switch (address) {
	case 0x4000:
		return pulse1.byte0;
	case 0x4001:
		return pulse1.sweep;
	case 0x4002:
		return pulse1.timerLow;
	case 0x4003:
		return pulse1.byte3;
	case 0x4004:
		return pulse2.byte0;
	case 0x4005:
		return pulse2.sweep;
	case 0x4006:
		return pulse2.timerLow;
	case 0x4007:
		return pulse2.byte3;
	case 0x4008:
		return triangle.byte0;
	case 0x4009:
		return triangle.unused;
	case 0x400a:
		return triangle.timerLow;
	case 0x400b:
		return triangle.byte3;
	case 0x400c:
		return noise.byte0;
	case 0x400d:
		return noise.unused;
	case 0x400e:
		return noise.byte2;
	case 0x400f:
		return noise.byte3;
	case 0x4010:
		return dmc.byte0;
	case 0x4011:
		return dmc.byte1;
	case 0x4012:
		return dmc.sampleAddress;
	case 0x4013:
		return dmc.sampleLength;
	case 0x4015:
		result = (dmcInterrupt << 7) | (frameInterrupt << 6) |
			((noise.lengthCounterLoad > 0) << 3) | ((triangle.lengthCounterLoad > 0) << 2) |
			((pulse2.lengthCounterLoad > 0) << 1) | (pulse1.lengthCounterLoad > 0);
		if (!fetch) frameInterrupt = false;
		return result;
	case 0x4017:
		return frameCounter;
	}
	return 0;
}

void APU::Write(uint16_t address, uint8_t value) {

}

void APU::ClockQuarterFrame() {

}

void APU::ClockHalfFrame() {
	if (pulse1.lengthCounterLoad && !pulse1.envelopeLoop) pulse1.lengthCounterLoad--;
	if (pulse2.lengthCounterLoad && !pulse2.envelopeLoop) pulse2.lengthCounterLoad--;
	if (triangle.lengthCounterLoad && !triangle.linearCounterControl) triangle.lengthCounterLoad--;
	if (noise.lengthCounterLoad && !noise.envelopeLoop) noise.lengthCounterLoad--;
	if (dmc.loadCounter) dmc.loadCounter--;
}

void APU::RequestIRQ() {
	frameInterrupt = true;
}