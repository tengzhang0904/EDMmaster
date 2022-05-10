/********************************************************************/
/* Communicate with  Agilent 34401A  via RS232     */
/* Last Modified 03/19/2012 by KUNYAN ZHU                           */ 

/* The general flow of the code is                                  */
/*    Open Resource Manager                                         */
/*    Open VISA Session to an Instrument                            */
/*    Configure the Serial Port                                     */
/*    Write the Identification Query Using viWrite                  */
/*    Try to Read a Response With viRead                            */
/*    Close the VISA Session                                        */

/********************************************************************/

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_DEPRECATE)
#define _CRT_SECURE_NO_DEPRECATE
#endif


#ifndef _HVDivider_included_
#define _HVDivider_included_
#define _CRT_SECURE_NO_DEPRECATE

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <math.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <windows.h> 
#include "NI VISA/visa.h"
#pragma comment(lib, "visa32.lib")
using namespace std;

#define MEAS_PTS 20  //total num of points in finite sample mode

class HVDivider{

   public:
	   float HVdata[MEAS_PTS];
	   float HVdataS; //single measurment
	   void init();
	   bool WriteHVFile(const string filename); //Write data in finite sample mode to a file
       void MeasureHV(int HVoptions); //Measurement w/ Agilent 34401A  via RS232
	   ViStatus SetRS232(ViSession instr_num,ViStatus status_num);//set RS232 Attributes
       ViStatus WriteRS232(char str[], ViSession instr_num, ViStatus status_num); //write RS232
};


void HVDivider::init()
{   
	for (int i=0;i<MEAS_PTS;i++)
	{
		HVdata[i]=0.0;
	}
	HVdataS=0.0;
}


bool HVDivider::WriteHVFile(const string filename)//write Data to file
{
	ofstream datalog( filename.c_str(),ios::out | ios::trunc); //delete previous log then write the new one
	datalog.precision(6); //set precision to 6 digits
	datalog.setf(ios::fixed,ios::floatfield);   // floatfield set to fixed
	for(int i=0;i<MEAS_PTS;i++){
		datalog<<left<<this->HVdata[i]<<endl; //formated output 
	  }
    cout<<"HVDivider:: data written to file"<<endl<<filename<<endl;
    return true;
}

