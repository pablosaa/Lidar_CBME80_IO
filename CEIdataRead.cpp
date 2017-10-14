/* Program to read the ceilometer CBME80 data. The ceilometer is installed next to the radiometer ADMIRARI and the measurements must be related to the observation angle of the radiometer. The data file does not include the observation angle at which the measurements were done.
   Information: pablosaa@uni-bonn.de
   Person: Pablo Saavedra Garfias
   2010
*/

#include<iostream>
#include<fstream>
#include<string>
#include<iomanip>
#include<mex.h>

using namespace std;

void mexFunction(int nlhs, mxArray *plhs[],int nrhs, const mxArray *prhs[]){

  /* Declaring the output structure variables */
  mxArray *INVAR, *OUTVAR[2];
  const char *field_name[] = {"Time_hr","Ran","BSp","CB1","CB2","CD1","CD2"};
  mxArray *time_value, *range_value, *profile_value, *cloudb1, *cloudb2,*cloudD1, *cloudD2;


  ifstream fin;
  char *FileName; // input file, including path!
  string pivot;
  string garbage;
  int Statvar;
  int FileLength,i,j,k,l,idx=-1, iProf=0;
  int const nProf=5800, nRan=360; //252 360;
  double Time_ceiHr[nProf], Secs;
  int Year, Month, Day, Hour, Minu;
  string CeiloID, Status;
  int CloudBase1, CloudDepth1,CloudBase2, CloudDepth2,MaxRange;
  double CB1[nProf],CB2[nProf],CD1[nProf],CD2[nProf];
  int BSProfile;
  // Using dynamic memory for large matrices: (Otherwise MATLAB problems!)
  double **AllBSP;
  AllBSP = new double*[nProf];

  /* Checking output variables */
  if (nlhs!=1) mexErrMsgTxt("One output variable needed!");
  /* Checking input variables */
  if (nrhs>1) mexErrMsgTxt("One input file is required, see help!");
  if (nrhs==0){ /* open file browser */
    INVAR = mxCreateString("*.dat");
    Statvar = mexCallMATLAB(2,OUTVAR, 1,&INVAR,"uigetfile");
    if (Statvar!=0) mexErrMsgTxt("File Selection not possible");
    FileLength = mxGetN(OUTVAR[0])+mxGetN(OUTVAR[1])+1;
    /* passing the path */
    FileName = (char *) mxCalloc(FileLength,sizeof(char));
    mxGetString(OUTVAR[1],FileName,FileLength);
    /* passing the file name */
    FileLength = mxGetN(OUTVAR[0])+1;
    mxGetString(OUTVAR[0],FileName+mxGetN(OUTVAR[1]),FileLength);
  }
  else{
    if(nrhs==1 & mxIsChar(prhs[0])!=1)
      mexErrMsgTxt("Input file must be a string.");
    FileLength = mxGetN(prhs[0])+1;
    FileName = (char *) mxCalloc(FileLength,sizeof(char));
    mxGetString(prhs[0],FileName,FileLength);
  }
  fin.open(FileName,ios::in);
  if (!fin){
    cout<< "Ceilometer file cannot be opened.\n";
    mwSize dims[2]={1,1};
    plhs[0] = mxCreateStructArray(2,dims,7,field_name);
    return;
  }
  mexPrintf("Open archive %s\n",FileName);

  while(!fin.eof()){
    getline(fin, pivot);
    idx=pivot.find("<");
    if(idx<0) continue;

    sscanf(pivot.substr(idx,20).c_str(),"<%04d%02d%02d%02d%02d%6f>",
  	   &Year,&Month,&Day,&Hour,&Minu,&Secs);
  
    Time_ceiHr[iProf] = (double) Hour+ ((double) Minu)/60+ Secs/3600;

    fin>>CeiloID;
    fin>>Status;
    fin>>garbage;
    if(garbage.find("/")!=0){
      sscanf(garbage.c_str(),"%d",&CloudBase1);
      CB1[iProf]= (double) CloudBase1; }
    else CB1[iProf]= mxGetNaN();
    fin>>garbage;
    if(garbage.find("/")!=0){
      sscanf(garbage.c_str(),"%d",&CloudDepth1);
      CB2[iProf]= (double) CloudBase2; }
    else CB2[iProf]= mxGetNaN();
    fin>>garbage;
    if(garbage.find("/")!=0){
      sscanf(garbage.c_str(),"%d",&CloudBase2);
      CD1[iProf]= (double) CloudDepth1; }
    else CD1[iProf]= mxGetNaN();
    fin>>garbage;
    if(garbage.find("/")!=0){
      sscanf(garbage.c_str(),"%d",&CloudDepth2);
      CD2[iProf]= (double) CloudDepth2; }
    else CD2[iProf]= mxGetNaN();

    for(i=0;i<4;i++) fin>>garbage;
    sscanf(garbage.c_str(),"%d",&MaxRange);

    fin.ignore(100,'\n');
    fin.ignore(100,'\n');
    fin.ignore(100,'\n');
    AllBSP[iProf] = new double[nRan];
    for(i=0;i<18;i++)
      for(j=0;j<20;j++){
  	fin>>garbage;
  	sscanf(garbage.c_str(),"%3x",&BSProfile);
  	AllBSP[iProf][(i*20)+j] = (double) BSProfile;

      }
    fin.ignore(50,'\n');
    iProf++;
  }  // end of loop while !eof
  fin.close();
  mxFree(FileName);
  //mexPrintf("Finnish to load data\n");

  /* Create a iProf-by-360 array  */ 
  mwSize dims[2]={1,1};
  plhs[0] = mxCreateStructArray(2,dims,7,field_name);
  if (plhs[0]==NULL) mexErrMsgTxt("Error while creation of output structure\n");
  
  //mexPrintf("Populating output variable\n");

  /* Populate the output variables */
  time_value = mxCreateDoubleMatrix(iProf,1,mxREAL);
  profile_value = mxCreateDoubleMatrix(iProf,nRan,mxREAL);
  range_value = mxCreateDoubleMatrix(nRan,1,mxREAL);
  cloudb1 = mxCreateDoubleMatrix(iProf,1,mxREAL);
  cloudb2 = mxCreateDoubleMatrix(iProf,1,mxREAL);
  cloudD1 = mxCreateDoubleMatrix(iProf,1,mxREAL);
  cloudD2 = mxCreateDoubleMatrix(iProf,1,mxREAL);

  k=0;   // aux index to create the range variable
  for(i=0;i<nRan;i++){
    for(j=0;j<iProf;j++){
      *(mxGetPr(profile_value)+((i*iProf)+j)) = AllBSP[j][i];
      mxSetField(plhs[0],0,"BSp",profile_value);
      if (i<1){  // this for variables which do not depend on range:
   	*(mxGetPr(time_value)+j) = (double) Time_ceiHr[j];
   	mxSetField(plhs[0],0,"Time_hr",time_value);
   	*(mxGetPr(cloudb1)+j) = CB1[j];
   	mxSetField(plhs[0],0,"CB1",cloudb1);
   	*(mxGetPr(cloudb2)+j) = CB2[j];
   	mxSetField(plhs[0],0,"CB2",cloudb2);
   	*(mxGetPr(cloudD1)+j) = CD1[j];
   	mxSetField(plhs[0],0,"CD1",cloudD1);
   	*(mxGetPr(cloudD2)+j) = CD2[j];
   	mxSetField(plhs[0],0,"CD2",cloudD2);
      }
    }
    /* Populating the range vector Ran (see CBME80 manual) */
    if (i<160) *(mxGetPr(range_value)+i) = (double) (k+=10);
    else *(mxGetPr(range_value)+i) = (double) (k+=30);
    mxSetField(plhs[0],0,"Ran",range_value);
  }
  
  // mxDestroyArray(time_value);
  // mxDestroyArray(profile_value);
  // mxDestroyArray(range_value);
  // mxDestroyArray(cloudb1);
  // mxDestroyArray(cloudb2);
  // mxDestroyArray(cloudD1);
  // mxDestroyArray(cloudD2);
  //return 0;
}

// end of program.
