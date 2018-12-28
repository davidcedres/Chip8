# include <core.hpp>
# include <cstdlib> // rand
# include <stdexcept>

Core::Core()
{
	// data members initialization
	drawFlag = true;
	memory.fill(0);
	v.fill(0);
	pc = 0x200;
	i = 0;
	delayTimer = 0;
	soundTimer = 0;
	pixelData.fill(false);
	pad.fill(false);
	
	// font sprites are stored in memory
	std::array<byte_t, 80> fonts {
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};

	for (auto i = 0; i < fonts.size(); ++i)
		memory[i] = fonts[i];
}

word_t Core::fetch() const
{
	return memory[pc] << 8 | memory[pc+1];
}

void Core::execute(word_t opcode)
{
	auto x = (opcode & 0x0F00) >> 8;
	auto y = (opcode & 0x00F0) >> 4;
	auto n = opcode & 0x000F;
	auto nn = opcode & 0x00FF;
	auto nnn = opcode & 0x0FFF;

	// 00E0 - Clear the display.
	if (opcode == 0x00E0)
	{
		pixelData.fill(false);
		drawFlag = true;
		pc += 2;
	}

	// 00EE - Return from a subroutine.
	else if (opcode == 0x00EE)
	{
		pc = stack.top() + 2;
		stack.pop();
	}

	// 1NNN - Jump to location nnn.
	else if ((opcode & 0xF000) == 0x1000)
	{
		pc = nnn;
	}

	// 2NNN - Call subroutine at nnn.
	else if ((opcode & 0xF000) == 0x2000)
	{
		stack.push(pc);
		pc = nnn;
	}
		
	// 3XNN - Skips the next instruction if VX equals NN
	else if ((opcode & 0xF000) == 0x3000)
	{
		pc += v[x] == nn ? 4 : 2;
	}
	
	// 4XNN - Skips the next instruction if VX doesn't equal NN
	else if ((opcode & 0xF000) == 0x4000)
	{
		pc += v[x] != nn ? 4 : 2;
	}
		
	// 5XNN - Skips the next instruction if VX equals VY
	else if ((opcode & 0xF000) == 0x5000)
	{
		pc += v[x] == v[y] ? 4 : 2;
	}
	
	// 6XNN - Sets VX to NN
	else if ((opcode & 0xF000) == 0x6000)
	{
		v[x] = nn;
		pc += 2;
	}

	// 7XNN - Adds NN to VX (Carry flag is not changed)
	else if ((opcode & 0xF000) == 0x7000)
	{
		v[x] += nn;
		pc += 2;
	}	
	
	else if ((opcode & 0xF000) == 0x8000)
	{
		// 8XY0 - Sets VX to the value of VY
		if (n == 0)
		{
			v[x] = v[y];
			pc += 2;
		}
			
		// 8XY1 - Sets VX to VX or VY
		else if (n == 1)
		{
			v[x] |= v[y];
			pc += 2;
		}

		// 8XY2 - Sets VX to VX and VY
		else if (n == 2)
		{
			v[x] &= v[y];
			pc += 2;
		}

		// 8XY3 - Sets VX to VX xor VY
		else if (n == 3)
		{
			v[x] ^= v[y];
			pc += 2;
		}

		// 8XY4 - Adds VY to VX. VF is set to 1 when there's a carry, 
		// and to 0 when there isn't
		else if (n == 4)
		{
			v[15] = v[x] + v[y] > 255 ? 1 : 0;
			v[x] += v[y];
			pc += 2;
		}

		// 8XY5 - VY is subtracted from VX. 
		// VF is set to 0 when there's a borrow, and 1 when there isn't
		else if (n == 5)
		{
			v[15] = v[y] > v[x] ? 0 : 1;
			v[x] -= v[y];		
			pc += 2;
		}

		// 8XY6 - Stores the least significant bit of VX in VF and then shifts
		// VX to the right by 1
		else if (n == 6)
		{
			v[15] = v[x] & 1;
			v[x] >>= 1;
			pc += 2;
		}

		// 8XY7 - Sets VX to VY minus VX.
		// VF is set to 0 when there's a borrow, and 1 when there isn't. 
		else if (n == 7)
		{
			v[15] = v[x] > v[y] ? 0 : 1;
			v[x] = v[y] - v[x];
			pc += 2;
		}	

		// 8XYE - Stores the most significant bit of VX in VF and then
		// shifts VX to the left by 1
		else if (n == 0xE)
		{
			v[15] = (v[x]) >> 7;
			v[x] <<= 1;
			pc += 2;
		}

		else throw std::runtime_error("Oops! A wild opcode has appeared.");
	}

	// 9XY0 - Skips the next instruction if VX doesn't equal VY
	else if ((opcode & 0xF000) == 0x9000)
	{
		pc += v[x] != v[y] ? 4 : 2;
	}

	// ANNN - Sets I to the address NNN
	else if ((opcode & 0xF000) == 0xA000)
	{
		i = nnn;
		pc += 2;
	}

	// BNNN - Jumps to the address NNN plus V0
	else if ((opcode & 0xF000) == 0xB000)
	{
		pc = nnn + v[0];
	}
	
	// CXNN - Sets VX to the result of a bitwise and operation on a
	// random number and NN
	else if ((opcode & 0xF000) == 0xC000)
	{
		v[x] = std::rand() % 255 & nn;
		pc += 2;
	}

	// DXYN: Draws a sprite at coordinate (VX, VY) that has a width of 8
	// pixels and a height of N pixels.
	// Each row of 8 pixels is read as bit-coded starting from memory
	// location I;
	// I value doesn't change after the execution of this instruction.
	// VF is set to 1 if any screen pixels are flipped from set to unset
	// when the sprite is drawn, and to 0 if that doesn't happen.
	else if ((opcode & 0xF000) == 0xD000)
	{
		v[15] = 0;
		for (auto row = 0; row < n; ++row)
		{
			auto byte = memory[i+row];
			for (auto col = 0; col < 8; ++col)
			{
				if (byte & (0b10000000 >> col))
				{
					auto pos = (v[y] + row) * 64 + v[x] + col;
					if (pixelData[pos])
						v[15] = 1;
					pixelData[pos] ^= 1;
				}
			}
		}
		drawFlag = true;
		pc += 2;
	}

	else if ((opcode & 0xF000) == 0xE000)
	{
		// EX9E - Skips the next instruction if the key stored in VX is pressed
		if ((opcode & 0x00FF) == 0x009E)
		{
			pc += pad[v[x]] == true ? 4 : 2;
		}
		
		// EXA1 - Skips the next instruction if the key stored in VX isn't pressed
		else if ((opcode & 0x00FF) == 0x00A1)
		{
			pc += pad[v[x]] == false ? 4 : 2;
		}

		else throw std::runtime_error("Oops! A wild opcode has appeared.");
	}

	else if ((opcode & 0xF000) == 0xF000)
	{
		// FX07 - Sets VX to the value of the delay timer
		if ((opcode & 0x00FF) == 0x0007)
		{
			v[x] = delayTimer;
			pc += 2;
		}

		// FX0A - A key press is awaited, and then stored in VX
		else if ((opcode & 0x00FF) == 0x000A)
		{
			auto keyPressed = false;

			for (auto i = 0; i < 16; ++i)
				if (pad[i] == true)
				{
					v[x] = i;
					keyPressed = true;
				}

			if (not keyPressed)
				return;

			pc += 2;
		}

		// FX15 - Sets the delay timer to VX
		else if ((opcode & 0x00FF) == 0x0015)
		{
			delayTimer = v[x];
			pc += 2;
		}

		// FX18 - Sets the sound timer to VX
		else if ((opcode & 0x00FF) == 0x0018)
		{
			soundTimer = v[x];
			pc += 2;
		}

		// FX1E - Adds VX to I
		// VF is set to 1 when range overflow (I+VX>0xFFF), and 0 when there isn't.
		else if ((opcode & 0x00FF) == 0x001E)
		{
			v[15] = i + v[x] > 4095 ? 1 : 0;
			i += v[x];
			// todo, mask i with 4096
			pc += 2;
		}

		// FX29 - Sets I to the location of the sprite for the character in VX.
	    // Characters 0-F (in hexadecimal) are represented by a 4x5 font
		else if ((opcode & 0x00FF) == 0x0029)
		{
			i = v[x] * 5;
			pc += 2;
		}

		// FX33 - Stores the Binary-coded decimal representation of VX
    	// at the addresses I, I plus 1, and I plus 2
		else if ((opcode & 0x00FF) == 0x0033)
		{
			memory[i] = v[x] / 100;
			memory[i+1] = (v[x] % 100) / 10;
			memory[i+2] = v[x] % 10;
			pc += 2;
		}

		// FX55 - Stores V0 to VX (including VX) in memory starting at address I
		else if ((opcode & 0x00FF) == 0x0055)
		{
			for (auto j = 0; j < x+1; ++j)
				memory[i+j] = v[j];
			pc += 2;
		}

		// FX65 - Fills V0 to VX (including VX) with values from memory
		// starting at address I
		else if ((opcode & 0x00FF) == 0x0065)
		{
			for (auto j = 0; j < x+1; ++j)
				v[j] = memory[i+j];
			pc += 2;
		}

		else throw std::runtime_error("Oops! A wild opcode has appeared.");
	}

	else throw std::runtime_error("Oops! A wild opcode has appeared.");
}

void Core::updateTimers()
{
	if (delayTimer > 0)
		delayTimer -= 1;
	if (soundTimer > 0)
		soundTimer -= 1;
}