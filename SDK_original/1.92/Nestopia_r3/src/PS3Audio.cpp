#include "cell.h"
#include "PS3Audio.h"

static int16_t samples[1024];

void					ProcessAudioThread				(uint64_t aBcD)
{
	sys_event_t event;

	float* buffer = (float*)malloc(sizeof(float) * 512);

	float* silentbuffer = (float*)malloc(sizeof(float) * 512);
	for(int i = 0; i != 512; i ++)	
	{
		silentbuffer[i] = 0;
	}
	
	while(((volatile PS3Audio*)ps3_audio) == 0)
	{
		sys_timer_usleep(5000);
	}
	
	int eventreturn = CELL_OK;
	
	while(1)
	{
		eventreturn = sys_event_queue_receive(ps3_audio->EventQueue, &event, 0);
	
		if(eventreturn != CELL_OK && eventreturn != ETIMEDOUT)
		{
			free(buffer);
			free(silentbuffer);
			sys_ppu_thread_exit(0);
		}
	
		if(CELL_OK == eventreturn)
		{
			if (ps3_audio->GetBufferAmount() < 256)
			{
				cellAudioAddData(ps3_audio->Port, silentbuffer, 256, 1);
				continue;
			}

			ps3_audio->GetSamples((uint32_t*)samples, 256);

			for (uint32_t i = 0; i < 256 * 2; i ++)
			{
				buffer[i] = ((float)samples[i]) / 32768.0f;
			}
			
			cellAudioAddData(ps3_audio->Port, buffer, 256, 1);
		}
	}
}


						PS3Audio::PS3Audio				()
{
	cellSysmoduleLoadModule(CELL_SYSMODULE_AUDIO);
	
	cellAudioInit();
	
	cellAudioCreateNotifyEventQueue(&EventQueue, &EventKey);
	cellAudioSetNotifyEventQueue(EventKey);

	CellAudioPortParam portparam = {2, BlockCount, 0, 0};
	cellAudioPortOpen(&portparam, &Port);

	sys_ppu_thread_create(&ThreadID, ProcessAudioThread, 0, 0, 65536, 0, "AUDIOTHREAD");

	cellAudioGetPortConfig(Port, &Config);

	ReadPosition = 0;
	WritePosition = 0;
	BufferCount = 0;
	BufferBusy = false;
	memset(RingBuffer, 0, sizeof(RingBuffer));
	
	cellAudioPortStart(Port);
}


						PS3Audio::~PS3Audio				()
{
	cellAudioPortStop(Port);
	cellAudioPortClose(Port);
	cellAudioRemoveNotifyEventQueue(EventKey);	
	sys_event_queue_destroy(EventQueue, SYS_EVENT_QUEUE_DESTROY_FORCE);
	cellAudioQuit();
	cellSysmoduleUnloadModule(CELL_SYSMODULE_AUDIO);						
}
						
void					PS3Audio::AddSamples			(uint32_t* aSamples, uint32_t aCount)
{
	while(BufferBusy);

	BufferBusy = true;
	
	for(int i = 0; i != aCount; i ++)
	{
		RingBuffer[WritePosition++] = aSamples[i];
		BufferCount ++;
		WritePosition = WritePosition == 96000 ? 0 : WritePosition;
	}
	
	BufferBusy = false;
}

void					PS3Audio::GetSamples			(uint32_t* aSamples, uint32_t aCount)
{
	while(BufferBusy);

	BufferBusy = true;

	for(int i = 0; i != aCount; i ++)
	{
		aSamples[i] = RingBuffer[ReadPosition++];
		BufferCount --;
		ReadPosition = ReadPosition == 96000 ? 0: ReadPosition;
	}
	
	BufferBusy = false;
}

volatile uint32_t 		PS3Audio::GetBufferAmount		()
{
	return BufferCount;
}
