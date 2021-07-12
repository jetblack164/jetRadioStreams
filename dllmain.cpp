#include "pch.h"
#include "bass.h"
#include <string>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>
#include <iostream>
#include <sstream>

//#include <stringapiset.h>
//#include <atlstr.h>
using namespace std;


struct Pos
{
	float  x = 0;
	float  y = 0;
	float  z = 0;


};

struct Orient
{
	float dirx = 0;
	float diry = 0;
	float dirz = 0;

	float upx = 0;
	float upy = 0;
	float upz = 0;

};

mutex mtx;
atomic<bool> radiosOn(false);
atomic<bool> ioLock(false);
atomic<bool> updated(false);
vector<Pos> radiosPos;
vector <string>radiosID;
vector <Orient> radiosOrient;
vector<bool> radiosEnabled;
vector <HSTREAM>radiosStream;
vector< float>radiosVolume;
vector <int> radiosThreadID;
string toBeDeleted = "";
int timeLastCreate = 0;

int threadCount = 0;
vector<int> markedThreads;


/// UTILITY ARRAYS
vector<string> stringToArray(string inputString)
{
	string editString = inputString;
	vector<string> endArray;



	editString = editString.substr(0, editString.size() - 1);
	editString = editString.substr(1, editString.size() - 1);
	editString = editString.append(",");
	
	while (editString.size() > 0)
	{
		string tempString;
		tempString = editString.substr(0, editString.find(","));
		editString.erase(0, editString.find(",") + 1);
		endArray.push_back(tempString);
		if (editString.size() == 1)
		{
			editString = "";

		}


	}


	return endArray;
}



