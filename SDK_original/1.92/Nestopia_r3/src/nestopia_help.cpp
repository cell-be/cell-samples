#include "cell.h"
#include <fstream>

#include "PS3Video.h"
#include "PS3Audio.h"
#include "PS3Input.h"
#include "nestopia_help.h"

using namespace Nes::Api;
using namespace Nes;

namespace
{
	Setting		SettingDef[] = 
	{
		Setting("Save State Slot      ", " 0\0 1\0 2\0 3\0 4\0 5\0 6\0 7\0 8\0 9\0", 0),
		Setting("Disable Sprite Limit ", "No*\0Yes\0", 0),
		Setting("Crop                 ", "No\0Yes*\0", 0),
		Setting("Vsync                ", "No*\0Yes\0", 0),
		Setting("Underscan            ", " 0%*\0 5%\0 10%\0", 0),
		Setting("Texture Filter       ", "Off*\0On\0", 0)
	};
	
	enum SettingID {STATE_SLOT, DISABLE_SPRITE, CROP, VSYNC, UNDERSCAN, TEXTURE_FILTER, SETTING_COUNT};
	
	const std::string		SaveDir = "/dev_hdd0/game/NSTP90000/USRDIR/save/";
}

bool NST_CALLBACK			VideoLock						(void* userData, Video::Output& video)
{
	NestopiaEmu* emu = (NestopiaEmu*)userData;

	video.pixels = emu->Buffer->GetPixels();
	video.pitch = emu->Buffer->GetWidth() * 4;
	return true;
}

void NST_CALLBACK 			VideoUnlock						(void* userData, Video::Output& video)
{
	NestopiaEmu* emu = (NestopiaEmu*)userData;
	
	if(SettingDef[CROP].GetSelected() == 1)
	{
		emu->Buffer->SetViewport(0, 8, 256, 224);
	}
	else
	{
		emu->Buffer->SetViewport(0, 0, 256, 240);
	}
	
	emu->Buffer->SetUnderScan(SettingDef[UNDERSCAN].GetSelected() * 5);
	emu->Buffer->SetFilter(SettingDef[TEXTURE_FILTER].GetSelected());
	emu->Buffer->SetVsync(SettingDef[VSYNC].GetSelected());
	
	if(sys_time_get_system_time() - emu->MessageTime < 5000 * 1000)
	{
		emu->Buffer->PutString(emu->Message.c_str(), 4, 4, 0xA0A0A0A0);
	}
	
	if(emu->Counter.DrawNow())
	{
		ps3_video->ShowFrame(emu->Buffer);
	}
}

void NST_CALLBACK 			DoFileIO						(void *userData, User::File& file)
{
	NestopiaEmu* emu = (NestopiaEmu*)userData;

	unsigned char *compbuffer;
	int compsize, compoffset;
	char mbstr[512];

	switch (file.GetAction())
	{
		case User::File::LOAD_BATTERY: // load in battery data from a file
		case User::File::LOAD_EEPROM: // used by some Bandai games, can be treated the same as battery files
		case User::File::LOAD_TAPE: // for loading Famicom cassette tapes
		case User::File::LOAD_TURBOFILE: // for loading turbofile data
		{
			std::ifstream savefile((SaveDir + emu->SaveFileName).c_str(), std::ifstream::in | std::ifstream::binary );

			if (savefile.is_open())
			{
				file.SetContent(savefile);
			}
		
			return;
		}

		case User::File::SAVE_BATTERY: // save battery data to a file
		case User::File::SAVE_EEPROM: // can be treated the same as battery files
		case User::File::SAVE_TAPE: // for saving Famicom cassette tapes
		case User::File::SAVE_TURBOFILE: // for saving turbofile data
		{
			std::ofstream batteryFile((SaveDir + emu->SaveFileName).c_str(), std::ifstream::out | std::ifstream::binary );
			const void* savedata;
			unsigned long savedatasize;

			file.GetContent(savedata, savedatasize);

			if (batteryFile.is_open())
				batteryFile.write((const char*) savedata, savedatasize);
			return;
		}
	}
}


							NestopiaEmu::NestopiaEmu		()
{
	Video::Output::lockCallback.Set(VideoLock, (void*)this);
	Video::Output::unlockCallback.Set(VideoUnlock, (void*)this);
	User::fileIoCallback.Set(DoFileIO, (void*)this);

	EmuVideo = new Video::Output;
	EmuSound = new Sound::Output;
	EmuPads  = new Input::Controllers;

	SettingsMenu = new SettingMenu(SettingDef, SETTING_COUNT);
	SettingsMenu->ReadFile("/dev_hdd0/game/NSTP90000/USRDIR/config.bin", 0x00000001);
	
	//TODO: Support other filters
	Video::RenderState renderState;
	renderState.bits.count = 32;
	renderState.bits.mask.r = 0x00ff0000;
	renderState.bits.mask.g = 0x0000ff00;
	renderState.bits.mask.b = 0x000000ff;
	renderState.filter = Video::RenderState::FILTER_NONE;
	renderState.width = 256;
	renderState.height = 240;	
	Buffer = new FrameBuffer(256, 240);
	
	Video(Nestopia).EnableUnlimSprites(SettingDef[DISABLE_SPRITE].GetSelected());
	Video(Nestopia).SetRenderState(renderState);
	
	EmuSound->samples[0] = (void*)Samples;
	EmuSound->length[0] = 48000 / 60;
	EmuSound->samples[1] = NULL;
	EmuSound->length[1] = 0;
	
	MessageTime = 0;
}

							NestopiaEmu::~NestopiaEmu		()
{
	Machine(Nestopia).Power(false);
	Machine(Nestopia).Unload();
	
	delete EmuVideo;
	delete EmuSound;
	delete EmuPads;
	
	SettingsMenu->WriteFile("/dev_hdd0/game/NSTP90000/USRDIR/config.bin", 0x00000001);	
	delete SettingsMenu;
	delete Buffer;	
}
							
