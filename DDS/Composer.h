/***************************************************************

*  This class compute the data array for a pulse train

*  Last modified 2012/01/12 by KZ

****************************************************************/

#ifndef _Composer_h_included_
#define _Composer_h_included_
#define _CRT_SECURE_NO_DEPRECATE

#include "DDSfunc.h"
#include "DAQmx/NIDAQmx.h"
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <windows.h> 
#include "Timer/Timer.h"
#include "Pulse.h"
#include "PulseTrain.h"
#pragma comment(lib, "NIDAQmx.lib")
using namespace std;


/*********************************************************************************************/
//Update Ramp Rate Clock once and do not update output
				 void UpdateRRC(PulseTrain* Train, string Data_sRRC, string Choice1, string COM)
				 {
	                  string RC[6]; //beak the Hex-12 string into 3 Hex-2 strings
	                  for(int i=0;i<6;i++) {RC[i]=Data_sRRC[i];} 
	                  Train->append(Str2Int(HexAdd(COM,"1A",RC[0]+RC[1],Choice1)));
	                  Train->append(Str2Int(HexAdd(COM,"1B",RC[2]+RC[3],Choice1)));
	                  Train->append(Str2Int(HexAdd(COM,"1C",RC[4]+RC[5],Choice1)));
				 }

//Update Frequency Tunning Word once and do not update output
				 void UpdateFTW(PulseTrain* Train, string Data_sFTW, string Choice1, string COM)
				 {
					 string FW[12]; //break the Hex-12 string into 6 Hex-2 strings
					 for(int i=0;i<12;i++) {FW[i]=Data_sFTW[i];}   
	                 Train->append(Str2Int(HexAdd(COM,"04",FW[0]+FW[1],Choice1)));
	                 Train->append(Str2Int(HexAdd(COM,"05",FW[2]+FW[3],Choice1)));
	                 Train->append(Str2Int(HexAdd(COM,"06",FW[4]+FW[5],Choice1)));
	                 Train->append(Str2Int(HexAdd(COM,"07",FW[6]+FW[7],Choice1)));
	                 Train->append(Str2Int(HexAdd(COM,"08",FW[8]+FW[9],Choice1)));
	                 Train->append(Str2Int(HexAdd(COM,"09",FW[10]+FW[11],Choice1))); 
				 }

//Update Delta Frequency Word once and do not update output
				 void UpdateDFW(PulseTrain* Train, string Data_sDFW, string Choice1, string COM)
				 {
					 string DF[12]; //break the Hex-12 string into 6 Hex-2 strings
					 for(int j=0;j<12;j++) {DF[j]=Data_sDFW[j];}
	                 Train->append(Str2Int(HexAdd(COM,"10",DF[0]+DF[1],Choice1)));
	                 Train->append(Str2Int(HexAdd(COM,"11",DF[2]+DF[3],Choice1)));
	                 Train->append(Str2Int(HexAdd(COM,"12",DF[4]+DF[5],Choice1)));
	                 Train->append(Str2Int(HexAdd(COM,"13",DF[6]+DF[7],Choice1)));
	                 Train->append(Str2Int(HexAdd(COM,"14",DF[8]+DF[9],Choice1)));
	                 Train->append(Str2Int(HexAdd(COM,"15",DF[10]+DF[11],Choice1))); 
				 }

//Update Amplitude word once and update output once
				 void UpdateATW(PulseTrain* Train, string Data_sATW, string Choice1, string Choice2, string COM)
				 {
					 string AW[4]; //break the Hex-12 string into 2 Hex-2 strings
	                 for(int j=0;j<4;j++) {AW[j]=Data_sATW[j];} 
	                 Train->append(Str2Int(HexAdd(COM,"21",AW[0]+AW[1],Choice1)));
	                 Train->append(Str2Int(HexAdd(COM,"22",AW[2]+AW[3],Choice2))); //update output once
				 }