void deleteEntries(int idToStop)
{
	mtx.lock();
	std::cout << "stop is stuck at: before delete";
	//BASS_StreamFree(radiosStream.at(idToStop));

	toBeDeleted = (radiosID.at(idToStop));
	radiosPos.erase(radiosPos.begin() + idToStop);
	radiosOrient.erase(radiosOrient.begin() + idToStop);
	radiosEnabled.erase(radiosEnabled.begin() + idToStop);
	radiosStream.erase(radiosStream.begin() + idToStop);
	radiosID.erase(radiosID.begin() + idToStop);
	radiosVolume.erase(radiosVolume.begin() + idToStop);
	//std::cout << dumpArrays(radiosPos, radiosID, radiosOrient, radiosEnabled, radiosStream, radiosVolume);

	markedThreads.push_back(radiosThreadID.at(idToStop));
	radiosThreadID.erase(radiosThreadID.begin() + idToStop);

	mtx.unlock();

}
void shutdownStreams()
{
	mtx.lock();
	vector<string> allStreams = radiosID;
	mtx.unlock();
	radiosOn = false;
	int i = 0;
	while (allStreams.size() > 0)
	{
		
		deleteEntries(0);
		mtx.lock();
		 allStreams = radiosID;
		mtx.unlock();




	}
		/*if (allStreams.size() > 0)
		{
			for (int i = 0; i <= allStreams.size() - 1; i++)
			{

				vector<string>::iterator streamI = find(allStreams.begin(), allStreams.end(), allStreams.at(i));
				if (streamI != allStreams.end())
				{

					ioLock = true;
					int idToStop = streamI - allStreams.begin();
					deleteEntries(idToStop);
					

					ioLock = false;


				}
			}

		}
		*/









}
void checker() {

	while (radiosOn)
	{
		if (updated)
		{
			updated = false;

		}
		else
		{
			shutdownStreams();


		}
		
		Sleep(5000);
		
	}



}
/*
//-- Dynamic BASS.dll linking and initialization
typedef int(*BASS_Init_PTR)(int, DWORD, DWORD, HWND, const GUID*); BASS_Init_PTR bass_Init;
typedef int(*BASS_ErrorGetCode_PTR)(); BASS_ErrorGetCode_PTR bass_ErrorGetCode;
typedef HSTREAM(*BASS_StreamCreateURL_PTR)(const char* url, DWORD offset, DWORD flags, DOWNLOADPROC* proc, void* user); BASS_StreamCreateURL_PTR bass_StreamCreateURL;
typedef BOOL(*BASS_ChannelPlay_PTR)(DWORD handle, BOOL restart); BASS_ChannelPlay_PTR bass_ChannelPlay;
typedef BOOL(*BASS_StreamFree_PTR)(HSTREAM handle); BASS_StreamFree_PTR bass_StreamFree;
typedef BOOL(*BASS_ChannelSetAttribute_PTR)(DWORD handle, DWORD attrib, float value); BASS_ChannelSetAttribute_PTR bass_ChannelSetAttribute;
typedef BOOL(*BASS_ChannelSet3DPosition_PTR)(DWORD handle, BASS_3DVECTOR* pos, BASS_3DVECTOR* orient, BASS_3DVECTOR* vel); BASS_ChannelSet3DPosition_PTR bass_ChannelSet3DPosition;
typedef void(*BASS_Apply3D_PTR)(); BASS_Apply3D_PTR bass_Apply3D;
typedef void(*BASS_Set3DPosition_PTR)(BASS_3DVECTOR* pos, BASS_3DVECTOR* vel, BASS_3DVECTOR* front, BASS_3DVECTOR* top); BASS_Set3DPosition_PTR bass_Set3DPosition;
typedef BOOL(*BASS_Set3DFactors_PTR)(float distf, float rollf, float doppf); BASS_Set3DFactors_PTR bass_Set3DFactors;
typedef BOOL(*BASS_SetDevice_PTR)(DWORD device); BASS_SetDevice_PTR bass_SetDevice;
typedef BOOL(*BASS_Get3DPosition_PTR)(BASS_3DVECTOR* pos, BASS_3DVECTOR* vel, BASS_3DVECTOR* front, BASS_3DVECTOR* top); BASS_Get3DPosition_PTR bass_Get3DPosition;

HINSTANCE hInst;
void loadBass() {  //-- Call this function to initialize BASS and BASS functions
     
	std::cout << "initialising BASS\n";
	char path[MAX_PATH]=" ";
	wchar_t wpath[MAX_PATH];
	MultiByteToWideChar(0, 0, path,  MAX_PATH, wpath, MAX_PATH);
	LPWSTR lwpPath =wpath;
	 //char pathString[MAX_PATH];
	//32 bit
	//GetModuleFileName(GetModuleHandle(TEXT("jetRadioStream.dll")), lwpPath, MAX_PATH);
	 //64 Bit
	try {
		GetModuleFileName(GetModuleHandle(TEXT("jetRadioStream_x64.dll")), lwpPath, MAX_PATH);  //-- Assign current directory to path
		}
	catch(exception e)
	{

		return;
	}
	
	wstring tempDir =(lwpPath);  //-- Convert to string
	string dir = string (tempDir.begin(),tempDir.end());
	
	
	dir = dir.substr(0, dir.find_last_of("\\/"));  //-- Remove filename from directory
	//32bit
	//dir = dir + "\\bass.dll";  //-- And put bass.dll in it's place											   
 //64 bit
	dir = dir + "\\bass_x64.dll";  //-- And put bass.dll in it's place
	std::cout << "\n\n" + dir;
	wchar_t wdir[MAX_PATH];
	MultiByteToWideChar(0, 0, dir.c_str(), dir.length(), wdir, MAX_PATH);
	LPWSTR lpwDir = wdir;
	
	try {
		hInst = LoadLibrary(lpwDir);  //-- Load DLL file
	}
	catch (exception e)
	{

		return;
	}

	if (hInst != NULL)
	{

		std::cout << "FinishedSetting up strange vars\n";
		bass_Init = (BASS_Init_PTR)GetProcAddress(hInst, "BASS_Init");
		bass_ErrorGetCode = (BASS_ErrorGetCode_PTR)GetProcAddress(hInst, "BASS_ErrorGetCode");
		bass_StreamCreateURL = (BASS_StreamCreateURL_PTR)GetProcAddress(hInst, "BASS_StreamCreateURL");
		bass_ChannelPlay = (BASS_ChannelPlay_PTR)GetProcAddress(hInst, "BASS_ChannelPlay");
		bass_StreamFree = (BASS_StreamFree_PTR)GetProcAddress(hInst, "BASS_StreamFree");
		bass_ChannelSetAttribute = (BASS_ChannelSetAttribute_PTR)GetProcAddress(hInst, "BASS_ChannelSetAttribute");
		bass_ChannelSet3DPosition = (BASS_ChannelSet3DPosition_PTR)GetProcAddress(hInst, "BASS_ChannelSet3DPosition");
		bass_Apply3D = (BASS_Apply3D_PTR)GetProcAddress(hInst, "BASS_Apply3D");
		bass_Set3DPosition = (BASS_Set3DPosition_PTR)GetProcAddress(hInst, "BASS_Set3DPosition");
		bass_Set3DFactors = (BASS_Set3DFactors_PTR)GetProcAddress(hInst, "BASS_Set3DFactors");
		bass_SetDevice = (BASS_SetDevice_PTR)GetProcAddress(hInst, "BASS_SetDevice");
		bass_Get3DPosition = (BASS_Get3DPosition_PTR)GetProcAddress(hInst, "BASS_Get3DPosition");
		std::cout << "assigned bass functions\n";
		int bass = bass_Init(-1, 44100, BASS_DEVICE_3D, 0, NULL);
		//int bass = bass_Init(-1, 44100, BASS_DEVICE_3D, 0, NULL);
		std::cout << "bass init passed functions\n";
		bass_Set3DFactors(1, 1, 1);
		std::cout << "finished initialising BASS\n";


	}
	else
	{
		DWORD lastError = GetLastError();

	}
};
*/


