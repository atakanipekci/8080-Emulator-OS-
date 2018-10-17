#include <iostream>
#include "8080emuCPP.h"
#include "gtuos.h"
#include <stdlib.h>  
#include <time.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits.h>
#include <unistd.h>
#include <sys/stat.h> 
#include <fcntl.h>


using namespace std;


GTUOS::GTUOS(const CPU8080 &cpu, int DebugNo){

	thread mainThread;
	debug=DebugNo;
	startid=1;
	fileid=0;
	switchCycle=0;
	mainThread.CPUState=*(cpu.state);
	mainThread.CPUState.cc= cpu.state->cc;
	mainThread.blocker.push_back(-1);
	mainThread.id=(int)startid;
	startid++;
	mainThread.startCycle = 0;
	mainThread.totalCycle= 0;
	mainThread.address=0;
	mainThread.threadState=running;
	threadTable.push_back(mainThread);
	currentID=mainThread.id;
	fileS newFile;
	/*newFile.id=5;
	strcpy(newFile.name,"fk0");
	newFile.data[0]=0;
	newFile.totalBytes=0;
	fileTable[0]=newFile;

	newFile.id=6;
	strcpy(newFile.name,"fk1");
	newFile.data[0]=0;
	newFile.totalBytes=0;
	fileTable[1]=newFile;
	*/
	
	
	
	//cout<<fileTable[0].name<<endl;

}

void GTUOS::printFiles(){

int i;
	//cout<<"print started"<<endl;
	for(i=0; i<15; i++){
		if(fileTable[i].name[0]=='\0')
			break;


		cout<<"\n||||||||||||||||||||||||||||||||||||||||||||||||"<<endl;
		cout<<"File Name: "<<fileTable[i].name<<"\tTotal Bytes: "<<fileTable[i].totalBytes;
		char * timeStr = ctime(&fileTable[i].timeC);
      	timeStr[strlen(timeStr)-1] = '\0';
      	printf("\tCreation Time:  %s", timeStr);

      	timeStr = ctime(&fileTable[i].timeA);
      	timeStr[strlen(timeStr)-1] = '\0';
      	printf("\tLast Access Time:  %s", timeStr);

      	timeStr = ctime(&fileTable[i].timeM);
      	timeStr[strlen(timeStr)-1] = '\0';
      	printf("\tLast Modification Time:  %s\n", timeStr);
      	cout<<"||||||||||||||||||||||||||||||||||||||||||||||||\n"<<endl;
	}
	//cout<<"print ended"<<endl;
}

void GTUOS::writeFileSystem(string name){

	//cout<<"start writing"<<endl;
	ofstream outfile;
  	outfile.open(name.c_str(), ios::binary );
  	outfile.write((char *)&fileTable, sizeof(fileTable));
  	outfile.close();
  	//cout<<"wrote"<<endl;

}
void GTUOS::readFileSystem(string name){


	ifstream infile; 
    infile.open(name.c_str(), ios::binary ) ;
  	infile.read((char *)&fileTable, sizeof(fileTable));
  	infile.close();
  	//cout<<"read"<<endl;

}


void GTUOS::robinSwitch(CPU8080 &cpu,uint64_t currentCycles,uint64_t maxCycles){

	int size=threadQueue.size();
	thread next,old;
	int i=0,k,m;
	bool flag=false;

	if(size>=1){

		for(i=0; i<threadTable.size(); i++){
			if(threadTable[i].id==currentID)
				break;


		}

		//cout<<threadTable[i].id<<","<<threadTable[i].CPUState.pc<<endl;
		//cout<<currentID<<endl;
		//cout<<threadTable.size()<<","<<currentID<<endl;
		

		if((currentCycles-switchCycle)>=maxCycles || threadTable[i].threadState==blocked){


			//cout<<"switch"<<endl;
			//cout<<threadTable[i].CPUState.pc<<",";
			threadTable[i].totalCycle+=currentCycles-switchCycle;
			switchCycle=currentCycles;
			threadTable[i].CPUState=*(cpu.state);
			if(threadTable[i].threadState!=blocked)
				threadTable[i].threadState=ready;
			threadQueue.push(threadTable[i]);
			old=threadTable[i];
			
				do{
				flag=false;
				next=threadQueue.front();
				threadQueue.pop();
				if(next.threadState==blocked){
					//cout<<currentID<<"blocked found"<<next.id<<",,"<<threadTable.size()<<endl;
					for(k=0; k<threadTable.size(); k++){
						//cout<<"k"<<threadTable[k].id<<endl;

						for(m=0; m<next.blocker.size(); m++){
							if(threadTable[k].id==next.blocker[m]){
								flag=true;
								break;
							}
						}

					}
					if(flag==true){
						threadQueue.push(next);
					}
				}
				}while(flag==true);
			currentID=next.id;
			*(cpu.state)=next.CPUState;
			//cout<<next.CPUState.pc<<endl;
			for(i=0; i<threadTable.size(); i++){
			if(threadTable[i].id==currentID)
				break;

			}
			threadTable[i].threadState=running;
			//cout<<"id:"<<currentID<<endl;
			switchInfo(old,next);
		}



	}


}

