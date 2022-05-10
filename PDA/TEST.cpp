//windows and NI related headers and libraries
#include <iostream>
#include <iomanip>
#include <string>
#include <windows.h> 
#include <cstdlib>
#include <stdio.h>                    
#include <stdlib.h>                  
#include <fstream> 
#include <math.h>
#include "PDA.h"
using namespace std;

void main(void)
{
	PDA *test=new PDA();
	test->Acquire(10);
	//test->GenerateTestData();
	const string PDA_Data="PDA_Data.txt"; //PDA DATA FILENAME
    test->WriteDataFile(PDA_Data);
	delete test;
}