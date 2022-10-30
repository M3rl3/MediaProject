#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <FMOD/fmod.hpp>
#include <FMOD/fmod_dsp_effects.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <iostream>
#include <string>
#include <fstream>

#include "SoundGUI.h"

SoundManager man;
void CallBack(GLFWwindow* window, const int key, int scancode, const int action, const int mods);

SoundGUI::SoundGUI() {

}

SoundGUI::~SoundGUI() {

}

int SoundGUI::Initialize() {
	
	// GLFW window setup
	if (!glfwInit()) {
		return 1;
	}

	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	window = glfwCreateWindow(1366, 768, "Media Fundamentals", nullptr, nullptr);

	//If window wasn't created
	if (!window) {
		return 1;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); //v-sync

	//Load process
	if (!gladLoadGLLoader((GLADloadproc)(glfwGetProcAddress))) {
		std::cerr << "Error: unable to obtain pocess address.";
		return 1;
	}

	int screen_width, screen_height;
	glfwGetFramebufferSize(window, &screen_width, &screen_height);
	glViewport(0, 0, screen_width, screen_height);

	glfwSetKeyCallback(window, CallBack);

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

	//Init dear ImGUI	
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
	ImGui::StyleColorsDark();		//dark theme

	if (man.Initialize() != 0) {
		return 1;
	}

	ManagerInit();
	ReadFromFile();
	ManageDSPS();

	score = 0;
	
	return 0;
}

