/***************************************************************

*  This class define data and methods for a pulse train

*  IThe array will automatically expand its size when necessary

*  Last modified 2011/12/09 by KZ

****************************************************************/


#ifndef _PulseTrain_h_included_
#define _PulseTrain_h_included_
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
#include <fstream> //datafile input/output
#pragma comment(lib, "NIDAQmx.lib")
using namespace std;

/***************************************************************/
// Constants
/***************************************************************/
const extern double  FIFOCLK; //PCI6534 FIFO UPDATE CLK [4.656613e-3, 20e6]Hz
const extern double  GPSCLK; //10MHz GPS CLK 
const extern int INCRT_SIZE; //step size for an expanion of array length

class PulseTrain{
	
  private:
     uInt32* arrPtr;        // Dynamic member
     int max;              // Maximum quantity without reallocation new storage.
     int cnt;              // Number of present array elements 
     void expand( int newMax);   // Helps enlarge the array

  public:
     PulseTrain( int n );         // Constructor
     PulseTrain( int n, uInt32 val);
     ~PulseTrain(); // Destructor

	 bool Compute(string PulseParameterExcelFilename);//Compute pulse data, see "Composer.h"  
	 int  length() const { return cnt; } //get array length
	 uInt32* getArray() {return arrPtr; } //get entire data array

	 bool WriteDataFile(string filename);//write Data to file
	 bool ReadDataFile(const string filename); //read Data from file, another constructor for PulseTrain

     PulseTrain(const PulseTrain& src);   // Copy constructor
     PulseTrain& operator=( const PulseTrain&); // Assignment

     uInt32& operator[](int i);        // Subscript operator.
     uInt32  operator[](int i) const;

     // Methods to append a floating-point number 
     // or an array of floating-point numbers:
     void append( uInt32 val);
     void append( const PulseTrain& v); 
     PulseTrain& operator+=( uInt32 val)
     {
         append( val);   return *this;
     }

     PulseTrain& operator+=( const PulseTrain& v) 
     {
         append(v);   return *this;
     }

     // Methods to insert a floating-point number 
     // or an array of floating-point numbers:
     bool insert( uInt32 val, int pos); 
     bool insert( const PulseTrain& v, int pos ); 

     bool remove(int pos);     // Delete at position pos.
	 bool replace(int pos, uInt32 val);     // replace at position pos.

};

PulseTrain::PulseTrain( int n )
{
   max = n;   cnt = 0;  
   arrPtr = new uInt32[max];
}

PulseTrain::PulseTrain(int n, uInt32 val)
{
   max = n;   cnt = 0;
   arrPtr  = new uInt32[max];
   for( int i=0; i < max; ++i)
       arrPtr[i] = val;
}

// Copy Constructor:
PulseTrain::PulseTrain(const PulseTrain& src)
{
   max = src.max;
   cnt = src.cnt;
   arrPtr = new uInt32[max];

   for( int i = 0; i < cnt; i++ )
      arrPtr[i] = src.arrPtr[i];
}

// Destructor:
PulseTrain::~PulseTrain()
{
   delete[] arrPtr;
}

// Assignment:
PulseTrain& PulseTrain::operator=( const PulseTrain& src )
{
   if( this != &src )              // No self assignment!
   {
      max = src.max; 
      cnt = src.cnt;

      delete[] arrPtr;             // Release memory,
      arrPtr = new uInt32[max];     // reallocate and
      for( int i=0; i < cnt; i++)  // copy elements.
        arrPtr[i] = src.arrPtr[i];
   }
   return *this;
}

// Subscript operator for objects, that are not const:
uInt32& PulseTrain::operator[]( int i ) 
{
   if( i < 0 || i >= cnt )       // Range checking
   {
      cerr << "\n class PulseTrain: Out of Range! "; 
      exit(1);
   }
   return arrPtr[i];
}

// Subscript operator for const objects:
uInt32 PulseTrain::operator[]( int i ) const
{
   if( i < 0 || i >= cnt )       // Range checking
   {
      cerr << "\n class PulseTrain: Out of Range! "; 
      exit(1);
   }
   return arrPtr[i];
}

