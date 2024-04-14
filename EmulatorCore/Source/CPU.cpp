#include "CPU.h"

CPU::Opcode::Opcode(CPU::Opcode::AddressingMode _addressing, void(CPU::* f)(), std::string _name, uint8_t _cycles, bool _write, bool _crossPageCycle)
	: addressing(_addressing), func(f), name(_name), cycles(_cycles), write(_write), crossPageCycle(_crossPageCycle) {}

CPU::CPU(Bus* interconnect)
	: bus(interconnect) {
	Reset();
}

void CPU::Reset() {
	f = 0x24;
	ac = x = y = 0;
	sp = 0xfd;
	pc = disasmPC = GetResetAddress();
	clock = 0;
}

uint16_t CPU::GetResetAddress() {
	return (uint16_t)bus->Read(0xfffc, true) | ((uint16_t)bus->Read(0xfffd, true) << 8);
}

uint16_t CPU::GetNMIAddress() {
	return (uint16_t)bus->Read(0xfffa, true) | ((uint16_t)bus->Read(0xfffb, true) << 8);
}

uint16_t CPU::GetBreakAddress() {
	return (uint16_t)bus->Read(0xfffe, true) | ((uint16_t)bus->Read(0xffff, true) << 8);
}

void CPU::Step() {
	disasmPC = pc;

	opcode = bus->Read(pc++, true);
	clock += opcodeTable[opcode].cycles;
	Decode();

	if (disasm) {
		DisasmCurrent();
	}

	Execute();
	if (opcodeTable[opcode].write) {
		WriteBack();
	}
}

void CPU::Execute() {
	(this->*opcodeTable[opcode].func)();
}

void CPU::WriteBack() {
	if (opcodeTable[opcode].addressing == Opcode::AddressingMode::ACC) {
		ac = value;
		return;
	}
	bus->Write(address, value);
}

void CPU::Push8(uint8_t val) {
	bus->Write(0x100 + (uint16_t)sp, val);
	sp--;
}

void CPU::Push16(uint16_t val) {
	bus->Write(0x100 + (uint16_t)sp, (val >> 8) & 0xff);
	sp--;
	bus->Write(0x100 + (uint16_t)sp, val & 0xff);
	sp--;
}

uint8_t CPU::Pull8() {
	sp++;
	return bus->Read(0x100 + (uint16_t)sp);
}

uint16_t CPU::Pull16() {
	sp++;
	uint16_t lsb = bus->Read(0x100 + (uint16_t)sp);
	sp++;
	uint16_t msb = (uint16_t)bus->Read(0x100 + (uint16_t)sp);
	return (msb << 8) | lsb;
}

uint16_t PageWrapAdd(uint16_t address) {
	return ((address & 0xff00) | ((address + 1) & 0xff));
}