//Update Phase Register 1 once and do not update output
				 void UpdatePHR(PulseTrain* Train, string Data_sPHR, string Choice1, string COM)
				 {
					 string PH[4]; //break the Hex-12 string into 2 Hex-2 strings
	                 for(int j=0;j<4;j++) {PH[j]=Data_sPHR[j];} 
	                 Train->append(Str2Int(HexAdd(COM,"00",PH[0]+PH[1],Choice1)));
	                 Train->append(Str2Int(HexAdd(COM,"01",PH[2]+PH[3],Choice1))); 
				 }
/*********************************************************************************************/


bool PulseTrain::Compute(string PulseParameterExcelFilename)
{    
    /*********************************************/
    /* Constants
    /*********************************************/
	double DDSCLK=GPSCLK*30.0; //DDS system clock is set to be 300MHz
	double RampRate=GPSCLK*10; //update within each subsection 

	//Phase definitions in unit of pi
	double CompositePIPhase=0.0; //default phase for composite PI, the 2nd one needs to be 0.5
    const static unsigned int CompositeN=3; //total number of composite pulses, must be odd
 
	/*********************************************/
    /* CONFIG DDS board
    /*********************************************/
	//CONFIG PORT D (size-2 Hex)
	string DIP="000000";//DIP switch settings-checked 
	string opt0=Bin2Hex(DIP+"00");//Latch 16-bit data bus, Master Reset
	string opt1=Bin2Hex(DIP+"01");//Latch 16-bit data bus, Load data into DDS buffer
	string opt2=Bin2Hex(DIP+"10");//Latch 16-bit data bus, Load data and Update output
	string opt3=Bin2Hex(DIP+"11");//Latch 16-bit data bus only, Do not communicate with AD9852

	//CONFIG PORT A
       // DIOA4:5
	      string HoldKeying="0@";//Hold function and Shaped Keying function-NOT INUSE
	       /* "0@"  Both off 
	       /* "1@"  Hold on
	       /* "2@"  Shaped Keying on
	       /* "3@"  Both on
	       /*the 2nd hex @ is reserved for self-timing mask and strobe source*/

	   // DIOA1
          string COM_ON="03"; //write HIGH to self-timing channel  DIOA0 and DIOA1, use one channel as FIFOCLK monitor
		  string COM_STOP="00"; //STOP FIFO update CLK

	//CONFIG Control register (see AD9852 page 23/32)
	string CR1D="00000000";
	string CR1E="01011110"; //PLL ENABLE, REFCLK=30*GPSCLK
	string CR20="00100000"; //SINC, DISABLE 12-bit AMP MULTIPLIER
    /* Mode 011(chirp), use External update clock(logic low), and clear accumulator as follows:  */
       string CR1FCLR1="10000110"; //Clear frequency accumulator ACC1
	   string CR1FCLR2="01000110"; //Clear frequency/phase accumulator ACC2
	   string CR1F="00000110"; //do not clear ACC1 and ACC2

	uInt32 DO_NOTHING=Str2Int(HexAdd(COM_ON,"23","00",opt3));

    /***************************************************************/
    /* DDS Control Flow
    /***************************************************************/
	//after initialization cnt=0; each append operation: cnt++
	//Write first data NULL 
	   cnt=0;
       append(DO_NOTHING);//do nothing

    //Master reset: and wait in 'Master reset' mode
	 for (unsigned int kwait=1; kwait<=20; kwait++)  //wait for 20 clock cycles
	   {
		   append(Str2Int(HexAdd(COM_ON,"23","00",opt0)));
	   }

   /***************************************************************/
   /* Config and output a series of pulses
   /***************************************************************/
    //define variables to use in loop structure 
	  double f0; //Center frequency, 1uHz resolution
	  double HSR; //Half-frequency sweeping range
	  double duration; //pulse duration
	  double AMP_PK; //Peak Amplitude; Min 0.0, Max 1.0

	  static double f_start; //start frequency  
	  static string sFTW;//FTW string type
	  unsigned int UPDATE_REG_SIZE; 
	    /* # of registers updated from section to section
	     * UPDATE_REG_SIZE=6 @ full range sweep 
		 * UPDATE_REG_SIZE=8 @ full range sweep+ amplitude control 
		 * UPDATE_REG_SIZE=3 @ small range (100kHz) sweep
		 * UPDATE_REG_SIZE=5 @ small range (100kHz) sweep+ amplitude control
		 * UPDATE_REG_SIZE=2 @ amplitude control ONLY*/
	  unsigned int sections; //number of linear sections
      string sDFW; double DFLS; //DFW related definitions see below
	  string sATW; //ATW related definitions
	  string sPHR; //PHR related functions

	  MicrowavePulse OnePulse;//A pulse, will read from Excel file
      OnePulse.init(); OnePulse.Header(); //default setting and printer header
	  bool Endflag=false; //when reach pulse train end, this will trun true
	  bool isFinite=true;  //when come to a continuous output this will be false
   
      for(unsigned int PULSENUM=1;((PULSENUM<=99)&&(!Endflag));PULSENUM++) //read series of pulses from defined excel file
       {  
	      OnePulse.ReadExcel(PulseParameterExcelFilename.c_str(),PULSENUM); 
		  f0=OnePulse.getCenterFrequency(); //Center frequency, 1uHz resolution
	      HSR=OnePulse.getHFSR(); //Half-frequency sweeping range
	      duration=(OnePulse.getDuration())*0.001; //the front factor is in ms
	      AMP_PK=OnePulse.getAmpPeak(); //Peak Amplitude; Min 0.0, Max 1.0
	      if (f0>1.0)  //OnePulse.Display(); //do not display blanks
		   //cout<<"Data index before "<<PULSENUM<<" pulse:"<<cnt<<endl; 

         /***************************************************************/
         /* Config each pulse before start(updated as of Jan09, 2012)
		 * Use Ext. update clock
		 * Clear frequency/phase accumulator
		 * Update Control register
		 * Update Ramp Rate Clock
         /***************************************************************/
		  if (int(OnePulse.getPulseType())!=0) //Last empty row in excel parameter file
		  {
		     //Clear frequency/phase accumulator ACC2, use Mode 011 and Ext. update clock
             append(Str2Int(HexAdd(COM_ON,"1F",Bin2Hex(CR1FCLR2),opt2))); 
		     append(Str2Int(HexAdd(COM_ON,"1F",Bin2Hex(CR1F),opt1))); //Ext. update clock
             // Update Control register, see AD9852 datasheet
	         append(Str2Int(HexAdd(COM_ON,"20",Bin2Hex(CR20),opt1)));
	         append(Str2Int(HexAdd(COM_ON,"1E",Bin2Hex(CR1E),opt1)));
	         append(Str2Int(HexAdd(COM_ON,"1D",Bin2Hex(CR1D),opt1)));
             //Update Ramp Rate Clock: in case this is different for each pulse, send it later
	         string sRRC=RRC(1.0/RampRate,DDSCLK); 
		     UpdateRRC(this, sRRC, opt1, COM_ON); 
		  }

		 /***************************************************************/
        /* Select and compute each pulse
        /***************************************************************/
	     switch (OnePulse.getPulseType()) {  //case swicthing between pulse shapes

		  case 1:   //Adiabatic Rapid Passage
                 // Update Frequency Tunning Word once
	             f_start=f0-HSR; //start frequency  
	             sFTW=FTW(f_start,DDSCLK);  
				 UpdateFTW(this, sFTW, opt1, COM_ON);

                //Real-time update delta frequency word and amplitude
	             UPDATE_REG_SIZE=8; 
	             sections=(unsigned int)floor(duration*FIFOCLK/UPDATE_REG_SIZE); 

                 for (unsigned int i=1;i<=sections;i++)
	                { 
	                   //Frequency start-end difference for each section
	                   DFLS=Freq(f0,HSR,(double)i/sections)-Freq(f0,HSR,(double)(i-1)/sections); 
	                   //Frequency difference at each Ramp Rate Update within each section
	                   DFLS=DFLS/((duration/sections)*RampRate);	 

	                   sDFW=DFW(DFLS,DDSCLK);  
					   UpdateDFW(this, sDFW, opt1, COM_ON);
          
		               //Update amplitude once
	                   sATW=ATW(SIN2(AMP_PK, (double)i/sections));  
	                   UpdateATW(this, sATW, opt1, opt2, COM_ON); //update output once
	                } 

				/*************************************************************
	             * Always set delta frequency word and amplitude to 0              
	             * This will also allow the last section to finish output RF */
	   	        sDFW=DFW(0.0,DDSCLK);  //ZERO DFW
		        sATW=ATW(0.0); //ZERO AMP
                UpdateDFW(this, sDFW, opt1, COM_ON);
	            UpdateATW(this, sATW, opt1, opt2, COM_ON); //update output once  
			 break;
      
           case 2:  //PI pulse
                // Update Frequency Tunning Word once
	             f_start=f0; //start frequency  
	             sFTW=FTW(f_start,DDSCLK);  
				 UpdateFTW(this, sFTW, opt1, COM_ON);

                //Real-time update delta frequency word and amplitude
	             UPDATE_REG_SIZE=2; 
	             sections=(unsigned int)floor(duration*FIFOCLK/UPDATE_REG_SIZE); 

                 for (unsigned int i=1;i<=sections;i++)
	                {         
		               //Update amplitude once
	                   sATW=ATW(Blackman(AMP_PK, (double)i/sections));  //cout<<sATW<<"\n";
                       UpdateATW(this, sATW, opt1, opt2, COM_ON); //update output once
	                } 

                /*************************************************************
	             * Always set amplitude to 0               
	             * This will also allow the last section to finish output RF */
		        sATW=ATW(0.0); //ZERO AMP
                UpdateATW(this, sATW, opt1, opt2, COM_ON); //update output once    
		   break; 

	     case 3:   //Composite Blackman PI
		         // Update Frequency Tunning Word once
	             f_start=f0; //start frequency  
	             sFTW=FTW(f_start,DDSCLK);  
				 UpdateFTW(this, sFTW, opt1, COM_ON);  

				 //Loop 3 times, 2nd time write a 90 degree phase offset
				 for (unsigned icn=1; icn<=3; icn++)
				 {
						//Clear frequency accumulator ACC1 and return to begining frequency FTW1
                        append(Str2Int(HexAdd(COM_ON,"1F",Bin2Hex(CR1FCLR1),opt2))); 
		                append(Str2Int(HexAdd(COM_ON,"1F",Bin2Hex(CR1F),opt1))); //Ext. Update clock

				        //Update  Phase register once    
					    if (icn==2)  CompositePIPhase=0.5; //shift the 2nd pulse by 90 degrees
				        sPHR=PHR(CompositePIPhase, PI); //PHR(double phase, double unit)
	                    UpdatePHR(this, sPHR, opt1, COM_ON); 

                        //Real-time update delta frequency word and amplitude
	                    UPDATE_REG_SIZE=2; 
						if (icn==1||icn==3)  //the 1st and 3rd pulse are pi/2 pulses
							 sections=(unsigned int)floor((duration/2.0)*FIFOCLK/UPDATE_REG_SIZE);  //half duration
						else
	                         sections=(unsigned int)floor(duration*FIFOCLK/UPDATE_REG_SIZE);  //full duration for 2nd pulse

                        for (unsigned int i=1;i<=sections;i++)
	                        {         
		                        //Update amplitude once
	                            sATW=ATW(Blackman(AMP_PK, (double)i/sections));  //cout<<sATW<<"\n";
                                UpdateATW(this, sATW, opt1, opt2, COM_ON); //update output once
	                        } 

                        /*************************************************************
	                     * Always set amplitude to 0               
	                     * This will also allow the last section to finish output RF */
		                 sATW=ATW(0.0); //ZERO AMP
                         UpdateATW(this, sATW, opt1, opt2, COM_ON); //update output once
						/***************************************************************************/
				 }
			break;

		 case 4:   //Composite Adiabatic Rapid Passage
			     // Update Frequency Tunning Word once
	             f_start=f0-HSR; //start frequency  
	             sFTW=FTW(f_start,DDSCLK);  
				 UpdateFTW(this, sFTW, opt1, COM_ON); 

				 //Loop ARP pulse CompositeN times, each time clear frequency/phase register and write a phase offset
				 for (unsigned icn=1; icn<=CompositeN; icn++)
				 {
					   /***************************************************************************/ 
					   //Clear frequency accumulator ACC1 and return to begining frequency FTW1
                       append(Str2Int(HexAdd(COM_ON,"1F",Bin2Hex(CR1FCLR1),opt2))); 
		               append(Str2Int(HexAdd(COM_ON,"1F",Bin2Hex(CR1F),opt1))); //Ext. Update clock 

				       //Update Phase adjust register once              
				       sPHR=PHR(CompositePhase(icn,CompositeN), PI); //PHR(double phase, double unit)
	                   UpdatePHR(this, sPHR, opt1, COM_ON); 
					   //cout<<"Phase:"<<CompositePhase(icn,CompositeN)<<" String: "<<sPHR<<endl;

                       //Real-time update delta frequency word and amplitude
	                   UPDATE_REG_SIZE=8; 
	                   sections=(unsigned int)floor(duration*FIFOCLK/UPDATE_REG_SIZE); 

                       for (unsigned int i=1;i<=sections;i++)
	                       { 
	                            //Frequency start-end difference for each section
	                            DFLS=Freq(f0,HSR,(double)i/sections)-Freq(f0,HSR,(double)(i-1)/sections); 
	                            //Frequency difference at each Ramp Rate Update within each section
	                            DFLS=DFLS/((duration/sections)*RampRate);	 

	                            sDFW=DFW(DFLS,DDSCLK);
					            UpdateDFW(this, sDFW, opt1, COM_ON);
          
		                        //Update amplitude once
	                            sATW=ATW(SIN2(AMP_PK, (double)i/sections));  
	                            UpdateATW(this, sATW, opt1, opt2, COM_ON); //update output once
	                        } 

				        /**********************************************************
	                     * Always set delta frequency word and amplitude to 0              
	                     * This will also allow the last section to finish output RF */
	   	                sDFW=DFW(0.0,DDSCLK);  //ZERO DFW
		                sATW=ATW(0.0); //ZERO AMP
                        UpdateDFW(this, sDFW, opt1, COM_ON);
	                    UpdateATW(this, sATW, opt1, opt2, COM_ON); //update output once
						/***************************************************************************/
				 }
			break;


		  case 5:   //Composite Blackman PI/2
		         // Update Frequency Tunning Word once
	             f_start=f0; //start frequency  
	             sFTW=FTW(f_start,DDSCLK);  
				 UpdateFTW(this, sFTW, opt1, COM_ON);  

				 //Loop 2 times, 2nd time write a 90 degree phase offset
				 for (unsigned icn=1; icn<=2; icn++)
				 {
					   //Clear frequency accumulator ACC1 and return to begining frequency FTW1
                        append(Str2Int(HexAdd(COM_ON,"1F",Bin2Hex(CR1FCLR1),opt2))); 
		                append(Str2Int(HexAdd(COM_ON,"1F",Bin2Hex(CR1F),opt1))); //Ext. Update clock 

				        //Update  Phase register once    
					    if (icn==2)  
						{
							CompositePIPhase=0.5; //shift the 2nd pulse by 90 degrees
							sPHR=PHR(CompositePIPhase, PI); //PHR(double phase, double unit)
							UpdatePHR(this, sPHR, opt1, COM_ON); 
						}

                        //Real-time update delta frequency word and amplitude
	                    UPDATE_REG_SIZE=2; 
						sections=(unsigned int)floor((duration)*FIFOCLK/UPDATE_REG_SIZE);  //half duration

                        for (unsigned int i=1;i<=sections;i++)
	                        {         
		                        //Update amplitude once
	                            sATW=ATW(Blackman(AMP_PK, (double)i/sections));  //cout<<sATW<<"\n";
								//sATW=ATW(1.0);  
                                UpdateATW(this, sATW, opt1, opt2, COM_ON); //update output once
	                        } 

                        /*************************************************************
	                     * Always set amplitude to 0               
	                     * This will also allow the last section to finish output RF */
		                 sATW=ATW(0.0); //ZERO AMP
                         UpdateATW(this, sATW, opt1, opt2, COM_ON); //update output once
						/***************************************************************************/
				 }
			break;

		case 6:   //Half Adiabatic Rapid Passage, pulse stops at full intensity
                 // Update Frequency Tunning Word once
	             f_start=f0-HSR; //start frequency  
	             sFTW=FTW(f_start,DDSCLK);  
				 UpdateFTW(this, sFTW, opt1, COM_ON);

                //Real-time update delta frequency word and amplitude
	             UPDATE_REG_SIZE=8; 
	             sections=(unsigned int)floor(duration*FIFOCLK/UPDATE_REG_SIZE); 

                 for (unsigned int i=1;i<=unsigned int(sections/2);i++)
	                { 
	                   //Frequency start-end difference for each section
	                   DFLS=Freq(f0,HSR,(double)i/sections)-Freq(f0,HSR,(double)(i-1)/sections); 
	                   //Frequency difference at each Ramp Rate Update within each section
	                   DFLS=DFLS/((duration/sections)*RampRate);	 

	                   sDFW=DFW(DFLS,DDSCLK);  
					   UpdateDFW(this, sDFW, opt1, COM_ON);
          
		               //Update amplitude once
	                   sATW=ATW(SIN2(AMP_PK, (double)i/sections));  
	                   UpdateATW(this, sATW, opt1, opt2, COM_ON); //update output once
	                } 

				/*************************************************************
	             * Always set delta frequency word and amplitude to 0              
	             * This will also allow the last section to finish output RF */
	   	        sDFW=DFW(0.0,DDSCLK);  //ZERO DFW
		        sATW=ATW(0.0); //ZERO AMP
                UpdateDFW(this, sDFW, opt1, COM_ON);
	            UpdateATW(this, sATW, opt1, opt2, COM_ON); //update output once  
			 break;

			case 7:   //Composite Half AFPs
			     // Update Frequency Tunning Word once
	             f_start=f0-HSR; //start frequency  
	             sFTW=FTW(f_start,DDSCLK);  
				 UpdateFTW(this, sFTW, opt1, COM_ON); 

				 append(Str2Int(HexAdd(COM_ON,"1F",Bin2Hex(CR1FCLR1),opt2))); 

				 //Loop ARP pulse CompositeN times, each time clear frequency/phase register and write a phase offset
				 for (unsigned icn=1; icn<=2; icn++)
				 {
					   /***************************************************************************/ 
					   //Clear frequency accumulator ACC1 and return to begining frequency FTW1
                       
		               append(Str2Int(HexAdd(COM_ON,"1F",Bin2Hex(CR1F),opt1))); //Ext. Update clock 

					   //Real-time update delta frequency word and amplitude
					   UPDATE_REG_SIZE=8; 
					   sections=(unsigned int)floor(duration*FIFOCLK/UPDATE_REG_SIZE); 

					   //Update  Phase register once    
					    if (icn==1)  
						{
							CompositePIPhase=0.0; //shift the 2nd pulse by 90 degrees
							sPHR=PHR(CompositePIPhase, PI); //PHR(double phase, double unit)
							UpdatePHR(this, sPHR, opt1, COM_ON);

						   for (unsigned int i=1;i<=unsigned int(sections/2);i++)
							   { 
									//Frequency start-end difference for each section
									DFLS=Freq(f0,HSR,(double)i/sections)-Freq(f0,HSR,(double)(i-1)/sections); 
									//Frequency difference at each Ramp Rate Update within each section
									DFLS=DFLS/((duration/sections)*RampRate);	 

									sDFW=DFW(DFLS,DDSCLK);
									UpdateDFW(this, sDFW, opt1, COM_ON);
          
									//Update amplitude once
									sATW=ATW(SIN2(AMP_PK, (double)i/sections));  
									UpdateATW(this, sATW, opt1, opt2, COM_ON); //update output once
								}
						}

				       //Update  Phase register once    
					    if (icn==2)  
						{
							CompositePIPhase=0.5; //shift the 2nd pulse by 90 degrees
							sPHR=PHR(CompositePIPhase, PI); //PHR(double phase, double unit)
							UpdatePHR(this, sPHR, opt1, COM_ON); 

							 for (unsigned int i=unsigned int(sections/2);i<=sections;i++)
							   { 
									//Frequency start-end difference for each section
									DFLS=Freq(f0,HSR,(double)i/sections)-Freq(f0,HSR,(double)(i-1)/sections); 
									//Frequency difference at each Ramp Rate Update within each section
									DFLS=DFLS/((duration/sections)*RampRate);	 

									sDFW=DFW(DFLS,DDSCLK);
									UpdateDFW(this, sDFW, opt1, COM_ON);
          
									//Update amplitude once
									sATW=ATW(SIN2(AMP_PK, (double)i/sections));  
									UpdateATW(this, sATW, opt1, opt2, COM_ON); //update output once
								} 
						}
           
				        /**********************************************************
	                     * Always set delta frequency word and amplitude to 0              
	                     * This will also allow the last section to finish output RF */
	   	                sDFW=DFW(0.0,DDSCLK);  //ZERO DFW
		                sATW=ATW(0.0); //ZERO AMP
                        UpdateDFW(this, sDFW, opt1, COM_ON);
	                    UpdateATW(this, sATW, opt1, opt2, COM_ON); //update output once
						/***************************************************************************/
				 }
			break;

	      case 7778: //output a continous pulse with no-stop, used for troubleshooting only
	            /***************************************************************/
                /* This is serial# of 10W amplifier, used as a safety password
                /* turn off or disconnect the amplifier
				/* Otherwise things in the chamber can be damaged!
				/* To enable continuous output: (1)write 7778 in PulseType, 
				                                (2)comment line replace(cnt-1, 0);
				/* To terminate continuous output: enable line replace(cnt-1, 0);
                /***************************************************************/
				 cout<<"Turn off or disconnect the amplifier!!!"<<endl;
				 cout<<"continuous output RF"<<endl
					 <<"amplitude "<<AMP_PK<<endl
					 <<"frequency "<<f0<<endl;

                // Update Frequency Tunning Word once
	             f_start=f0;  
	             sFTW=FTW(f_start,DDSCLK);    
				 UpdateFTW(this, sFTW, opt1, COM_ON);
                
				// Update Amplitude once
				sATW=ATW(AMP_PK); 
                UpdateATW(this, sATW, opt1, opt2, COM_ON); //update output once

                Endflag=true; //terminate the loop and do not read more pulse paramaters
				isFinite=false;//indentified as continuous output
		  break;

	  default:
			   Endflag=true; //exit the loop
	 } //end of switching options

	 if ((!Endflag)&&(isFinite))
	 {
	      //Clear frequency/phase accumulator ACC2
          append(Str2Int(HexAdd(COM_ON,"1F",Bin2Hex(CR1FCLR2),opt2))); 
		  append(Str2Int(HexAdd(COM_ON,"1F",Bin2Hex(CR1F),opt1))); //Ext. update clock 
 
          //When the pulse ends, check whether to disable FIFO update CLK
          if(OnePulse.getPulseTerminator())
		        append(Str2Int(HexAdd(COM_STOP,"23","00",opt3)));     
	 }
	 
   } //end of loop for all pulses 

    replace(cnt-1, 0); //replace last element with 0 to disable FIFO update CLK and write zeros on all lines
    cout<<"DDS::Data computation complete..."<<endl;
	return true;
} 

#endif



//AD5892 Engineer comment on phases for nonlinear frequency chirps
/* Do we need to clear frequency/phase accumulator before each pulse, 
and rewrite frequency word and phase adjust register? 
Or can we just write phase register 1 for each pulse?
Answer: Our DDS parts are almost always 'phase continuous', 
meaning the phase accumulator will keep it's previous value when register setting are changed. 
The only time it isn't is when the Clear Phase Accumulator bit gets set which clears it. 
The value programmed into the Phase Registers is a phase offset added after the Phase Accumulator. 
If you want the chirp to start at a fixed phase for each chirp, 
then yes you will most likely need to clear it each time. 
But if you only need to maintain a phase offset between the chirps then clearing 
the phase accumulator isn't needed.
*/