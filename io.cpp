# include <SFML/Graphics.hpp>

# include <iostream>
# include <fstream>
# include <stdexcept>

# include <io.hpp>
# include <core.hpp>


void io::loadROM(Core & chip, const std::string & filePath)
{
	std::ifstream fileHandler(filePath, std::ios::binary | std::ios::ate);

	if (not fileHandler.good())
		throw std::invalid_argument("ROM could not be loaded");

	auto romSize = fileHandler.tellg();

	fileHandler.seekg(0);
	fileHandler.read(reinterpret_cast<char*>(chip.memory.data()+0x200), romSize);
	fileHandler.close();
	std::clog << filePath << " loaded correctly \n";
}

void io::updateKeypad(Core & chip)
{
	chip.pad[0x0] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::X);
	chip.pad[0x1] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num1);
	chip.pad[0x2] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num2);
	chip.pad[0x3] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num3);
	chip.pad[0x4] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q);
	chip.pad[0x5] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W);
	chip.pad[0x6] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E);
	chip.pad[0x7] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A);
	chip.pad[0x8] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S);
	chip.pad[0x9] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D);
	chip.pad[0xa] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z);
	chip.pad[0xb] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::C);
	chip.pad[0xc] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num4);
	chip.pad[0xd] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R);
	chip.pad[0xe] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::F);
	chip.pad[0xf] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::V);
}

void io::buzz(sf::RenderWindow & window)
{
	window.clear(sf::Color::Red);
	std::clog << "buzz \n";
}

void io::drawToWindow(const std::array<bool, 2048> & pixelData, 
		sf::RenderWindow & window, char scale)
{
	for (auto row = 0; row < 32; ++row)
	{
		for (auto col = 0; col < 64; ++col)
		{
			sf::RectangleShape rectangle;
			rectangle.setSize(sf::Vector2f(scale, scale));

			auto pixel = pixelData[row*64+col];
			sf::Color color = pixel ? sf::Color::White : sf::Color::Black;
			rectangle.setFillColor(color);

			rectangle.setPosition(col * scale, row * scale);
			window.draw(rectangle);
		}
	}
}