void CPU::Decode() {
	uint16_t baseAddress;
	switch (opcodeTable[opcode].addressing) {
	case Opcode::AddressingMode::IMPL:
		break;
	case Opcode::AddressingMode::REL:
		value = bus->Read(pc, true);
		pc++;
		address = pc + (int8_t)value;
		value = bus->Read(address, opcodeTable[opcode].write);
		break;
	case Opcode::AddressingMode::IMM:
		value = bus->Read(pc, true);
		pc++;
		break;
	case Opcode::AddressingMode::ZP:
		address = bus->Read(pc, true);
		pc++;
		value = bus->Read(address & 0xff, opcodeTable[opcode].write);
		break;
	case Opcode::AddressingMode::ZPX:
		address = (bus->Read(pc, true) + x) & 0xff;
		pc++;
		value = bus->Read(address & 0xff, opcodeTable[opcode].write);
		break;
	case Opcode::AddressingMode::ZPY:
		address = (bus->Read(pc, true) + y) & 0xff;
		pc++;
		value = bus->Read(address & 0xff, opcodeTable[opcode].write);
		break;
	case Opcode::AddressingMode::ABS:
		address = (uint16_t)bus->Read(pc, true) | ((uint16_t)bus->Read(pc + 1, true) << 8);
		pc += 2;
		value = bus->Read(address, opcodeTable[opcode].write);
		break;
	case Opcode::AddressingMode::ABSX:
		baseAddress = ((uint16_t)bus->Read(pc, true) | ((uint16_t)bus->Read(pc + 1, true) << 8));
		address = baseAddress + x;
		if (opcodeTable[opcode].crossPageCycle && ((address & 0xff00) != (baseAddress & 0xff00))) {
			clock++;
		}
		pc += 2;
		value = bus->Read(address, opcodeTable[opcode].write);
		break;
	case Opcode::AddressingMode::ABSY:
		baseAddress = ((uint16_t)bus->Read(pc, true) | ((uint16_t)bus->Read(pc + 1, true) << 8));
		address = baseAddress + y;
		if (opcodeTable[opcode].crossPageCycle && ((address & 0xff00) != (baseAddress & 0xff00))) {
			clock++;
		}
		pc += 2;
		value = bus->Read(address, opcodeTable[opcode].write);
		break;
	case Opcode::AddressingMode::ACC:
		value = ac;
		break;
	case Opcode::AddressingMode::IND:
		address = bus->Read(pc, true) | (bus->Read(PageWrapAdd(pc), true) << 8);
		address = (uint16_t)bus->Read(address, true) | ((uint16_t)bus->Read(PageWrapAdd(address), true) << 8);
		pc += 2;
		value = bus->Read(address, opcodeTable[opcode].write);
		break;
	case Opcode::AddressingMode::INDX:
		value = (bus->Read(pc, true) + x);
		pc++;
		address = (uint16_t)bus->Read(value, true) | ((uint16_t)bus->Read((value + 1) & 0xff, true) << 8);
		value = bus->Read(address, opcodeTable[opcode].write);
		break;
	case Opcode::AddressingMode::INDY:
		value = (bus->Read(pc, true));
		pc++;
		baseAddress = ((uint16_t)bus->Read(value, true) | ((uint16_t)bus->Read((value + 1) & 0xff, true) << 8));
		address = baseAddress + y;
		if (opcodeTable[opcode].crossPageCycle && ((address & 0xff00) != (baseAddress & 0xff00))) {
			clock++;
		}
		value = bus->Read(address, opcodeTable[opcode].write);
		break;
	default:
		std::cout << "Unsupported addressing mode: "<<(uint8_t)opcodeTable[opcode].addressing << std::endl;
	}
}

void CPU::NMIException() {
	Step();
	Push16(pc);
	Push8(f);
	pc = GetNMIAddress();
}

void CPU::ADC() {
	uint16_t temp = value + ac + f_carry;
	f_overflow = !((ac ^ value) & 0x80) && ((ac ^ temp) & 0x80);
	ac = temp & 0xff;
	f_carry = temp > 0xff;
	f_sign = ac >> 7;
	f_zero = ac == 0;
}

void CPU::AND() {
	ac &= value;
	f_sign = ac >> 7;
	f_zero = ac == 0;
}

void CPU::ASL() {
	f_carry = value >> 7;
	value = value << 1;
	f_sign = value >> 7;
	f_zero = value == 0;
}

void CPU::BCC() {
	if (!f_carry) {
		pc = address;
		clock++;
	}
}

void CPU::BCS() {
	if (f_carry) {
		pc = address;
		clock++;
	}
}

void CPU::BEQ() {
	if (f_zero) {
		pc = address;
		clock++;
	}
}

void CPU::BIT() {
	f_sign = (value >> 7) & 1;
	f_overflow = (value >> 6) & 1;
	f_zero = ((ac & value) == 0);
}

void CPU::BMI() {
	if (f_sign) {
		pc = address;
		clock++;
	}
}

void CPU::BNE() {
	if (!f_zero) {
		pc = address;
		clock++;
	}
}

void CPU::BPL() {
	if (!f_sign) {
		pc = address;
		clock++;
	}
}

void CPU::BRK() {
	Push16(pc);
	f_break = 1;
	f_1 = 1;
	Push8(f);
	f_int = 1;
	pc = GetBreakAddress();
}

void CPU::BVC() {
	if (!f_overflow) {
		pc = address;
		clock++;
	}
}

void CPU::BVS() {
	if (f_overflow) {
		pc = address;
		clock++;
	}
}

void CPU::CLC() {
	f_carry = 0;
}

void CPU::CLD() {
	f_decimal = 0;
}

void CPU::CLI() {
	f_int = 0;
}

void CPU::CLV() {
	f_overflow = 0;
}

void CPU::CMP() {
	uint16_t temp = ac - value;
	f_carry = temp < 0x100;
	f_sign = (temp & 0xff) >> 7;
	f_zero = (temp & 0xff) == 0;
}

void CPU::CPX() {
	uint16_t temp = (uint16_t)x - value;
	f_carry = temp < 0x100;
	f_sign = (temp & 0xff) >> 7;
	f_zero = (temp & 0xff) == 0;
}

