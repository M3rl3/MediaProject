#pragma once

#include <vector>
#include <string>

class SoundGUI {
	
	struct Song {
		std::string name;
		std::string genre;
	};
	struct SongLoader {
		Song song0, song1, song2,
			 song3, song4, song5,
			 song6, song7, song8, 
			 song9;
	}LoadInstance;

	bool isMPEG3 = false;
	bool isWAVE = false;
	bool isPaused = false;
	bool loaded = true;
	bool playOnce = true;
	
	int readIndex = 0;
	int score = 0;
	
	
	std::vector<std::string> soundFiles;

public:

	GLFWwindow* window;

	SoundGUI();
	~SoundGUI();

	int Initialize();
	void Render();
	void Update();
	void NewFrame();
	void ShutDown();

	void ManagerInit();
	void ManageDSPS();
	void ReadFromFile();
	void LoadSongFromFile();
};