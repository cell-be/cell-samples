#include "cell.h"

#include "utility.h"
#include "PS3Video.h"

bool						WantToDie							();


							Setting::Setting					(std::string aName, const char* aItems, uint32_t aValue) : Name(aName), Selected(aValue)
{
	const char* position = aItems;

	while(position[0] != '\0')
	{
		Items.push_back(std::string(position));
		
		while(position[0] != '\0')
		{
			position ++;
		}
		
		position ++;
	}
}

void						Setting::Draw						(uint32_t* aFrame, uint32_t aFrameWidth, uint32_t aFrameHeight, uint32_t aX, uint32_t aY, uint32_t aColor)
{
	ps3_video->PutString(aFrame, aFrameWidth, aFrameHeight, Name.c_str(), aX, aY, aColor);
	ps3_video->PutString(aFrame, aFrameWidth, aFrameHeight, Items[Selected].c_str(), aX + Name.length() + 2, aY, aColor);
}

void						Setting::Input						()
{
	Selected -= ps3_input->ButtonDown(0, PS3_BUTTON_LEFT) ? 1 : 0;
	Selected += ps3_input->ButtonDown(0, PS3_BUTTON_RIGHT) ? 1 : 0;
	Selected = Selected < 0 ? Items.size() - 1 : Selected;
	Selected = Selected >= Items.size() ? 0 : Selected;
}

void						SettingMenu::ReadFile				(const char* aName, uint32_t aID)
{
	FILE* file = fopen(aName, "rb");
	if(!file)
	{
		return;
	}
	
	uint32_t item;
	
	fread(&item, 4, 1, file);
	if(aID == item)
	{
		for(int i = 0; i != Count; i ++)
		{
			if(fread(&item, 4, 1, file) != 1)
			{
				break;
			}
			
			Settings[i].SetSelected(item);
		}
	}
	
	fclose(file);
	return;
	
}

void						SettingMenu::WriteFile				(const char* aName, uint32_t aID)
{
	FILE* file = fopen(aName, "wb");
	if(!file)
	{
		return;
	}
	
	fwrite(&aID, 4, 1, file);
	
	uint32_t item;
	
	for(int i = 0; i != Count; i ++)
	{
		item = Settings[i].GetSelected();
		fwrite(&item, 4, 1, file);
	}
	
	fclose(file);
}

void						SettingMenu::DoSettings				()
{
	while(!WantToDie())
	{
		ps3_input->Refresh();
		
		if(ps3_input->ButtonPressed(0, PS3_BUTTON_CIRCLE))
		{
			return;
		}
		
		Selected -= ps3_input->ButtonPressed(0, PS3_BUTTON_UP) ? 1 : 0;
		Selected += ps3_input->ButtonPressed(0, PS3_BUTTON_DOWN) ? 1 : 0;
		Selected = Selected < 0 ? Count - 1 : Selected;
		Selected = Selected >= Count ? 0 : Selected;
		
		Settings[Selected].Input();

		Draw();
		sys_timer_usleep(1000 * 100);
	}
}

void						SettingMenu::Draw					()
{
	memset(FrameBuffer, 0, sizeof(FrameBuffer));

	for(int i = 0; i != Count; i ++)
	{
		Settings[i].Draw(FrameBuffer, FrameWidth, FrameHeight, 2, 2 + i, i == Selected ? 0xA0A0A0A0 : 0xA0000FF);
	}
	
	ps3_video->ShowFrame(FrameBuffer, FrameWidth, FrameHeight);
}
		
