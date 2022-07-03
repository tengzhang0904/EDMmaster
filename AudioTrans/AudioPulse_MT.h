//Functional call to AudioPulse

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

void AudioDoppio(unsigned int audioid)
{   

    const string audiofolder=MMCfolder+"AudioTrans/";
    const string AudioDoppioExcelFile=audiofolder+"AudioPulse.xls";
    const string audiodoppiologfile=audiofolder+"Magdatalog.txt"; //default logfile name
  
    //Audio 2 frequency transtion waveform test code
	MagPulse *MagDuopulse=new MagPulse();
	MagDuopulse->init();
	MagDuopulse->ExcelRead(AudioDoppioExcelFile, audioid+1); //start with audioid=1, first effective row
	MagDuopulse->Calculate();
	MagDuopulse->WriteFile(audiodoppiologfile);
	MagDuopulse->AudioDoppioExe(30.0);
	delete MagDuopulse;
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

void AudioDportPol(int Pol)//update 8 DO channels once on the AO card based on the user defined polarity
{   
	const string audiofolder=MMCfolder+"AudioTrans/";
    const string DportParaExcelFile=audiofolder+"DigitalLines.xls";

	MMCDigit8 *Dport=new MMCDigit8();
	Dport->updateDigit8(Pol);
	//Dport->ReadExcel(DportParaExcelFile);

	Dport->updateDigPort();
	delete Dport;
}


#endif

