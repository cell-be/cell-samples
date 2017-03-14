#ifndef NESTOPIA_HELP_H
#define NESTOPIA_HELP_H

#include <string>

#include "core/api/NstApiEmulator.hpp"
#include "core/api/NstApiVideo.hpp"
#include "core/api/NstApiSound.hpp"
#include "core/api/NstApiInput.hpp"
#include "core/api/NstApiMachine.hpp"
#include "core/api/NstApiUser.hpp"
#include "core/api/NstApiNsf.hpp"
#include "core/api/NstApiMovie.hpp"
#include "core/api/NstApiFds.hpp"
#include "core/api/NstApiRewinder.hpp"
#include "core/api/NstApiCartridge.hpp"
#include "core/api/NstApiCheats.hpp"
#include "core/NstCrc32.hpp"
#include "core/NstChecksum.hpp"
#include "core/NstXml.hpp"

#include "utility.h"
#include "PS3Video.h"

using namespace Nes::Api;
using namespace Nes;


class	NestopiaEmu
{
	public:
									NestopiaEmu			();
									~NestopiaEmu		();
							
		void						LoadGame			(std::string aFileName);
		void						Frame				();
		
	protected:
		friend bool NST_CALLBACK 	VideoLock			(void* userData, Video::Output& video);
		friend void NST_CALLBACK 	VideoUnlock			(void* userData, Video::Output& video);
		friend void NST_CALLBACK 	DoLog				(void *userData, const char *string, ulong length);
		friend void NST_CALLBACK 	DoFileIO			(void *userData, User::File& file);
	
		FrameBuffer*				Buffer;
	
		Nes::Api::Emulator			Nestopia;
		Video::Output* 				EmuVideo;
		Sound::Output* 				EmuSound;
		Input::Controllers*			EmuPads;
	
		SettingMenu*				SettingsMenu;

		std::string					SaveFileName;
		std::string					StateFileName;
		uint32_t					Samples[48000];

		FastCounter					Counter;
		
		std::string					Message;
		uint64_t					MessageTime;
};

#endif
