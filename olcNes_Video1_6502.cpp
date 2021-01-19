#include <iostream>
#include <sstream>

#include "Bus.h"
#include "6502.h"

#define _PGE_APPLICATION
#include "PixelGameEngine.h"



class Demo_6502 : public ::PixelGameEngine
{
public:
	Demo_6502() { sAppName = "6502 Demonstration"; }

	Bus nes;
	std::map<uint16_t, std::string> mapAsm;

	std::string hex(uint32_t n, uint8_t d)
	{
		std::string s(d, '0');
		for (int i = d - 1; i >= 0; i--, n >>= 4)
			s[i] = "0123456789ABCDEF"[n & 0xF];
		return s;
	};

	void DrawRam(int x, int y, uint16_t nAddr, int nRows, int nColumns)
	{
		int nRamX = x, nRamY = y;
		for (int row = 0; row < nRows; row++)
		{
			std::string sOffset = "$" + hex(nAddr, 4) + ":";
			for (int col = 0; col < nColumns; col++)
			{
				sOffset += " " + hex(nes.read(nAddr, true), 2);
				nAddr += 1;
			}
			DrawString(nRamX, nRamY, sOffset);
			nRamY += 10;
		}
	}

	void DrawCpu(int x, int y)
	{
		std::string status = "STATUS: ";
		DrawString(x , y , "STATUS:", ::WHITE);
		DrawString(x  + 64, y, "N", nes.cpu.status & 6502::N ? ::GREEN : ::RED);
		DrawString(x  + 80, y , "V", nes.cpu.status & 6502::V ? ::GREEN : ::RED);
		DrawString(x  + 96, y , "-", nes.cpu.status & 6502::U ? ::GREEN : ::RED);
		DrawString(x  + 112, y , "B", nes.cpu.status & 6502::B ? ::GREEN : ::RED);
		DrawString(x  + 128, y , "D", nes.cpu.status & 6502::D ? ::GREEN : ::RED);
		DrawString(x  + 144, y , "I", nes.cpu.status & 6502::I ? ::GREEN : ::RED);
		DrawString(x  + 160, y , "Z", nes.cpu.status & 6502::Z ? ::GREEN : ::RED);
		DrawString(x  + 178, y , "C", nes.cpu.status & 6502::C ? ::GREEN : ::RED);
		DrawString(x , y + 10, "PC: $" + hex(nes.cpu.pc, 4));
		DrawString(x , y + 20, "A: $" +  hex(nes.cpu.a, 2) + "  [" + std::to_string(nes.cpu.a) + "]");
		DrawString(x , y + 30, "X: $" +  hex(nes.cpu.x, 2) + "  [" + std::to_string(nes.cpu.x) + "]");
		DrawString(x , y + 40, "Y: $" +  hex(nes.cpu.y, 2) + "  [" + std::to_string(nes.cpu.y) + "]");
		DrawString(x , y + 50, "Stack P: $" + hex(nes.cpu.stkp, 4));
	}

	void DrawCode(int x, int y, int nLines)
	{
		auto it_a = mapAsm.find(nes.cpu.pc);
		int nLineY = (nLines >> 1) * 10 + y;
		if (it_a != mapAsm.end())
		{
			DrawString(x, nLineY, (*it_a).second, ::CYAN);
			while (nLineY < (nLines * 10) + y)
			{
				nLineY += 10;
				if (++it_a != mapAsm.end())
				{
					DrawString(x, nLineY, (*it_a).second);
				}
			}
		}

		it_a = mapAsm.find(nes.cpu.pc);
		nLineY = (nLines >> 1) * 10 + y;
		if (it_a != mapAsm.end())
		{
			while (nLineY > y)
			{
				nLineY -= 10;
				if (--it_a != mapAsm.end())
				{
					DrawString(x, nLineY, (*it_a).second);
				}
			}
		}
	}

	bool OnUserCreate()
	{
		/*
			*=$8000
			LDX #10
			STX $0000
			LDX #3
			STX $0001
			LDY $0000
			LDA #0
			CLC
			loop
			ADC $0001
			DEY
			BNE loop
			STA $0002
			NOP
			NOP
			NOP
		*/
		
		// Convert hex string into bytes for RAM
		std::stringstream ss;
		ss << "A2 0A 8E 00 00 A2 03 8E 01 00 AC 00 00 A9 00 18 6D 01 00 88 D0 FA 8D 02 00 EA EA EA";
		uint16_t nOffset = 0x8000;
		while (!ss.eof())
		{
			std::string b;
			ss >> b;
			nes.ram[nOffset++] = (uint8_t)std::stoul(b, nullptr, 16);
		}

		// Set Reset Vector
		nes.ram[0xFFFC] = 0x00;
		nes.ram[0xFFFD] = 0x80;

		// Dont forget to set IRQ and NMI vectors if you want to play with those
				
		// Extract dissassembly
		mapAsm = nes.cpu.disassemble(0x0000, 0xFFFF);

		// Reset
		nes.cpu.reset();
		return true;
	}

	bool OnUserUpdate(float fElapsedTime)
	{
		Clear(::DARK_BLUE);


		if (GetKey(::Key::SPACE).bPressed)
		{
			do
			{
				nes.cpu.clock();
			} 
			while (!nes.cpu.complete());
		}

		if (GetKey(::Key::R).bPressed)
			nes.cpu.reset();

		if (GetKey(::Key::I).bPressed)
			nes.cpu.irq();

		if (GetKey(::Key::N).bPressed)
			nes.cpu.nmi();

		// Draw Ram Page 0x00		
		DrawRam(2, 2, 0x0000, 16, 16);
		DrawRam(2, 182, 0x8000, 16, 16);
		DrawCpu(448, 2);
		DrawCode(448, 72, 26);


		DrawString(10, 370, "SPACE = Step Instruction    R = RESET    I = IRQ    N = NMI");

		return true;
	}
};





int main()
{
	Demo_6502 demo;
	demo.Construct(680, 480, 2, 2);
	demo.Start();
	return 0;
}