//-- Dynamic BASS.dll linking and initialization
	typedef int(*BASS_Init_PTR)(int, DWORD, DWORD, HWND, const GUID*); BASS_Init_PTR bass_Init;
	typedef int(*BASS_ErrorGetCode_PTR)(); BASS_ErrorGetCode_PTR bass_ErrorGetCode;
	typedef HSTREAM(*BASS_StreamCreateURL_PTR)(const char *url, DWORD offset, DWORD flags, DOWNLOADPROC *proc, void *user); BASS_StreamCreateURL_PTR bass_StreamCreateURL;
	typedef BOOL(*BASS_ChannelPlay_PTR)(DWORD handle, BOOL restart); BASS_ChannelPlay_PTR bass_ChannelPlay;
	typedef BOOL(*BASS_StreamFree_PTR)(HSTREAM handle); BASS_StreamFree_PTR bass_StreamFree;
	typedef BOOL(*BASS_ChannelSetAttribute_PTR)(DWORD handle, DWORD attrib, float value); BASS_ChannelSetAttribute_PTR bass_ChannelSetAttribute;
	typedef BOOL(*BASS_ChannelSet3DPosition_PTR)(DWORD handle, BASS_3DVECTOR *pos, BASS_3DVECTOR *orient, BASS_3DVECTOR *vel); BASS_ChannelSet3DPosition_PTR 
	bass_ChannelSet3DPosition;
	typedef void(*BASS_Apply3D_PTR)(); BASS_Apply3D_PTR bass_Apply3D;
	typedef void(*BASS_Set3DPosition_PTR)(BASS_3DVECTOR *pos, BASS_3DVECTOR *vel, BASS_3DVECTOR *front, BASS_3DVECTOR *top); BASS_Set3DPosition_PTR bass_Set3DPosition;
	typedef BOOL(*BASS_Set3DFactors_PTR)(float distf, float rollf, float doppf); BASS_Set3DFactors_PTR bass_Set3DFactors;
	typedef BOOL(*BASS_SetDevice_PTR)(DWORD device); BASS_SetDevice_PTR bass_SetDevice;
	typedef BOOL(*BASS_Get3DPosition_PTR)(BASS_3DVECTOR *pos, BASS_3DVECTOR *vel, BASS_3DVECTOR *front, BASS_3DVECTOR *top); BASS_Get3DPosition_PTR bass_Get3DPosition;
	HINSTANCE hInst;
	void loadBass() {  //-- Call this function to initialize BASS and BASS functions
		char path[MAX_PATH];
		//64bit
		GetModuleFileName( GetModuleHandle("jetRadioStream_x64.dll"), path, MAX_PATH );  //-- Assign current directory to path
		//32bit
		//GetModuleFileName(GetModuleHandle("jetRadioStream.dll"), path, MAX_PATH);  //-- Assign current directory to path
		string dir = path;  //-- Convert to string
		dir = dir.substr(0, dir.find_last_of("\\/"));  //-- Remove filename from directory
		//64bit
		dir = dir + "\\bass_x64.dll";  //-- And put bass.dll in it's place
		//32bit
		//dir = dir + "\\bass.dll";  //-- And put bass.dll in it's place
		hInst = LoadLibrary(TEXT(dir.c_str()));  //-- Load DLL file
		

		hInst = LoadLibrary(TEXT(dir.c_str()));  //-- Load DLL file
		bass_Init = (BASS_Init_PTR)GetProcAddress(hInst, "BASS_Init");
		bass_ErrorGetCode = (BASS_ErrorGetCode_PTR)GetProcAddress(hInst, "BASS_ErrorGetCode");
		bass_StreamCreateURL = (BASS_StreamCreateURL_PTR)GetProcAddress(hInst, "BASS_StreamCreateURL");
		bass_ChannelPlay = (BASS_ChannelPlay_PTR)GetProcAddress(hInst, "BASS_ChannelPlay");
		bass_StreamFree = (BASS_StreamFree_PTR)GetProcAddress(hInst, "BASS_StreamFree");
		bass_ChannelSetAttribute = (BASS_ChannelSetAttribute_PTR)GetProcAddress(hInst, "BASS_ChannelSetAttribute");
		bass_ChannelSet3DPosition = (BASS_ChannelSet3DPosition_PTR)GetProcAddress(hInst, "BASS_ChannelSet3DPosition");
		bass_Apply3D = (BASS_Apply3D_PTR)GetProcAddress(hInst, "BASS_Apply3D");
		bass_Set3DPosition = (BASS_Set3DPosition_PTR)GetProcAddress(hInst, "BASS_Set3DPosition");
		bass_Set3DFactors = (BASS_Set3DFactors_PTR)GetProcAddress(hInst, "BASS_Set3DFactors");
		bass_SetDevice = (BASS_SetDevice_PTR)GetProcAddress(hInst, "BASS_SetDevice");
		bass_Get3DPosition = (BASS_Get3DPosition_PTR)GetProcAddress(hInst, "BASS_Get3DPosition");
		int bass = bass_Init(-1, 44100, BASS_DEVICE_3D, 0, NULL);
		bass_Set3DFactors(1, 1, 1);
	};


	


































