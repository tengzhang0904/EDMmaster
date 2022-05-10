/***************************************************************

*  C++ Header file, define all DDS functions

*  Last modified 2012/01/12 by KZ

****************************************************************/


#ifndef _DDSfunc_h_included_
#define _DDSfunc_h_included_
#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable: 4996)

#include <stdio.h>
#include <math.h>
#include <iostream>
#include <string>
using namespace std;

#define PI   3.141592653589793


/***************************************************************/
// Conversion Functions
/***************************************************************/

//Function to convert unsigned long int to hex
string Dec2Hex(unsigned __int64 num) 
{
   static char str[1]; 
   sprintf(str,"%llX",num); //long long hex type
   return str;
}

//Function to convert long int to hex (return string of size N)
string HexN(unsigned __int64 num, unsigned __int8 N) 
{
   string ss=Dec2Hex(num);
   string s="0";
   if (ss.size()<N)
       for (int i=ss.size();i<N;i++){ss=s+ss;}//fill in blanks with 0 in the front
   return ss;
}

//add three size-2 Hex to form a size-8 Hex (32bit)
string HexAdd(string PA, string PB,string PC,string PD) 
{
	return PD+PC+PB+PA; //Port D/C/B, Port A is blanked out
}

//convert hex string (size 8) to unsigned int
unsigned int Str2Int(string str) 
{
  if (str.size()<=8)
       return strtoul(str.c_str(),NULL,16); //str.c_str() convert string str to const char* type
  else 
       cout<<"Error: string size out range\n"; return 0;
}

//Function to convert size-4 binary to size-1 hex
string B2H(string str) 
{
  if (str.size()==4) 
    {    
        if(str.compare("1111") == 0) return "F";
	else if(str.compare("1110") == 0) return "E";
	else if(str.compare("1101")== 0) return "D";
	else if(str.compare("1100")== 0) return "C";
	else if(str.compare("1011")== 0) return "B";
	else if(str.compare("1010")== 0) return "A";
	else if(str.compare("1001")== 0) return "9";
	else if(str.compare("1000")== 0) return "8";
	else if(str.compare("0111")== 0) return "7";
	else if(str.compare("0110")== 0) return "6";
	else if(str.compare("0101")== 0) return "5";
	else if(str.compare("0100")== 0) return "4";
	else if(str.compare("0011")== 0) return "3";
	else if(str.compare("0010")== 0) return "2";
	else if(str.compare("0001")== 0) return "1";
	else if(str.compare("0000")== 0) return "0";
	else cout<<"B2H: Input must be binary\n"; return "0";
    }
  else 
   {
	   cout<<"B2H: Input binary must be size-4\n";
	   return "0"; 
	} 
}

//Function to convert  size-1 hex to size-4 binary
string H2B(string str) 
{
  if (str.size()==1) 
    {    
        if(str.compare("F") == 0) return "1111";
	else if(str.compare("E") == 0) return "1110";
	else if(str.compare("D")== 0) return "1101";
	else if(str.compare("C")== 0) return "1100";
	else if(str.compare("B")== 0) return "1011";
	else if(str.compare("A")== 0) return "1010";
	else if(str.compare("9")== 0) return "1001";
	else if(str.compare("8")== 0) return "1000";
	else if(str.compare("7")== 0) return "0111";
	else if(str.compare("6")== 0) return "0110";
	else if(str.compare("5")== 0) return "0101";
	else if(str.compare("4")== 0) return "0100";
	else if(str.compare("3")== 0) return "0011";
	else if(str.compare("2")== 0) return "0010";
	else if(str.compare("1")== 0) return "0001";
	else if(str.compare("0")== 0) return "0000";
	else cout<<"H2B: Input must be Hex\n"; return "0000";
    }
  else 
   {
	   cout<<"H2B: Input hex must be size-1\n";
	   return "NULL"; 
	} 
}

//Function to convert size-8 binary to size-2 hex
string Bin2Hex(string str) 
{
  if (str.size()==8) 
    {    
	   string s1=str.substr(0,4);string s2=str.substr(4);
       return B2H(s1)+B2H(s2);
    }
  else 
   {
	   cout<<"Bin2Hex: Input binary must be size-8\n";
	   return "NULL"; 
	} 
}

//Function to convert size-8 hexto size-32 binary
string Hex2Bin(string s) 
{
  if (s.size()==8) 
    {  
	   string str[8];
	   for (int i=0;i<8;i++) {str[i]=s[i];}
       return H2B(str[0])+H2B(str[1])+"-"+H2B(str[2])+H2B(str[3])+"-"+H2B(str[4])+H2B(str[5])+"-"+H2B(str[6])+H2B(str[7]);			   
    }
  else if (s.size()==16) 
    {  
	   string str[8];
	   for (int i=0;i<8;i++) {str[i]=s[8+i];}
       return H2B(str[0])+H2B(str[1])+"-"+H2B(str[2])+H2B(str[3])+"-"+H2B(str[4])+H2B(str[5])+"-"+H2B(str[6])+H2B(str[7]);			   
    }
  
  else 
   {
	   cout<<"Hex2Bin: Input hex must be size-8/16\n";
	   return "NULL"; 
	} 
}

//sign function
double sgn(double x) 
{
  if (x>0.0)		  return 1.0;
  else if (x<0.0)     return -1.0;
  else                return 0.0;
}

/***************************************************************/
// DDS SPECIAL FUNCTIONS
/***************************************************************/

