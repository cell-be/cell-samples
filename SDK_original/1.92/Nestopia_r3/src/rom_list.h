#ifndef ROM_LIST_H
#define ROM_LIST_H

#include "PS3Video.h"
#include "PS3Input.h"

class			ROMList
{
	public:
								ROMList				();
								~ROMList			();
					
		const char*				GetROM				();
		
	protected:
		void					Draw				();
		void					Input				();
		
		void					Push				(const char* aPath);
		void					Pop					();
		
		static const uint32_t	MaxDepth = 16;
		
		FrameBuffer*			Buffer;

		class		Directory
		{
			public:
				std::vector<std::string>			items;
				int									selected;
			
				Directory(const char* aPath);
				std::string							path;
		};
		
		Directory*				CurrentDirectory;
		Directory*				DirectoryStack[MaxDepth];
		uint32_t				CurrentDepth;
	
		bool					HaveROM;
		std::string				ROMName;
};

#endif
