//Example fo nonlinear fitting and demostartion on how to use it


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


As in this example, lsfitcreatefg function is choosen
/*************************************************************************
Nonlinear least squares fitting using gradient only, without individual weights.

Nonlinear task min(F(c)) is solved, where

    F(c) = ((f(c,x[0])-y[0]))^2 + ... + ((f(c,x[n-1])-y[n-1]))^2,

    * N is a number of points,
    * M is a dimension of a space points belong to,
    * K is a dimension of a space of parameters being fitted,
    * x is a set of N points, each of them is an M-dimensional vector,
    * c is a K-dimensional vector of parameters being fitted

This subroutine uses only f(c,x[i]) and its gradient.

INPUT PARAMETERS:
    X       -   array[0..N-1,0..M-1], points (one row = one point)
    Y       -   array[0..N-1], function values.
    C       -   array[0..K-1], initial approximation to the solution,
    N       -   number of points, N>1
    M       -   dimension of space
    K       -   number of parameters being fitted
    CheapFG -   boolean flag, which is:
                * True  if both function and gradient calculation complexity
                        are less than O(M^2).  An improved  algorithm  can
                        be  used  which corresponds  to  FGJ  scheme  from
                        MINLM unit.
                * False otherwise.
                        Standard Jacibian-bases  Levenberg-Marquardt  algo
                        will be used (FJ scheme).

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 17.08.2009 by Bochkanov Sergey
*************************************************************************/
/**********************************************************************************************************************************/


/**********************************************************************************
*method to fill a readl_xd_array in ALGLIB

///////////////////////////////////////////////
real_1d_array cc;
cc.setlength(3);
double num[3]={10.15, 5.13, 0.18};
cc.setcontent(3,num);
cout<<cc.tostring(10).c_str()<<endl;
double *ccnum=cc.getcontent();  
cout<<setprecision(10)<<ccnum[0]<<endl;

///////////////////////////////////////////////
real_2d_array xy;
xy.setlength(iArrayWidth, iArrayHeight);

for ( int i = 0; i < iArrayHeight; i++ )
{
   for ( int j = 0; j < iArrayWidth; j++ )
   {
      xy(i,j) = m_Array[i][j];
   }
}

***********************************************************************************/

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

void function_cx_1_func(const real_1d_array &c, const real_1d_array &x, double &func, void *ptr) 
{
    // this callback calculates f(c,x)=exp(-c0*sqr(x0))
    // where x is a position on X-axis and c is adjustable parameter
    func = exp(-c[0]*pow(x[0],2));
}
void function_cx_1_grad(const real_1d_array &c, const real_1d_array &x, double &func, real_1d_array &grad, void *ptr) 
{
    // this callback calculates f(c,x)=exp(-c0*sqr(x0)) and gradient G={df/dc[i]}
    // where x is a position on X-axis and c is adjustable parameter.
    // IMPORTANT: gradient is calculated with respect to C, not to X
    func = exp(-c[0]*pow(x[0],2));
    grad[0] = -pow(x[0],2)*func;
}

int main(int argc, char **argv)
{
    // In this example we demonstrate exponential fitting by
    //     f(x) = exp(-c*x^2)
    // subject to bound constraints
    //     0.0 <= c <= 1.0
    // using function value only.
    real_2d_array x = "[[-1],[-0.8],[-0.6],[-0.4],[-0.2],[0],[0.2],[0.4],[0.6],[0.8],[1.0]]";
    real_1d_array y = "[0.223130, 0.382893, 0.582748, 0.786628, 0.941765, 1.000000, 0.941765, 0.786628, 0.582748, 0.382893, 0.223130]";
    real_1d_array c = "[0.3]"; 
	//real_1d_array bndl = "[0.0]";
    //real_1d_array bndu = "[1.0]";
    double epsf = 0;
    double epsx = 0.000001;
    ae_int_t maxits = 0;
    ae_int_t info;
    lsfitstate state;
    lsfitreport rep;

    lsfitcreatefg(x, y, c, true, state);
	//lsfitsetbc(state, bndl, bndu);    //set constrains will lead to c=1.0
    lsfitsetcond(state, epsf, epsx, maxits);
    alglib::lsfitfit(state, function_cx_1_func, function_cx_1_grad);
    lsfitresults(state, info, c, rep);
    printf("%d\n", int(info)); // EXPECTED: 2

	double result=*(c.getcontent());  // EXPECTED: 1.5
	cout<<setprecision(10)<<result<<endl;
	
	system("pause");
	return 0;
}