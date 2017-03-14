#include <string.h>
#include <stdio.h>

#include "PS3Input.h"

namespace
{
	uint32_t		ButtonIndex[][2] = {{2,8}, {2, 1}, {3, 0x80}, {3, 0x40}, {3, 0x10}, {3, 0x20}, {2, 0x10}, {2, 0x40}, {2, 0x80}, {2, 0x20}, {3, 4}, {3, 1}, {2, 2}, {3, 8}, {3, 2}, {2, 4}};
}

					PS3Input::PS3Input						()
{
	cellSysmoduleLoadModule(CELL_SYSMODULE_IO);	
	cellPadInit(7);
	cellPadClearBuf(0);
}

					PS3Input::~PS3Input						()
{
	cellPadEnd();
	cellSysmoduleUnloadModule(CELL_SYSMODULE_IO);
}

uint32_t			PS3Input::PadCount						()
{
	return 1;
}

void				PS3Input::Refresh						()
{
	cellPadGetData(0, &CurrentState[0]);
	
	for(int i = 0; i != ButtonCount; i ++)
	{
		HeldState[i] = CurrentState[0].button[ButtonIndex[i][0]] & ButtonIndex[i][1];
		
		if(!ButtonPressed(0, i))
		{
			SingleState[i] = 0;
		}

		if(ButtonPressed(0, i) && SingleState[i] == 0)
		{
			SingleState[i] = 1;
		}
	}
}

bool				PS3Input::ButtonPressed					(uint32_t aPad, uint32_t aButton)
{
	return aButton < ButtonCount ? HeldState[aButton] : 0;
}

bool				PS3Input::ButtonDown					(uint32_t aPad, uint32_t aButton)
{
	if(aButton >= ButtonCount || aPad >= 8) return false;

	if(SingleState[aButton] == 0 || SingleState[aButton] == 2) return false;
	SingleState[aButton] = 2;
	return true;
}
