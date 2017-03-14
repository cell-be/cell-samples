#include <PSGL/psgl.h>
#include <PSGL/psglu.h>
#include <stdio.h>

#include "PS3Video.h"
#include "font.h"

namespace
{
	const GLfloat	verts43[] = {-1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f};
	const GLfloat	verts169[] = {-0.75f, -1.0f, 0.0f, 0.75f, -1.0f, 0.0f, 0.75f, 1.0f, 0.0f, -0.75f, 1.0f, 0.0f};	
	const GLfloat	tverts[] = {0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f};
}


						PS3Video::PS3Video				()
{
	PSGLinitOptions initOpts = 
	{
		enable: PSGL_INIT_MAX_SPUS | PSGL_INIT_INITIALIZE_SPUS | PSGL_INIT_HOST_MEMORY_SIZE,
		maxSPUs: 1, 
		initializeSPUs: false,
		persistentMemorySize: 0,
		transientMemorySize: 0,
		errorConsole: 0,
		fifoSize: 0,  
		hostMemorySize: 128 * 1024 * 1024
	};
	
	psglInit(&initOpts);
	
	Device = psglCreateDeviceAuto(GL_ARGB_SCE, GL_NONE, GL_MULTISAMPLING_NONE_SCE);
	Context = psglCreateContext();
	psglMakeCurrent(Context, Device);
    psglResetCurrentContext();

	AspectRatio = psglGetDeviceAspectRatio(Device);

	GLuint width, height;
	psglGetRenderBufferDimensions(Device, &width, &height);
	glViewport(0, 0, width, height);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	//TODO: Enable it later
	glDisable(GL_VSYNC_SCE);

	glEnable(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, (AspectRatio == 4.0f / 3.0f) ? verts43 : verts169);
	glTexCoordPointer(2, GL_FLOAT, 0, tverts);

	Width = Height = UnderScan = Filter = Vsync = 0;
}

						PS3Video::~PS3Video				()
{
	if(Context)
	{
		psglDestroyContext(Context);
	}

	if(Device)
	{
		psglDestroyDevice(Device);
	}
	
	psglExit();						
}
						
void					PS3Video::ShowFrame				(FrameBuffer* aBuffer)
{
	if(UnderScan != aBuffer->UnderScan)
	{
		GLuint width, height;
		psglGetRenderBufferDimensions(Device, &width, &height);
		
		double percent = (double)aBuffer->UnderScan / 100.0;
				
		double widthP = width * (percent / 2);
		double heightP = height * (percent / 2);
				
		glViewport((GLuint)widthP, (GLuint)heightP, (GLuint)(width - (width * percent)), (GLuint)(height - (height * percent)));
		
		UnderScan = aBuffer->UnderScan;
	}
	
	if(Vsync != aBuffer->Vsync)
	{
		if(aBuffer->Vsync)
		{
			glEnable(GL_VSYNC_SCE);
		}
		else
		{
			glDisable(GL_VSYNC_SCE);
		}
	
		Vsync = aBuffer->Vsync;
	}
	
	if(Filter != aBuffer->Filter)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, aBuffer->Filter == 0 ? GL_NEAREST : GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, aBuffer->Filter == 0 ? GL_NEAREST : GL_LINEAR);
		
		Filter = aBuffer->Filter;
	}

	ShowFrame(aBuffer->Pixels, aBuffer->Width, aBuffer->Height, aBuffer->ViewX, aBuffer->ViewY, aBuffer->ViewWidth, aBuffer->ViewHeight);
}
						
void					PS3Video::ShowFrame				(uint32_t* aPixels, uint32_t aWidth, uint32_t aHeight)
{
	ShowFrame(aPixels, aWidth, aHeight, 0, 0, aWidth, aHeight);
}

void					PS3Video::ShowFrame				(uint32_t* aPixels, uint32_t aWidth, uint32_t aHeight, uint32_t aX, uint32_t aY, uint32_t aSubWidth, uint32_t aSubHeight)
{
	glClear(GL_COLOR_BUFFER_BIT);

	if(Width != aSubWidth || Height != aSubHeight)
	{
		Width = aSubWidth;
		Height = aSubHeight;

		GLuint bufferID = 1;
		glDeleteBuffers(1, &bufferID);		
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, bufferID);
		glBufferData(GL_PIXEL_UNPACK_BUFFER_ARB, Width * Height * 4, 0, GL_STREAM_DRAW);
	}
	
	uint32_t* texture = (uint32_t*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY);
	
	for(int i = 0; i != aSubHeight; i ++)
	{
		for(int j = 0; j != aSubWidth; j ++)
		{
			texture[i * aSubWidth + j] = aPixels[(aY + i) * aWidth + aX + j] << 8 | 0xFF;
		}
	}
	
	glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER_ARB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, 0);
	
	glDrawArrays(GL_QUADS, 0, 4);
	
	psglSwap();
}
		
void					PS3Video::PutString				(uint32_t* aPixels, uint32_t aWidth, uint32_t aHeight, const char* aMessage, uint32_t aX, uint32_t aY, uint32_t aColor)
{
	aX = aX * FontWidth;
	aY = aY * FontHeight;

	if(aY + FontHeight >= aHeight || aX + FontWidth >= aWidth)
	{
		return;
	}

	for(int i = 0; i != strlen(aMessage); i ++)
	{
		uint32_t font_x = (aMessage[i] % FontCharsOnRow) * FontWidth;
		uint32_t font_y = (aMessage[i] / FontCharsOnRow) * FontHeight;

		if(aX + FontWidth * i >= aWidth - (FontWidth * 2))
		{
			return;
		}
		
		for(int y = 0; y != FontHeight; y ++)
		{
			for(int x = 0; x != FontWidth; x ++)
			{
				if(thefont[(font_y + y) * FontImageWidth + font_x + x] == 1)
				{
					aPixels[(y + aY) * aWidth + (x + aX + (FontWidth * i))] = aColor;
				}
			}
		}
	}
}
