#ifndef PS3INPUT_H
#define PS3INPUT_H

#include "cell.h"

enum
{
	PS3_BUTTON_START, PS3_BUTTON_SELECT, PS3_BUTTON_SQUARE, PS3_BUTTON_CROSS, 
	PS3_BUTTON_TRIANGLE, PS3_BUTTON_CIRCLE, PS3_BUTTON_UP, PS3_BUTTON_DOWN, 
	PS3_BUTTON_LEFT, PS3_BUTTON_RIGHT, PS3_BUTTON_L1, PS3_BUTTON_L2, 
	PS3_BUTTON_L3, PS3_BUTTON_R1, PS3_BUTTON_R2, PS3_BUTTON_R3
};

class				PS3Input
{
	public:
									PS3Input				();
									~PS3Input				();
					
		void						Refresh					();
		uint32_t					PadCount				();
		bool						ButtonPressed			(uint32_t aPad, uint32_t aButton);
		bool						ButtonDown				(uint32_t aPad, uint32_t aButton);
		
	protected:
		static const uint32_t		ButtonCount = 16;

		uint32_t					HeldState[ButtonCount];
		uint32_t					SingleState[ButtonCount];		
		
		CellPadData					CurrentState[8];
};

extern PS3Input*					ps3_input;

#endif