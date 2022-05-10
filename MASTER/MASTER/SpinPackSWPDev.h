//multi-location spin pack for a scan: child calss
//add calculation methods that can be used in MasterDlg, not not in sub-dialogs due to LGLIB package restriction
#pragma once

#ifndef _SpinPackSWPDev_h_included_
#define _SpinPackSWPDev_h_included_
#define _CRT_SECURE_NO_DEPRECATE

#include "SpinPackSWP.h" //parent class for SpinPack
#include "ALGLIB/NLSF_MT.h"//Mathematical operations: integration, nonlinear fit, etc.

//class definition of a spin pack: child class
///////////////////////////////////////////////////////////////////////////////////////////////////////////
class SpinPackDev : public SpinPack
{
  public:
	 //extended methods for bias scans
	 bool ReadSpinTXT(string filename); //read average Spin from a TXT file
	 bool WriteSpinTXT(string filename); //write average Spin to a TXT file
	 void FitLinear(); //linear fitting of Spin[][] and set LinearCoef[][]
     void CalculateDSDB(); //calculate first order derivative dS/dB
	 void FitQuad();  //quad fitting of dSdB[][] and set QuadCoef[][]
	 void FitSine(); //constrained sine fit for precession S(t[ms]) and set SineCoef[][]
     bool WriteSpinPackTXT(string filename); //write all data members to a TXT file