void SoundGUI::NewFrame() {
	//Start a new frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void SoundGUI::Update() {
	//The main gui part
	static const char* possibleGenres[] = {"None Selected", "Alternative", 
											"Rock", "Metal", "Rap", "Pop", 
											"Jazz", "Country", "Blues"};
	ImGui::Begin("Compression");
	LoadSongFromFile();
	ImGui::End();

	ImGui::Begin("Sound Controls");
	
	SoundManager::ChannelGroup* channelGroup0;
	man.FetchChannelGroup("master", &channelGroup0);
	SoundManager::ChannelGroup* channelGroup1;
	man.FetchChannelGroup("sounds", &channelGroup1);
	SoundManager::ChannelGroup* channelGroup2;
	man.FetchChannelGroup("sfx", &channelGroup2);
	
	float volume0 = 0.0f;
	
	man.GetVolume("master", &volume0);
	volume0 = volume0 * 100;
	ImGui::SliderFloat("Master", &volume0, 0.0f, 100.0f, "%.0f");
	volume0 = volume0 / 100;
	man.SetVolume("master", volume0);

	bool enabled0 = true;

	man.GetChannelGroupStatus("master", &enabled0);
	ImGui::SameLine();
	ImGui::Checkbox("Muted0", &enabled0);
	man.SetChannelGroupStatus("master", enabled0);

	float volume1 = 0.0f;

	man.GetVolume("sounds", &volume1);
	volume1 = volume1 * 100;
	ImGui::SliderFloat("Sounds", &volume1, 0.0f, 100.0f, "%.0f");
	volume1 = volume1 / 100;
	man.SetVolume("sounds", volume1);

	bool enabled1 = true;

	man.GetChannelGroupStatus("sounds", &enabled1);
	ImGui::SameLine();
	ImGui::Checkbox("Muted1", &enabled1);
	man.SetChannelGroupStatus("sounds", enabled1);
	
	float volume2 = 0.0f;

	man.GetVolume("sfx", &volume2);
	volume2 = volume2 * 100;
	ImGui::SliderFloat("sfx", &volume2, 0.0f, 100.0f, "%.0f");
	volume2 = volume2 / 100;
	man.SetVolume("sfx", volume2);

	bool enabled2 = true;

	man.GetChannelGroupStatus("sfx", &enabled2);
	ImGui::SameLine();
	ImGui::Checkbox("Muted2", &enabled2);
	man.SetChannelGroupStatus("sfx", enabled2);

	float pitch0;
	channelGroup0->current_grp->getPitch(&pitch0);
	ImGui::SliderFloat("Pitch", &pitch0, 0.5f, 2.0f, "%.2f");
	channelGroup0->current_grp->setPitch(pitch0);
	
	ImGui::SliderFloat("Pan", &channelGroup0->current_pan, -1.0f, 1.0f, "%.2f");
	man.SetChannelPan("master", channelGroup0->current_pan);

	
	if (ImGui::Button("Stop all sounds")) {
		
		channelGroup1->current_grp->stop();
		channelGroup2->current_grp->stop();
	}
	ImGui::End();
	
	ImGui::Begin("Music Mania 1.0");
	ImGui::Text("The rule is simple: guess the genre of the song playing! ");
	
	if (ImGui::Button("Pause")) {
		if (isPaused == true) {
			isPaused = false;
			man.PauseSounds("sounds", isPaused);
			
		}
		else if (isPaused == false) {
			isPaused = true;
			man.PauseSounds("sounds", isPaused);
		}
	}
	ImGui::Separator();

	static int selectedItem0 = 0;
	ImGui::Text(LoadInstance.song0.name.c_str());
	if (ImGui::Button("Play0")) {
		man.PlaySounds(LoadInstance.song0.name, "sounds");	
	}
	ImGui::SameLine();
	
	if (ImGui::Combo("Select a Genre0: ", &selectedItem0, possibleGenres, IM_ARRAYSIZE(possibleGenres), 3)) {
		if (selectedItem0 == 2) {
			man.PlaySounds("arrow.wav", "sfx");
			ImGui::Text("Correct!");
			score++;
		}
	}

	static int selectedItem1 = 0;
	ImGui::Text(LoadInstance.song1.name.c_str());
	if (ImGui::Button("Play1")) {
		man.PlaySounds(LoadInstance.song1.name, "sounds");
	}
	ImGui::SameLine();
	if (ImGui::Combo("Select a Genre1: ", &selectedItem1, possibleGenres, IM_ARRAYSIZE(possibleGenres), 3)) {
		if (selectedItem1 == 3) {
			man.PlaySounds("arrow.wav", "sfx");
			ImGui::Text("Correct!");
			score++;
		}
	}

	static int selectedItem2 = 0;
	ImGui::Text(LoadInstance.song2.name.c_str());
	if (ImGui::Button("Play2")) {
		man.PlaySounds(LoadInstance.song2.name, "sounds");
	}
	ImGui::SameLine();
	if (ImGui::Combo("Select a Genre2: ", &selectedItem2, possibleGenres, IM_ARRAYSIZE(possibleGenres), 3)) {
		if (selectedItem2 == 2) {
			man.PlaySounds("arrow.wav", "sfx");
			ImGui::Text("Correct!");
			score++;
		}
	}

	static int selectedItem3 = 0;
	ImGui::Text(LoadInstance.song3.name.c_str());
	if (ImGui::Button("Play3")) {
		man.PlaySounds(LoadInstance.song3.name, "sounds");
	}
	ImGui::SameLine();
	if (ImGui::Combo("Select a Genre3: ", &selectedItem3, possibleGenres, IM_ARRAYSIZE(possibleGenres), 3)) {
		if (selectedItem3 == 4) {
			man.PlaySounds("arrow.wav", "sfx");
			ImGui::Text("Correct!");
			score++;
		}
	}

	static int selectedItem4 = 0;
	ImGui::Text(LoadInstance.song4.name.c_str());
	if (ImGui::Button("Play4")) {
		man.PlaySounds(LoadInstance.song4.name, "sounds");
	}
	ImGui::SameLine();
	if (ImGui::Combo("Select a Genre4: ", &selectedItem4, possibleGenres, IM_ARRAYSIZE(possibleGenres), 3)) {
		if (selectedItem4 == 8) {
			man.PlaySounds("arrow.wav", "sfx");
			ImGui::Text("Correct!");
			score++;
		}
	}

	static int selectedItem5 = 0;
	ImGui::Text(LoadInstance.song5.name.c_str());
	if (ImGui::Button("Play5")) {
		man.PlaySounds(LoadInstance.song5.name, "sounds");
	}
	ImGui::SameLine();
	if (ImGui::Combo("Select a Genre5: ", &selectedItem5, possibleGenres, IM_ARRAYSIZE(possibleGenres), 3)) {
		if (selectedItem5 == 7) {
			man.PlaySounds("arrow.wav", "sfx");
			ImGui::Text("Correct!");
			score++;
		}
	}

	static int selectedItem6 = 0;
	ImGui::Text(LoadInstance.song6.name.c_str());
	if (ImGui::Button("Play6")) {
		man.PlaySounds(LoadInstance.song6.name, "sounds");
	}
	ImGui::SameLine();
	if (ImGui::Combo("Select a Genre6: ", &selectedItem6, possibleGenres, IM_ARRAYSIZE(possibleGenres), 3)) {
		if (selectedItem6 == 1) {
			man.PlaySounds("arrow.wav", "sfx");
			ImGui::Text("Correct!");
			score++;
		}
	}

	static int selectedItem7 = 0;
	ImGui::Text(LoadInstance.song7.name.c_str());
	if (ImGui::Button("Play7")) {
		man.PlaySounds(LoadInstance.song7.name, "sounds");
	}
	ImGui::SameLine();
	if (ImGui::Combo("Select a Genre7: ", &selectedItem7, possibleGenres, IM_ARRAYSIZE(possibleGenres), 3)) {
		if (selectedItem7 == 5) {
			man.PlaySounds("arrow.wav", "sfx");
			ImGui::Text("Correct!");
			score++;
		}
	}

	static int selectedItem8 = 0;
	ImGui::Text(LoadInstance.song8.name.c_str());
	if (ImGui::Button("Play8")) {
		man.PlaySounds(LoadInstance.song8.name, "sounds");
	}
	ImGui::SameLine();
	if (ImGui::Combo("Select a Genre8: ", &selectedItem8, possibleGenres, IM_ARRAYSIZE(possibleGenres), 3)) {
		if (selectedItem8 == 3) {
			man.PlaySounds("arrow.wav", "sfx");
			ImGui::Text("Correct!");
			score++;
		}
	}

	static int selectedItem9 = 0;
	ImGui::Text(LoadInstance.song9.name.c_str());
	if (ImGui::Button("Play9")) {
		man.PlaySounds(LoadInstance.song9.name, "sounds");
	}
	ImGui::SameLine();
	if (ImGui::Combo("Select a Genre9: ", &selectedItem9, possibleGenres, IM_ARRAYSIZE(possibleGenres), 3)) {
		if (selectedItem9 == 6) {
			man.PlaySounds("arrow.wav", "sfx");
			ImGui::Text("Correct!");
			score++;
		}
	}
	
	std::string tmp = std::to_string(score);
	ImGui::Text("Score: ");
	ImGui::SameLine();
	ImGui::Text(tmp.c_str());
	if (score >= 10) {
		ImGui::Text("You win!");
		if (playOnce) {
			man.PlaySounds("air_raid.wav", "sfx");
			playOnce = false;
		}
	}
	
	ImGui::End();

	ImGui::Begin("DSP Effects");
	ImGui::Text("Master");
	if (ImGui::Button("Pitch Shift")) {
		man.AddDSPEffect("master", "PitchShift");
	}
	ImGui::SameLine();
	if (ImGui::Button("Echo")) {
		man.AddDSPEffect("master", "Echo");
	}
	ImGui::SameLine();
	if (ImGui::Button("Distortion")) {
		man.AddDSPEffect("master", "Distortion");
	}
	//ImGui::Separator();
	ImGui::Text("Music");
	if (ImGui::Button("Chorus")) {
		man.AddDSPEffect("sounds", "Chorus");
	}
	ImGui::SameLine();
	if (ImGui::Button("Tremolo")) {
		man.AddDSPEffect("sounds", "Tremolo");
	}
	ImGui::SameLine();
	if (ImGui::Button("Reverb")) {
		man.AddDSPEffect("sounds", "Reverb");
	}
	//ImGui::Separator();
	ImGui::Text("SFX");
	if (ImGui::Button("Delay")) {
		man.AddDSPEffect("sfx", "Delay");
	}
	ImGui::SameLine();
	if (ImGui::Button("Fader")) {
		man.AddDSPEffect("sfx", "Fader");
	}
	ImGui::SameLine();
	if (ImGui::Button("ITEcho")) {
		man.AddDSPEffect("sfx", "ITEcho");
	}
	ImGui::End();

	ImGui::Begin("Info");
	ImGui::Text("Numeric keys 1-5: Random sound effects.");
	ImGui::Text("Spacebar key: Global stop button.");
	ImGui::Separator();
	ImGui::Text("These songs were picked at random ");
	ImGui::Text("from my spotify most played playlist.");
	ImGui::Text("All licenses belong to their respective owners.");
	ImGui::Text("No copyright violation is intended.");
	ImGui::Separator();
	ImGui::Text("Warning: the sound effects in this ");
	ImGui::Text("appliction may damage your ears/hardware.");
	ImGui::Text("Please set an appropriate volume before playing.");
	
	ImGui::End();

}

