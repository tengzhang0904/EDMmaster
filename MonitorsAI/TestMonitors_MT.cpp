
#include "MonitorsAI_MT.h"
#include <stdio.h>
#include "DAQmx/NIDAQmx.h" //NI device lib
#pragma comment(lib, "NIDAQmx.lib")

using namespace std;


int main(void)
{
	float Tleakgedata;
	//HVdata=HVDivS();
	Tleakagedata = TopLeakageAI(0);
	printf("%f\n",Tleakagedata);
	return 0;
}