string dumpArrays(vector<Pos> Poses, vector<string> IDs, vector<Orient> Orients, vector<bool> Enabled, vector <HSTREAM> Streams, vector<float> Volumes)
{


	string outputString, posString, idString, orientString, enabledString, streamString, volumeString = "";
	posString = "Positions Array (len: " + to_string(Poses.size()) + "):    ";
	idString = "IDs Array(len: " + to_string(IDs.size()) + "):    ";
	orientString = "Orients Array(len: " + to_string(Orients.size()) + "):    ";
	enabledString = "Enabled Array(len: " + to_string(Enabled.size()) + "):    ";
	streamString = "Streams Array(len: " + to_string(Streams.size()) + "):    ";
	volumeString = "Volumes Array(len: " + to_string(Volumes.size()) + "):    ";
	if ((Poses.size() == IDs.size()) && (IDs.size() == Orients.size()) && (Orients.size() == Enabled.size()) && (Enabled.size() == Streams.size()) && (Streams.size() == Volumes.size()))
	{
		for (int i = 0; i < Poses.size(); i++)
		{

			posString = posString + to_string(Poses[i].x) + "," + to_string(Poses[i].y) + "," + to_string(Poses[i].z) + "|";
			idString = idString + IDs[i] + "|";
			orientString = orientString + to_string(Orients[i].dirx) + "," + to_string(Orients[i].diry) + "," + to_string(Orients[i].dirz) + "," + to_string(Orients[i].upx) + "," + to_string(Orients[i].upy) + "," + to_string(Orients[i].upz) + "|";
			enabledString = enabledString + to_string(Enabled[i]) + "|";
			streamString = enabledString + to_string(streamString[i]) + "|";
			volumeString = volumeString + to_string(Volumes[i]) + "|";
		}

		outputString = "\n\n" + posString + "\n"+  idString+ "\n" + orientString + "\n" + enabledString + "\n" + streamString + "\n" + posString + "\n" + volumeString;

		std::cout << outputString;

	}
	else
	{
		outputString = "Error! Arrays are not all the same size!";

	}
	return outputString;



}