void GTUOS::switchInfo(thread oldThread, thread newThread){

int i;
	if(debug==2 && oldThread.id !=newThread.id){

		cout<<"OLD ID: "<<oldThread.id<<" CYCLES SPENT: "<<oldThread.totalCycle<<endl;
		cout<<"NEW ID: "<<newThread.id<<endl;

	}
	else if(debug==3 && oldThread.id !=newThread.id){

		for(i=0; i<threadTable.size();i++){

			cout<<"id: "<<threadTable[i].id;
			cout<<"| a: "<<(int)threadTable[i].CPUState.a;
			cout<<"| b: "<<(int)threadTable[i].CPUState.b;
			cout<<"| c: "<<(int)threadTable[i].CPUState.c;
			cout<<"| d: "<<(int)threadTable[i].CPUState.d;
			cout<<"| e: "<<(int)threadTable[i].CPUState.e;
			cout<<"| h: "<<(int)threadTable[i].CPUState.h;
			cout<<"| l: "<<(int)threadTable[i].CPUState.l;
			cout<<"| start cycle: "<<(int)threadTable[i].startCycle;
			cout<<"| total cycle: "<<(int)threadTable[i].totalCycle;
			cout<<"| state: ";
			switch(threadTable[i].threadState){

				case 1:cout<<"running"; break;
				case 2:cout<<"blocked"; break;
				case 0:cout<<"ready"; break;


			}
			cout<<" function adress: "<<(int)threadTable[i].address;
			cout<<endl;

		}

	}


}



uint64_t GTUOS::handleCall(CPU8080 & cpu,uint64_t cycles){
	
	switch(cpu.state->a){

	case 1:
		return printB(cpu);
	case 2:
		return printMem(cpu);
	case 3:
		return readB(cpu);
	case 4:
		return readMem(cpu);
	case 5:
		return printStr(cpu);
	case 6:
		return readStr(cpu);
	case 7:
		return getRnd(cpu);
	case 8:
		TExit(cpu,cycles); return 50;
	case 9:
		TJoin(cpu,cycles); return 40;
	case 10:
		TYield(cpu,cycles); return 40;
	case 11:
		return TCreate(cpu,cycles);
	case 12:
		return fileCreate(cpu);
	case 13:
		return fileClose(cpu);
	case 14:
		return fileOpen(cpu);
	case 15:
		return fileRead(cpu);
	case 16:
		return fileWrite(cpu);
	case 17:
		return fileSeek(cpu);
	case 18:
		return dirRead(cpu);

	default:
		cout<<"Unimplemented Call"<<endl;
		throw -1;
		break;

	}




}

uint64_t GTUOS::printB(CPU8080 & cpu){
	//cout<<"CALL: PRINT_B"<<endl;
	cout<<(int)cpu.state->b<<endl;

	return 10;

}

uint64_t GTUOS::readB(CPU8080 & cpu){

	int bValue;
	//cout<<"CALL: READ_B"<<endl;
	cout<<"Enter an integer between 0 and 255: "<<endl;
	cin>>bValue;
	if(bValue >255)
		bValue=255;

	else if(bValue<0)
		bValue=0;
	cpu.state->b=bValue;

	return 10;

}

uint64_t GTUOS::printMem(CPU8080 & cpu){

	uint16_t memAddress;
	uint16_t shiftedB;

	//out<<"CALL: PRINT_MEM"<<endl;
	shiftedB=((cpu.state->b)<<8);
	
	memAddress= shiftedB | cpu.state->c;

	cout<<(int)cpu.memory->at(memAddress)<<endl;


	return 10;

}

