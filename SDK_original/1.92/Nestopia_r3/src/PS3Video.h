#ifndef PS3VIDEO_H
#define	PS3VIDEO_H

#include "cell.h"

class							PS3Video;

class							FrameBuffer
{
	friend class				PS3Video;

	public:
								FrameBuffer				(uint32_t aWidth, uint32_t aHeight);
								~FrameBuffer			();

		void					Clear					(uint32_t aColor);
		
		uint32_t				GetWidth				();
		uint32_t				GetHeight				();
		uint32_t*				GetPixels				();
		
		void					SetViewport				(uint32_t aX, uint32_t aY, uint32_t aWidth, uint32_t aHeight);
		void					SetUnderScan			(uint32_t aPercent);
		void					SetFilter				(uint32_t aOn);
		void					SetVsync				(uint32_t aOn);

		void					PutString				(const char* aMessage, uint32_t aX, uint32_t aY, uint32_t aColor);
		
	protected:
		uint32_t				Width;
		uint32_t				Height;
		uint32_t*				Pixels;
		
		uint32_t				UnderScan;
		uint32_t				Filter;
		uint32_t				Vsync;
		
		uint32_t				ViewX, ViewY, ViewWidth, ViewHeight;
};

class							PS3Video
{
	public:	
								PS3Video				();
								~PS3Video				();
						
						
		void					ShowFrame				(FrameBuffer* aBuffer);
		void					ShowFrame				(uint32_t* aPixels, uint32_t aWidth, uint32_t aHeight);
		void					ShowFrame				(uint32_t* aPixels, uint32_t aWidth, uint32_t aHeight, uint32_t aX, uint32_t aY, uint32_t aSubWidth, uint32_t aSubHeight);
		void					PutString				(uint32_t* aPixels, uint32_t aWidth, uint32_t aHeight, const char* aMessage, uint32_t aX, uint32_t aY, uint32_t aColor);
		
	protected:
		PSGLdevice*				Device;
		PSGLcontext*			Context;
		GLfloat					AspectRatio;
		
		uint32_t				Width;
		uint32_t				Height;
		uint32_t				UnderScan;
		uint32_t				Filter;
		uint32_t				Vsync;
		
		static const int		FontWidth = 8;
		static const int		FontCharsOnRow = 32;
		static const int		FontHeight = 16;
		static const int		FontImageWidth = 256;		
};

extern PS3Video*				ps3_video;

#endif
