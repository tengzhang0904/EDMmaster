#ifndef _DATASET_h_included_
#define _DATASET_h_included_
#define _CRT_SECURE_NO_DEPRECATE

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <windows.h> 
#include <math.h>
using namespace std;

#include "Excel_Link/libxl.h" //excel linking lib
#pragma comment(lib, "libxl.lib")
using namespace libxl;

#define LENGTH 30
#define WIDTH 2

class DATASET
{
  public: 
     double data[WIDTH][LENGTH];
	 void init();
	 int getLength() {return LENGTH;}
	 int getWidth() { return WIDTH;}
	 bool LoadExcel(string filename);
};

void DATASET::init()
{
	for (int i=0;i<WIDTH;i++)
		for (int j=0;j<LENGTH;j++)
		{
             data[i][j]=0.0;
		}
}

bool DATASET::LoadExcel(string filename)
{
	Book* book = xlCreateBook();
   if(book->load(filename.c_str())) //open the excel book
   {
      Sheet* sheet = book->getSheet(0); //get 1st sheet fromt the excel book
      if(sheet)
      {     
		  for (int i=0;i<WIDTH;i++)
		     for (int j=0;j<LENGTH;j++)
		        {
                    data[i][j]=sheet->readNum(j+1, i); //first row is text
		        }
      }
   }
   book->release();
   return true;
}

#endif