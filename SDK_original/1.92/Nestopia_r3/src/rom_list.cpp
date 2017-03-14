//TODO: Get Font metrics elsewhere

#include "cell.h"
#include <vector>
#include <string>
#include <stdlib.h>
#include <stdio.h>

#include "rom_list.h"
#include "PS3Video.h"

volatile bool			WantToDie					();

				ROMList::Directory::Directory	(const char* aPath)
{
	path = aPath;
	selected = 0;

	int dirhandle;
	cellFsOpendir(aPath, &dirhandle);
	
	while(1)
	{
		uint64_t readsize;
		CellFsDirent item;
		cellFsReaddir(dirhandle, &item, &readsize);
		
		if(readsize == 0)
		{
			break;
		}
		
		if(item.d_type == CELL_FS_TYPE_DIRECTORY)
		{
			if(strcmp(item.d_name, ".") == 0 || strcmp(item.d_name, "..") == 0)
			{
				continue;
			}
			
			items.push_back(std::string(item.d_name) + std::string("/"));
		}
		else if(item.d_type == CELL_FS_TYPE_REGULAR)
		{
			items.push_back(std::string(item.d_name));
		}
	}
}

						ROMList::ROMList			()
{
	Buffer = new FrameBuffer(640, 480);

	HaveROM = false;
	CurrentDepth = 0;

	Push("/");
}

						ROMList::~ROMList			()
{
	delete Buffer;
}
		
const char*				ROMList::GetROM				()
{
	HaveROM = false;

	//TODO: Better timing, now
	while(!WantToDie() && HaveROM == false)
	{
		Input();
		Draw();
		sys_timer_usleep(1000 * 100);
	}
	
	//TODO: Not a good idea
	return WantToDie() ? 0 : ROMName.c_str();
}
		

void					ROMList::Draw				()
{
	Buffer->Clear(0);

	uint32_t LineCount = Buffer->GetHeight() / 16;
	
	int online = 0;
	for(int i = CurrentDirectory->selected - LineCount / 2; i != CurrentDirectory->selected + LineCount / 2; i ++)
	{
		online ++;
	
		if(i < 0)
		{
			continue;
		}
		
		if(i >= CurrentDirectory->items.size())
		{
			break;
		}
		
		Buffer->PutString(CurrentDirectory->items[i].c_str(), 2, online, i == CurrentDirectory->selected ? 0xA0A0A0A0 : 0xA0000FF);
	}
	
	ps3_video->ShowFrame(Buffer);
}

void					ROMList::Input				()
{
	ps3_input->Refresh();

	uint32_t LineCount = Buffer->GetHeight() / 16;	
	
	uint32_t oldselect = CurrentDirectory->selected;
	
	CurrentDirectory->selected -= ps3_input->ButtonPressed(0, PS3_BUTTON_UP) ? 1 : 0;
	CurrentDirectory->selected += ps3_input->ButtonPressed(0, PS3_BUTTON_DOWN) ? 1 : 0;
	CurrentDirectory->selected -= ps3_input->ButtonPressed(0, PS3_BUTTON_LEFT) ? LineCount : 0;
	CurrentDirectory->selected += ps3_input->ButtonPressed(0, PS3_BUTTON_RIGHT) ? LineCount : 0;
	
	CurrentDirectory->selected = CurrentDirectory->selected < 0 ? 0 : CurrentDirectory->selected;
	CurrentDirectory->selected = CurrentDirectory->selected >= CurrentDirectory->items.size() - 1 ? CurrentDirectory->items.size() - 1 : CurrentDirectory->selected;
	
	if(ps3_input->ButtonDown(0, PS3_BUTTON_CIRCLE))
	{
		Pop();
		return;
	}
	
	if(ps3_input->ButtonDown(0, PS3_BUTTON_CROSS))
	{
		if(CurrentDirectory->items[CurrentDirectory->selected].find("/") != std::string::npos)
		{
			Push((CurrentDirectory->path + "/" + CurrentDirectory->items[CurrentDirectory->selected]).c_str());
		}
		else
		{
			HaveROM = true;
			ROMName = CurrentDirectory->path + CurrentDirectory->items[CurrentDirectory->selected];
		}
	}
}

void					ROMList::Push				(const char* aPath)
{
	if(CurrentDepth < MaxDepth - 1)
	{
		DirectoryStack[CurrentDepth ++] = new Directory(aPath);
	}
	
	CurrentDirectory = DirectoryStack[CurrentDepth - 1];
}

void					ROMList::Pop				()
{
	if(CurrentDepth > 1)
	{
		delete DirectoryStack[--CurrentDepth];
		CurrentDirectory = DirectoryStack[CurrentDepth - 1];
	}
}
