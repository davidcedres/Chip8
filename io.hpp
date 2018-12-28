# include <string>
# include <array> // how can I forward declare this ???

class Core;

namespace sf {
	class RenderWindow;
}

namespace io {
	void loadROM(Core & chip, const std::string & filePath);
	void updateKeypad(Core & chip);
	void buzz(sf::RenderWindow & window);
	void drawToWindow(const std::array<bool, 2048> & pixelData, 
		sf::RenderWindow & window, char scale);
} // namespace