void streamCreate(int streamID, string streamName, string streamURL, Pos radioPos, Orient orientToRadio, float volume,int threadID)
{

	
	HSTREAM stream = bass_StreamCreateURL(streamURL.c_str(), 1, BASS_SAMPLE_MONO | BASS_SAMPLE_3D, NULL, 0);
	
	mtx.lock();
	vector<string> streamNames = radiosID;
	vector<string>::iterator streamI = find(streamNames.begin(), streamNames.end(), streamName);

	if (streamI != streamNames.end())
	{
		int idToReplace = streamI - streamNames.begin();
		radiosStream.at(idToReplace) = stream;
	}

	mtx.unlock();



	

	//INIT Variables to be used by BASS
	BASS_3DVECTOR position = { radioPos.x,radioPos.y,radioPos.z };
	BASS_3DVECTOR* positions = &position;
	BASS_3DVECTOR zeros = { 0,0,0 };
	BASS_3DVECTOR* zero = &zeros;
	BASS_3DVECTOR dir = { orientToRadio.dirx, orientToRadio.diry ,orientToRadio.dirz };
	BASS_3DVECTOR* dirVector = &dir;
	BASS_3DVECTOR up = { orientToRadio.upx,orientToRadio.upy,orientToRadio.upz };
	BASS_3DVECTOR* upVector = &up;
	const char* url = streamURL.c_str();
	//DONE with INIT

	
	//Sets position
	bass_Set3DPosition(zero, zero, dirVector, upVector);

	bool test = bass_ChannelSet3DPosition(stream, positions, zero, zero);

	bass_Apply3D();

	//Plays the Channel
	HCHANNEL channel = bass_ChannelPlay(stream, false);
	bass_ChannelSetAttribute(stream, BASS_ATTRIB_VOL, volume);
	bool thisRadioEnabled = true;
	bool isEnabled=true;
	
	while ((radiosOn) && (thisRadioEnabled))

	{ 
		if (!ioLock)
		{
			
			mtx.lock();
			vector <int> deleteArray = markedThreads;
			
			vector<int>::iterator streamI = find(deleteArray.begin(), deleteArray.end(), threadID);
			std::cout << streamName +" :"+to_string(threadID)+"\n";
			int biggestID = 0;
			int biggestIDi = 0;
		

			if (streamI != deleteArray.end())
			{

			
					vector<int>::iterator streamI = find(deleteArray.begin(), deleteArray.end(), threadID);
					int idToStop = streamI - deleteArray.begin();
					std::cout << "deleting this thread: " + to_string(idToStop);
					if (streamI != deleteArray.end())
					{
						std::cout << " deleting this thread: " + to_string(idToStop)+"\n";
						markedThreads.erase(markedThreads.begin() + idToStop);
					}

				




				mtx.unlock();
				thisRadioEnabled = false;

			}
			else
			{
				mtx.unlock();
				thisRadioEnabled = isEnabled;
			}
			vector <int> dumpArray = deleteArray;
			std::cout << "threadID:" + to_string(threadID) + " at index: " + to_string(streamI - deleteArray.begin())+"\n";
			for (int i = 0; i < dumpArray.size(); i++)
			{
				std::cout << to_string(i) + ": " + to_string(dumpArray.at(i)) + "\n";

			}
		


		}
		Sleep(50);

		

		}

	bass_StreamFree(stream);
	
		

}

