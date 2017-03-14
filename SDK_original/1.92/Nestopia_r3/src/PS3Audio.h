#ifndef PS3AUDIO_H
#define	PS3AUDIO_H

#include "cell.h"

class							PS3Audio
{
	public:	
								PS3Audio				();
								~PS3Audio				();

		void					AddSamples				(uint32_t* aSamples, uint32_t aCount);
		void					GetSamples				(uint32_t* aSamples, uint32_t aCount);
		volatile uint32_t 		GetBufferAmount			();

	protected:
		friend void				ProcessAudioThread		(uint64_t aBcD);
	
		sys_event_queue_t		EventQueue;
		sys_ipc_key_t			EventKey;
		sys_ppu_thread_t		ThreadID;
	
		static const int		BlockCount = 16;
	
		uint32_t				Port;
		CellAudioPortConfig		Config;

		uint32_t 				RingBuffer[96000];
		uint32_t 				ReadPosition;
		uint32_t 				WritePosition;
		uint32_t 				BufferCount;
		volatile bool			BufferBusy;
};

extern PS3Audio*				ps3_audio;

#endif
