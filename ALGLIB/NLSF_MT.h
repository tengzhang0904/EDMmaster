//Nonlinear least square fitting function interface
//1D cubic spline and integrate 

/*******************************************************************************************************************************
* http://www.alglib.net/interpolation/leastsquares.php#levmar

ALGLIB users can choose between three operating modes of nonlinear solver 
which differ in what information about function being fitted they need:

F-mode: when only function value is needed. User don't have to calculate analytic gradient/Jacobian - 
        it will be done automatically using combination of numerical differentiation (for precision) 
		and secant updates (for better speed). 
		It is the slowest, but the simplest operating mode provided by ALGLIB nonlinear solver.
FG-mode: in this mode user should implement calculation of both function and its analytic gradient. 
        This mode is faster than F-mode and can find solution with better accuracy.
FGH-mode: when user should provide function value, analytic gradient, analytic Hessian. 
         This mode can be beneficial when you have high level of noise in your data and cheap Hessian.

Any of the modes mentioned above can be used to solve unweighted or weighted problems. 
Letters in the mode name are appended to the constructor function name; 
if you use weighted version, W is appended too. So we have 6 versions of constructor functions:

lsfitcreatef - nonlinear curve fitting using function value f(x|c) only
lsfitcreatewf - nonlinear curve fitting using function value f(x|c) only, weighted setting
lsfitcreatefg - nonlinear curve fitting using function value f(x|c) and gradient with respect to c
lsfitcreatewfg - nonlinear curve fitting using function value f(x|c) and gradient with respect to c, weighted setting
lsfitcreatefgh - nonlinear curve fitting using function value f(x|c), gradient and Hessian with respect to c
lsfitcreatewfgh - nonlinear curve fitting using function value f(x|c), gradient and Hessian with respect to c, weighted setting

What operating mode to choose? For a quick start we recommend to choose F-mode, 
because it is the simplest of all nonlinear fitting modes provided by ALGLIB. 
You just calculate function value at given point x with respect to the vector of tunable parameters c, 
and ALGLIB package solves all numerical differentiation issues. 
However, as we told above, gradient-free nonlinear fitting is easy to use, but is not efficient. 
Furthermore, numerical differentiation doesn't allow us to find solution with accuracy significantly 
higher than numerical differentiation step. Thus if you need high performance or high accuracy, 
you should implement calculation of analytic gradient and switch to FG-mode.
/**********************************************************************************************************************************/

/*************************************************************************
lsfitcreatef function
Nonlinear least squares fitting using function values only.

Combination of numerical differentiation and secant updates is used to
obtain function Jacobian.

Nonlinear task min(F(c)) is solved, where

    F(c) = (f(c,x[0])-y[0])^2 + ... + (f(c,x[n-1])-y[n-1])^2,

    * N is a number of points,
    * M is a dimension of a space points belong to,
    * K is a dimension of a space of parameters being fitted,
    * w is an N-dimensional vector of weight coefficients,
    * x is a set of N points, each of them is an M-dimensional vector,
    * c is a K-dimensional vector of parameters being fitted

This subroutine uses only f(c,x[i]).

INPUT PARAMETERS:
    X       -   array[0..N-1,0..M-1], points (one row = one point)
    Y       -   array[0..N-1], function values.
    C       -   array[0..K-1], initial approximation to the solution,
    N       -   number of points, N>1
    M       -   dimension of space
    K       -   number of parameters being fitted
    DiffStep-   numerical differentiation step;
                should not be very small or large;
                large = loss of accuracy
                small = growth of round-off errors

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state
*************************************************************************/


/*************************************************************************
lsfitresults function:
Nonlinear least squares fitting results.

Called after return from LSFitFit().

INPUT PARAMETERS:
    State   -   algorithm state

OUTPUT PARAMETERS:
    Info    -   completetion code:
                    *  1    relative function improvement is no more than
                            EpsF.
                    *  2    relative step is no more than EpsX.
                    *  4    gradient norm is no more than EpsG
                    *  5    MaxIts steps was taken
                    *  7    stopping conditions are too stringent,
                            further improvement is impossible
    C       -   array[0..K-1], solution
    Rep     -   optimization report. Following fields are set:
                * Rep.TerminationType completetion code:
                * RMSError          rms error on the (X,Y).
                * AvgError          average error on the (X,Y).
                * AvgRelError       average relative error on the non-zero Y
                * MaxError          maximum error
                                    NON-WEIGHTED ERRORS ARE CALCULATED
                * WRMSError         weighted rms error on the (X,Y).
*************************************************************************/