//Update Functions
void updatePos(int streamID, Pos radioPos, Orient orientToRadio)
{


	updated = true;
	int id = streamID;
	mtx.lock();
	radiosPos.at(id) = radioPos;
	radiosOrient.at(id) = orientToRadio;
	HSTREAM stream = radiosStream.at(id);
	BASS_3DVECTOR position = { radioPos.x,radioPos.y,radioPos.z };
	BASS_3DVECTOR* positions = &position;
	BASS_3DVECTOR zeros = { 0,0,0 };
	BASS_3DVECTOR* zero = &zeros;
	BASS_3DVECTOR dir = { orientToRadio.dirx, orientToRadio.diry ,orientToRadio.dirz };
	BASS_3DVECTOR* dirVector = &dir;
	BASS_3DVECTOR up = { orientToRadio.upx,orientToRadio.upy,orientToRadio.upz };
	BASS_3DVECTOR* upVector = &up;
	

	mtx.unlock();



	bass_Set3DPosition(zero, zero, dirVector, upVector);
	bool test = bass_ChannelSet3DPosition(stream, positions, zero, zero);
	bass_Apply3D();





}

void updateVolume(string streamName, float newVolume)
{

	mtx.lock();
	vector <string> idList = radiosID;
	mtx.unlock();

	vector<string>::iterator streamI = find(idList.begin(), idList.end(), streamName);
	if (streamI != idList.end())
	{
		int id = streamI - idList.begin();
		std::cout << "Element found in myvector: " << id << '\n';



		mtx.lock();
		HSTREAM stream = radiosStream.at(id);
		mtx.unlock();
		bass_ChannelSetAttribute(stream, BASS_ATTRIB_VOL, newVolume);

	}
	else
	{
		std::cout << "Cant find a Stream with that name/id";
	}




}








//MAIN BODY

extern "C"
{
	__declspec (dllexport) void __stdcall RVExtensionVersion(char* output, int outputSize);
	__declspec (dllexport) int __stdcall RVExtensionArgs(char* output, int outputSize, const char* function, const char** argv, int argc);

}




void __stdcall RVExtensionVersion(char* output, int outputSize) {

	loadBass();
	std::cout << "finished initialising BASS\n";
 }



