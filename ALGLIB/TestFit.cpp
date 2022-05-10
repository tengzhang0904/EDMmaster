/* A simple version of fitting demo
*start debugging and run "TestFit.cpp"
*/

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <windows.h> 
#include <math.h>
#include <float.h>
using namespace std;

#include "ALGLIB/DATASET.h"
#include "ALGLIB/NLSF_MT.h"

const extern string MMCfolder("C:/MMC_SYSINT/");

void main(void)
{   
    const string levmarExcelFile=MMCfolder+"ALGLIB/"+"Testdata.xls";
	DATASET *set=new DATASET();
	set->init();
	set->LoadExcel(levmarExcelFile);

	/*****************************************************************************************************************
    Choose function type and number of parameters 
	case 1:   PARA=2
		 Linear_func = c[0]+c[1]*x[0];
	*****************************************************************************************************************
	case 2:   PARA=3
		 Quadratic_func = c[0]+c[1]*x[0]+c[2]*pow(x[0],2);
    *****************************************************************************************************************
	case 3:   PARA=4
		 Gaussian_func = c[0]+c[1]*exp(-0.5*pow((x[0]-c[2])/c[3],2));
	*****************************************************************************************************************
	case 4:   PARA=4
		 Lorentzian_func = c[0]+c[1]/(1+pow((x[0]-c[2])/c[3],2));
	*****************************************************************************************************************
	case 5:   PARA=3
		 Exponential_func = c[0]+c[1]*exp(-x[0]/c[2]);
	*****************************************************************************************************************
	case 6:   PARA=7
		 DoubleGaussian_func = c[0]+c[1]*exp(-0.5*pow((x[0]-c[2])/c[3],2))+c[4]*exp(-0.5*pow((x[0]-c[5])/c[6],2));
	*********************************************************************************
	case 7:   PARA=5
		 GaussianLinear_func = c[0]+c[1]*exp(-0.5*pow((x[0]-c[2])/c[3],2))+c[4]*x[0];
	*****************************************************************************************************************
	case 8:   PARA=3
		 Optical_func = c[0]+c[1]/(x[0]+c[2]);
	*****************************************************************************************************************
	case 9:   PARA=4
		 Sinc_func = c[0]+c[1]*sin(c[2]*x[0]+c[3])/(c[2]*x[0]+c[3]);
	*****************************************************************************************************************
	case 10:  PARA=6
		 WavePack_func = c[0]+c[1]*sin(c[2]*x[0]+c[3])*exp(-0.5*pow((x[0]-c[4])/c[5],2));

    ****************************************************************************************************************/
	unsigned int function_type=4;
	const unsigned int PARAS=4;

	//initial guess of fitting function parameters
	double p[PARAS]={0.5, 3.0, 8.0, 5.0}; 
	
	double *result=NLSF(set, 1, p, PARAS, function_type);
    for(int i=0; i<PARAS; ++i)
        printf("%.7g ", result[i]);
	cout<<endl<<endl;


	delete set;
    system("pause");

}