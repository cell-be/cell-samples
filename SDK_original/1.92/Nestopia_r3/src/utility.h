#ifndef UTILITY_H_
#define UTILITY_H_

#include <string>
#include <vector>
#include "cell.h"
#include "PS3Input.h"

class				FastCounter
{
	public:
									FastCounter					(uint32_t aSpeed = 4, uint32_t aButton = PS3_BUTTON_R2) : Speed(aSpeed), Counter(0), Button(aButton){};
									~FastCounter				(){};
								
		void						Tick						(){Counter = Counter + 1 == Speed ? 0 : Counter + 1;}
		bool						DrawNow						(){return !ps3_input->ButtonPressed(0, Button) || Counter == 0;}
								
	protected:
		uint32_t					Button;
		uint32_t					Speed;
		uint32_t					Counter;
};

class				Setting
{
	public:
									Setting						(std::string aName, std::vector<std::string> aItems, uint32_t aValue) : Name(aName), Items(aItems), Selected(aValue){};
									Setting						(std::string aName, const char* aItems, uint32_t aValue);

		virtual void				Draw						(uint32_t* aFrame, uint32_t aFrameWidth, uint32_t aFrameHeight, uint32_t aX, uint32_t aY, uint32_t aColor);
		void						Input						();

		uint32_t					GetSelected					(){return Selected;}
		uint32_t					SetSelected					(uint32_t aIndex){Selected = aIndex;}

	protected:
		std::string					Name;
	
		std::vector<std::string>	Items;
		int32_t						Selected;
};

class				SettingMenu
{
	public:
									SettingMenu					(Setting* aSettings, uint32_t aCount) : Settings(aSettings), Count(aCount), Selected(0){}
									
		void						ReadFile					(const char* aName, uint32_t aID);
		void						WriteFile					(const char* aName, uint32_t aID);
		
		void						DoSettings					();
		void						Draw						();
		
	protected:
		Setting*					Settings;
		uint32_t					Count;
		int32_t						Selected;
		
		const static uint32_t		FrameWidth = 512;
		const static uint32_t		FrameHeight = 512;
		uint32_t					FrameBuffer[FrameWidth * FrameHeight];
};

#endif