/*************************************************************************
lsfitsetcond function
Stopping conditions for nonlinear least squares fitting.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    EpsF    -   stopping criterion. Algorithm stops if
                |F(k+1)-F(k)| <= EpsF*max{|F(k)|, |F(k+1)|, 1}
    EpsX    -   >=0
                The subroutine finishes its work if  on  k+1-th  iteration
                the condition |v|<=EpsX is fulfilled, where:
                * |.| means Euclidian norm
                * v - scaled step vector, v[i]=dx[i]/s[i]
                * dx - ste pvector, dx=X(k+1)-X(k)
                * s - scaling coefficients set by LSFitSetScale()
    MaxIts  -   maximum number of iterations. If MaxIts=0, the  number  of
                iterations   is    unlimited.   Only   Levenberg-Marquardt
                iterations  are  counted  (L-BFGS/CG  iterations  are  NOT
                counted because their cost is very low compared to that of
                LM).

NOTE

Passing EpsF=0, EpsX=0 and MaxIts=0 (simultaneously) will lead to automatic
stopping criterion selection (according to the scheme used by MINLM unit).
*************************************************************************/



/*************************************************************************
lsfitreport class
Least squares fitting report:
    TaskRCond       reciprocal of task's condition number
    IterationsCount number of internal iterations

    RMSError        RMS error
    AvgError        average error
    AvgRelError     average relative error (for non-zero Y[I])
    MaxError        maximum error

    WRMSError       weighted RMS error
*************************************************************************/



#ifndef _NLSF_MT_h_included_
#define _NLSF_MT_h_included_
#define _CRT_SECURE_NO_DEPRECATE

/***********************************************************************************/
//ALGLIB include files   ***********************************************************
#include <math.h>
#include "ALGLIB/src/alglibinternal.cpp"
#include "ALGLIB/src/alglibinternal.h"
#include "ALGLIB/src/alglibmisc.cpp"
#include "ALGLIB/src/alglibmisc.h"
#include "ALGLIB/src/ap.cpp"
#include "ALGLIB/src/ap.h"
#include "ALGLIB/src/dataanalysis.cpp"
#include "ALGLIB/src/dataanalysis.h"
#include "ALGLIB/src/diffequations.cpp"
#include "ALGLIB/src/diffequations.h"
#include "ALGLIB/src/fasttransforms.cpp"
#include "ALGLIB/src/fasttransforms.h"
#include "ALGLIB/src/integration.cpp"
#include "ALGLIB/src/integration.h"
#include "ALGLIB/src/interpolation.cpp"
#include "ALGLIB/src/interpolation.h"
#include "ALGLIB/src/linalg.cpp"
#include "ALGLIB/src/linalg.h"
#include "ALGLIB/src/optimization.cpp"
#include "ALGLIB/src/optimization.h"
#include "ALGLIB/src/solvers.cpp"
#include "ALGLIB/src/solvers.h"
#include "ALGLIB/src/specialfunctions.cpp"
#include "ALGLIB/src/specialfunctions.h"
#include "ALGLIB/src/statistics.cpp"
#include "ALGLIB/src/statistics.h"
#include "ALGLIB/src/stdafx.h"
using namespace alglib;
/***********************************************************************************/

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <cstdlib>
using namespace std;

#include "ALGLIB/DATASET.h" //NLSF testing data set

#include "MASTER/MASTER/MASTERDlg.h" //MMC front panel
#include "MASTER/MASTER/DataPack.h"  //Data management
using namespace DATACONST;

/*********************************************************************************
Linear function 
*********************************************************************************/
void Linear_func(const real_1d_array &c, const real_1d_array &x, double &func, void *ptr) 
{
    func = c[0]+c[1]*x[0];
}

