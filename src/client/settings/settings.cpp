#include "settings.h"
#include <cstdlib>
#include <filesystem>
#include <fstream>

namespace
{
	std::string getSaveFilePath()
	{
#ifdef _WIN32
		// Windows has no HOME; app settings go in %APPDATA% (e.g. C:\Users\you\AppData\Roaming).
		const char* base = std::getenv("APPDATA");
		std::filesystem::path dir { base ? std::filesystem::path(base) : std::filesystem::path(".") };
		dir /= "NekoCat";
#else
		// getenv returns null if the variable isn't set; building a std::string from null crashes.
		const char* home = std::getenv("HOME");
		std::filesystem::path dir {
			std::filesystem::path(home ? home : ".") /
			"Library" /
			"Application Support"/
			"NekoCat"
		};
#endif

		std::filesystem::create_directories(dir);

		return (dir / "prefSaved.txt").string();

	}
}



Settings loadSettings()
{

	std::ifstream settingsFile(getSaveFilePath());
	Settings settings {};

	if(settingsFile.is_open() && !settingsFile.fail())
	{
		std::string line {};
		std::uint32_t tmpColor {};
		float tmpScale {};
		float tmpDelay {};
		float tmpSpeed {};

        while (std::getline(settingsFile, line)) {
		std::size_t pos = line.find('=');

		std::string key = line.substr(0, pos);
		std::string value = line.substr(pos+1);

		if (key == "color") tmpColor = static_cast<std::uint32_t>(std::stoul(value));
		else if (key == "scale") tmpScale = std::stof(value);
		else if (key == "delay") tmpDelay = std::stof(value);
		else if (key == "speed") tmpSpeed = std::stof(value);
        }

		settings.color = GetColor(tmpColor);
		settings.scale = tmpScale;
		settings.delay = tmpDelay;
		settings.speed = tmpSpeed;

		settingsFile.close();
	} 
	else
	{
		settings.color = PINK;
		settings.scale = 1.5f;
		settings.delay = 0.0f;
		settings.speed = 2.0f;
	}

	return settings;
}

void saveSettings(Settings& settings)
{

	std::ofstream outFile(getSaveFilePath());

	outFile << "color=" << static_cast<std::uint32_t>(ColorToInt(settings.color)) << std::endl;
	outFile << "scale=" << settings.scale << std::endl;
	outFile << "delay=" << settings.delay << std::endl;
	outFile << "speed=" << settings.speed << std::endl;

	outFile.close();
}