void						NestopiaEmu::LoadGame			(std::string aFileName)
{
	std::string nopath = aFileName.substr(aFileName.rfind('/') + 1);

	if(aFileName.rfind('.') == std::string::npos)
	{
		SaveFileName = nopath + ".sav";
		StateFileName = nopath + ".st";
	}
	else
	{
		SaveFileName = nopath.substr(0, nopath.rfind('.') + 1) + ".sav";
		StateFileName = nopath.substr(0, nopath.rfind('.') + 1) + ".st";
	}

	std::ifstream file(aFileName.c_str(), std::ios::in | std::ios::binary);
	Machine(Nestopia).Load(file, Machine::FAVORED_NES_NTSC);

	//TODO: Support other controllers
	Input(Nestopia).ConnectController(0, Input::PAD1);
	Input(Nestopia).ConnectController(1, Input::PAD2);

	Sound(Nestopia).SetSampleBits(16);
	Sound(Nestopia).SetSampleRate(48000);
	Sound(Nestopia).SetVolume(Sound::ALL_CHANNELS, 100);
	Sound(Nestopia).SetSpeaker(Sound::SPEAKER_STEREO);	
		
	Machine(Nestopia).Power(true);
}

void						NestopiaEmu::Frame				()
{
	Counter.Tick();

	ps3_input->Refresh();
	
	//TODO: Support multiplayer
	EmuPads->pad[0].buttons = 0;
	if(ps3_input->ButtonPressed(0, PS3_BUTTON_UP)) EmuPads->pad[0].buttons |= 0x10;
	if(ps3_input->ButtonPressed(0, PS3_BUTTON_DOWN)) EmuPads->pad[0].buttons |= 0x20;
	if(ps3_input->ButtonPressed(0, PS3_BUTTON_LEFT)) EmuPads->pad[0].buttons |= 0x40;
	if(ps3_input->ButtonPressed(0, PS3_BUTTON_RIGHT)) EmuPads->pad[0].buttons |= 0x80;
	if(ps3_input->ButtonPressed(0, PS3_BUTTON_SQUARE)) EmuPads->pad[0].buttons |= 0x2;
	if(ps3_input->ButtonPressed(0, PS3_BUTTON_CROSS)) EmuPads->pad[0].buttons |= 0x1;
	if(ps3_input->ButtonPressed(0, PS3_BUTTON_SELECT)) EmuPads->pad[0].buttons |= 0x4;
	if(ps3_input->ButtonPressed(0, PS3_BUTTON_START)) EmuPads->pad[0].buttons |= 8;

	Nestopia.Execute(EmuVideo, EmuSound, EmuPads);	
	
	if(Counter.DrawNow())
	{
		ps3_audio->AddSamples(Samples, 48000 / 60);
	}
	
	if(ps3_input->ButtonDown(0, PS3_BUTTON_R3))
	{
		SettingsMenu->DoSettings();
		Video(Nestopia).EnableUnlimSprites(SettingDef[DISABLE_SPRITE].GetSelected());
	}
	
	if(ps3_input->ButtonPressed(0, PS3_BUTTON_L3) && ps3_input->ButtonDown(0, PS3_BUTTON_L1))
	{
		Machine machine(Nestopia);
		std::ofstream os((SaveDir + StateFileName + (char)('0' + SettingDef[STATE_SLOT].GetSelected())).c_str());
		Nes::Result res = machine.SaveState(os, Nes::Api::Machine::USE_COMPRESSION);	
		
		Message = "State Saved";
		MessageTime = sys_time_get_system_time();
	}
		
	if(ps3_input->ButtonPressed(0, PS3_BUTTON_L3) && ps3_input->ButtonDown(0, PS3_BUTTON_R1))
	{
		Machine machine(Nestopia);
		std::ifstream is((SaveDir + StateFileName + (char)('0' + SettingDef[STATE_SLOT].GetSelected())).c_str());
		Nes::Result res = machine.LoadState(is);

		Message = "State Loaded";
		MessageTime = sys_time_get_system_time();		
	}
	
}