/*********************************************************************************
Quadratic function 
*********************************************************************************/
void Quadratic_func(const real_1d_array &c, const real_1d_array &x, double &func, void *ptr) 
{
    func = c[0]+c[1]*x[0]+c[2]*pow(x[0],2);
}

/*********************************************************************************
Gaussian function 
*********************************************************************************/
void Gaussian_func(const real_1d_array &c, const real_1d_array &x, double &func, void *ptr) 
{
    func = c[0]+c[1]*exp(-0.5*pow((x[0]-c[2])/c[3],2));
}

/*********************************************************************************
Lorentzian function 
*********************************************************************************/
void Lorentzian_func(const real_1d_array &c, const real_1d_array &x, double &func, void *ptr) 
{
    func = c[0]+c[1]/(1+pow((x[0]-c[2])/c[3],2));
}

/*********************************************************************************
Exponential function 
*********************************************************************************/
void Exponential_func(const real_1d_array &c, const real_1d_array &x, double &func, void *ptr) 
{
    func = c[0]+c[1]*exp(-x[0]/c[2]);
}

/*********************************************************************************
Double Gaussian function 
*********************************************************************************/
void DoubleGaussian_func(const real_1d_array &c, const real_1d_array &x, double &func, void *ptr) 
{
    func = c[0]+c[1]*exp(-0.5*pow((x[0]-c[2])/c[3],2))+c[4]*exp(-0.5*pow((x[0]-c[5])/c[6],2));
}

/*********************************************************************************
Gaussian Linear function 
*********************************************************************************/
void GaussianLinear_func(const real_1d_array &c, const real_1d_array &x, double &func, void *ptr) 
{
    func = c[0]+c[1]*exp(-0.5*pow((x[0]-c[2])/c[3],2))+c[4]*x[0];
}

/*********************************************************************************
Optical function 
*********************************************************************************/
void Optical_func(const real_1d_array &c, const real_1d_array &x, double &func, void *ptr) 
{
    func = c[0]+c[1]/(x[0]+c[2]);
}

/*********************************************************************************
Sinc function 
*********************************************************************************/
void Sinc_func(const real_1d_array &c, const real_1d_array &x, double &func, void *ptr) 
{
    func = c[0]+c[1]*sin(c[2]*x[0]+c[3])/(c[2]*x[0]+c[3]);
}

/*********************************************************************************
Constrained sine function  (for spin precession)
*********************************************************************************/
void ConstrSine_func(const real_1d_array &c, const real_1d_array &x, double &func, void *ptr) 
{
    func = c[0]*sin(TPI*c[1]*x[0]+c[2])-c[0]*sin(c[2]);
}

/*********************************************************************************
WavePack function 
*********************************************************************************/
void WavePack_func(const real_1d_array &c, const real_1d_array &x, double &func, void *ptr) 
{
    func = c[0]+c[1]*sin(c[2]*x[0]+c[3])*exp(-0.5*pow((x[0]-c[4])/c[5],2));
}


