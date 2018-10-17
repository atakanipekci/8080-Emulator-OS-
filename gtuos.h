#ifndef H_GTUOS
#define H_GTUOS

#include "8080emuCPP.h"
#include <string>
#include <vector>
#include <queue>
#include <time.h>
using namespace std;

typedef enum ThreadState{
	ready=0,running=1, blocked=2

}ThreadState;

typedef struct thread {
	
	State8080 CPUState;
	uint64_t id;
	uint64_t startCycle;
	uint64_t totalCycle;

	ThreadState threadState;
	uint64_t address;
	vector<uint64_t> blocker;

} thread;

typedef struct fileS {
	
	
	int id;
	char name[256];
	uint8_t data[256];
	int position;
	int totalBytes;
	bool open;
	time_t timeC;
	time_t timeA;
	time_t timeM;

	//FILE *fptr;
	

} fileS;


class GTUOS{
	public:
		GTUOS(const CPU8080 &cpu,int debugno);
		uint64_t handleCall(CPU8080 & cpu,uint64_t cycles);
		void saveMemory(const CPU8080 & cpu, std::string fileName);
		void robinSwitch(CPU8080 &cpu,uint64_t currentCycles,uint64_t maxCycles);
		void readFileSystem(string name);
		void writeFileSystem(string name);
		void printFiles();
		vector<thread> threadTable;
		//vector<fileS> fileTable;
		fileS fileTable[15];
	private:
		int debug;
		queue<thread> threadQueue;
		uint8_t startid;
		uint8_t fileid;
		uint64_t currentID;
		uint64_t switchCycle;
		uint64_t printB(CPU8080 & cpu);
		uint64_t printMem(CPU8080 & cpu);
		uint64_t readB(CPU8080 & cpu);
		uint64_t printStr(CPU8080 & cpu);
		uint64_t readMem(CPU8080 & cpu);
		uint64_t readStr(CPU8080 & cpu);
		uint64_t getRnd(CPU8080 & cpu);
		uint64_t fileCreate(CPU8080 & cpu);
		uint64_t fileOpen(CPU8080 & cpu);
		uint64_t fileClose(CPU8080 & cpu);
		uint64_t fileRead(CPU8080 & cpu);
		uint64_t fileWrite(CPU8080 & cpu);
		uint64_t fileSeek(CPU8080 & cpu);
		uint64_t dirRead(CPU8080 & cpu);
		uint64_t TCreate(CPU8080 & cpu,uint64_t cycles);
		uint64_t TExit(CPU8080 & cpu,uint64_t currentCycles);
		uint64_t TJoin(CPU8080 & cpu,uint64_t currentCycles);
		uint64_t TYield(CPU8080 & cpu,uint64_t currentCycles);
		void switchInfo(thread oldThread, thread newThread);
		
};

#endif