uint64_t GTUOS::readMem(CPU8080 & cpu){

	int value;
	uint16_t memAddress;
	uint16_t shiftedB;

	//cout<<"CALL: READ_MEM"<<endl;
	cout<<"Enter an integer between 0 and 255: "<<endl;
	cin>>value;
	if(value >255)
		value=255;

	else if(value<0)
		value=0;

	shiftedB=((cpu.state->b)<<8);
	
	memAddress= shiftedB | cpu.state->c;

	cpu.memory->at(memAddress)=value;

	return 10;

}

uint64_t GTUOS::printStr(CPU8080 & cpu){

	uint16_t memAddress;
	uint16_t shiftedB;
	
	//cout<<"CALL: PRINT_STR"<<endl;

	shiftedB=((cpu.state->b)<<8);
	
	memAddress= shiftedB | cpu.state->c;

	for(; cpu.memory->at(memAddress)!='\0'; memAddress++){

		cout<<cpu.memory->at(memAddress);

	}

		//cout<<endl;

	return 100;

}
uint64_t GTUOS::readStr(CPU8080 & cpu){

	uint16_t memAddress;
	uint16_t shiftedB;
	string str;
	int i;

	//cout<<"CALL: READ_STR"<<endl;
	cout<<"Enter a string: "<<endl;
	shiftedB=((cpu.state->b)<<8);
	
	memAddress= shiftedB | cpu.state->c;
	//cin.ignore();
	getline(cin,str);
	//cout<<str<<"::"<<endl;

	for(i=0;i<str.length();i++,memAddress++){

		cpu.memory->at(memAddress)=str[i];

	}
		cpu.memory->at(memAddress)='\0';

	return 100;

}



uint64_t GTUOS::getRnd(CPU8080 & cpu){

	int bValue;

	//cout<<"CALL: GET_RND"<<endl;

	bValue=rand()%256;
	//cout<<bValue<<endl;
	cpu.state->b=bValue;

	return 5;

}

uint64_t GTUOS::TCreate(CPU8080 & cpu,uint64_t cycles){

	uint16_t functionAddress;
	uint16_t shiftedB;
	thread newThread;
	
	//cout<<currentID<<"called"<<(int)startid<<endl;
	shiftedB=((cpu.state->b)<<8);
	
	functionAddress= shiftedB | cpu.state->c;
	cpu.state->pc+=1;
	cpu.state->b=startid;
	newThread.CPUState=*(cpu.state);
	newThread.CPUState.pc=functionAddress;
	newThread.id=(int)startid;
	newThread.blocker.push_back(-1);
	startid++;
	newThread.startCycle = cycles;
	newThread.totalCycle= 0;
	newThread.address=functionAddress;
	newThread.threadState=ready;

	threadTable.push_back(newThread);
	threadQueue.push(newThread);
	//cpu.state->pc=functionAddress;


	return 80;

}

uint64_t GTUOS::TExit(CPU8080 & cpu,uint64_t currentCycles){

	uint16_t functionAddress;
	uint16_t shiftedB;
	int i,k,m,j;
	bool flag=false;
	thread next,old;
	
	for(i=0; i<threadTable.size(); i++){
			if(threadTable[i].id==currentID)
				break;


	}
	/*for(j=0; j<threadTable.size(); j++){
			if(threadTable[j].id==3)
				break;


	}*/


	//cout<<currentID<<"exit"<<endl;
	//cout<<threadTable[j].CPUState.pc<<endl;
			threadTable[i].totalCycle+=currentCycles-switchCycle+50;
			switchCycle=currentCycles;
			threadTable[i].CPUState=*(cpu.state);

			if(threadTable[i].threadState!=blocked)
				threadTable[i].threadState=ready;
			old=threadTable[i];
			threadTable.erase(threadTable.begin()+i);
			//cout<<(int)threadTable[i].CPUState.pc<<endl;
			if(threadQueue.size()>0){
				
				do{
				flag=false;
				next=threadQueue.front();
				threadQueue.pop();
				if(next.threadState==blocked){

					for(k=0; k<threadTable.size(); k++){
						for(m=0; m<next.blocker.size(); m++){
							if(threadTable[k].id==next.blocker[m]){
								flag=true;
								break;
							}
						}

					}
					if(flag==true){
						threadQueue.push(next);
					}
				}
				}while(flag==true);
				//cout<<(int)next.CPUState.pc<<endl;
				//cout<<currentID<<endl;
				currentID=next.id;
				//cout<<currentID<<endl;
				*(cpu.state)=next.CPUState;
				//cpu.state->pc=next.CPUState.pc;
				for(i=0; i<threadTable.size(); i++){
				if(threadTable[i].id==currentID)
					break;

				}
				threadTable[i].threadState=running;
				switchInfo(old,next);
			}
	return 0;

}


