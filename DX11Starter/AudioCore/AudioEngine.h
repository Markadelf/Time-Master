//Audio Engine Header File//

#ifndef _AUDIO_ENGINE_H_
#define _AUDIO_ENGINE_H_

#include "fmod_studio.hpp"
#include "fmod.hpp"
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <cmath>

using namespace std;
//Vector3 struct to place sound in 3D space.
struct Vector3 {
	float x;
	float y;
	float z;
};

//Implementation struct to make calls to the FMOD API. 
//Contains code to initialize and shut down FMOD Engine and hold instances of both Studio and Low-level system objects for FMOD.
//Holds the map of all sounds and events played.
//Calls Update to FMOD.
struct Implementation {
	Implementation();
	~Implementation();

	void Update();

	FMOD::Studio::System* mpStudioSystem;
	FMOD::System* mpSystem;

	int mnNextChannelId;

	typedef map<string, FMOD::Sound*> SoundMap;
	typedef map<int, FMOD::Channel*> ChannelMap;
	typedef map<string, FMOD::Studio::EventInstance*> EventMap;
	typedef map<string, FMOD::Studio::Bank*> BankMap;
	BankMap mBanks;
	EventMap mEvents;
	SoundMap mSounds;
	ChannelMap mChannels;
};

//Engine class handles calls to Implementation to start, stop and update FMOD.
//Handles loading, playing, stopping and updating info on sounds and events.
class CAudioEngine {

public:
	static void Init();
	static void Update();
	static void Shutdown();
	static int ErrorCheck(FMOD_RESULT result);
	void LoadBank(const std::string& strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags);
	void LoadEvent(const std::string& strEventName);
	void LoadSound(const string &strSoundName, bool b3d = true, bool bLooping = false, bool bStream = false);
	void UnLoadSound(const string &strSoundName);	
	int PlaySounds(const string &strSoundName, const Vector3& vPos = Vector3{ 0, 0, 0 }, float fVolumedB = 0.0f);
	void PlayEvent(const string &strEventName);	
	void StopEvent(const string &strEventName, bool bImmediate = false);	
	void SetChannel3dPosition(int nChannelId, const Vector3& vPosition);
	void SetChannelVolume(int nChannelId, float fVolumedB);	
	bool IsEventPlaying(const string &strEventName) const;
	float dbToVolume(float dB);
	float VolumeTodB(float volume);
	FMOD_VECTOR VectorToFmod(const Vector3& vPosition);

	/*ToDo
	ReOrganize:
	void CAudioEngine::GetEventParameter(const string &strEventName, const string &strParameterName, float* parameter);
	void CAudioEngine::SetEventParameter(const string &strEventName, const string &strParameterName, float fValue);	
	*/
};

#endif