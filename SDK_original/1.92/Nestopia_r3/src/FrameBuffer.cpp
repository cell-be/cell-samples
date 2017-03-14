#include "PS3Video.h"
#include "font.h"


						FrameBuffer::FrameBuffer			(uint32_t aWidth, uint32_t aHeight) : Width(aWidth), Height(aHeight), ViewX(0), ViewY(0), ViewWidth(aWidth), ViewHeight(aHeight), UnderScan(0), Filter(0), Vsync(0)
{
	Pixels = (uint32_t*)malloc(Width * Height * 4);
}
						
						FrameBuffer::~FrameBuffer			()
{
	free(Pixels);
}

void					FrameBuffer::Clear					(uint32_t aColor)
{
	for(int i = 0; i != Width * Height; i ++)
	{
		Pixels[i] = aColor;
	}
}	
								
uint32_t				FrameBuffer::GetWidth				()
{
	return Width;
}

uint32_t				FrameBuffer::GetHeight				()
{
	return Height;
}

uint32_t*				FrameBuffer::GetPixels				()
{
	return Pixels;
}
		
void					FrameBuffer::SetViewport			(uint32_t aX, uint32_t aY, uint32_t aWidth, uint32_t aHeight)
{
	if(aX + aWidth > Width || aY + aHeight > Height)
	{
		return;
	}
	
	ViewX = aX;
	ViewY = aY;
	ViewWidth = aWidth;
	ViewHeight = aHeight;
}

void					FrameBuffer::SetFilter				(uint32_t aOn)
{
	Filter = aOn;
}

void					FrameBuffer::SetUnderScan			(uint32_t aPercent)
{
	UnderScan = aPercent;
}

void					FrameBuffer::SetVsync				(uint32_t aOn)
{
	Vsync = aOn;
}

void					FrameBuffer::PutString				(const char* aMessage, uint32_t aX, uint32_t aY, uint32_t aColor)
{
	//TODO: Get font size elsewhere
	aX = aX * 8;
	aY = aY * 16;

	if(aY + 16 >= Height || aX + 8 >= Width)
	{
		return;
	}

	for(int i = 0; i != strlen(aMessage); i ++)
	{
		uint32_t font_x = (aMessage[i] % 32) * 8;
		uint32_t font_y = (aMessage[i] / 32) * 16;

		if(aX + 8 * i >= Width - (8 * 2))
		{
			return;
		}
		
		for(int y = 0; y != 16; y ++)
		{
			for(int x = 0; x != 8; x ++)
			{
				if(thefont[(font_y + y) * 256 + font_x + x] == 1)
				{
					Pixels[(y + aY) * Width + (x + aX + (8 * i))] = aColor;
				}
			}
		}
	}
}
