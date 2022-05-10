//implementation of SpinPack
//note this must be done in a separate fiel to avoid LINK 2005 errors

#include "stdafx.h"
#include "SpinPackSWP.h"


//init all data memebers
void SpinPack::init() 
{
	ACTSTEP=SWPSTPMAX;
	for (int i=0; i<SWPSTPMAX;i++)
	{
		for(int j=0; j<VertSubSec*2+1;j++)
		{
			Spin[i][j]=0.0;
		}
	}
	for (int i=0; i<3;i++)
	{
		for(int j=0; j<VertSubSec*2;j++)
		{
			LinearCoef[i][j]=0.0;
		}
	}
    for (int i=0; i<SWPSTPMAX-1;i++)
	{
		for(int j=0; j<VertSubSec*2+1;j++)
		{
			dSdB[i][j]=0.0;
		}
	}
	for (int i=0; i<4;i++)
	{
		for(int j=0; j<VertSubSec*2;j++)
		{
			QuadCoef[i][j]=0.0;
		}
	}
	for (int i=0; i<4;i++)
	{
		for(int j=0; j<VertSubSec*2;j++)
		{
			SineCoef[i][j]=0.0;
		}
	}

	//Gradient data
	for (int i=0; i<SWPSTPMAX;i++)
	{
		for(int j=0; j<3;j++)
		{
			Gradient[i][j]=0.0;
		}
	}
	for (int i=0; i<3;i++)
	{
		for(int j=0; j<2;j++)
		{
			GradLinCoef[i][j]=0.0;
		}
	}
}


//read all data members from a TXT file for bias scans
bool SpinPack::ReadSpinPackTXT(string filename)
{
	bool flag=false; 
	ifstream datalog;
    datalog.open (filename.c_str(), ios::in); 
	flag=!(datalog.fail()); //check if file is open
 if (flag)
 {
	datalog>>ACTSTEP; 
	for (int i=0; i<ACTSTEP;i++)
	{
		for(int j=0; j<VertSubSec*2+1;j++)
		{
			datalog>>Spin[i][j];
		}
	}
	for (int i=0; i<3;i++)
	{
		for(int j=0; j<VertSubSec*2;j++)
		{
			datalog>>LinearCoef[i][j];
		}
	}
    for (int i=0; i<ACTSTEP-1;i++)
	{
		for(int j=0; j<VertSubSec*2+1;j++)
		{
			datalog>>dSdB[i][j];
		}
	}
	for (int i=0; i<4;i++)
	{
		for(int j=0; j<VertSubSec*2;j++)
		{
			datalog>>QuadCoef[i][j];
		}
	}
	for (int i=0; i<4;i++)
	{
		for(int j=0; j<VertSubSec*2;j++)
		{
			datalog>>SineCoef[i][j];
		}
	}
 }
	datalog.close();
	return flag;
}

//read all data members from a TXT file for graident scans
bool SpinPack::ReadGradPackTXT(string filename)
{
	bool flag=false; 
	ifstream datalog;
    datalog.open (filename.c_str(), ios::in); 
	flag=!(datalog.fail()); //check if file is open
 if (flag)
 {
	datalog>>ACTSTEP; 
	for (int i=0; i<ACTSTEP;i++)
	{
		for(int j=0; j<VertSubSec*2+1;j++)
		{
			datalog>>Spin[i][j];
		}
	}
	for (int i=0; i<ACTSTEP;i++)
	{
		for(int j=0; j<3;j++)
		{
			datalog>>Gradient[i][j];
		}
	}
	for (int i=0; i<3;i++)
	{
		for(int j=0; j<2;j++)
		{
			datalog>>GradLinCoef[i][j];
		}
	}
 }
	datalog.close();
	return flag;
}