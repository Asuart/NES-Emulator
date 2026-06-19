#pragma once
#include "Globals.h"
#include "Bus.h"
#include "PPU.h"

class Bus;

class CPU {
public:
	uint64_t clock;
	uint32_t stepClock;

	CPU(Bus& bus);

	void Reset();
	uint32_t Step();
	void NMIException();

private:
	struct Opcode {
		enum class AddressingMode { IMPL = 0, REL, IMM, ZP, ZPX, ZPY, ABS, ABSX, ABSY, INDX, INDY, ACC, IND };

		AddressingMode addressing;
		void(CPU::* func)();
		std::string name;
		uint8_t cycles;
		bool write;
		bool crossPageCycle;

		Opcode(AddressingMode addressing = AddressingMode::IMPL, void(CPU::* f)() = &CPU::NOP, std::string name = "Uninitialized", uint8_t cycles = 1, bool write = false, bool crossPageCycel = false);
	};

	Bus& bus;

	union {
		uint8_t f;
		struct {
			uint8_t f_carry : 1;
			uint8_t f_zero : 1;
			uint8_t f_int : 1;
			uint8_t f_decimal : 1;
			uint8_t f_break : 1;
			uint8_t f_1 : 1;
			uint8_t f_overflow : 1;
			uint8_t f_sign : 1;
		};
	};

	uint8_t ac, x, y, sp;
	uint16_t pc, disasmPC;

	uint8_t opcode;
	uint8_t value;
	uint16_t address;

	bool disasm = false;

	void Decode();
	void Execute();
	void WriteBack();

	uint16_t GetResetAddress();
	uint16_t GetNMIAddress();
	uint16_t GetBreakAddress();

	void Push8(uint8_t val);
	void Push16(uint16_t val);
	uint8_t Pull8();
	uint16_t Pull16();

	void DisasmCurrent();

	// official
	void ADC();
	void AND();
	void ASL();
	void BCC();
	void BCS();
	void BEQ();
	void BIT();
	void BMI();
	void BNE();
	void BPL();
	void BRK();
	void BVC();
	void BVS();
	void CLC();
	void CLD();
	void CLI();
	void CLV();
	void CMP();
	void CPX();
	void CPY();
	void DEC();
	void DEX();
	void DEY();
	void EOR();
	void INC();
	void INX();
	void INY();
	void JMP();
	void JSR();
	void LDA();
	void LDX();
	void LDY();
	void LSR();
	void NOP();
	void ORA();
	void PHA();
	void PHP();
	void PLA();
	void PLP();
	void ROL();
	void ROR();
	void RTI();
	void RTS();
	void SBC();
	void SEC();
	void SED();
	void SEI();
	void STA();
	void STX();
	void STY();
	void TAX();
	void TAY();
	void TSX();
	void TXA();
	void TXS();
	void TYA();

	//unofficial
	void DOP();
	void ANC();
	void SAX();
	void ARR();
	void ALR();
	void ATX();
	void AXA();
	void AXS();
	void DCP();
	void ISC();
	void KIL();
	void LAR();
	void LAX();
	void RLA();
	void RRA();
	void SLO();
	void SRE();
	void SXA();
	void SYA();
	void TOP();
	void XAA();
	void XAS();