uint64_t GTUOS::TJoin(CPU8080 & cpu,uint64_t currentCycles){

	uint16_t functionAddress;
	uint16_t shiftedB;
	int i,j,k,m;
	bool flag=false;
	thread next,old;
	//cout<<"join"<<(int)cpu.state->b<<endl;
	for(j=0; j<threadTable.size(); j++){
			if(threadTable[j].id==(int)cpu.state->b)
				break;


	}

	if(j!=threadTable.size() && (int)cpu.state->b!=currentID){
		//cout<<"here"<<endl;
		for(i=0; i<threadTable.size(); i++){
			if(threadTable[i].id==currentID)
				break;


		}
			cpu.state->pc+=1;
			threadTable[i].totalCycle+=currentCycles-switchCycle+40;
			switchCycle=currentCycles;
			threadTable[i].CPUState=*(cpu.state);
		
			threadTable[i].threadState=blocked;
			threadTable[i].blocker.push_back((int)cpu.state->b);
			//cout<<"join"<<threadTable[i].blocker.size();
			//cout<<"id:"<<currentID<<"bl:"<<threadTable[i].blocker<<endl;
			if(threadQueue.size()>0){
				threadQueue.push(threadTable[i]);
				old=threadTable[i];
				do{
				flag=false;
				next=threadQueue.front();
				threadQueue.pop();
				if(next.threadState==blocked){

					for(k=0; k<threadTable.size(); k++){
						for(m=0; m<next.blocker.size(); m++){
							if(threadTable[k].id==next.blocker[m]){
								flag=true;
								break;
							}
						}

					}
					if(flag==true){
						threadQueue.push(next);
					}
				}
				}while(flag==true);


				
				currentID=next.id;
				*(cpu.state)=next.CPUState;
				for(i=0; i<threadTable.size(); i++){
				if(threadTable[i].id==currentID)
					break;

				}
				threadTable[i].threadState=running;
				switchInfo(old,next);
			}



	}
	
	return 0;

}


uint64_t GTUOS::TYield(CPU8080 & cpu,uint64_t currentCycles){

	uint16_t functionAddress;
	uint16_t shiftedB;
	int i,j,k,m;
	bool flag=false;
	thread next;
	//cout<<"j"<<(int)cpu.state->b<<endl;
	for(j=0; j<threadTable.size(); j++){
			if(threadTable[j].id==(int)cpu.state->b)
				break;


	}

	if(j!=threadTable.size() && (int)cpu.state->b!=currentID){

		for(i=0; i<threadTable.size(); i++){
			if(threadTable[i].id==currentID)
				break;


		}
			//cpu.state->pc+=1;
			threadTable[i].totalCycle+=currentCycles-switchCycle+40;
			switchCycle=currentCycles;
			threadTable[i].CPUState=*(cpu.state);
		
			threadTable[i].threadState=ready;
			//cout<<"join"<<threadTable[i].blocker.size();
			//cout<<"id:"<<currentID<<"bl:"<<threadTable[i].blocker<<endl;
			if(threadQueue.size()>0){
				threadQueue.push(threadTable[i]);

				do{
				flag=false;
				next=threadQueue.front();
				threadQueue.pop();
				if(next.threadState==blocked){

					for(k=0; k<threadTable.size(); k++){
						for(m=0; m<next.blocker.size(); m++){
							if(threadTable[k].id==next.blocker[m]){
								flag=true;
								break;
							}
						}

					}
					if(flag==true){
						threadQueue.push(next);
					}
				}
				}while(flag==true);


				
				currentID=next.id;
				*(cpu.state)=next.CPUState;
				for(i=0; i<threadTable.size(); i++){
				if(threadTable[i].id==currentID)
					break;

				}
				threadTable[i].threadState=running;
			}


	}
	
	return 0;

}
uint64_t GTUOS::fileCreate(CPU8080 & cpu){

	uint16_t memAddress;
	uint16_t shiftedB;
	int i;
	int fid=0;
	fileS newFile;
	string str;
	
	//cout<<"CALL: PRINT_STR"<<endl;

	shiftedB=((cpu.state->b)<<8);
	
	memAddress= shiftedB | cpu.state->c;

	for(; cpu.memory->at(memAddress)!='\0'; memAddress++){

		//cout<<cpu.memory->at(memAddress);
		str+=(char)cpu.memory->at(memAddress);

	}

	
	for(i=0; i<15; i++){
		//cout<<fileTable[i].name<<endl;
		if(strcmp (fileTable[i].name,str.c_str()) == 0){
				
				cout<<"file already exists"<<endl;
				return 0;
				break;
		}
	}

	for (i=0;i<15;i++){


		if(fileTable[i].name[0]=='\0'){
			break;
		}
		fid++;
	}
	//cout<<"fid:"<<fid<<","<<i<<endl;

	newFile.id=fid;
	strcpy(newFile.name,str.c_str());
	//newFile.name=str;
	
	newFile.totalBytes=0;
	newFile.position=0;
	newFile.open=false;
	newFile.timeC=time(NULL);
	newFile.timeA=time(NULL);
	newFile.timeM=time(NULL);
	fileTable[fid]=newFile;
	cout<<fileTable[fid].name<<" created"<<endl;
	//cout<<fileTable[0].name<<endl;
	//writeFileSystem("file.dat");
	/*ofstream outfile (str.c_str());
	if (outfile.fail())
    {
        cout << "error" << endl;
    }
*/
   

		//cout<<endl;

	return 0;

}

