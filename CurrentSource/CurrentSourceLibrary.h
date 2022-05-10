/* CurrentSourceLibrary.h */
#pragma once

/* Update all the DACs with the decimal inputs listed here.  Most likely used for 
   calibration purposes.  The channels corresponding to the coils listed are stored 
   in Settings.xls file. Returns 1 if there is an error where the DAC does not update
   and -1 if there is an error where the DAC updates. */
extern int UpdateChannels (int Nx,
						   int Ny,
						   int Nz,
						   int Nxx,
						   int Nyy,
						   int Nzz,
						   int Nxy,
						   int Nxz);

/* Updates all currents in parallel.  Determines the appropriate current required 
   to make the magnitude B fields from the transfer function.  The channels corresponding
   to the coils listed are stored in Settings.xls file. Returns 1 if there is an error 
   where the DAC does not update and -1 if there is an error where the DAC updates. */
extern int UpdateAllCurrents (double Ix, 
							  double Iy, 
							  double Iz, 
							  double Ixx, 
							  double Iyy, 
							  double Izz, 
							  double Ixy,
							  double Ixz);

/* Determines the appropriate current required to create BFieldMag given in Gauss.
   The calibration matrix will be used and is stored in the Settings.xls file.  
   The correct decimal input is determined from the transfer function for the 
   current source.  Returns 1 if there is an error where the DAC does not update and 
   -1 if there is an error where the DAC updates. */
extern int UpdateAllBFields (double Bx, 
							 double By, 
							 double Bz, 
							 double Bxx, 
							 double Byy, 
							 double Bzz, 
							 double Bxy,
							 double Bxz);