	 //extended methods for gradient scans
     void CalculateGrad(); //calculate gradients for a grad scan
	 void GradFit(); //linear fitting of Gradient[][] and set GradLinCoef[][]
     bool WriteGradPackTXT(string filename); //write all data members to a TXT file for a grad scan
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////
//constrained sine fit for precession S(t[ms]) and set SineCoef[][]
void SpinPackDev::FitSine()  
{
	    //fitting definitions, see ALGLIB function documentation
        double epsf = 0;
        double epsx = 1.0E-9;
        ae_int_t maxits = 1000000;
        ae_int_t info;
        lsfitstate state;
        lsfitreport rep;
		ae_int_t n=ACTSTEP; //data array length
	    ae_int_t m=3; //total number of function parameters
		real_2d_array x; x.setlength(n, 1); // x values
		real_1d_array y; y.setlength(n); // y values
		real_1d_array c; c.setlength(m); //Guess parameters

		for(int k=0; k<VertSubSec*2; k++) //loop for 6 locations
		{
	        //copy data to ALGLIB arrays
	        for (int i=0; i<n; i++)  
		    {
			   x[i][0]=Spin[i][VertSubSec*2]*0.001; //convert times to seconds
	           y[i]=Spin[i][k];
		    }

			//run fitting function
		    c[0]=2.0; c[1]=1.0/(x[n-1][0]-x[0][0]); c[2]=0.0;//initial guess parameters
            lsfitcreatef(x, y, c, true, state);  //use ALGLIB F-mode, no Jacobian
            lsfitsetcond(state, epsf, epsx, maxits);
            alglib::lsfitfit(state, ConstrSine_func); //choose various fitting functions w/ Jacobian fitting
	        lsfitresults(state, info, c, rep); //get fitting results stored in c[]

			//transfer fitting parameters
			SineCoef[0][k]=c[0];  //Amplitude
			SineCoef[1][k]=c[1];  //Frequency in Hz
			SineCoef[2][k]=c[2];  //Phase offset
			SineCoef[3][k]=-(c[0]/3.0)*(c[1]/ZEEMAN)*cos(c[2]); //B in microGauss
		}
}

//quad fitting of dSdB[][] and set QuadCoef[][]
void SpinPackDev::FitQuad()  
{
	    //fitting definitions, see ALGLIB function documentation
        double epsf = 0;
        double epsx = 1.0E-9;
        ae_int_t maxits = 1000000;
        ae_int_t info;
        lsfitstate state;
        lsfitreport rep;
		ae_int_t n=ACTSTEP-1; //dS/dB data array length
	    ae_int_t m=3; //total number of function parameters
		real_2d_array x; x.setlength(n, 1); // x values
		real_1d_array y; y.setlength(n); // y values
		real_1d_array c; c.setlength(m); //Guess parameters

		for(int k=0; k<VertSubSec*2; k++) //loop for 6 locations
		{
	        //copy data to ALGLIB arrays
	        for (int i=0; i<n; i++)  
		    {
			   x[i][0]=dSdB[i][VertSubSec*2]; //step value for as last element
	           y[i]=dSdB[i][k];
		    }

			//run fitting function
		    c[0]=1.0; c[1]=0.5; c[2]=0.0; //initial guess parameters
            lsfitcreatef(x, y, c, true, state);  //use ALGLIB F-mode, no Jacobian
            lsfitsetcond(state, epsf, epsx, maxits);
            alglib::lsfitfit(state, Quadratic_func); //choose various fitting functions w/ Jacobian fitting
	        lsfitresults(state, info, c, rep); //get fitting results stored in c[]

			//transfer fitting parameters
			QuadCoef[0][k]=c[0]; QuadCoef[1][k]=c[1]; QuadCoef[2][k]=c[2];
			if (c[2]!=0.0) QuadCoef[3][k]=-c[1]/(2.0*c[2]); //location for extrema
		}
}

//calculate first order derivative dS/dB
void SpinPackDev::CalculateDSDB() 
{
	for (int i=0; i<ACTSTEP-1;i++)
	{
		dSdB[i][VertSubSec*2]=(Spin[i+1][VertSubSec*2]+Spin[i][VertSubSec*2])/2.0; 
		for(int j=0; j<VertSubSec*2;j++)
		{
			dSdB[i][j]=(Spin[i+1][j]-Spin[i][j])/(Spin[i+1][VertSubSec*2]-Spin[i][VertSubSec*2]);
		}
	}
}

//linear fitting of Spin[][] and set LinearCoef[][]
void SpinPackDev::FitLinear()
{
	    //fitting definitions, see ALGLIB function documentation
        double epsf = 0;
        double epsx = 1.0E-9;
        ae_int_t maxits = 1000000;
        ae_int_t info;
        lsfitstate state;
        lsfitreport rep;
		ae_int_t n=ACTSTEP; //data array length
	    ae_int_t m=2; //total number of function parameters
		real_2d_array x; x.setlength(n, 1); // x values
		real_1d_array y; y.setlength(n); // y values
		real_1d_array c; c.setlength(m); //Guess parameters

		for(int k=0; k<VertSubSec*2; k++) //loop for 6 locations
		{
	        //copy data to ALGLIB arrays
	        for (int i=0; i<n; i++)  
		    {
			   x[i][0]=Spin[i][VertSubSec*2]; //step value for as last element
	           y[i]=Spin[i][k];
		    }

			//run fitting function
		    c[0]=1.0; c[1]=0.5; //initial guess parameters
            lsfitcreatef(x, y, c, true, state);  //use ALGLIB F-mode, no Jacobian
            lsfitsetcond(state, epsf, epsx, maxits);
            alglib::lsfitfit(state, Linear_func); //choose various fitting functions w/ Jacobian fitting
	        lsfitresults(state, info, c, rep); //get fitting results stored in c[]

			//transfer fitting parameters
			LinearCoef[0][k]=c[0]; LinearCoef[1][k]=c[1];
			if (c[1]!=0.0) LinearCoef[2][k]=-c[0]/c[1]; //zero crossings
		}
}

//read average Spin from a TXT file
bool SpinPackDev::ReadSpinTXT(string filename)
{
	bool flag=false; 
	ifstream datalog;
    datalog.open (filename.c_str(), ios::in); 
	flag=!(datalog.fail()); //check if file is open
	int i=0; //sweep steps counter
    if (flag)
    {
		while((i<ACTSTEP)&&(!datalog.eof( ))) //read until end of file
	    {
		  for(int j=0; j<VertSubSec*2+1;j++)
		  {
			datalog>>Spin[i][j];
		  }
		  i++;
        }
	   ACTSTEP=i-1;  //set Acutal number of data points during a scan
	}

	datalog.close();
	return flag;
}

//write average Spin to a TXT file
bool SpinPackDev::WriteSpinTXT(string filename)
{
	bool flag=false; 
	ofstream datalog;
	datalog.open(filename.c_str(),ios::out | ios::trunc); //delete previous log then write the new one
    flag=!(datalog.fail()); //check if file is open
    if (flag)
    {
	    for (int i=0; i<ACTSTEP;i++)
	    {
		    for(int j=0; j<VertSubSec*2+1;j++)
		    {
			    datalog<<Spin[i][j]<<" ";
		    }
		    datalog<<endl;
	    }
    }
	datalog.close();
	return flag;
}

//write all data members to a TXT file
bool SpinPackDev::WriteSpinPackTXT(string filename) 
{
	bool flag=false; 
	ofstream datalog;
	datalog.open(filename.c_str(),ios::out | ios::trunc); //delete previous log then write the new one
    flag=!(datalog.fail()); //check if file is open
 if (flag)
  {
	datalog<<ACTSTEP<<endl; 
	for (int i=0; i<ACTSTEP;i++)
	{
		for(int j=0; j<VertSubSec*2+1;j++)
		{
			datalog<<Spin[i][j]<<" ";
		}
		datalog<<endl;
	}
	for (int i=0; i<3;i++)
	{
		for(int j=0; j<VertSubSec*2;j++)
		{
			datalog<<LinearCoef[i][j]<<" ";
		}
		datalog<<endl;
	}
    for (int i=0; i<ACTSTEP-1;i++)
	{
		for(int j=0; j<VertSubSec*2+1;j++)
		{
			datalog<<dSdB[i][j]<<" ";
		}
		datalog<<endl;
	}
	for (int i=0; i<4;i++)
	{
		for(int j=0; j<VertSubSec*2;j++)
		{
			datalog<<QuadCoef[i][j]<<" ";
		}
		datalog<<endl;
	}
	for (int i=0; i<4;i++)
	{
		for(int j=0; j<VertSubSec*2;j++)
		{
			datalog<<SineCoef[i][j]<<" ";
		}
		datalog<<endl;
	}

 }
	datalog.close();
	return flag;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////



//extended methods for gradient scans
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//calculate gradients for a grad scan
void SpinPackDev::CalculateGrad()
{
	//Calculate 2 gradient for the gradient scan
	for(int j=0;j<ACTSTEP;j++)   
	{
	   Gradient[j][2]=Spin[j][VertSubSec*2]; //sweep value array
	   Gradient[j][0]=(Spin[j][1]-Spin[j][2])/(5*0.36);//Gpy from +Z side data
	   Gradient[j][1]=(Spin[j][1]+Spin[j][2])/2-(Spin[j][4]+Spin[j][5])/2; //Gpz from +/- Z sides
	}

}

//linear fitting of Gradient[][] and set GradLinCoef[][]
void SpinPackDev::GradFit()
{
   //Linear fitting to all section data

	    //fitting definitions, see ALGLIB function documentation
        double epsf = 0;
        double epsx = 1.0E-9;
        ae_int_t maxits = 1000000;
        ae_int_t info;
        lsfitstate state;
        lsfitreport rep;
		ae_int_t n=ACTSTEP; //data array length
	    ae_int_t m=2; //total number of function parameters
		real_2d_array x; x.setlength(n, 1); // x values
		real_1d_array y; y.setlength(n); // y values
		real_1d_array c; c.setlength(m); //Guess parameters

		for(int k=0; k<2; k++) //loop for 2 gradients
		{
	        //copy data to ALGLIB arrays
	        for (int i=0; i<n; i++)  
		    {
			   x[i][0]=Gradient[i][2]; //step value for as last element
	           y[i]=Gradient[i][k];
		    }

			//run fitting function
		    c[0]=1.0; c[1]=0.5; //initial guess parameters
            lsfitcreatef(x, y, c, true, state);  //use ALGLIB F-mode, no Jacobian
            lsfitsetcond(state, epsf, epsx, maxits);
            alglib::lsfitfit(state, Linear_func); //choose various fitting functions w/ Jacobian fitting
	        lsfitresults(state, info, c, rep); //get fitting results stored in c[]

			//transfer fitting parameters
			GradLinCoef[0][k]=c[0]; GradLinCoef[1][k]=c[1];
			if (c[1]!=0.0) GradLinCoef[2][k]=-c[0]/c[1]; //zero crossings
		}
}

//write all data members to a TXT file for a grad scan
bool SpinPackDev::WriteGradPackTXT(string filename)
{
	bool flag=false; 
	ofstream datalog;
	datalog.open(filename.c_str(),ios::out | ios::trunc); //delete previous log then write the new one
    flag=!(datalog.fail()); //check if file is open
 if (flag)
  {
	datalog<<ACTSTEP<<endl; 
	for (int i=0; i<ACTSTEP;i++)
	{
		for(int j=0; j<VertSubSec*2+1;j++)
		{
			datalog<<Spin[i][j]<<" ";
		}
		datalog<<endl;
	}
   	for (int i=0; i<ACTSTEP;i++)
	{
		for(int j=0; j<3;j++)
		{
			datalog<<Gradient[i][j]<<" ";
		}
		datalog<<endl;
	}
	for (int i=0; i<3;i++)
	{
		for(int j=0; j<2;j++)
		{
			datalog<<GradLinCoef[i][j]<<" ";
		}
		datalog<<endl;
	}
 }
	datalog.close();
	return flag;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif