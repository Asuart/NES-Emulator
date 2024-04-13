#include "CPU.h"

CPU::Opcode::Opcode(CPU::Opcode::AddressingMode _addressing, void(CPU::* f)(), std::string _name, uint8_t _cycles, bool _write, bool _crossPageCycle)
	: addressing(_addressing), func(f), name(_name), cycles(_cycles), write(_write), crossPageCycle(_crossPageCycle) {}

CPU::CPU(Bus* interconnect)
	: bus(interconnect) {
	InitOpcodeTable();
	Reset();
}

void CPU::Reset() {
	f = 0x24;
	ac = x = y = 0;
	sp = 0xfd;
	pc = disasmPC = GetResetAddress();
	clock = 0;
}

void CPU::InitOpcodeTable() {
	for (int i = 0; i < opcodeTable.size(); i++) {
		opcodeTable[i] = Opcode(Opcode::AddressingMode::IMPL, &CPU::UNHANDLED, "UNHANDLED", 1);
	}

	opcodeTable[0x69] = Opcode(Opcode::AddressingMode::IMM,  &CPU::ADC, "ADC", 2);
	opcodeTable[0x65] = Opcode(Opcode::AddressingMode::ZP, &CPU::ADC, "ADC", 3);
	opcodeTable[0x75] = Opcode(Opcode::AddressingMode::ZPX, &CPU::ADC, "ADC", 4);
	opcodeTable[0x6d] = Opcode(Opcode::AddressingMode::ABS, &CPU::ADC, "ADC", 4);
	opcodeTable[0x7d] = Opcode(Opcode::AddressingMode::ABSX, &CPU::ADC, "ADC", 4, false, true);
	opcodeTable[0x79] = Opcode(Opcode::AddressingMode::ABSY, &CPU::ADC, "ADC", 4, false, true);
	opcodeTable[0x61] = Opcode(Opcode::AddressingMode::INDX, &CPU::ADC, "ADC", 6);
	opcodeTable[0x71] = Opcode(Opcode::AddressingMode::INDY, &CPU::ADC, "ADC", 5, false, true);
	opcodeTable[0x29] = Opcode(Opcode::AddressingMode::IMM, &CPU::AND, "AND", 2);
	opcodeTable[0x25] = Opcode(Opcode::AddressingMode::ZP, &CPU::AND, "AND", 3);
	opcodeTable[0x35] = Opcode(Opcode::AddressingMode::ZPX, &CPU::AND, "AND", 4);
	opcodeTable[0x2d] = Opcode(Opcode::AddressingMode::ABS, &CPU::AND, "AND", 4);
	opcodeTable[0x3d] = Opcode(Opcode::AddressingMode::ABSX, &CPU::AND, "AND", 4, false, true);
	opcodeTable[0x39] = Opcode(Opcode::AddressingMode::ABSY, &CPU::AND, "AND", 4, false, true);
	opcodeTable[0x21] = Opcode(Opcode::AddressingMode::INDX, &CPU::AND, "AND", 6);
	opcodeTable[0x31] = Opcode(Opcode::AddressingMode::INDY, &CPU::AND, "AND", 5, false, true);
	opcodeTable[0x0a] = Opcode(Opcode::AddressingMode::ACC, &CPU::ASL, "ASL", 2, true);
	opcodeTable[0x06] = Opcode(Opcode::AddressingMode::ZP, &CPU::ASL, "ASL", 5, true);
	opcodeTable[0x16] = Opcode(Opcode::AddressingMode::ZPX, &CPU::ASL, "ASL", 6, true);
	opcodeTable[0x0e] = Opcode(Opcode::AddressingMode::ABS, &CPU::ASL, "ASL", 6, true);
	opcodeTable[0x1e] = Opcode(Opcode::AddressingMode::ABSX, &CPU::ASL, "ASL", 7, true);
	opcodeTable[0x90] = Opcode(Opcode::AddressingMode::REL, &CPU::BCC, "BCC", 2, false, true);
	opcodeTable[0xb0] = Opcode(Opcode::AddressingMode::REL, &CPU::BCS, "BCS", 2, false, true);
	opcodeTable[0xf0] = Opcode(Opcode::AddressingMode::REL, &CPU::BEQ, "BEQ", 2, false, true);
	opcodeTable[0x24] = Opcode(Opcode::AddressingMode::ZP, &CPU::BIT, "BIT", 3);
	opcodeTable[0x2c] = Opcode(Opcode::AddressingMode::ABS, &CPU::BIT, "BIT", 4);
	opcodeTable[0x30] = Opcode(Opcode::AddressingMode::REL, &CPU::BMI, "BMI", 2, false, true);
	opcodeTable[0xd0] = Opcode(Opcode::AddressingMode::REL, &CPU::BNE, "BNE", 2, false, true);
	opcodeTable[0x10] = Opcode(Opcode::AddressingMode::REL, &CPU::BPL, "BPL", 2, false, true);
	opcodeTable[0x00] = Opcode(Opcode::AddressingMode::IMM, &CPU::BRK, "BRK", 7);
	opcodeTable[0x50] = Opcode(Opcode::AddressingMode::REL, &CPU::BVC, "BVC", 2, false, true);
	opcodeTable[0x70] = Opcode(Opcode::AddressingMode::REL, &CPU::BVS, "BVS", 2, false, true);
	opcodeTable[0x18] = Opcode(Opcode::AddressingMode::IMPL, &CPU::CLC, "CLC", 2);
	opcodeTable[0xd8] = Opcode(Opcode::AddressingMode::IMPL, &CPU::CLD, "CLD", 2);
	opcodeTable[0x58] = Opcode(Opcode::AddressingMode::IMPL, &CPU::CLI, "CLI", 2);
	opcodeTable[0xb8] = Opcode(Opcode::AddressingMode::IMPL, &CPU::CLV, "CLV", 2);
	opcodeTable[0xc9] = Opcode(Opcode::AddressingMode::IMM, &CPU::CMP, "CMP", 2);
	opcodeTable[0xc5] = Opcode(Opcode::AddressingMode::ZP, &CPU::CMP, "CMP", 3);
	opcodeTable[0xd5] = Opcode(Opcode::AddressingMode::ZPX, &CPU::CMP, "CMP", 4);
	opcodeTable[0xcd] = Opcode(Opcode::AddressingMode::ABS, &CPU::CMP, "CMP", 4);
	opcodeTable[0xdd] = Opcode(Opcode::AddressingMode::ABSX, &CPU::CMP, "CMP", 4, false, true);
	opcodeTable[0xd9] = Opcode(Opcode::AddressingMode::ABSY, &CPU::CMP, "CMP", 4, false, true);
	opcodeTable[0xc1] = Opcode(Opcode::AddressingMode::INDX, &CPU::CMP, "CMP", 6);
	opcodeTable[0xd1] = Opcode(Opcode::AddressingMode::INDY, &CPU::CMP, "CMP", 5, false, true);
	opcodeTable[0xe0] = Opcode(Opcode::AddressingMode::IMM, &CPU::CPX, "CPX", 2);
	opcodeTable[0xe4] = Opcode(Opcode::AddressingMode::ZP, &CPU::CPX, "CPX", 3);
	opcodeTable[0xec] = Opcode(Opcode::AddressingMode::ABS, &CPU::CPX, "CPX", 4);
	opcodeTable[0xc0] = Opcode(Opcode::AddressingMode::IMM, &CPU::CPY, "CPY", 2);
	opcodeTable[0xc4] = Opcode(Opcode::AddressingMode::ZP, &CPU::CPY, "CPY", 3);
	opcodeTable[0xcc] = Opcode(Opcode::AddressingMode::ABS, &CPU::CPY, "CPY", 4);
	opcodeTable[0xc6] = Opcode(Opcode::AddressingMode::ZP, &CPU::DEC, "DEC", 5, true);
	opcodeTable[0xd6] = Opcode(Opcode::AddressingMode::ZPX, &CPU::DEC, "DEC", 6, true);
	opcodeTable[0xce] = Opcode(Opcode::AddressingMode::ABS, &CPU::DEC, "DEC", 6, true);
	opcodeTable[0xde] = Opcode(Opcode::AddressingMode::ABSX, &CPU::DEC, "DEC", 7, true);
	opcodeTable[0xca] = Opcode(Opcode::AddressingMode::IMPL, &CPU::DEX, "DEX", 2);
	opcodeTable[0x88] = Opcode(Opcode::AddressingMode::IMPL, &CPU::DEY, "DEY", 2);
	opcodeTable[0x49] = Opcode(Opcode::AddressingMode::IMM, &CPU::EOR, "EOR", 2);
	opcodeTable[0x45] = Opcode(Opcode::AddressingMode::ZP, &CPU::EOR, "EOR", 3);
	opcodeTable[0x55] = Opcode(Opcode::AddressingMode::ZPX, &CPU::EOR, "EOR", 4);
	opcodeTable[0x4d] = Opcode(Opcode::AddressingMode::ABS, &CPU::EOR, "EOR", 4);
	opcodeTable[0x5d] = Opcode(Opcode::AddressingMode::ABSX, &CPU::EOR, "EOR", 4, false, true);
	opcodeTable[0x59] = Opcode(Opcode::AddressingMode::ABSY, &CPU::EOR, "EOR", 4, false, true);
	opcodeTable[0x41] = Opcode(Opcode::AddressingMode::INDX, &CPU::EOR, "EOR", 6);
	opcodeTable[0x51] = Opcode(Opcode::AddressingMode::INDY, &CPU::EOR, "EOR", 5, false, true);
	opcodeTable[0xe6] = Opcode(Opcode::AddressingMode::ZP, &CPU::INC, "INC", 5, true);
	opcodeTable[0xf6] = Opcode(Opcode::AddressingMode::ZPX, &CPU::INC, "INC", 6, true);
	opcodeTable[0xee] = Opcode(Opcode::AddressingMode::ABS, &CPU::INC, "INC", 6, true);
	opcodeTable[0xfe] = Opcode(Opcode::AddressingMode::ABSX, &CPU::INC, "INC", 7, true);
	opcodeTable[0xe8] = Opcode(Opcode::AddressingMode::IMPL, &CPU::INX, "INX", 2);
	opcodeTable[0xc8] = Opcode(Opcode::AddressingMode::IMPL, &CPU::INY, "INY", 2);
	opcodeTable[0x4c] = Opcode(Opcode::AddressingMode::ABS, &CPU::JMP, "JMP", 3);
	opcodeTable[0x6c] = Opcode(Opcode::AddressingMode::IND, &CPU::JMP, "JMP", 5);
	opcodeTable[0x20] = Opcode(Opcode::AddressingMode::ABS, &CPU::JSR, "JSR", 6);
	opcodeTable[0xa9] = Opcode(Opcode::AddressingMode::IMM, &CPU::LDA, "LDA", 2);
	opcodeTable[0xa5] = Opcode(Opcode::AddressingMode::ZP, &CPU::LDA, "LDA", 3);
	opcodeTable[0xb5] = Opcode(Opcode::AddressingMode::ZPX, &CPU::LDA, "LDA", 4);
	opcodeTable[0xad] = Opcode(Opcode::AddressingMode::ABS, &CPU::LDA, "LDA", 4);
	opcodeTable[0xbd] = Opcode(Opcode::AddressingMode::ABSX, &CPU::LDA, "LDA", 4, false, true);
	opcodeTable[0xb9] = Opcode(Opcode::AddressingMode::ABSY, &CPU::LDA, "LDA", 4, false, true);
	opcodeTable[0xa1] = Opcode(Opcode::AddressingMode::INDX, &CPU::LDA, "LDA", 6);
	opcodeTable[0xb1] = Opcode(Opcode::AddressingMode::INDY, &CPU::LDA, "LDA", 5, false, true);
	opcodeTable[0xa2] = Opcode(Opcode::AddressingMode::IMM, &CPU::LDX, "LDX", 2);
	opcodeTable[0xa6] = Opcode(Opcode::AddressingMode::ZP, &CPU::LDX, "LDX", 3);
	opcodeTable[0xb6] = Opcode(Opcode::AddressingMode::ZPY, &CPU::LDX, "LDX", 4);
	opcodeTable[0xae] = Opcode(Opcode::AddressingMode::ABS, &CPU::LDX, "LDX", 4);
	opcodeTable[0xbe] = Opcode(Opcode::AddressingMode::ABSY, &CPU::LDX, "LDX", 4, false, true);
	opcodeTable[0xa0] = Opcode(Opcode::AddressingMode::IMM, &CPU::LDY, "LDY", 2);
	opcodeTable[0xa4] = Opcode(Opcode::AddressingMode::ZP, &CPU::LDY, "LDY", 3);
	opcodeTable[0xb4] = Opcode(Opcode::AddressingMode::ZPX, &CPU::LDY, "LDY", 4);
	opcodeTable[0xac] = Opcode(Opcode::AddressingMode::ABS, &CPU::LDY, "LDY", 4);
	opcodeTable[0xbc] = Opcode(Opcode::AddressingMode::ABSX, &CPU::LDY, "LDY", 4, false, true);
	opcodeTable[0x4a] = Opcode(Opcode::AddressingMode::ACC, &CPU::LSR, "LSR", 2, true);
	opcodeTable[0x46] = Opcode(Opcode::AddressingMode::ZP, &CPU::LSR, "LSR", 5, true);
	opcodeTable[0x56] = Opcode(Opcode::AddressingMode::ZPX, &CPU::LSR, "LSR", 6, true);
	opcodeTable[0x4e] = Opcode(Opcode::AddressingMode::ABS, &CPU::LSR, "LSR", 6, true);
	opcodeTable[0x5e] = Opcode(Opcode::AddressingMode::ABSX, &CPU::LSR, "LSR", 7, true);
	opcodeTable[0xea] = Opcode(Opcode::AddressingMode::IMPL, &CPU::NOP, "NOP", 2);
	opcodeTable[0x09] = Opcode(Opcode::AddressingMode::IMM, &CPU::ORA, "ORA", 2);
	opcodeTable[0x05] = Opcode(Opcode::AddressingMode::ZP, &CPU::ORA, "ORA", 3);
	opcodeTable[0x15] = Opcode(Opcode::AddressingMode::ZPX, &CPU::ORA, "ORA", 4);
	opcodeTable[0x0d] = Opcode(Opcode::AddressingMode::ABS, &CPU::ORA, "ORA", 4);
	opcodeTable[0x1d] = Opcode(Opcode::AddressingMode::ABSX, &CPU::ORA, "ORA", 4, false, true);
	opcodeTable[0x19] = Opcode(Opcode::AddressingMode::ABSY, &CPU::ORA, "ORA", 4, false, true);
	opcodeTable[0x01] = Opcode(Opcode::AddressingMode::INDX, &CPU::ORA, "ORA", 6);
	opcodeTable[0x11] = Opcode(Opcode::AddressingMode::INDY, &CPU::ORA, "ORA", 5, false, true);
	opcodeTable[0x48] = Opcode(Opcode::AddressingMode::IMPL, &CPU::PHA, "PHA", 3);
	opcodeTable[0x08] = Opcode(Opcode::AddressingMode::IMPL, &CPU::PHP, "PHP", 3);
	opcodeTable[0x68] = Opcode(Opcode::AddressingMode::IMPL, &CPU::PLA, "PLA", 4);
	opcodeTable[0x28] = Opcode(Opcode::AddressingMode::IMPL, &CPU::PLP, "PLP", 4);
	opcodeTable[0x2a] = Opcode(Opcode::AddressingMode::ACC, &CPU::ROL, "ROL", 2, true);
	opcodeTable[0x26] = Opcode(Opcode::AddressingMode::ZP, &CPU::ROL, "ROL", 5, true);
	opcodeTable[0x36] = Opcode(Opcode::AddressingMode::ZPX, &CPU::ROL, "ROL", 6, true);
	opcodeTable[0x2e] = Opcode(Opcode::AddressingMode::ABS, &CPU::ROL, "ROL", 6, true);
	opcodeTable[0x3e] = Opcode(Opcode::AddressingMode::ABSX, &CPU::ROL, "ROL", 7, true);
	opcodeTable[0x6a] = Opcode(Opcode::AddressingMode::ACC, &CPU::ROR, "ROR", 2, true);
	opcodeTable[0x66] = Opcode(Opcode::AddressingMode::ZP, &CPU::ROR, "ROR", 5, true);
	opcodeTable[0x76] = Opcode(Opcode::AddressingMode::ZPX, &CPU::ROR, "ROR", 6, true);
	opcodeTable[0x6e] = Opcode(Opcode::AddressingMode::ABS, &CPU::ROR, "ROR", 6, true);
	opcodeTable[0x7e] = Opcode(Opcode::AddressingMode::ABSX, &CPU::ROR, "ROR", 7, true);
	opcodeTable[0x40] = Opcode(Opcode::AddressingMode::IMPL, &CPU::RTI, "RTI", 6);
	opcodeTable[0x60] = Opcode(Opcode::AddressingMode::IMPL, &CPU::RTS, "RTS", 6);
	opcodeTable[0xe9] = Opcode(Opcode::AddressingMode::IMM, &CPU::SBC, "SBC", 2);
	opcodeTable[0xe5] = Opcode(Opcode::AddressingMode::ZP, &CPU::SBC, "SBC", 3);
	opcodeTable[0xf5] = Opcode(Opcode::AddressingMode::ZPX, &CPU::SBC, "SBC", 4);
	opcodeTable[0xed] = Opcode(Opcode::AddressingMode::ABS, &CPU::SBC, "SBC", 4);
	opcodeTable[0xfd] = Opcode(Opcode::AddressingMode::ABSX, &CPU::SBC, "SBC", 4, false, true);
	opcodeTable[0xf9] = Opcode(Opcode::AddressingMode::ABSY, &CPU::SBC, "SBC", 4, false, true);
	opcodeTable[0xe1] = Opcode(Opcode::AddressingMode::INDX, &CPU::SBC, "SBC", 6);
	opcodeTable[0xf1] = Opcode(Opcode::AddressingMode::INDY, &CPU::SBC, "SBC", 5, false, true);
	opcodeTable[0x38] = Opcode(Opcode::AddressingMode::IMPL, &CPU::SEC, "SEC", 2);
	opcodeTable[0xf8] = Opcode(Opcode::AddressingMode::IMPL, &CPU::SED, "SED", 2);
	opcodeTable[0x78] = Opcode(Opcode::AddressingMode::IMPL, &CPU::SEI, "SEI", 2);
	opcodeTable[0x85] = Opcode(Opcode::AddressingMode::ZP, &CPU::STA, "STA", 3, true);
	opcodeTable[0x95] = Opcode(Opcode::AddressingMode::ZPX, &CPU::STA, "STA", 4, true);
	opcodeTable[0x8d] = Opcode(Opcode::AddressingMode::ABS, &CPU::STA, "STA", 4, true);
	opcodeTable[0x9d] = Opcode(Opcode::AddressingMode::ABSX, &CPU::STA, "STA", 5, true);
	opcodeTable[0x99] = Opcode(Opcode::AddressingMode::ABSY, &CPU::STA, "STA", 5, true);
	opcodeTable[0x81] = Opcode(Opcode::AddressingMode::INDX, &CPU::STA, "STA", 6, true);
	opcodeTable[0x91] = Opcode(Opcode::AddressingMode::INDY, &CPU::STA, "STA", 6, true);
	opcodeTable[0x86] = Opcode(Opcode::AddressingMode::ZP, &CPU::STX, "STX", 3, true);
	opcodeTable[0x96] = Opcode(Opcode::AddressingMode::ZPY, &CPU::STX, "STX", 4, true);
	opcodeTable[0x8e] = Opcode(Opcode::AddressingMode::ABS, &CPU::STX, "STX", 4, true);
	opcodeTable[0x84] = Opcode(Opcode::AddressingMode::ZP, &CPU::STY, "STY", 3, true);
	opcodeTable[0x94] = Opcode(Opcode::AddressingMode::ZPX, &CPU::STY, "STY", 4, true);
	opcodeTable[0x8c] = Opcode(Opcode::AddressingMode::ABS, &CPU::STY, "STY", 4, true);
	opcodeTable[0xaa] = Opcode(Opcode::AddressingMode::IMPL, &CPU::TAX, "TAX", 2);
	opcodeTable[0xa8] = Opcode(Opcode::AddressingMode::IMPL, &CPU::TAY, "TAY", 2);
	opcodeTable[0xba] = Opcode(Opcode::AddressingMode::IMPL, &CPU::TSX, "TSX", 2);
	opcodeTable[0x8a] = Opcode(Opcode::AddressingMode::IMPL, &CPU::TXA, "TXA", 2);
	opcodeTable[0x9a] = Opcode(Opcode::AddressingMode::IMPL, &CPU::TXS, "TXS", 2);
	opcodeTable[0x98] = Opcode(Opcode::AddressingMode::IMPL, &CPU::TYA, "TYA", 2);

	// unofficial
	opcodeTable[0x04] = Opcode(Opcode::AddressingMode::ZP,   &CPU::DOP, "DOP", 3);
	opcodeTable[0x14] = Opcode(Opcode::AddressingMode::ZPX, &CPU::DOP, "DOP", 3);
	opcodeTable[0x34] = Opcode(Opcode::AddressingMode::ZPX, &CPU::DOP, "DOP", 3);
	opcodeTable[0x44] = Opcode(Opcode::AddressingMode::ZP, &CPU::DOP, "DOP", 3);
	opcodeTable[0x54] = Opcode(Opcode::AddressingMode::ZPX, &CPU::DOP, "DOP", 3);
	opcodeTable[0x64] = Opcode(Opcode::AddressingMode::ZP, &CPU::DOP, "DOP", 3);
	opcodeTable[0x74] = Opcode(Opcode::AddressingMode::ZPX, &CPU::DOP, "DOP", 3);
	opcodeTable[0x80] = Opcode(Opcode::AddressingMode::IMM, &CPU::DOP, "DOP", 3);
	opcodeTable[0x82] = Opcode(Opcode::AddressingMode::IMM, &CPU::DOP, "DOP", 3);
	opcodeTable[0x89] = Opcode(Opcode::AddressingMode::IMM, &CPU::DOP, "DOP", 3);
	opcodeTable[0xc2] = Opcode(Opcode::AddressingMode::IMM, &CPU::DOP, "DOP", 3);
	opcodeTable[0xd4] = Opcode(Opcode::AddressingMode::ZPX, &CPU::DOP, "DOP", 3);
	opcodeTable[0xe2] = Opcode(Opcode::AddressingMode::IMM, &CPU::DOP, "DOP", 3);
	opcodeTable[0xf4] = Opcode(Opcode::AddressingMode::ZPX, &CPU::DOP, "DOP", 3);
	opcodeTable[0x0b] = Opcode(Opcode::AddressingMode::IMM, &CPU::ANC, "ANC", 3);
	opcodeTable[0x2b] = Opcode(Opcode::AddressingMode::IMM, &CPU::ANC, "ANC", 3);
	opcodeTable[0x87] = Opcode(Opcode::AddressingMode::ZP, &CPU::SAX, "SAX", 3, true);
	opcodeTable[0x97] = Opcode(Opcode::AddressingMode::ZPY, &CPU::SAX, "SAX", 3, true);
	opcodeTable[0x83] = Opcode(Opcode::AddressingMode::INDX, &CPU::SAX, "SAX", 3, true);
	opcodeTable[0x8f] = Opcode(Opcode::AddressingMode::ABS, &CPU::SAX, "SAX", 3, true);
	opcodeTable[0x6b] = Opcode(Opcode::AddressingMode::IMM, &CPU::ARR, "ARR", 3);
	opcodeTable[0x4b] = Opcode(Opcode::AddressingMode::IMM, &CPU::ALR, "ALR", 3);
	opcodeTable[0xab] = Opcode(Opcode::AddressingMode::IMM, &CPU::ATX, "ATX", 3);
	opcodeTable[0x9f] = Opcode(Opcode::AddressingMode::ABSY, &CPU::AXA, "AXA", 3);
	opcodeTable[0x93] = Opcode(Opcode::AddressingMode::INDY, &CPU::AXA, "AXA", 3);
	opcodeTable[0xcb] = Opcode(Opcode::AddressingMode::IMM, &CPU::AXS, "AXS", 3);
	opcodeTable[0xc7] = Opcode(Opcode::AddressingMode::ZP, &CPU::DCP, "DCP", 3, true);
	opcodeTable[0xd7] = Opcode(Opcode::AddressingMode::ZPX, &CPU::DCP, "DCP", 3, true);
	opcodeTable[0xcf] = Opcode(Opcode::AddressingMode::ABS, &CPU::DCP, "DCP", 3, true);
	opcodeTable[0xdf] = Opcode(Opcode::AddressingMode::ABSX, &CPU::DCP, "DCP", 3, true);
	opcodeTable[0xdb] = Opcode(Opcode::AddressingMode::ABSY, &CPU::DCP, "DCP", 3, true);
	opcodeTable[0xc3] = Opcode(Opcode::AddressingMode::INDX, &CPU::DCP, "DCP", 3, true);
	opcodeTable[0xd3] = Opcode(Opcode::AddressingMode::INDY, &CPU::DCP, "DCP", 3, true);
	opcodeTable[0xe7] = Opcode(Opcode::AddressingMode::ZP, &CPU::ISC, "ISC", 3, true);
	opcodeTable[0xf7] = Opcode(Opcode::AddressingMode::ZPX, &CPU::ISC, "ISC", 3, true);
	opcodeTable[0xef] = Opcode(Opcode::AddressingMode::ABS, &CPU::ISC, "ISC", 3, true);
	opcodeTable[0xff] = Opcode(Opcode::AddressingMode::ABSX, &CPU::ISC, "ISC", 3, true);
	opcodeTable[0xfb] = Opcode(Opcode::AddressingMode::ABSY, &CPU::ISC, "ISC", 3, true);
	opcodeTable[0xe3] = Opcode(Opcode::AddressingMode::INDX, &CPU::ISC, "ISC", 3, true);
	opcodeTable[0xf3] = Opcode(Opcode::AddressingMode::INDY, &CPU::ISC, "ISC", 3, true);
	opcodeTable[0x02] = Opcode(Opcode::AddressingMode::IMPL, &CPU::KIL, "KIL", 3);
	opcodeTable[0x12] = Opcode(Opcode::AddressingMode::IMPL, &CPU::KIL, "KIL", 3);
	opcodeTable[0x22] = Opcode(Opcode::AddressingMode::IMPL, &CPU::KIL, "KIL", 3);
	opcodeTable[0x32] = Opcode(Opcode::AddressingMode::IMPL, &CPU::KIL, "KIL", 3);
	opcodeTable[0x42] = Opcode(Opcode::AddressingMode::IMPL, &CPU::KIL, "KIL", 3);
	opcodeTable[0x52] = Opcode(Opcode::AddressingMode::IMPL, &CPU::KIL, "KIL", 3);
	opcodeTable[0x62] = Opcode(Opcode::AddressingMode::IMPL, &CPU::KIL, "KIL", 3);
	opcodeTable[0x72] = Opcode(Opcode::AddressingMode::IMPL, &CPU::KIL, "KIL", 3);
	opcodeTable[0x92] = Opcode(Opcode::AddressingMode::IMPL, &CPU::KIL, "KIL", 3);
	opcodeTable[0xb2] = Opcode(Opcode::AddressingMode::IMPL, &CPU::KIL, "KIL", 3);
	opcodeTable[0xd2] = Opcode(Opcode::AddressingMode::IMPL, &CPU::KIL, "KIL", 3);
	opcodeTable[0xf2] = Opcode(Opcode::AddressingMode::IMPL, &CPU::KIL, "KIL", 3);
	opcodeTable[0xbb] = Opcode(Opcode::AddressingMode::ABSY, &CPU::LAR, "LAR", 3);
	opcodeTable[0xa7] = Opcode(Opcode::AddressingMode::ZP, &CPU::LAX, "LAX", 3);
	opcodeTable[0xb7] = Opcode(Opcode::AddressingMode::ZPY, &CPU::LAX, "LAX", 3);
	opcodeTable[0xaf] = Opcode(Opcode::AddressingMode::ABS, &CPU::LAX, "LAX", 3);
	opcodeTable[0xbf] = Opcode(Opcode::AddressingMode::ABSY, &CPU::LAX, "LAX", 3);
	opcodeTable[0xa3] = Opcode(Opcode::AddressingMode::INDX, &CPU::LAX, "LAX", 3);
	opcodeTable[0xb3] = Opcode(Opcode::AddressingMode::INDY, &CPU::LAX, "LAX", 3);
	opcodeTable[0x27] = Opcode(Opcode::AddressingMode::ZP, &CPU::RLA, "RLA", 3, true);
	opcodeTable[0x37] = Opcode(Opcode::AddressingMode::ZPX, &CPU::RLA, "RLA", 3, true);
	opcodeTable[0x2f] = Opcode(Opcode::AddressingMode::ABS, &CPU::RLA, "RLA", 3, true);
	opcodeTable[0x3f] = Opcode(Opcode::AddressingMode::ABSX, &CPU::RLA, "RLA", 3, true);
	opcodeTable[0x3b] = Opcode(Opcode::AddressingMode::ABSY, &CPU::RLA, "RLA", 3, true);
	opcodeTable[0x23] = Opcode(Opcode::AddressingMode::INDX, &CPU::RLA, "RLA", 3, true);
	opcodeTable[0x33] = Opcode(Opcode::AddressingMode::INDY, &CPU::RLA, "RLA", 3, true);
	opcodeTable[0x67] = Opcode(Opcode::AddressingMode::ZP, &CPU::RRA, "RRA", 3, true);
	opcodeTable[0x77] = Opcode(Opcode::AddressingMode::ZPX, &CPU::RRA, "RRA", 3, true);
	opcodeTable[0x6f] = Opcode(Opcode::AddressingMode::ABS, &CPU::RRA, "RRA", 3, true);
	opcodeTable[0x7f] = Opcode(Opcode::AddressingMode::ABSX, &CPU::RRA, "RRA", 3, true);
	opcodeTable[0x7b] = Opcode(Opcode::AddressingMode::ABSY, &CPU::RRA, "RRA", 3, true);
	opcodeTable[0x63] = Opcode(Opcode::AddressingMode::INDX, &CPU::RRA, "RRA", 3, true);
	opcodeTable[0x73] = Opcode(Opcode::AddressingMode::INDY, &CPU::RRA, "RRA", 3, true);
	opcodeTable[0xeb] = Opcode(Opcode::AddressingMode::IMM, &CPU::SBC, "SBC", 3);
	opcodeTable[0x07] = Opcode(Opcode::AddressingMode::ZP, &CPU::SLO, "SLO", 3, true);
	opcodeTable[0x17] = Opcode(Opcode::AddressingMode::ZPX, &CPU::SLO, "SLO", 3, true);
	opcodeTable[0x0f] = Opcode(Opcode::AddressingMode::ABS, &CPU::SLO, "SLO", 3, true);
	opcodeTable[0x1f] = Opcode(Opcode::AddressingMode::ABSX, &CPU::SLO, "SLO", 3, true);
	opcodeTable[0x1b] = Opcode(Opcode::AddressingMode::ABSY, &CPU::SLO, "SLO", 3, true);
	opcodeTable[0x03] = Opcode(Opcode::AddressingMode::INDX, &CPU::SLO, "SLO", 3, true);
	opcodeTable[0x13] = Opcode(Opcode::AddressingMode::INDY, &CPU::SLO, "SLO", 3, true);
	opcodeTable[0x47] = Opcode(Opcode::AddressingMode::ZP, &CPU::SRE, "SRE", 3, true);
	opcodeTable[0x57] = Opcode(Opcode::AddressingMode::ZPX, &CPU::SRE, "SRE", 3, true);
	opcodeTable[0x4f] = Opcode(Opcode::AddressingMode::ABS, &CPU::SRE, "SRE", 3, true);
	opcodeTable[0x5f] = Opcode(Opcode::AddressingMode::ABSX, &CPU::SRE, "SRE", 3, true);
	opcodeTable[0x5b] = Opcode(Opcode::AddressingMode::ABSY, &CPU::SRE, "SRE", 3, true);
	opcodeTable[0x43] = Opcode(Opcode::AddressingMode::INDX, &CPU::SRE, "SRE", 3, true);
	opcodeTable[0x53] = Opcode(Opcode::AddressingMode::INDY, &CPU::SRE, "SRE", 3, true);
	opcodeTable[0x9e] = Opcode(Opcode::AddressingMode::ABSY, &CPU::SXA, "SXA", 3, true);
	opcodeTable[0x9c] = Opcode(Opcode::AddressingMode::ABSX, &CPU::SYA, "SYA", 3, true);
	opcodeTable[0x0c] = Opcode(Opcode::AddressingMode::ABS, &CPU::TOP, "TOP", 3);
	opcodeTable[0x1c] = Opcode(Opcode::AddressingMode::ABSX, &CPU::TOP, "TOP", 3);
	opcodeTable[0x3c] = Opcode(Opcode::AddressingMode::ABSX, &CPU::TOP, "TOP", 3);
	opcodeTable[0x5c] = Opcode(Opcode::AddressingMode::ABSX, &CPU::TOP, "TOP", 3);
	opcodeTable[0x7c] = Opcode(Opcode::AddressingMode::ABSX, &CPU::TOP, "TOP", 3);
	opcodeTable[0xdc] = Opcode(Opcode::AddressingMode::ABSX, &CPU::TOP, "TOP", 3);
	opcodeTable[0xfc] = Opcode(Opcode::AddressingMode::ABSX, &CPU::TOP, "TOP", 3);
	opcodeTable[0x8b] = Opcode(Opcode::AddressingMode::IMM, &CPU::XAA, "XAA", 3);
	opcodeTable[0x9b] = Opcode(Opcode::AddressingMode::ABSY, &CPU::XAS, "XAS", 3);
	opcodeTable[0x1a] = Opcode(Opcode::AddressingMode::IMPL, &CPU::NOP, "NOP", 3);
	opcodeTable[0x3a] = Opcode(Opcode::AddressingMode::IMPL, &CPU::NOP, "NOP", 3);
	opcodeTable[0x5a] = Opcode(Opcode::AddressingMode::IMPL, &CPU::NOP, "NOP", 3);
	opcodeTable[0x7a] = Opcode(Opcode::AddressingMode::IMPL, &CPU::NOP, "NOP", 3);
	opcodeTable[0xda] = Opcode(Opcode::AddressingMode::IMPL, &CPU::NOP, "NOP", 3);
	opcodeTable[0xfa] = Opcode(Opcode::AddressingMode::IMPL, &CPU::NOP, "NOP", 3);
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

void CPU::UNHANDLED() {
	std::cout << "Unhandled opcode: " << std::hex << "0x" << opcode << std::endl;
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