int __stdcall RVExtensionArgs(char* output, int outputSize, const char* function, const char** argv, int argc)
{


	//function name
	//
	// stream init
	// stream start
	//stream stop
	//stream volume
	//stream update

	// ARGUMENT STRUCTURE [streamID,soundPosition,playerOrientation,streamURL,volume]

	///INITIALISATION OF THE ARGS PASSED THROUGH THE EXTENSION
	string streamID = argv[0];
	string soundPositionTemp = argv[1];
	string playerOrientationTemp = argv[2];
	string streamURL = argv[3];
	string newVolume = (argv[4]);
	float volume = atof(newVolume.c_str());

	//parsing args from the function call
	Pos newPosRadio;
	Orient newOrientToRadio;

	streamURL = streamURL.substr(1, streamURL.length() - 1);
	streamURL = streamURL.substr(0, streamURL.length() - 1);

	vector<string> soundPosition = stringToArray(soundPositionTemp);
	newPosRadio.x = -atof(soundPosition.at(0).c_str());
	newPosRadio.y = atof(soundPosition.at(1).c_str());
	newPosRadio.z = atof(soundPosition.at(2).c_str());

	vector<string> DirUpVectors = stringToArray(playerOrientationTemp);
	newOrientToRadio.dirx = -atof(DirUpVectors.at(0).c_str());
	newOrientToRadio.diry = atof(DirUpVectors.at(1).c_str());
	newOrientToRadio.dirz = atof(DirUpVectors.at(2).c_str());
	newOrientToRadio.upx = -atof(DirUpVectors.at(3).c_str());
	newOrientToRadio.upy = atof(DirUpVectors.at(4).c_str());
	newOrientToRadio.upz = atof(DirUpVectors.at(5).c_str());
	std::cout << "everytihg fine with var parsing\n";
	//The data has been parsed and is now ready to be sent to a function.

	
if (!strncmp(function, "init", 4)) // detect checking for result
	{
		
		std::cout << "got into init function";
		loadBass();
		std::cout << "init bass";
		bass_Set3DFactors(1, 1, 1);
		std::cout << "init 3d factors";
		strncpy_s(output, outputSize, "Initialised correctly!", _TRUNCATE); //returns confirmation that everything ran ok (probably)
		mtx.lock();
		radiosOn = true; //disable this and all radios turn off!
		mtx.unlock();
		updated = true;
		thread checkerThread(checker);
		checkerThread.detach();
		std::cout << "Version 2.0";
	}
	else if (!strncmp(function, "start", 5))
	{
		vector<string >currIDList;
		vector<string>::iterator  endElement;
		vector<string>::iterator streamI;

		if ((!ioLock))
		{
									 
			
			//find duplicates


			mtx.lock();
			currIDList = radiosID;

			mtx.unlock();
	



			streamI = find(currIDList.begin(), currIDList.end(), streamID);
			if (streamI != currIDList.end())
			{
				string debugString = "A Stream with this ID Already exists!! Try another!";
				strncpy_s(output, outputSize, debugString.c_str(), _TRUNCATE);


			}
			else
			{
				ioLock = true;
				const char* url = streamURL.c_str();
				//HSTREAM   = BASS_Mixer_StreamCreate(44100, 1, BASS_SAMPLE_3D);

				HSTREAM stream = bass_StreamCreateURL("", 1, BASS_SAMPLE_MONO | BASS_SAMPLE_3D, NULL, 0);

				mtx.lock();
				//adds Entries to the soon to be created audio stream.
				radiosPos.push_back(newPosRadio);
				radiosID.push_back(streamID);
				radiosOrient.push_back(newOrientToRadio);
				radiosEnabled.push_back(true);
				radiosVolume.push_back(volume);
				radiosStream.push_back(stream);
				int streamIntID = radiosID.size() - 1; //converts the  "ID" into an iterator so that we can use it to identify things like its name/ID , position, etc..
				threadCount = threadCount+1;
				
				int threadID=threadCount;
				radiosThreadID.push_back(threadCount);
				mtx.unlock();
				std::cout <<to_string(threadID) + " is the id of thread \n";
				thread spawnStream(streamCreate, streamIntID, streamID, streamURL, newPosRadio, newOrientToRadio, volume,threadID);

				//spawnStream.get_id();
				spawnStream.detach(); // start parallel process
				ioLock = false;
				std::cout << " detached ioLock";
				strncpy_s(output, outputSize, (to_string(streamIntID) + "| " + streamID).c_str(), _TRUNCATE); // ticket n


			}














		}
		else
		{
			cout << "yikes";
			strncpy_s(output, outputSize, "ioLock!", _TRUNCATE);

		}
	}
	else if (!strncmp(function, "stop", 4)) // detect checking for result
	{

		mtx.lock();
		vector<string> idList = radiosID;
		mtx.unlock();


		if (!ioLock)
		{
			//finds if the given ID exsits
			std::cout << "stop is stuck at: ioLock";
			vector<string>::iterator streamI = find(idList.begin(), idList.end(), streamID);
			if (streamI != idList.end())
			{

				ioLock = true;
				int idToStop = streamI - idList.begin();
				deleteEntries(idToStop);
				/*
				mtx.lock();
				std::cout << "stop is stuck at: before delete";
				//BASS_StreamFree(radiosStream.at(idToStop));

				toBeDeleted= (radiosID.at(idToStop ) );
				radiosPos.erase(radiosPos.begin() + idToStop);
				radiosOrient.erase(radiosOrient.begin() + idToStop);
				radiosEnabled.erase(radiosEnabled.begin() + idToStop);
				radiosStream.erase(radiosStream.begin() + idToStop);
				radiosID.erase(radiosID.begin() + idToStop);
				radiosVolume.erase(radiosVolume.begin() + idToStop);
				//std::cout << dumpArrays(radiosPos, radiosID, radiosOrient, radiosEnabled, radiosStream, radiosVolume);
				
				markedThreads.push_back(radiosThreadID.at(idToStop));
				radiosThreadID.erase(radiosThreadID.begin() + idToStop);
				
				mtx.unlock();*/
				std::cout << "stop is stuck at: after delete";
				
				ioLock = false;

				string debugString = ("STOPPING STREAM: " + to_string(idToStop) + ", " + streamID);
				strncpy_s(output, outputSize, debugString.c_str(), _TRUNCATE);

			}
			else
			{

				string debugString;
				mtx.lock();
				debugString = dumpArrays(radiosPos, radiosID, radiosOrient, radiosEnabled, radiosStream, radiosVolume);
				mtx.unlock();

				debugString = "Couldn't find a stream with the ID specified  " + debugString;
				strncpy_s(output, outputSize, debugString.c_str(), _TRUNCATE); // result is not ready
			}


		}


	}
	else if (!strncmp(function, "update", 6))
	{

		if (!ioLock)
		{
			mtx.lock();

			vector <string> idList = radiosID;
			mtx.unlock();



			vector<string>::iterator streamI = find(idList.begin(), idList.end(), streamID);
			if (streamI != idList.end())
			{
				mtx.lock();
				int id = streamI - idList.begin();
				mtx.unlock();
				updatePos(id, newPosRadio, newOrientToRadio); //updates position with BASS, sends back confirmation

				string debugString;
				mtx.lock();
				debugString = dumpArrays(radiosPos, radiosID, radiosOrient, radiosEnabled, radiosStream, radiosVolume);
				mtx.unlock();
				debugString = "position update sent.. Additional Debug info: " + debugString;


				strncpy_s(output, outputSize, debugString.c_str(), _TRUNCATE);
			}

		}
		else
		{


			string debugString;
			mtx.lock();
			debugString = dumpArrays(radiosPos, radiosID, radiosOrient, radiosEnabled, radiosStream, radiosVolume);
			mtx.unlock();
			debugString = "could not find stream with that ID" + debugString;
			strncpy_s(output, outputSize, debugString.c_str(), _TRUNCATE);
			std::cout << dumpArrays(radiosPos, radiosID, radiosOrient, radiosEnabled, radiosStream, radiosVolume);
		}
	}
	else if (!strncmp(function, "volume", 6))
	{

		updateVolume(streamID, volume); //updates volume with BASS and sends back confirmation
		strncpy_s(output, outputSize, "volume update sent", _TRUNCATE);

	}
	else if (!strncmp(function, "shutdown", 8))
	{

	thread shutdownThread(shutdownStreams);
	shutdownThread.detach();
	


	}
	else
	{
	strncpy_s(output, outputSize, "INVALID COMMAND", _TRUNCATE); // other input
	}


}
