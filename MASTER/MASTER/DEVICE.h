//Include header files for all devices and tasks and global definitions
//used at multiple places
#pragma once

#ifndef _DEVICE_h_included_
#define _DEVICE_h_included_
#define _CRT_SECURE_NO_DEPRECATE

namespace DEVICEHEADER
{   
	//Main folder contains all subprograms
    const extern string MMCfolder("C:/MMC_SYSINT/");

    //Include header files for all devices and tasks and global definitions
    #include "WinSock/MMCS_MT.h"
    #include "AudioTrans/AudioPulse_MT.h"
    #include "DDS/DDS_MT.h"
    #include "HV DIVIDER/HVDivider_MT.h"
    #include "MonitorsAI/MonitorsAI.h"
    #include "PDA/PDA.h"
    #include "CurrentSource/CurrentSource_MT.h"
    #include "Pause Timing/TimingPause_MT.h"

}

#endif