void CPU::CPY() {
	uint16_t temp = (uint16_t)y - value;
	f_carry = temp < 0x100;
	f_sign = (temp & 0xff) >> 7;
	f_zero = (temp & 0xff) == 0;
}

void CPU::DEC() {
	value--;
	f_sign = value >> 7;
	f_zero = value == 0;
}

void CPU::DEX() {
	x--;
	f_sign = x >> 7;
	f_zero = x == 0;
}

void CPU::DEY() {
	y--;
	f_sign = y >> 7;
	f_zero = y == 0;
}

void CPU::EOR() {
	ac = ac ^ value;
	f_sign = ac >> 7;
	f_zero = ac == 0;
}

void CPU::INC() {
	value++;
	f_sign = value >> 7;
	f_zero = value == 0;
}

void CPU::INX() {
	x++;
	f_sign = x >> 7;
	f_zero = x == 0;
}

void CPU::INY() {
	y++;
	f_sign = y >> 7;
	f_zero = y == 0;
}

void CPU::JMP() {
	pc = address;
}
void CPU::JSR() {
	Push16(pc - 1);
	pc = address;
}

void CPU::LDA() {
	ac = value;
	f_sign = ac >> 7;
	f_zero = ac == 0;
}

void CPU::LDX() {
	x = value;
	f_sign = x >> 7;
	f_zero = x == 0;
}

void CPU::LDY() {
	y = value;
	f_sign = y >> 7;
	f_zero = y == 0;
}

void CPU::LSR() {
	f_carry = value & 1;
	value = value >> 1;
	f_sign = 0;
	f_zero = value == 0;
}

void CPU::NOP() {}

void CPU::ORA() {
	ac = ac | value;
	f_sign = ac >> 7;
	f_zero = ac == 0;
}

void CPU::PHA() {
	Push8(ac);
}

void CPU::PHP() {
	f_break = 1;
	f_1 = 1;
	Push8(f);
	f_break = 0;
}

void CPU::PLA() {
	ac = Pull8();
	f_sign = ac >> 7;
	f_zero = ac == 0;
}

void CPU::PLP() {
	uint8_t curBreak = f_break;
	uint8_t cur1 = f_1;
	f = Pull8();
	f_break = curBreak;
	f_1 = cur1;
}

void CPU::ROL() {
	bool carry = value >> 7;
	value <<= 1;
	if (f_carry) value |= 0b1;
	else value &= ~0b1;
	f_carry = carry;
	f_sign = value >> 7;
	f_zero = value == 0;
}

void CPU::ROR() {
	bool carry = value & 1;
	value >>= 1;
	if (f_carry) value |= 0b10000000;
	else value &= ~0b10000000;
	f_carry = carry;
	f_sign = value >> 7;
	f_zero = value == 0;
}

void CPU::RTI() {
	uint8_t curBreak = f_break;
	uint8_t cur1 = f_1;
	f = Pull8();
	f_break = curBreak;
	f_1 = cur1;
	pc = Pull16();
}

void CPU::RTS() {
	pc = Pull16() + 1;
}

void CPU::SBC() {
	uint16_t temp = ac - value - !f_carry;
	f_overflow = ((ac ^ temp) & 0x80) && ((ac ^ value) & 0x80);
	f_carry = temp < 0x100;
	ac = (temp & 0xff);
	f_sign = ac >> 7;
	f_zero = ac == 0;
}

void CPU::SEC() {
	f_carry = 1;
}

void CPU::SED() {
	f_decimal = 1;
}

void CPU::SEI() {
	f_int = 1;
}

void CPU::STA() {
	value = ac;
}

void CPU::STX() {
	value = x;
}

void CPU::STY() {
	value = y;
}

void CPU::TAX() {
	x = ac;
	f_sign = x >> 7;
	f_zero = x == 0;
}

void CPU::TAY() {
	y = ac;
	f_sign = y >> 7;
	f_zero = y == 0;
}

void CPU::TSX() {
	x = sp;
	f_sign = x >> 7;
	f_zero = x == 0;
}

void CPU::TXA() {
	ac = x;
	f_sign = ac >> 7;
	f_zero = ac == 0;
}

void CPU::TXS() {
	sp = x;
}

void CPU::TYA() {
	ac = y;
	f_sign = ac >> 7;
	f_zero = ac == 0;
}

// unofficial
void CPU::DOP() {}

void CPU::ANC() {
	ac = ac & value;
	f_sign = ac >> 7;
	f_zero = ac == 0;
	f_carry = ac >> 7;
}

