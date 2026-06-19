#pragma once
#include "Globals.h"

struct OAMEntry {
	uint8_t y;
	union {
		uint8_t tile;
		struct {
			uint8_t bank : 1;
			uint8_t bigIndex : 7;
		};
	};
	union {
		uint8_t params;
		struct {
			uint8_t palette : 2;
			uint8_t unused : 3;
			uint8_t priority : 1;
			uint8_t flipX : 1;
			uint8_t flipY : 1;
		};
	};
	uint8_t x;
};

