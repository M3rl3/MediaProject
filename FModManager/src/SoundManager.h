#pragma once

#include <FMOD/fmod.hpp>
#include <FMOD/fmod_errors.h>

#include <string>
#include <conio.h>
#include <map>

#define MAX_CHANNELS 255

class SoundManager {

public:

	struct ChannelGroup
	{
		FMOD::ChannelGroup* current_grp;
		float current_pan;
		float dsp_pitch;

		ChannelGroup() : current_grp(nullptr), current_pan(0.0f), dsp_pitch(1.0f) {}
	};

	std::map<std::string, FMOD::Sound*> sounds;
	FMOD::Channel* channel;

	SoundManager();
	~SoundManager();

	int Initialize();
	void ShutDown();

	int CreateChannelGroup(std::string name);
	int SetParentChannel(const std::string& child_name, const std::string& parent_name);
	int SetChannelGroupVolume(const std::string& name, float volume);
	int LoadSounds(const std::string& name, const std::string& path, const int mode);
	int PlaySounds(const std::string& sound_name, const std::string& channel_group_name);
	int PauseSounds(const std::string& channel_group_name, bool paused);
	int FetchChannelGroup(const std::string& name, ChannelGroup** channel_group);
	int GetChannelGroupStatus(const std::string& name, bool* enabled);
	int SetChannelGroupStatus(const std::string& name, bool enabled);
	int SetChannelPan(const std::string& name, const float pan);
	int CreateDSPEffect(const std::string& name, FMOD_DSP_TYPE dsp_type, const float value);
	int GetDSPEffect(const std::string& name, FMOD::DSP** dsp);
	int AddDSPEffect(const std::string& channel_group_name, const std::string& effect_name);

	float GetVolume(const std::string& name, float* volume);
	float SetVolume(const std::string& name, float volume);
	//int Stop(const std::string& sound_name, const std::string& channel_group_name);


private:

	FMOD_RESULT last_result = FMOD_OK;
	FMOD::System* fmod_sys = nullptr;

	std::map<std::string, ChannelGroup*> channel_groups;

	
	std::map<std::string, FMOD::DSP*> dsps;

};