void CPU::SAX() {
	value = ac & x;
}

void CPU::ARR() {
	ac = ac & value;
	bool carry = ac & 1;
	ac = ac >> 1;
	if (f_carry) ac |= 0b10000000;
	else ac = ac & ~0b10000000;
	switch ((ac & 0b1100000) >> 5) {
	case 0:
		f_carry = 0;
		f_overflow = 0;
		break;
	case 1:
		f_overflow = 1;
		f_carry = 0;
		break;
	case 2:
		f_carry = 1;
		f_overflow = 1;
		break;
	case 3:
		f_carry = 1;
		f_overflow = 0;
		break;
	}
	f_sign = ac >> 7;
	f_zero = ac == 0;
}

void CPU::ALR() {
	ac = ac & value;
	f_carry = ac & 1;
	ac = ac >> 1;
	f_sign = ac >> 7;
	f_zero = ac == 0;
}

void CPU::ATX() {
	ORA();
	AND();
	TAX();
}

void CPU::AXA() {
	value = ac & x & 7;
}

void CPU::AXS() {
	uint16_t temp = ac & x;
	temp -= value;
	f_carry = temp < 0x100;
	x = temp & 0xff;
	f_sign = x >> 7;
	f_zero = x == 0;
}

void CPU::DCP() {
	value = (value - 1) & 0xff;
	uint16_t temp = (uint16_t)ac - (uint16_t)value;
	f_carry = temp < 0x100;
	f_sign = (temp & 0xff) >> 7;
	f_zero = (temp & 0xff) == 0;
}

void CPU::ISC() {
	value = (value + 1) & 0xff;
	uint16_t temp = (uint16_t)ac - (uint16_t)value - !f_carry;
	f_sign = (temp & 0xff) >> 7;
	f_zero = (temp & 0xff) == 0;
	f_carry = temp < 0x100;
	f_overflow = ((ac ^ temp) & 0x80) && ((ac ^ value) & 0x80);
	ac = (temp & 0xff);
}

void CPU::KIL() {}

void CPU::LAR() {
	ac = value & sp;
	x = ac;
	sp = ac;
	f_sign = ac >> 7;
	f_zero = ac == 0;
}

void CPU::LAX() {
	f_sign = (value & 0xff) >> 7;
	f_zero = (value & 0xff) == 0;
	ac = value;
	x = value;
}

void CPU::RLA() {
	bool carry = value & 0x80;
	value <<= 1;
	value |= f_carry;
	ac = ac & value;
	f_sign = ac >> 7;
	f_zero = ac == 0;
	f_carry = carry;
}

void CPU::RRA() {
	bool carry = value & 0x01;
	value >>= 1;
	value |= (f_carry << 7);
	f_carry = carry;
	uint16_t temp = value + (uint16_t)ac + f_carry;
	f_carry = temp > 0xff;
	f_sign = (temp & 0xff) >> 7;
	f_zero = (temp & 0xff) == 0;
	f_overflow = !((ac ^ value) & 0x80) && ((ac ^ temp) & 0x80);
	ac = temp & 0xff;
}

void CPU::SLO() {
	f_carry = (value & 0xff) >> 7;
	value <<= 1;
	value &= 0xff;
	ac |= value;
	f_sign = ac >> 7;
	f_zero = ac == 0;
}

void CPU::SRE() {
	f_carry = value & 1;
	value = (value >> 1) & 0x7F;
	ac ^= value;
	f_sign = ac >> 7;
	f_zero = ac == 0;
}

void CPU::SXA() {
	uint8_t temp = (x & ((address >> 8) + 1)) & 0xff;
	uint8_t val = (address - y) & 0xff;
	if ((y + val) <= 0xff) {
		value = temp;
	}
}

void CPU::SYA() {
	uint8_t temp = (y & ((address >> 8) + 1)) & 0xff;
	uint8_t val = (address - x) & 0xff;
	if ((x + val) <= 0xff) {
		value = temp;
	}
}

void CPU::TOP() {}

void CPU::XAA() {
	TXA();
	AND();
}

void CPU::XAS() {}

void CPU::DisasmCurrent() {
	std::cout << std::hex << "0x" << disasmPC << ": (0x" <<(int32_t)opcode << ") " << opcodeTable[opcode].name << " 0x" << address;
	std::cout << " A: " << (int32_t)ac << " X: " << (int32_t)x << " Y: " << (int32_t)y;
	std::cout << " P: " << (int32_t)f << " SP: " << (int32_t)sp;
	std::cout << "\n";
}
