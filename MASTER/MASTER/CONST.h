//Specifying data set global consts
//used at multiple places
#pragma once

#ifndef _CONST_h_included_
#define _CONST_h_included_
#define _CRT_SECURE_NO_DEPRECATE

namespace DATACONST
{   
	//universal math const
	const double TPI=6.28318;
	const double ZEEMAN=0.35; // 0.35MHz/G

	/*************************************************************************************
	/* PDA DAQ and plot settings:
	/* 7-pulse detection, set PDANshot=8 and BKGST=7;
	/* single pulse detection, set PDANshot=2 and BKGST=1;
	**************************************************************************************/   
	const unsigned short PDANpixel =50, PDANshot = 8, PDANofst=25; //PDA data size,  50 point each shot
	const unsigned short BKGST =7; //background shot id (YAG)  

	const unsigned short AMNumChls=16,AMNumShots=5;  //Analog monitors channels and shots

	const unsigned short samplesize=60;


	/**************************************************************************************
	/* Magnetometry and EDM SCAN pixel grouping information
	**************************************************************************************/ 
	const unsigned short SWPSTPMAX=20; //max number of sweep steps
	const unsigned short VertSubSec=3; //number of subsections along the atom array
	const int mF[7]={3, 2, 1, 0, -1, -2, -3};//Zeeman sublevel detection order
	
	const unsigned int EDMinfocnt=3; //num. of EDM info counts: set 2 for m=0 population, asymmetry and other info
	const unsigned int EDMGrpAvg=3; //num. of total groups for each lattice side
	const unsigned int EDMGrpIdxRange[2]={3, 21}; //Index truncation range for group average, choose from 0~24 
	

	/**************************************************************************************
	/* Automated EDM measurements constant settings
	**************************************************************************************/ 
	const unsigned int DialogWaitmseconds=5000; //MFC dialog auto pop up and wait time in milliseconds
	const unsigned int MAG_SCAN_POINTS=10; //Number of steps during a single magnetometry scan

}

#endif