	std::array<Opcode, 0x100> opcodeTable = {
		Opcode(Opcode::AddressingMode::IMM, &CPU::BRK, "BRK", 7),
		Opcode(Opcode::AddressingMode::INDX, &CPU::ORA, "ORA", 6),
		Opcode(Opcode::AddressingMode::IMPL, &CPU::KIL, "KIL", 3),
		Opcode(Opcode::AddressingMode::INDX, &CPU::SLO, "SLO", 3, true),
		Opcode(Opcode::AddressingMode::ZP, &CPU::DOP, "DOP", 3),
		Opcode(Opcode::AddressingMode::ZP, &CPU::ORA, "ORA", 3),
		Opcode(Opcode::AddressingMode::ZP, &CPU::ASL, "ASL", 5, true),
		Opcode(Opcode::AddressingMode::ZP, &CPU::SLO, "SLO", 3, true),
		Opcode(Opcode::AddressingMode::IMPL, &CPU::PHP, "PHP", 3),
		Opcode(Opcode::AddressingMode::IMM, &CPU::ORA, "ORA", 2),
		Opcode(Opcode::AddressingMode::ACC, &CPU::ASL, "ASL", 2, true),
		Opcode(Opcode::AddressingMode::IMM, &CPU::ANC, "ANC", 3),
		Opcode(Opcode::AddressingMode::ABS, &CPU::TOP, "TOP", 3),
		Opcode(Opcode::AddressingMode::ABS, &CPU::ORA, "ORA", 4),
		Opcode(Opcode::AddressingMode::ABS, &CPU::ASL, "ASL", 6, true),
		Opcode(Opcode::AddressingMode::ABS, &CPU::SLO, "SLO", 3, true),

		Opcode(Opcode::AddressingMode::REL, &CPU::BPL, "BPL", 2, false, true),
		Opcode(Opcode::AddressingMode::INDY, &CPU::ORA, "ORA", 5, false, true),
		Opcode(Opcode::AddressingMode::IMPL, &CPU::KIL, "KIL", 3),
		Opcode(Opcode::AddressingMode::INDY, &CPU::SLO, "SLO", 3, true),
		Opcode(Opcode::AddressingMode::ZPX, &CPU::DOP, "DOP", 3),
		Opcode(Opcode::AddressingMode::ZPX, &CPU::ORA, "ORA", 4),
		Opcode(Opcode::AddressingMode::ZPX, &CPU::ASL, "ASL", 6, true),
		Opcode(Opcode::AddressingMode::ZPX, &CPU::SLO, "SLO", 3, true),
		Opcode(Opcode::AddressingMode::IMPL, &CPU::CLC, "CLC", 2),
		Opcode(Opcode::AddressingMode::ABSY, &CPU::ORA, "ORA", 4, false, true),
		Opcode(Opcode::AddressingMode::IMPL, &CPU::NOP, "NOP", 3),
		Opcode(Opcode::AddressingMode::ABSY, &CPU::SLO, "SLO", 3, true),
		Opcode(Opcode::AddressingMode::ABSX, &CPU::TOP, "TOP", 3),
		Opcode(Opcode::AddressingMode::ABSX, &CPU::ORA, "ORA", 4, false, true),
		Opcode(Opcode::AddressingMode::ABSX, &CPU::ASL, "ASL", 7, true),
		Opcode(Opcode::AddressingMode::ABSX, &CPU::SLO, "SLO", 3, true),

		Opcode(Opcode::AddressingMode::ABS, &CPU::JSR, "JSR", 6),
		Opcode(Opcode::AddressingMode::INDX, &CPU::AND, "AND", 6),
		Opcode(Opcode::AddressingMode::IMPL, &CPU::KIL, "KIL", 3),
		Opcode(Opcode::AddressingMode::INDX, &CPU::RLA, "RLA", 3, true),
		Opcode(Opcode::AddressingMode::ZP, &CPU::BIT, "BIT", 3),
		Opcode(Opcode::AddressingMode::ZP, &CPU::AND, "AND", 3),
		Opcode(Opcode::AddressingMode::ZP, &CPU::ROL, "ROL", 5, true),
		Opcode(Opcode::AddressingMode::ZP, &CPU::RLA, "RLA", 3, true),
		Opcode(Opcode::AddressingMode::IMPL, &CPU::PLP, "PLP", 4),
		Opcode(Opcode::AddressingMode::IMM, &CPU::AND, "AND", 2),
		Opcode(Opcode::AddressingMode::ACC, &CPU::ROL, "ROL", 2, true),
		Opcode(Opcode::AddressingMode::IMM, &CPU::ANC, "ANC", 3),
		Opcode(Opcode::AddressingMode::ABS, &CPU::BIT, "BIT", 4),
		Opcode(Opcode::AddressingMode::ABS, &CPU::AND, "AND", 4),
		Opcode(Opcode::AddressingMode::ABS, &CPU::ROL, "ROL", 6, true),
		Opcode(Opcode::AddressingMode::ABS, &CPU::RLA, "RLA", 3, true),

		Opcode(Opcode::AddressingMode::REL, &CPU::BMI, "BMI", 2, false, true),
		Opcode(Opcode::AddressingMode::INDY, &CPU::AND, "AND", 5, false, true),
		Opcode(Opcode::AddressingMode::IMPL, &CPU::KIL, "KIL", 3),
		Opcode(Opcode::AddressingMode::INDY, &CPU::RLA, "RLA", 3, true),
		Opcode(Opcode::AddressingMode::ZPX, &CPU::DOP, "DOP", 3),
		Opcode(Opcode::AddressingMode::ZPX, &CPU::AND, "AND", 4),
		Opcode(Opcode::AddressingMode::ZPX, &CPU::ROL, "ROL", 6, true),
		Opcode(Opcode::AddressingMode::ZPX, &CPU::RLA, "RLA", 3, true),
		Opcode(Opcode::AddressingMode::IMPL, &CPU::SEC, "SEC", 2),
		Opcode(Opcode::AddressingMode::ABSY, &CPU::AND, "AND", 4, false, true),
		Opcode(Opcode::AddressingMode::IMPL, &CPU::NOP, "NOP", 3),
		Opcode(Opcode::AddressingMode::ABSY, &CPU::RLA, "RLA", 3, true),
		Opcode(Opcode::AddressingMode::ABSX, &CPU::TOP, "TOP", 3),
		Opcode(Opcode::AddressingMode::ABSX, &CPU::AND, "AND", 4, false, true),
		Opcode(Opcode::AddressingMode::ABSX, &CPU::ROL, "ROL", 7, true),
		Opcode(Opcode::AddressingMode::ABSX, &CPU::RLA, "RLA", 3, true),

		Opcode(Opcode::AddressingMode::IMPL, &CPU::RTI, "RTI", 6),
		Opcode(Opcode::AddressingMode::INDX, &CPU::EOR, "EOR", 6),
		Opcode(Opcode::AddressingMode::IMPL, &CPU::KIL, "KIL", 3),
		Opcode(Opcode::AddressingMode::INDX, &CPU::SRE, "SRE", 3, true),
		Opcode(Opcode::AddressingMode::ZP, &CPU::DOP, "DOP", 3),
		Opcode(Opcode::AddressingMode::ZP, &CPU::EOR, "EOR", 3),
		Opcode(Opcode::AddressingMode::ZP, &CPU::LSR, "LSR", 5, true),
		Opcode(Opcode::AddressingMode::ZP, &CPU::SRE, "SRE", 3, true),
		Opcode(Opcode::AddressingMode::IMPL, &CPU::PHA, "PHA", 3),
		Opcode(Opcode::AddressingMode::IMM, &CPU::EOR, "EOR", 2),
		Opcode(Opcode::AddressingMode::ACC, &CPU::LSR, "LSR", 2, true),
		Opcode(Opcode::AddressingMode::IMM, &CPU::ALR, "ALR", 3),
		Opcode(Opcode::AddressingMode::ABS, &CPU::JMP, "JMP", 3),
		Opcode(Opcode::AddressingMode::ABS, &CPU::EOR, "EOR", 4),
		Opcode(Opcode::AddressingMode::ABS, &CPU::LSR, "LSR", 6, true),
		Opcode(Opcode::AddressingMode::ABS, &CPU::SRE, "SRE", 3, true),

		Opcode(Opcode::AddressingMode::REL, &CPU::BVC, "BVC", 2, false, true),
		Opcode(Opcode::AddressingMode::INDY, &CPU::EOR, "EOR", 5, false, true),
		Opcode(Opcode::AddressingMode::IMPL, &CPU::KIL, "KIL", 3),
		Opcode(Opcode::AddressingMode::INDY, &CPU::SRE, "SRE", 3, true),
		Opcode(Opcode::AddressingMode::ZPX, &CPU::DOP, "DOP", 3),
		Opcode(Opcode::AddressingMode::ZPX, &CPU::EOR, "EOR", 4),
		Opcode(Opcode::AddressingMode::ZPX, &CPU::LSR, "LSR", 6, true),
		Opcode(Opcode::AddressingMode::ZPX, &CPU::SRE, "SRE", 3, true),
		Opcode(Opcode::AddressingMode::IMPL, &CPU::CLI, "CLI", 2),
		Opcode(Opcode::AddressingMode::ABSY, &CPU::EOR, "EOR", 4, false, true),
		Opcode(Opcode::AddressingMode::IMPL, &CPU::NOP, "NOP", 2),
		Opcode(Opcode::AddressingMode::ABSY, &CPU::SRE, "SRE", 3, true),
		Opcode(Opcode::AddressingMode::ABSX, &CPU::TOP, "TOP", 3),
		Opcode(Opcode::AddressingMode::ABSX, &CPU::EOR, "EOR", 4, false, true),
		Opcode(Opcode::AddressingMode::ABSX, &CPU::LSR, "LSR", 7, true),
		Opcode(Opcode::AddressingMode::ABSX, &CPU::SRE, "SRE", 3, true),

		Opcode(Opcode::AddressingMode::IMPL, &CPU::RTS, "RTS", 6),
		Opcode(Opcode::AddressingMode::INDX, &CPU::ADC, "ADC", 6),
		Opcode(Opcode::AddressingMode::IMPL, &CPU::KIL, "KIL", 3),
		Opcode(Opcode::AddressingMode::INDX, &CPU::RRA, "RRA", 3, true),
		Opcode(Opcode::AddressingMode::ZP, &CPU::DOP, "DOP", 3),
		Opcode(Opcode::AddressingMode::ZP, &CPU::ADC, "ADC", 3),
		Opcode(Opcode::AddressingMode::ZP, &CPU::ROR, "ROR", 5, true),
		Opcode(Opcode::AddressingMode::ZP, &CPU::RRA, "RRA", 3, true),
		Opcode(Opcode::AddressingMode::IMPL, &CPU::PLA, "PLA", 4),
		Opcode(Opcode::AddressingMode::IMM,  &CPU::ADC, "ADC", 2),
		Opcode(Opcode::AddressingMode::ACC, &CPU::ROR, "ROR", 2, true),
		Opcode(Opcode::AddressingMode::IMM, &CPU::ARR, "ARR", 3),
		Opcode(Opcode::AddressingMode::IND, &CPU::JMP, "JMP", 5),
		Opcode(Opcode::AddressingMode::ABS, &CPU::ADC, "ADC", 4),
		Opcode(Opcode::AddressingMode::ABS, &CPU::ROR, "ROR", 6, true),
		Opcode(Opcode::AddressingMode::ABS, &CPU::RRA, "RRA", 3, true),

		Opcode(Opcode::AddressingMode::REL, &CPU::BVS, "BVS", 2, false, true),
		Opcode(Opcode::AddressingMode::INDY, &CPU::ADC, "ADC", 5, false, true),
		Opcode(Opcode::AddressingMode::IMPL, &CPU::KIL, "KIL", 3),
		Opcode(Opcode::AddressingMode::INDY, &CPU::RRA, "RRA", 3, true),
		Opcode(Opcode::AddressingMode::ZPX, &CPU::DOP, "DOP", 3),
		Opcode(Opcode::AddressingMode::ZPX, &CPU::ADC, "ADC", 4),
		Opcode(Opcode::AddressingMode::ZPX, &CPU::ROR, "ROR", 6, true),
		Opcode(Opcode::AddressingMode::ZPX, &CPU::RRA, "RRA", 3, true),
		Opcode(Opcode::AddressingMode::IMPL, &CPU::SEI, "SEI", 2),
		Opcode(Opcode::AddressingMode::ABSY, &CPU::ADC, "ADC", 4, false, true),
		Opcode(Opcode::AddressingMode::IMPL, &CPU::NOP, "NOP", 3),
		Opcode(Opcode::AddressingMode::ABSY, &CPU::RRA, "RRA", 3, true),
		Opcode(Opcode::AddressingMode::ABSX, &CPU::TOP, "TOP", 3),
		Opcode(Opcode::AddressingMode::ABSX, &CPU::ADC, "ADC", 4, false, true),
		Opcode(Opcode::AddressingMode::ABSX, &CPU::ROR, "ROR", 7, true),
		Opcode(Opcode::AddressingMode::ABSX, &CPU::RRA, "RRA", 3, true),

		Opcode(Opcode::AddressingMode::IMM, &CPU::DOP, "DOP", 3),
		Opcode(Opcode::AddressingMode::INDX, &CPU::STA, "STA", 6, true),
		Opcode(Opcode::AddressingMode::IMM, &CPU::DOP, "DOP", 3),
		Opcode(Opcode::AddressingMode::INDX, &CPU::SAX, "SAX", 3, true),
		Opcode(Opcode::AddressingMode::ZP, &CPU::STY, "STY", 3, true),
		Opcode(Opcode::AddressingMode::ZP, &CPU::STA, "STA", 3, true),
		Opcode(Opcode::AddressingMode::ZP, &CPU::STX, "STX", 3, true),
		Opcode(Opcode::AddressingMode::ZP, &CPU::SAX, "SAX", 3, true),
		Opcode(Opcode::AddressingMode::IMPL, &CPU::DEY, "DEY", 2),
		Opcode(Opcode::AddressingMode::IMM, &CPU::DOP, "DOP", 3),
		Opcode(Opcode::AddressingMode::IMPL, &CPU::TXA, "TXA", 2),
		Opcode(Opcode::AddressingMode::IMM, &CPU::XAA, "XAA", 3),
		Opcode(Opcode::AddressingMode::ABS, &CPU::STY, "STY", 4, true),
		Opcode(Opcode::AddressingMode::ABS, &CPU::STA, "STA", 4, true),
		Opcode(Opcode::AddressingMode::ABS, &CPU::STX, "STX", 4, true),
		Opcode(Opcode::AddressingMode::ABS, &CPU::SAX, "SAX", 3, true),

		Opcode(Opcode::AddressingMode::REL, &CPU::BCC, "BCC", 2, false, true),
		Opcode(Opcode::AddressingMode::INDY, &CPU::STA, "STA", 6, true),
		Opcode(Opcode::AddressingMode::IMPL, &CPU::KIL, "KIL", 3),
		Opcode(Opcode::AddressingMode::INDY, &CPU::AXA, "AXA", 3),
		Opcode(Opcode::AddressingMode::ZPX, &CPU::STY, "STY", 4, true),
		Opcode(Opcode::AddressingMode::ZPX, &CPU::STA, "STA", 4, true),
		Opcode(Opcode::AddressingMode::ZPY, &CPU::STX, "STX", 4, true),
		Opcode(Opcode::AddressingMode::ZPY, &CPU::SAX, "SAX", 3, true),
		Opcode(Opcode::AddressingMode::IMPL, &CPU::TYA, "TYA", 2),
		Opcode(Opcode::AddressingMode::ABSY, &CPU::STA, "STA", 5, true),
		Opcode(Opcode::AddressingMode::IMPL, &CPU::TXS, "TXS", 2),
		Opcode(Opcode::AddressingMode::ABSY, &CPU::XAS, "XAS", 3),
		Opcode(Opcode::AddressingMode::ABSX, &CPU::SYA, "SYA", 3, true),
		Opcode(Opcode::AddressingMode::ABSX, &CPU::STA, "STA", 5, true),
		Opcode(Opcode::AddressingMode::ABSY, &CPU::SXA, "SXA", 3, true),
		Opcode(Opcode::AddressingMode::ABSY, &CPU::AXA, "AXA", 3),

		Opcode(Opcode::AddressingMode::IMM, &CPU::LDY, "LDY", 2),
		Opcode(Opcode::AddressingMode::INDX, &CPU::LDA, "LDA", 6),
		Opcode(Opcode::AddressingMode::IMM, &CPU::LDX, "LDX", 2),
		Opcode(Opcode::AddressingMode::INDX, &CPU::LAX, "LAX", 3),
		Opcode(Opcode::AddressingMode::ZP, &CPU::LDY, "LDY", 3),
		Opcode(Opcode::AddressingMode::ZP, &CPU::LDA, "LDA", 3),
		Opcode(Opcode::AddressingMode::ZP, &CPU::LDX, "LDX", 3),
		Opcode(Opcode::AddressingMode::ZP, &CPU::LAX, "LAX", 3),
		Opcode(Opcode::AddressingMode::IMPL, &CPU::TAY, "TAY", 2),
		Opcode(Opcode::AddressingMode::IMM, &CPU::LDA, "LDA", 2),
		Opcode(Opcode::AddressingMode::IMPL, &CPU::TAX, "TAX", 2),
		Opcode(Opcode::AddressingMode::IMM, &CPU::ATX, "ATX", 3),
		Opcode(Opcode::AddressingMode::ABS, &CPU::LDY, "LDY", 4),
		Opcode(Opcode::AddressingMode::ABS, &CPU::LDA, "LDA", 4),
		Opcode(Opcode::AddressingMode::ABS, &CPU::LDX, "LDX", 4),
		Opcode(Opcode::AddressingMode::ABS, &CPU::LAX, "LAX", 3),

		Opcode(Opcode::AddressingMode::REL, &CPU::BCS, "BCS", 2, false, true),
		Opcode(Opcode::AddressingMode::INDY, &CPU::LDA, "LDA", 5, false, true),
		Opcode(Opcode::AddressingMode::IMPL, &CPU::KIL, "KIL", 3),
		Opcode(Opcode::AddressingMode::INDY, &CPU::LAX, "LAX", 3),
		Opcode(Opcode::AddressingMode::ZPX, &CPU::LDY, "LDY", 4),
		Opcode(Opcode::AddressingMode::ZPX, &CPU::LDA, "LDA", 4),
		Opcode(Opcode::AddressingMode::ZPY, &CPU::LDX, "LDX", 4),
		Opcode(Opcode::AddressingMode::ZPY, &CPU::LAX, "LAX", 3),
		Opcode(Opcode::AddressingMode::IMPL, &CPU::CLV, "CLV", 2),
		Opcode(Opcode::AddressingMode::ABSY, &CPU::LDA, "LDA", 4, false, true),
		Opcode(Opcode::AddressingMode::IMPL, &CPU::TSX, "TSX", 2),
		Opcode(Opcode::AddressingMode::ABSY, &CPU::LAR, "LAR", 3),
		Opcode(Opcode::AddressingMode::ABSX, &CPU::LDY, "LDY", 4, false, true),
		Opcode(Opcode::AddressingMode::ABSX, &CPU::LDA, "LDA", 4, false, true),
		Opcode(Opcode::AddressingMode::ABSY, &CPU::LDX, "LDX", 4, false, true),
		Opcode(Opcode::AddressingMode::ABSY, &CPU::LAX, "LAX", 3),

		Opcode(Opcode::AddressingMode::IMM, &CPU::CPY, "CPY", 2),
		Opcode(Opcode::AddressingMode::INDX, &CPU::CMP, "CMP", 6),
		Opcode(Opcode::AddressingMode::IMM, &CPU::DOP, "DOP", 3),
		Opcode(Opcode::AddressingMode::INDX, &CPU::DCP, "DCP", 3, true),
		Opcode(Opcode::AddressingMode::ZP, &CPU::CPY, "CPY", 3),
		Opcode(Opcode::AddressingMode::ZP, &CPU::CMP, "CMP", 3),
		Opcode(Opcode::AddressingMode::ZP, &CPU::DEC, "DEC", 5, true),
		Opcode(Opcode::AddressingMode::ZP, &CPU::DCP, "DCP", 3, true),
		Opcode(Opcode::AddressingMode::IMPL, &CPU::INY, "INY", 2),
		Opcode(Opcode::AddressingMode::IMM, &CPU::CMP, "CMP", 2),
		Opcode(Opcode::AddressingMode::IMPL, &CPU::DEX, "DEX", 2),
		Opcode(Opcode::AddressingMode::IMM, &CPU::AXS, "AXS", 3),
		Opcode(Opcode::AddressingMode::ABS, &CPU::CPY, "CPY", 4),
		Opcode(Opcode::AddressingMode::ABS, &CPU::CMP, "CMP", 4),
		Opcode(Opcode::AddressingMode::ABS, &CPU::DEC, "DEC", 6, true),
		Opcode(Opcode::AddressingMode::ABS, &CPU::DCP, "DCP", 3, true),

		Opcode(Opcode::AddressingMode::REL, &CPU::BNE, "BNE", 2, false, true),
		Opcode(Opcode::AddressingMode::INDY, &CPU::CMP, "CMP", 5, false, true),
		Opcode(Opcode::AddressingMode::IMPL, &CPU::KIL, "KIL", 3),
		Opcode(Opcode::AddressingMode::INDY, &CPU::DCP, "DCP", 3, true),
		Opcode(Opcode::AddressingMode::ZPX, &CPU::DOP, "DOP", 3),
		Opcode(Opcode::AddressingMode::ZPX, &CPU::CMP, "CMP", 4),
		Opcode(Opcode::AddressingMode::ZPX, &CPU::DEC, "DEC", 6, true),
		Opcode(Opcode::AddressingMode::ZPX, &CPU::DCP, "DCP", 3, true),
		Opcode(Opcode::AddressingMode::IMPL, &CPU::CLD, "CLD", 2),
		Opcode(Opcode::AddressingMode::ABSY, &CPU::CMP, "CMP", 4, false, true),
		Opcode(Opcode::AddressingMode::IMPL, &CPU::NOP, "NOP", 3),
		Opcode(Opcode::AddressingMode::ABSY, &CPU::DCP, "DCP", 3, true),
		Opcode(Opcode::AddressingMode::ABSX, &CPU::TOP, "TOP", 3),
		Opcode(Opcode::AddressingMode::ABSX, &CPU::CMP, "CMP", 4, false, true),
		Opcode(Opcode::AddressingMode::ABSX, &CPU::DEC, "DEC", 7, true),
		Opcode(Opcode::AddressingMode::ABSX, &CPU::DCP, "DCP", 3, true),

		Opcode(Opcode::AddressingMode::IMM, &CPU::CPX, "CPX", 2),
		Opcode(Opcode::AddressingMode::INDX, &CPU::SBC, "SBC", 6),
		Opcode(Opcode::AddressingMode::IMM, &CPU::DOP, "DOP", 3),
		Opcode(Opcode::AddressingMode::INDX, &CPU::ISC, "ISC", 3, true),
		Opcode(Opcode::AddressingMode::ZP, &CPU::CPX, "CPX", 3),
		Opcode(Opcode::AddressingMode::ZP, &CPU::SBC, "SBC", 3),
		Opcode(Opcode::AddressingMode::ZP, &CPU::INC, "INC", 5, true),
		Opcode(Opcode::AddressingMode::ZP, &CPU::ISC, "ISC", 3, true),
		Opcode(Opcode::AddressingMode::IMPL, &CPU::INX, "INX", 2),
		Opcode(Opcode::AddressingMode::IMM, &CPU::SBC, "SBC", 2),
		Opcode(Opcode::AddressingMode::IMPL, &CPU::NOP, "NOP", 2),
		Opcode(Opcode::AddressingMode::IMM, &CPU::SBC, "SBC", 3),
		Opcode(Opcode::AddressingMode::ABS, &CPU::CPX, "CPX", 4),
		Opcode(Opcode::AddressingMode::ABS, &CPU::SBC, "SBC", 4),
		Opcode(Opcode::AddressingMode::ABS, &CPU::INC, "INC", 6, true),
		Opcode(Opcode::AddressingMode::ABS, &CPU::ISC, "ISC", 3, true),

		Opcode(Opcode::AddressingMode::REL, &CPU::BEQ, "BEQ", 2, false, true),
		Opcode(Opcode::AddressingMode::INDY, &CPU::SBC, "SBC", 5, false, true),
		Opcode(Opcode::AddressingMode::IMPL, &CPU::KIL, "KIL", 3),
		Opcode(Opcode::AddressingMode::INDY, &CPU::ISC, "ISC", 3, true),
		Opcode(Opcode::AddressingMode::ZPX, &CPU::DOP, "DOP", 3),
		Opcode(Opcode::AddressingMode::ZPX, &CPU::SBC, "SBC", 4),
		Opcode(Opcode::AddressingMode::ZPX, &CPU::INC, "INC", 6, true),
		Opcode(Opcode::AddressingMode::ZPX, &CPU::ISC, "ISC", 3, true),
		Opcode(Opcode::AddressingMode::IMPL, &CPU::SED, "SED", 2),
		Opcode(Opcode::AddressingMode::ABSY, &CPU::SBC, "SBC", 4, false, true),
		Opcode(Opcode::AddressingMode::IMPL, &CPU::NOP, "NOP", 3),
		Opcode(Opcode::AddressingMode::ABSY, &CPU::ISC, "ISC", 3, true),
		Opcode(Opcode::AddressingMode::ABSX, &CPU::TOP, "TOP", 3),
		Opcode(Opcode::AddressingMode::ABSX, &CPU::SBC, "SBC", 4, false, true),
		Opcode(Opcode::AddressingMode::ABSX, &CPU::INC, "INC", 7, true),
		Opcode(Opcode::AddressingMode::ABSX, &CPU::ISC, "ISC", 3, true),
	};
};