//  ---  The new functions  --- 

// Private auxiliary function to enlarge the array.
void PulseTrain::expand( int newMax)
{
   if( newMax == max)
      return;

   max = newMax;
   if( newMax < cnt)
      cnt = newMax;
   uInt32 *temp = new uInt32[newMax];
   for( int i = 0; i < cnt; ++i)
      temp[i] = arrPtr[i];

   delete[] arrPtr;
   arrPtr = temp;
   cout<<"PulseTrain expands by 1 step size, increase initial sample size to speed up"<<endl;
}

// Append floating-point number or an array of floats.
void PulseTrain::append( uInt32 val)
{ 
   if( cnt+1 > max)
       expand( cnt+INCRT_SIZE);
 
   arrPtr[cnt++] = val;
}

void PulseTrain::append( const PulseTrain& v)
{ 
   if( cnt + v.cnt > max)
       expand( cnt + v.cnt);
   
   int count = v.cnt;            // Necessary if v == *this
                                 
   for( int i=0; i < count; ++i)
     arrPtr[cnt++] = v.arrPtr[i];
}

// Insert a float or an array of floats
bool PulseTrain::insert( uInt32 val, int pos)
{
   return insert( PulseTrain(1,val), pos);
}

bool PulseTrain::insert( const PulseTrain& v, int pos )
{
   if( pos < 0 || pos >= cnt)
      return false;              // Invalid position

   if( max < cnt + v.cnt)
      expand(cnt + v.cnt);

   int i; 
   for( i = cnt-1; i >= pos; --i)    // Shift up 
      arrPtr[i+v.cnt] = arrPtr[i];   // starting at pos

   for( i = 0; i < v.cnt; ++i)       // Fill gap. 
      arrPtr[i+pos] = v.arrPtr[i];

   cnt = cnt + v.cnt;

   return true;
}

// To delete
bool PulseTrain::remove(int pos)
{ 
   if( pos >= 0 && pos < cnt)
   {
      for( int i = pos; i < cnt-1; ++i) 
         arrPtr[i] = arrPtr[i+1];
      --cnt;
      return true;
   }
   else
      return false;
}

// To replace
bool PulseTrain::replace(int pos, uInt32 val)
{
	 if( pos >= 0 && pos < cnt)
       {
         arrPtr[pos] = val;
         return true;
       }
	 else return false;
}

bool PulseTrain::WriteDataFile(string filename) //write Data to file
{
   ofstream datalog( filename.c_str(),ios::out | ios::trunc); //delete previous log then write the new one
   //ofstream datalog( filename.c_str(),ios::out | ios::app); //keep previous log then append the new one, for debugging
   int i=0;
   while( i < cnt)
         {  
	/*	    string ss; //write HEX,DEC,BIN types for debugging
			ss=HexN(arrPtr[i],8);
			datalog<<ss.c_str()<<"  "<<Hex2Bin(ss).c_str()<<"  "<<arrPtr[i]<<"  "<<i<<endl; 
    */ 
			datalog<<hex<<arrPtr[i]<<endl;
            if( !datalog)   break;
            else   ++i;
         }
   datalog.close();
   if( i < cnt)
   {
      cerr << "Error writing to file " << filename << endl;
      return false;
   }
   cout<<dec<<cnt<<" samples written to file"<<endl<<filename<<endl;
   return true;
}

bool PulseTrain::ReadDataFile(const string filename) //read Data from file, another constructor for PulseTrain
{
	int  i=0;  
	ifstream ppFile; //define a file pointer
    uInt32 Trolley=0; //intermediate data for file input
	ppFile.open(filename.c_str());
    while(!ppFile.eof())
          {
             if( i+1 > max)  expand(max+INCRT_SIZE); //enlarge the array size while data array outranging
			 ppFile>>hex>>Trolley;
			 arrPtr[i]=Trolley;
             i++;
          }
    ppFile.close();
	i--; 
	cnt=i;//set PulseTrain length
	if((i<max)&&(i>1))   return true;
	else   return false;
}


#endif

