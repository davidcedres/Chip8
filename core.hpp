# pragma once

# include <SFML/Graphics.hpp>
# include <array>
# include <stack>
# include <cstdint>

using address_t = uint16_t;
using byte_t = uint8_t;
using word_t = uint16_t;

class Core
{
public:
	Core();
	Core(const Core &) = delete;
	~Core() = default;

	word_t fetch() const;
	void execute(word_t opcode);
	void updateTimers();

	bool drawFlag;
	std::array<byte_t, 4096> memory;
	std::stack<address_t> stack;
	std::array<byte_t, 16> v;
	address_t pc;
	address_t i;
	byte_t delayTimer;
	byte_t soundTimer;
	std::array<bool, 2048> pixelData;
	std::array<bool, 16> pad;
};