uint64_t GTUOS::fileOpen(CPU8080 & cpu){

	uint16_t memAddress;
	uint16_t shiftedB;
	uint8_t fd;
	int i;
	string str;
	fileS newFile;
	
	//cout<<"CALL: PRINT_STR"<<endl;

	shiftedB=((cpu.state->b)<<8);
	
	memAddress= shiftedB | cpu.state->c;

	for(; cpu.memory->at(memAddress)!='\0'; memAddress++){

		//cout<<cpu.memory->at(memAddress);
		str+=(char)cpu.memory->at(memAddress);

	}

	for(i=0; i<15; i++){
		if(strcmp (fileTable[i].name,str.c_str()) == 0){
				
				fileTable[i].open=true;
				fileTable[i].timeA=time(NULL);
				cpu.state->b=(uint8_t)fileTable[i].id;
				cout<<str<<" file opened"<<endl;
				return 0;
		}
		
	}
	cout<<"file not found for opening"<<endl;

	

	return 0;

}

uint64_t GTUOS::fileClose(CPU8080 & cpu){

	uint16_t memAddress;
	uint16_t shiftedB;
	string str;
	int i;
	int targetFile;

	targetFile=(int)cpu.state->b;



	if(fileTable[targetFile].name[0]=='\0'){
		cout<<"file not found for closing"<<endl;
		return 0;
	}
	if(fileTable[targetFile].open==false){

		return 0;
	}
	fileTable[targetFile].open=false;
	fileTable[targetFile].timeA=time(NULL);
	cout<<fileTable[targetFile].name<<" file closed"<<endl;			


	

	/*if(close((uint8_t)cpu.state->b)==0){
		cout<<"file closed"<<endl;
	}*/

		//cout<<endl;

	return 0;

}

uint64_t GTUOS::fileRead(CPU8080 & cpu){

	uint16_t memAddress;
	uint16_t shiftedB;
	string str;
	int targetFile;
	int totalBytes;
	int i,j;
	

	//out<<"CALL: PRINT_MEM"<<endl;
	shiftedB=((cpu.state->b)<<8);
	
	memAddress= shiftedB | cpu.state->c;
	//uint8_t fd;
	//char buffer[3];

	//fd=(uint8_t)cpu.state->b;
	targetFile=(int)cpu.state->e;
	totalBytes=(int)cpu.state->d;


	if(fileTable[targetFile].name[0]=='\0'){
		cout<<"file not found for reading"<<endl;
		cpu.state->b=0;
		return 0;
	}

	if(fileTable[targetFile].open==false){
		cout<<"file is not open for reading"<<endl;
		cpu.state->b=0;
		return 0;
	}
	cout<<"file read called"<<endl;

	if(fileTable[targetFile].totalBytes<totalBytes)
		totalBytes=fileTable[targetFile].totalBytes;


	for(i=0; i<totalBytes; i++){


		cpu.memory->at(memAddress+i)=fileTable[targetFile].data[i];

		//cout<<(int)cpu.memory->at(memAddress+i)<<endl;

	}
	fileTable[targetFile].timeA=time(NULL);



	


	/*for(i=0; i<fileTable.size(); i++){
			
		if(fileTable[i].id==targetFile){

			break;
		}


	}
	for(j=0; j<totalBytes; j++){
	cout<<(char)fgetc(fileTable[i].fptr)<<endl;
	}

	

		//cout<<endl;

	return 0;*/
	cpu.state->b=(uint8_t)totalBytes;
	return 0;

}