//Function to calculate 32-bit update clock
string UDC(double period, double DDSCLK)  
{
  unsigned __int64 K=(unsigned __int64)floor(period*DDSCLK/2-1);
  if ((K>=1)&&(K<=4294967295)) //K range [1,2^32-1]
    {    
        return HexN(K,8); //return string of effective length 8, with "0"s in the front
    }
  else 
   {
	   cout<<"Error: UDC out of range\n";
	   return "00000000"; 
	}      
}


/***************************************************************/
// Frequency functions
/***************************************************************/

//Function to calculate freq(t) 
double Freq(double f0, double HSR, double tao) 
{  
	if (tao<=1.0&&tao>=0.0) 
	{
		   return f0+sgn(tao-0.5)*HSR*sqrt(1-pow(sin(PI*tao),4));
	}
    else   cout<<"Error: tao out range [0,1]\n"; return 0.0;
}

//Function to calculate dfdt
double dfdt(double HSR, double tao) 
{  
   return 2.0*PI*HSR*pow(sin(PI*tao),3)/sqrt(1+pow(sin(PI*tao),2));
}

//Function to calculate 48-bit Freq tune word
string FTW(double f,double DDSCLK)  
{
	if (f>=0.0)
    {    
		unsigned __int64 ftw=(unsigned __int64)floor(pow(2.0,48)*f/DDSCLK);//round down to an long int
        return HexN(ftw,12); 
    }
   else 
   {
	   cout<<"Error: FTW can not be negative\n";
	   return "000000000000"; 
	}
}

//Function to calculate 48-bit Delta freq word
string DFW(double df,double DDSCLK)  
{
   if (df>=0.0)
    {    
		__int64 dfw=(__int64)floor(pow(2.0,48)*df/DDSCLK);
        return HexN(dfw,12); 
    }
   else 
    {   
		__int64 dfw=(__int64)floor(pow(2.0,48)*df/DDSCLK);
		string sn=Dec2Hex(dfw);
		string sd="";
		for (int i=15;i>=4;i--) {sd=sn[i]+sd;} //remove the header MSB "FFFF"
        return sd;
    }     
}

//Function to calculate 20-bit ramp rate clock
string RRC(double period, double DDSCLK)  
{
  unsigned __int64 K=(unsigned __int64)floor(period*DDSCLK-1);
  if ((K>=1)&&(K<=1048575)) //K range [1,2^20-1]
    {    
        return HexN(K,6); //return string of effective length 6, with "0" in the front
    }
  else 
   {
	   cout<<"Error: RRC out of range\n";
	   return "000064"; //forced to ramp at 100 cycles of DDSCLK
	}      
}

/***************************************************************/
// Phase register functions
/***************************************************************/
string PHR(double phase, double unit)  //calculate phase register word
{
  int K=(int)((phase*unit)*(pow(2.0,14.0)-1)/(2.0*PI));  //14-bit phase word
  if ((K>=0)&&(K<=16383)) //K range [0,2^14-1]
    {    
        return HexN(K,4); //return string of effective length 4, with "0" in the front
    }
  else 
   {
	   cout<<"Error: Phase inout out of range\n";
	   return "0000"; 
	}      
}

double CompositePhase(unsigned int k, unsigned int N) //calculate Composite Phase
{
	int x,y,z;
	x=(int)floor((k+1)/2.0);
	y=(int)floor(k/2.0);
	z=N+1-2*x;
	//cout<<"k="<<k<<"   "<<"x="<<x<<"  "<<"y="<<y<<"  "<<"z="<<z<<endl;
	return (double)(1.0*z)*(1.0*y)/(1.0*N);
}


/***************************************************************/
// Amplitude functions
/***************************************************************/

//Function to calculate Amp(t), control DDS output voltage~ Rabi frequency ~ sqrt(RF power)
double SIN2(double Amax, double tao)   //sin^2 sweep
{  
	if ((tao<=1.0&&tao>=0.0)&&(Amax<=1.0&&Amax>=0.0))  return Amax*pow(sin(PI*tao),2);
	else    cout<<"Error: Amp or tao out range [0,1]\n"; return 0.0;
}

double Blackman(double Amax, double tao)   //Blackman sweep
{  
	if ((tao<=1.0&&tao>=0.0)&&(Amax<=1.0&&Amax>=0.0)) 
	{
		return Amax*(0.42+0.5*cos(PI*2*(tao-0.5))+0.08*cos(2*PI*2*(tao-0.5)));
	}
	else    cout<<"Error: Amp or tao out range [0,1]\n"; return 0.0;
}

double Triangle(double Amax, double tao)  //triangle sweep
{  
	if ((tao<=1.0&&tao>=0.0)&&(Amax<=1.0&&Amax>=0.0))  
	{
		if (tao<=0.5) return Amax*2.0*tao;
		else          return Amax*2.0*(1-tao);
	}
	else    cout<<"Error: Amp or tao out range [0,1]\n"; return 0.0;
}

double Stairs(double Amax, double tao)  //triangle sweep
{  
	double Nstairs=10;
	if ((tao<=1.0&&tao>=0.0)&&(Amax<=1.0&&Amax>=0.0))  
	{
		return Amax*(ceil(tao*Nstairs)/Nstairs);
	}
	else    cout<<"Error: Amp or tao out range [0,1]\n"; return 0.0;
}

//Function to calculate 12-bit Amp Tunning Word
string ATW(double amp)  
{
    unsigned __int64 atw=(unsigned __int64)floor(amp*4095);  //round down to an long int
	return  HexN(atw,4);
}


/***************************************************************/
// Misc functions
/***************************************************************/
void ClearScreen()
{
	system("CLS");
}

void Pause()
{
	system("pause");
}


#endif