void HVDivider::MeasureHV(int HVoptions)
{  
   static ViSession defaultRM;     
   static ViSession instr;     
   static ViStatus status;

   /*Start a serial handler*/
   status=viOpenDefaultRM (&defaultRM);
   if (status < VI_SUCCESS)  
   {
      printf ("Could not open a session to the VISA Resource Manager!\n");
      exit (EXIT_FAILURE);
   }
   
   /*Open a RS232 interface*/
   status = viOpen (defaultRM, "ASRL9::INSTR", VI_NULL, VI_NULL, &instr);
   if (status < VI_SUCCESS) 
   {
      printf ("Cannot open a session to the device.\n");
      goto Close;
   }                                   
      
   /*Set Attributes*/
   status=SetRS232(instr,status);
   if (status < VI_SUCCESS) 
             {
                printf ("Error configuring the device.\n");
	            goto Close;
             }

   /*use the viRead function to read a few current values*/
   unsigned int k=0;
   const int str_size=64; 
   static ViUInt32 retCount; //Number of bytes actually transferred
   static unsigned char buffer[str_size]; //Location of a buffer to receive data from device
   float Voltage=0.0;


/**************************************************************************************************/
   /*Write configure commands to multimeter***********************/ 
   status=WriteRS232("SYST:REM\n", instr,status); //set device to REMOTE
   status = viRead (instr, buffer, str_size, &retCount); 

   switch (HVoptions){

        case -1: //measure infinite number of times
			cout<<"HVDivider measure continuously..."<<endl;
            while(true){
            status=WriteRS232("MEAS:VOLT:DC?\n", instr,status); //Measure ONCE
            if (status < VI_SUCCESS) {
                printf ("Error commanding the device.\n");
	            goto Close;
             }
            status = viRead (instr, buffer, str_size, &retCount);
            if (status < VI_SUCCESS) {
                printf ("Error reading a response from the device.\n");
				cout<<hex<<status<<endl;
	            goto Close;
             }
		    else   {
			 sscanf((const char*)buffer, "%f", &Voltage);
			 Voltage=Voltage*(float)1.0; //HV Divider factor
			 system("CLS");
			 cout<<"HV Divider output"<<endl
				 <<fixed<<setprecision(9)<<Voltage<<" V"<<endl<<endl
				 <<"HV on Plates"<<endl
				 <<fixed<<setprecision(6)<<Voltage*1000.0<<" V"<<endl;
		    }
           }
        break;

        case 0: 
			 cout<<"HVDivider measure finite sample mode:"<<MEAS_PTS<<endl;
             while(k<MEAS_PTS){
             status=WriteRS232("MEAS:VOLT:DC?\n", instr,status); //Measure ONCE
             if (status < VI_SUCCESS) {
                printf ("Error commanding the device.\n");
	            goto Close;
              }
             status = viRead (instr, buffer, str_size, &retCount);
             if (status < VI_SUCCESS) {
                printf ("Error reading a response from the device.\n");
				cout<<hex<<status<<endl;
	            goto Close;
              }
		    else   {
			 sscanf((const char*)buffer, "%f", &Voltage);
			 Voltage=Voltage*(float)1000.0; //HV Divider factor
			 cout<<fixed<<setprecision(6)<<Voltage<<endl;
             this->HVdata[k]=Voltage;
		       }
	       k++;
          }
        break;

		case 1: 
			 cout<<"HVDivider single measurement mode:"<<endl;
             status=WriteRS232("MEAS:VOLT:DC?\n", instr,status); //Measure ONCE
             if (status < VI_SUCCESS) {
                printf ("Error commanding the device.\n");
	            goto Close;
              }
             status = viRead (instr, buffer, str_size, &retCount);
             if (status < VI_SUCCESS) {
                printf ("Error reading a response from the device.\n");
				cout<<hex<<status<<endl;
	            goto Close;
              }
		    else   {
			 sscanf((const char*)buffer, "%f", &Voltage);
			 Voltage=Voltage*(float)1000.0; //HV Divider factor
             this->HVdataS=Voltage;
		       }
        break;

        default: break;
   }

   status=WriteRS232("SYST:LOC\n", instr,status); //set device to LOCAL

/**************************************************************************************************/

  /*Close the device and interface*/
  Close:
     status = viClose (instr);
     status = viClose (defaultRM);
     fflush (stdin);
}


//Function to set RS232 Attributes
ViStatus HVDivider::SetRS232(ViSession instr_num,ViStatus status_num)
 {
    //Set the timeout in milliseconds
    status_num= viSetAttribute (instr_num, VI_ATTR_TMO_VALUE, 1000);
  
    //Set the baud rate
    status_num= viSetAttribute (instr_num, VI_ATTR_ASRL_BAUD, 9600);
  
    //Set the number of data bits contained in each frame
    status_num= viSetAttribute (instr_num, VI_ATTR_ASRL_DATA_BITS, 8);
  
    //Specify parity. Options
    status_num= viSetAttribute (instr_num, VI_ATTR_ASRL_PARITY, VI_ASRL_PAR_NONE);

	//Specify stop bit Options
    status_num= viSetAttribute (instr_num, VI_ATTR_ASRL_STOP_BITS, VI_ASRL_STOP_TWO); 

    //Set the type of flow control used by the transfer mechanism
    status_num= viSetAttribute (instr_num, VI_ATTR_ASRL_FLOW_CNTRL, VI_ASRL_FLOW_NONE);

    return status_num;
 }

//Function to write a command to a RS232 device
ViStatus HVDivider::WriteRS232(char str[], ViSession instr_num, ViStatus status_num)
 {  
    ViUInt32 writeCount;
    char stringinput[512];
    strcpy (stringinput,str);  //format data as fixed point decimal values
    status_num= viWrite (instr_num, (ViBuf)stringinput, (ViUInt32)strlen(stringinput), &writeCount);
    if (status_num < VI_SUCCESS)  printf ("Error writing to the device.\n");//this barely happens
    return status_num;
 }


#endif