uint64_t GTUOS::fileWrite(CPU8080 & cpu){

	uint16_t memAddress;
	uint16_t shiftedB;
	string str;
	int targetFile;
	int totalBytes;
	int i,j;
	int pos;
	

	//out<<"CALL: PRINT_MEM"<<endl;
	shiftedB=((cpu.state->b)<<8);
	
	memAddress= shiftedB | cpu.state->c;
	//uint8_t fd;
	//char buffer[3];

	//fd=(uint8_t)cpu.state->b;
	targetFile=(int)cpu.state->e;
	totalBytes=(int)cpu.state->d;

	cout<<"file write called"<<endl;
	if(fileTable[targetFile].name[0]=='\0'){
		cout<<"file not found for writing"<<endl;
		return 0;
	}

	if(fileTable[targetFile].open==false){
		cout<<"file is not open for writing"<<endl;
		return 0;
	}


	pos=fileTable[targetFile].position;
	for(i=0; i<totalBytes; i++){


		fileTable[targetFile].data[pos+i]=cpu.memory->at(memAddress+i*2);
		//cout<<(int)fileTable[targetFile].data[i]<<endl;
	}

	fileTable[targetFile].totalBytes+=totalBytes;
	fileTable[targetFile].position+=i;
	fileTable[targetFile].timeA=time(NULL);
	fileTable[targetFile].timeM=time(NULL);
}


uint64_t GTUOS::fileSeek(CPU8080 & cpu){

	uint16_t memAddress;
	uint16_t shiftedB;
	string str;
	char seek[10];
	int targetFile;
	int totalBytes;
	int i,j;
	

	//out<<"CALL: PRINT_MEM"<<endl;
	shiftedB=((cpu.state->b)<<8);
	
	memAddress= shiftedB | cpu.state->c;
	//uint8_t fd;
	//char buffer[3];
	for(; cpu.memory->at(memAddress)!='\0'; memAddress++){

		//cout<<cpu.memory->at(memAddress);

	}
	//fd=(uint8_t)cpu.state->b;
	targetFile=(int)cpu.state->d;

	return 0;

}

uint64_t GTUOS::dirRead(CPU8080 & cpu){

	uint16_t memAddress;
	uint16_t shiftedB;
	char buffer[50];
	string str;
	int targetFile;
	int totalBytes;
	int i,j;
	

	shiftedB=((cpu.state->b)<<8);
	
	memAddress= shiftedB | cpu.state->c;
	cout<<"dirread called"<<endl;
	str="";
	
	for(i=0; i<15; i++){
		if(fileTable[i].name[0]=='\0')
			break;
		
      	
		str+="File Name: ";
		str+=fileTable[i].name;
		str+=" Total Bytes: ";
		sprintf(buffer,"%d",fileTable[i].totalBytes);
		str+=buffer;
		str+=" Creation Time: ";

		char * timeStr = ctime(&fileTable[i].timeC);
      	timeStr[strlen(timeStr)-1] = '\0';

		str+=timeStr;

		str+=" Last Access Time: ";

		timeStr = ctime(&fileTable[i].timeA);
      	timeStr[strlen(timeStr)-1] = '\0';

		str+=timeStr;

		str+=" Last Modification Time: ";

		timeStr = ctime(&fileTable[i].timeM);
      	timeStr[strlen(timeStr)-1] = '\0';

      	str+=timeStr;
		str+="\n";
		

		
	}

	//str+='\0';



	for(i=0;i<str.length();i++,memAddress++){

		cpu.memory->at(memAddress)=str[i];

	}

	cout<<str<<endl;


	return 0;



}


void GTUOS::saveMemory(const CPU8080& cpu,string fileName){


	int i,j;
	ofstream output;
	fileName.replace(fileName.end()-4,fileName.end(),".mem");
	//cout<<fileName<<endl;

	output.open(fileName.c_str());

	for(i=0;i<0x10000; i+=16){

		output<<setw(4)<<hex<<i<<' ';
		//cout<<setw(4)<<hex<<i<<' ';

		for(j=0; j<16;j++){

			output<<setw(2)<<hex<<(int)cpu.memory->at(i+j)<<' ';
			//cout<<setw(2)<<hex<<(int)cpu.memory->at(i+j)<<' ';

		}
		output<<endl;
		//cout<<endl;


	}
	output.close();

}

