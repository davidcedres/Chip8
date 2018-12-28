# include <SFML/Graphics.hpp>

# include <core.hpp>
# include <io.hpp>
# include <stdexcept>


int main(int argc, char ** argv)
{
	if (argc != 2)
		throw std::invalid_argument("ROM path expected as argument");

	constexpr short SCALE = 10;
	constexpr short FRAMERATE = 60;

	sf::RenderWindow window(sf::VideoMode(64*SCALE, 32*SCALE), "Chip 8 Emulator");
	// window.setFramerateLimit(60);
	Core chip;

	io::loadROM(chip, argv[1]);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
			if (event.type == sf::Event::Closed)
				window.close();

		auto opcode = chip.fetch();
		chip.execute(opcode);
		chip.updateTimers();

		if (chip.drawFlag)
		{
			io::drawToWindow(chip.pixelData, window, SCALE);
			chip.drawFlag = false;
		}

		if (chip.soundTimer)
			io::buzz(window);

		io::updateKeypad(chip);

		window.display();

		sf::sleep(sf::milliseconds(1000.f/60));
	}
	return 0;
}