#include <iostream>
#include "8080emuCPP.h"
#include "gtuos.h"
#include "memory.h"
#include <stdlib.h>  

using namespace std;
	// This is just a sample main function, you should rewrite this file to handle problems 
	// with new multitasking and virtual memory additions.
int main (int argc, char**argv)
{

	uint16_t cycles=0;

	srand (time(NULL));

	if (argc != 4){
		std::cerr << "Usage: prog exeFile fileSystem debugOption\n";
		exit(1); 
	}
	int DEBUG = atoi(argv[3]);

	memory mem;
	CPU8080 theCPU(&mem);
	GTUOS	theOS(theCPU,DEBUG);

	string fileName=argv[1];
	string filesys=argv[2];
	theOS.readFileSystem(filesys);
	
	theCPU.ReadFileIntoMemoryAt(argv[1], 0x0000);
	//if old debug counts remove this if	
	if(DEBUG!=1){
		DEBUG=0;
	}
 
	do	
	{
		//cout<<"size:"<<theOS.threadTable.size()<<endl;
		theOS.robinSwitch(theCPU,cycles,100);
		cycles += theCPU.Emulate8080p(0);
		theOS.robinSwitch(theCPU,cycles,100);
		//cout<<"c:"<<cycles<<endl;
		if(theCPU.isSystemCall()){
			cycles+=theOS.handleCall(theCPU,cycles);
			if(DEBUG==1){

			theOS.printFiles();
			}
		}

		//if old debug counts uncomment this.
		/*if(DEBUG==2)
			getchar();*/



	}	while (!theCPU.isHalted())
;


	cout<<"Number of cycles the CPU took before halting: "<<cycles<<endl;
	theOS.saveMemory(theCPU,fileName);
	theOS.printFiles();
	theOS.writeFileSystem(filesys);
	return 0;
}

