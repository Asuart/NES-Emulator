#pragma once
#include "Globals.h"
#include "Bus.h"
#include "MMC.h"
#include "PPU.h"

class Bus;

class CPU {
public:
	uint64_t clock;

	CPU(Bus* interconnect);

	void Reset();
	void Step();
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

		Opcode(AddressingMode addressing = AddressingMode::IMPL, void(CPU::* f)() = &CPU::UNHANDLED, std::string name = "UNHANDLED", uint8_t cycles = 1, bool write = false, bool crossPageCycel = false);
	};

	Bus* bus;

	std::array<Opcode, 256> opcodeTable;

#pragma pack(push, 1)

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
#pragma pack(pop)


	uint8_t ac, x, y, sp;
	uint16_t pc, disasmPC;

	uint8_t opcode;
	uint8_t value;
	uint16_t address;

	bool disasm = false;

	void Decode();
	void Execute();
	void WriteBack();

	void InitOpcodeTable();

	uint16_t GetResetAddress();
	uint16_t GetNMIAddress();
	uint16_t GetBreakAddress();

	void Push8(uint8_t val);
	void Push16(uint16_t val);
	uint8_t Pull8();
	uint16_t Pull16();

	void DisasmCurrent();

	void UNHANDLED();

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
};

