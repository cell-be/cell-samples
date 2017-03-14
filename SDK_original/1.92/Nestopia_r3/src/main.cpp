//TODO: Input will likely crash if the controller is disconnected
//TODO: Can 'Quit Game' from the XMB be used to exit without restarting?
//TODO: Settings, saveing

#include "cell.h"

#include "PS3Input.h"
#include "PS3Video.h"
#include "PS3Audio.h"

#include "nestopia_help.h"
#include "rom_list.h"

SYS_PROCESS_PARAM(1001, 0x10000);

//Yum, globals. I can already hear the nerds!
PS3Video*			ps3_video;
PS3Input*			ps3_input;
PS3Audio*			ps3_audio;

void Log(const char* aString)
{
//	static FILE* logfile = 0;
//	if(logfile == 0)
//	{
		//logfile = fopen("/dev_usb006/debug.log", "w");
	//}

	//fprintf(logfile, "%s\n", aString);
	//fflush(logfile);
}

namespace
{
	NestopiaEmu*	nestopia = 0;
	volatile bool	want_to_die = false;
	ROMList*		filelist;
};

void				InitPS3					()
{
	//Wait for display to be ready
	while(true)
	{
		CellVideoOutState videoState;
		cellVideoOutGetState(CELL_VIDEO_OUT_PRIMARY, 0, &videoState);

		if(videoState.state == CELL_VIDEO_OUT_OUTPUT_STATE_ENABLED)
		{
			break;
		}
	}

	cellSysmoduleLoadModule(CELL_SYSMODULE_FS);		
	
	ps3_video = new PS3Video();
	ps3_input = new PS3Input();
	ps3_audio = new PS3Audio();
}

void				QuitPS3					()
{
	if(ps3_input)
	{
		delete ps3_input;
	}
	
	if(ps3_video)
	{
		delete ps3_video;
	}

	if(ps3_audio)
	{
		delete ps3_audio;
	}
	
	
	cellSysmoduleUnloadModule(CELL_SYSMODULE_FS);	
	cellSysutilUnregisterCallback(0);
}

volatile bool		WantToDie				()
{
	cellSysutilCheckCallback();
	return want_to_die;
}

static void			sysutil_callback		(uint64_t status, uint64_t param, void *userdata)
{
	(void)param;
	(void)userdata;

	switch (status)
	{
		case CELL_SYSUTIL_REQUEST_EXITGAME:
			want_to_die = true;
		break;
		default:
		break;
	}

	return;
}

void				ReloadGame				(const char* aFileName)
{
	if(nestopia)
	{
		delete nestopia;
	}
	
	nestopia = new NestopiaEmu();
	nestopia->LoadGame(aFileName);
}

int					main					()
{
	sys_spu_initialize(6, 1);
	cellSysutilRegisterCallback(0, (CellSysutilCallback)sysutil_callback, NULL);

	InitPS3();
	
	filelist = new ROMList();
	const char* romfilename = filelist->GetROM();
	
	if(romfilename == 0)
	{
		QuitPS3();
		exit(0);
	}

	ReloadGame(romfilename);

	while(!WantToDie())
	{
		while(!WantToDie() && ps3_audio->GetBufferAmount() > 2048)
		{
			sys_timer_usleep(500);
		}
	
		nestopia->Frame();
	}

	delete nestopia;

	QuitPS3();
	exit(0);
	
}
