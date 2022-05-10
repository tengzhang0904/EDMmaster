// Functional call to AudioPulse
// Last modified by Teng 12/23/2017

#ifndef _AudioPulse_MT_h_included_
#define _AudioPulse_MT_h_included_
#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <math.h>
#include <iostream>
#include <string>
#include <windows.h>
#include <time.h>
using namespace std;

#include "AudioPulse.h"

const extern string MMCfolder; //MMC_SYSINT

void Audio(unsigned int audioid)
{   

    const string audiofolder=MMCfolder+"AudioTrans/";
    const string AudioPulseParaExcelFile=audiofolder+"AudioPulse.xls";
    const string audiologfile=audiofolder+"Audiodatalog.txt"; //default logfile name
  
    //Audio transtion waveform test code
	AudioPulse *LFpulse=new AudioPulse();
	LFpulse->init();
	LFpulse->ReadExcel(AudioPulseParaExcelFile, audioid+1); //start with audioid=1, first effective row
	LFpulse->Calculate();
	LFpulse->WriteWaveformFile(audiologfile);
	LFpulse->AudioExe(true, 10.0);
	delete LFpulse;
}

void AudioDport()//update 8 DO channels once on the AO card
{   
	const string audiofolder=MMCfolder+"AudioTrans/";
    const string DportParaExcelFile=audiofolder+"DigitalLines.xls";
	MMCDigit8 *Dport=new MMCDigit8();
	Dport->ReadExcel(DportParaExcelFile);

	Dport->updateDigPort();
	delete Dport;
}

void AudioDportZero()
{
	MMCDigit8 *Dport = new MMCDigit8();
	Dport->updateDigPort(); // update all the channels to zero by default initialization

	delete Dport;
}

#endif

