//Example fo numerical integration and demostartion on how to use it

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

void int_function_1_func(double x, double xminusa, double bminusx, double &y, void *ptr) 
{
    // this callback calculates f(x)=exp(x)
    y = exp(x);
}

void main(int argc, char **argv)
{
    //
    // This example demonstrates integration of f=exp(x) on [0,1]:
    // * first, autogkstate is initialized
    // * then we call integration function
    // * and finally we obtain results with autogkresults() call
    //
    double a = 0;
    double b = 1;
    autogkstate s;
    double v;
    autogkreport rep;

    autogksmooth(a, b, s);
    alglib::autogkintegrate(s, int_function_1_func);
    autogkresults(s, v, rep);

    printf("%.2f\n", double(v)); // EXPECTED: 1.7182
    system("pause");
}