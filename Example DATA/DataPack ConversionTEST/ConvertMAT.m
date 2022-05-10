function Pack=ConvertMAT(datafile)

%function to convert a text file of DataPack members to matlab struct

%to convert this program into a standalone application, use mcc compiler:
%>> mcc -mv ConvertMat.m

MMCfolder='C:/MMC_SYSINT/LAB_DAT/';
filename=[MMCfolder datafile];



fid=fopen([filename '.DATXT'], 'r');
%////////////////////////////////////////////////////////////////////////////////////////
DataPack.Timing_seqid=fscanf(fid,'%u',1);  
DataPack.Timing_loops=fscanf(fid,'%s',1); 

DataPack.Coil_Cur=fscanf(fid,'%f',8);    

DataPack.HV_Por=fscanf(fid,'%u',1);             
DataPack.HV_Vol=fscanf(fid,'%f',1);
DataPack.HV_Leak=fscanf(fid,'%f',1);            
 
AMNumChls=16; AMNumShots=5; 
DataPack.Ana_Mon=zeros(AMNumShots, AMNumChls); 
for j=1:AMNumChls
    for i=1:AMNumShots
        DataPack.Ana_Mon(i,j)=fscanf(fid,'%f',1);
    end
end
DataPack.Ana_Mon=DataPack.Ana_Mon';

PDANpixel=50; PDANshot=2;
DataPack.PDA_Signal=zeros(PDANshot, PDANpixel);
for j=1:PDANpixel
    for i=1:PDANshot
        DataPack.PDA_Signal(i,j)=fscanf(fid,'%f',1);
    end
end
DataPack.PDA_Signal=DataPack.PDA_Signal';

DataPack.Room_Temp=fscanf(fid,'%f',1);
%////////////////////////////////////////////////////////////////////////////////////////
fclose(fid);

%save the DataPack
save(filename, 'DataPack');

Pack=DataPack;