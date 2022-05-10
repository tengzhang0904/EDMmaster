//multi-location spin pack for a scan: parent class
#pragma once

#ifndef _SpinPackSWP_h_included_
#define _SpinPackSWP_h_included_
#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <math.h>
#include <iomanip>
#include <string>
#include <windows.h> 
#include <cstdlib>
#include <iostream>
#include <fstream>
#include "CONST.h"
using namespace std;
using namespace DATACONST;

//class definition of a spin pack: parent class
///////////////////////////////////////////////////////////////////////////////////////////////////////////
class SpinPack
{
  public:

   //common definitions
   int ACTSTEP; //actual number of data points during a sweep
   double Spin[SWPSTPMAX][VertSubSec*2+1]; //array to store average spin for a sweep

   //Data memebers and methods for bias scans
   double LinearCoef[3][VertSubSec*2]; //linear fitting coefficients and Zero Crossings
   double dSdB[SWPSTPMAX-1][VertSubSec*2+1]; //derivative of Spin[][]
   double QuadCoef[4][VertSubSec*2]; //quad fitting coefficients and extrema location
   double SineCoef[4][VertSubSec*2]; //constrained sine fit vs precession time (ms) and B field
   virtual bool ReadSpinPackTXT(string filename) final; //read all data members from a TXT file for bias scans

   //Data memebers and methods for gradient scans
   double Gradient[SWPSTPMAX][3]; //array to store 2 gradients during a gradient scan
   double GradLinCoef[3][2]; //linear fitting coefficients for gradients zero crossing
   virtual bool ReadGradPackTXT(string filename) final; //read all data members from a TXT file for Grad scans

   virtual void init(); //init all data memebers

};


#endif