void SoundGUI::Render() {
	//Render imgui stuff to screen
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


void SoundGUI::ShutDown() {
	//Gracefully close everything down
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(window);
	glfwTerminate();
}

void SoundGUI::ManagerInit() {

	man.CreateChannelGroup("master");
	man.CreateChannelGroup("sounds");
	man.CreateChannelGroup("sfx");

	man.SetParentChannel("sounds", "master");
	man.SetParentChannel("sfx", "master");
	man.SetChannelGroupVolume("sounds", 0.5f);
	man.SetChannelGroupVolume("sfx", 0.5f);
}

void CallBack(GLFWwindow* window, const int key, int scancode, const int action, const int mods)
{
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{

		SoundManager::ChannelGroup* channelGroup0;
		man.FetchChannelGroup("master", &channelGroup0);

		SoundManager::ChannelGroup* channelGroup1;
		man.FetchChannelGroup("sfx", &channelGroup1);

		channelGroup1->current_grp->stop();
		channelGroup0->current_grp->stop();
	}

	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
	{
		man.PlaySounds("air_raid.wav", "sfx");
	}

	if (key == GLFW_KEY_2 && action == GLFW_PRESS)
	{
		man.PlaySounds("arrow.wav", "sfx");
	}

	if (key == GLFW_KEY_3 && action == GLFW_PRESS)
	{
		man.PlaySounds("kick.wav", "sfx");
	}
	if (key == GLFW_KEY_4 && action == GLFW_PRESS)
	{
		man.PlaySounds("tick.wav", "sfx");
	}
	if (key == GLFW_KEY_5 && action == GLFW_PRESS)
	{
		man.PlaySounds("za_warudo.mp3", "sfx");
	}
}

void SoundGUI::ManageDSPS() {
	man.CreateDSPEffect("PitchShift", FMOD_DSP_TYPE_PITCHSHIFT, 2.f);
	man.CreateDSPEffect("Echo", FMOD_DSP_TYPE_ECHO, 250.f);
	man.CreateDSPEffect("Distortion", FMOD_DSP_TYPE_DISTORTION, 1.f);
	man.CreateDSPEffect("Fader", FMOD_DSP_TYPE_FADER, 100.f);
	man.CreateDSPEffect("Chorus", FMOD_DSP_TYPE_CHORUS, 500.f);
	man.CreateDSPEffect("Delay", FMOD_DSP_TYPE_DELAY, 10000.f);
	man.CreateDSPEffect("Tremolo", FMOD_DSP_TYPE_TREMOLO, 1.f);
	man.CreateDSPEffect("Reverb", FMOD_DSP_TYPE_SFXREVERB, 500000000.f*500000000.f);
	man.CreateDSPEffect("ITEcho", FMOD_DSP_TYPE_ITECHO, 500.f);
}

void SoundGUI::ReadFromFile() {

	std::ifstream readFile("readfile.txt");
	std::string input;
	
	while (readFile >> input) {
		soundFiles.push_back(input);
		readIndex++;
	}
}

void SoundGUI::LoadSongFromFile() {
	if (loaded) {

		LoadInstance.song0.name = "My Dark Disquiet - Poets of the Fall";
		LoadInstance.song0.genre = "Rock";

		LoadInstance.song1.name = "Open Your Eyes - Alter Bridge";
		LoadInstance.song1.genre = "Metal";

		LoadInstance.song2.name = "Shadow of the Day - Linkin Park";
		LoadInstance.song2.genre = "Rock";

		LoadInstance.song3.name = "The Search - NF";
		LoadInstance.song3.genre = "Rap";

		LoadInstance.song4.name = "Dark Was the Night, Cold Was the Ground - Blind Willie Johnson";
		LoadInstance.song4.genre = "Blues";

		LoadInstance.song5.name = "Always Wanting You - Merle Haggard";
		LoadInstance.song5.genre = "Country";

		LoadInstance.song6.name = "Death With Dignity - Sufjan Stevens";
		LoadInstance.song6.genre = "Alternative";

		LoadInstance.song7.name = "Don't Look Back - Rick and Morty";
		LoadInstance.song7.genre = "Pop";

		LoadInstance.song8.name = "When a Demon Defiles a Witch - Whitechapel";
		LoadInstance.song8.genre = "Metal";

		LoadInstance.song9.name = "Keep On - Alfa Mist";
		LoadInstance.song9.genre = "Jazz";

		man.LoadSounds("arrow.wav", soundFiles[20], FMOD_DEFAULT);
		man.LoadSounds("air_raid.wav", soundFiles[21], FMOD_LOOP_NORMAL);
		man.LoadSounds("kick.wav", soundFiles[22], FMOD_LOOP_NORMAL);
		man.LoadSounds("tick.wav", soundFiles[23], FMOD_LOOP_NORMAL);
		man.LoadSounds("za_warudo.mp3", soundFiles[24], FMOD_LOOP_NORMAL);
		loaded = false;
	}

	ImGui::Text("Note: this option can only be accessed once ");
	ImGui::Text("for each individual run of this application. ");

	if (ImGui::Checkbox(".mp3", &isMPEG3)) {
		if (isMPEG3 == true) {
			man.LoadSounds(LoadInstance.song0.name, soundFiles[10], FMOD_DEFAULT);
			man.LoadSounds(LoadInstance.song1.name, soundFiles[11], FMOD_DEFAULT);
			man.LoadSounds(LoadInstance.song2.name, soundFiles[12], FMOD_DEFAULT);
			man.LoadSounds(LoadInstance.song3.name, soundFiles[13], FMOD_DEFAULT);
			man.LoadSounds(LoadInstance.song4.name, soundFiles[14], FMOD_DEFAULT);
			man.LoadSounds(LoadInstance.song5.name, soundFiles[15], FMOD_DEFAULT);
			man.LoadSounds(LoadInstance.song6.name, soundFiles[16], FMOD_DEFAULT);
			man.LoadSounds(LoadInstance.song7.name, soundFiles[17], FMOD_DEFAULT);
			man.LoadSounds(LoadInstance.song8.name, soundFiles[18], FMOD_DEFAULT);
			man.LoadSounds(LoadInstance.song9.name, soundFiles[19], FMOD_DEFAULT);
			isMPEG3 = true;
			
			std::cout << "\nLoaded mp3.";
		}
	}
	else if (ImGui::Checkbox(".wav", &isWAVE)) {
		if ((isWAVE == true)) {
			man.LoadSounds(LoadInstance.song0.name, soundFiles[0], FMOD_DEFAULT);
			man.LoadSounds(LoadInstance.song1.name, soundFiles[1], FMOD_DEFAULT);
			man.LoadSounds(LoadInstance.song2.name, soundFiles[2], FMOD_DEFAULT);
			man.LoadSounds(LoadInstance.song3.name, soundFiles[3], FMOD_DEFAULT);
			man.LoadSounds(LoadInstance.song4.name, soundFiles[4], FMOD_DEFAULT);
			man.LoadSounds(LoadInstance.song5.name, soundFiles[5], FMOD_DEFAULT);
			man.LoadSounds(LoadInstance.song6.name, soundFiles[6], FMOD_DEFAULT);
			man.LoadSounds(LoadInstance.song7.name, soundFiles[7], FMOD_DEFAULT);
			man.LoadSounds(LoadInstance.song8.name, soundFiles[8], FMOD_DEFAULT);
			man.LoadSounds(LoadInstance.song9.name, soundFiles[9], FMOD_DEFAULT);
			isWAVE = true;
			
			std::cout << "\nLoaded wave.";
		}
	}
	if (isMPEG3 == true) {
		ImGui::Text("Loaded MPEG3");
	}
	else if (isWAVE == true) {
		ImGui::Text("Loaded WAVE");
	}
}