/*********************************************************************************
Interface of fitting function
 *set: an multi column data set as defined in DATASET
 *col: which column of data to be fitted
 *p: guess function parameters
 *ftype: fitting function type
return an array of actual fitting parameters
*********************************************************************************/
double* NLSF(DATASET *set, unsigned int col, double *p, unsigned int ParaSize, unsigned int type)
{   
	ae_int_t n=set->getLength(); //data array length
	ae_int_t m=ParaSize; //total number of function parameters
    //cout<<"ParaSize: "<<m<<endl<<"Length: "<<n<<endl;

	//fitting definitions, see ALGLIB function documentation
    double epsf = 0;
    double epsx = 1.0E-9;
    ae_int_t maxits = 1000000;
    ae_int_t info;
    lsfitstate state;
    lsfitreport rep;

	//copy data to ALGLIB arrays
	real_2d_array x; // x values
    x.setlength(n, 1);
	for (int i=0; i<n; i++)  {x[i][0]=(double)set->data[0][i];}

	real_1d_array y; // y values
    y.setlength(n);
	y.setcontent(n,set->data[col]);

	real_1d_array c; //Guess parameters
    c.setlength(m);
	c.setcontent(m,p); //copy guess para to c-array

	//use ALGLIB F-mode, no Jacobian
    lsfitcreatef(x, y, c, true, state); 
    lsfitsetcond(state, epsf, epsx, maxits);
 
	//choose various fitting functions w/ Jacobian fitting
	switch (type)
	{		
		 case 1:  alglib::lsfitfit(state, Linear_func);break;
		 case 2:  alglib::lsfitfit(state, Quadratic_func);break;
		 case 3:  alglib::lsfitfit(state, Gaussian_func);break;
		 case 4:  alglib::lsfitfit(state, Lorentzian_func);break;
		 case 5:  alglib::lsfitfit(state, Exponential_func);break;
		 case 6:  alglib::lsfitfit(state, DoubleGaussian_func);break;
	     case 7:  alglib::lsfitfit(state, GaussianLinear_func);break;
		 case 8:  alglib::lsfitfit(state, Optical_func);break;
		 case 9:  alglib::lsfitfit(state, Sinc_func);break;
		 case 10:  alglib::lsfitfit(state, WavePack_func);break;	 
		 default:   break; 
	}

	lsfitresults(state, info, c, rep); //get fitting results

	cout<<"============ALGLIB F-mode report============"<<endl
		<<"============================================"<<endl
		<<"Termination type: "<<int(info)<<endl
		<<"Condition number reciprocal: "<<rep.taskrcond<<endl
        <<"Number of iterations: "<<rep.iterationscount<<endl
		<<"RMS error: "<<rep.rmserror<<endl
		<<"Average error: "<<rep.avgerror<<endl
		<<"Average relative error: "<<rep.avgrelerror<<endl
		<<"Maximum error: "<<rep.maxerror<<endl
		<<"Weighted RMS error: "<<rep.wrmserror<<endl
		<<"============================================"<<endl<<endl;

	//copy and return fitting parameters
	for (int i=0; i<m; i++)  {p[i]=c[i];}
	return p;
}

//End of nonlinear fitting definitions**********************************************/
/***********************************************************************************/



/*******************************************************************************************************/
// Integration of PDA signals  *************************************************************************/
/*******************************************************************************************************/
void SignalInte(DataPack *Pack, unsigned int shot, double Result[])
{
	const int PIX=PDANpixel/2;
	ae_int_t n=PIX; //data array length

	double data[3][PIX];
	for (int i=0; i<PIX; i++)  //copy data (a particular shot)
	{
		data[0][i]=i;
		data[1][i]=Pack->PDA_Signal[i][shot];   //-Z
		data[2][i]=Pack->PDA_Signal[i+PIX][shot];  //+Z
	}
		
	//spline 1d integrate of PDA signal
    real_1d_array x,y; // x and y values
	spline1dinterpolant s;
	double right_bound=data[0][n-1];
	
	for (int k=1; k<=2; k++)  //+/- Z sides
	{
        x.setlength(n); x.setcontent(n,data[0]);
	    y.setlength(n); y.setcontent(n,data[k]);

	    spline1dbuildcubic(x, y, s);//cubic spline

	    Result[k-1]=spline1dintegrate(s, right_bound); //left bound is min(x[])
	}

}

bool SignalSum(DataPack *Pack, unsigned int shot, double Result[], unsigned int idx[2])
{
	const int PIX=PDANpixel/2;

	double data[3][PIX];
	for (int i=0; i<PIX; i++)  //copy data (a particular shot)
	{
		data[0][i]=i;
		data[1][i]=Pack->PDA_Signal[i][shot];   //-Z
		data[2][i]=Pack->PDA_Signal[i+PIX][shot];  //+Z
	}

    if ((idx[0]>=idx[1])||(idx[0]<0)||(idx[1]>=25))
		return false;
    else 
    {
	   Result[0]=0.0;Result[1]=0.0;
	   for (unsigned int p=idx[0]; p<=idx[1]; p++)
	   {
			Result[0]+=data[1][p];
			Result[1]+=data[2][p];
	   }
	  return true;
    }
}


#endif


