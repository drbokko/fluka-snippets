#include <time.h>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>


#include <iostream>
#include <fstream>
#include <time.h>
#include <vector>

#include <TFile.h>
#include <TROOT.h>
#include <TH1.h> 
#include <TH2.h> 
#include <TH3.h> 
#include <TMath.h>

using namespace std;

#define MAX_MATERIALS     781 //
#define RHO_STEPS        0.05
// Fine Mesh
#define MAX_BINX          155        //  (21 + 10)*5
#define MAX_BINY          225        //  (35 + 10)*5
#define MAX_BINZ          404        //  (200 + 2)*5
// var_BinSize_X   = 2E-4; // [m] 
// var_BinSize_Y   = 2E-4; // [m] 
// var_BinSize_Z   = 2.5E-3; // [m]
// var_Offset_X    = 25;
// var_Offset_Y    = 25;
// var_Offset_Z    = 2;


// double CriticalDensity     =  10.000;
double _rho0               =  19.255;
double _rho[MAX_MATERIALS];
uint   _loadedMaterials    =   0;
uint   _VOXEL_IDX[MAX_BINX][MAX_BINY][MAX_BINZ];

// Mesh - Initialiaze to zero
double   var_BinSize_X   =  0;
double   var_BinSize_Y   =  0;
double   var_BinSize_Z   =  0;
int      var_Offset_X    =  0;
int      var_Offset_Y    =  0;
int      var_Offset_Z    =  0;
int      var_BinNumber_X =  1;
int      var_BinNumber_Y =  1;
int      var_BinNumber_Z =  1;

bool   SetMesh(string type);
string Material(double rho, int voxel_reg, string matbasename);
int    ReadDensityTable(std::string fname);

// Not Yet Implemented
// extern "C" { int writevoxel(double&,double&,double&,long&,long&,long&,long&); } 
// extern "C" { int fortranfuntion(double&,double&);}

bool SetMesh(string type){
  if (type == "fine"){
    var_BinSize_X   = 2E-4;   // [m]
    var_BinSize_Y   = 2E-4;   // [m] 
    var_BinSize_Z   = 2.5E-3; // [m]
    var_Offset_X    = 25;
    var_Offset_Y    = 25;
    var_Offset_Z    = 2;

    var_BinNumber_X = MAX_BINX;   // 105 + 50
    var_BinNumber_Y = MAX_BINY;   // 175 + 50
    var_BinNumber_Z = MAX_BINZ;   // 400 +  4
    return 0; // Ok

  }
  else if  (type == "intermediate"){
    var_BinSize_X   =  0.5E-3; // [m]
    var_BinSize_Y   =  0.5E-3; // [m] 
    var_BinSize_Z   =  2.5E-3; // [m]
    var_Offset_X    =  10;
    var_Offset_Y    =  10;
    var_Offset_Z    =  2;
    var_BinNumber_X =    62;        //  42 + 20
    var_BinNumber_Y =    90;        //  70 + 20
    var_BinNumber_Z =   404;        // 400 +  4  
    return 0; // Ok 
  }
  else if (type == "coarse" ) {
    // Coarse
    var_BinSize_X   =  1E-3; // [m]
    var_BinSize_Y   =  1E-3; // [m] 
    var_BinSize_Z   =  5E-3; // [m]
    var_Offset_X    =  5;
    var_Offset_Y    =  5;
    var_Offset_Z    =  1;
    var_BinNumber_X =    31;        //  21 + 10
    var_BinNumber_Y =    45;        //  35 + 10
    var_BinNumber_Z =   202;        // 200 +  2  
    return 0; // Ok
  }
  else 
    return 1; // Error
}


string i2s(int i){
  char c[20];
  sprintf(c,"%d",i);
  string s;
  s = c;
  return s;
}

string m2s(int i){
  char c[20];
  
  if ( i<10 )                  sprintf(c,"00%d",i);
  else if ( i>=10  && i<100 )  sprintf(c,"0%d",i);
  else if ( i>=100 && i<1000 ) sprintf(c,"%d",i);
  else return "-na";
  
  string s;
  s = c;
  return s;
}

string f2s(double d){
  char c[20];
  sprintf(c,"%.5f",d);
  string s;
  s = c;
  return s;
}

string d2s(double d){
  char c[20];
  sprintf(c,"%.14f",d);
  string s;
  s = c;
  return s;
}

// convert an integer to  a justified [nchar] fluka card  

string i2fc(int i, int nchar){
  char trun[10];
  sprintf(trun,"%d.",i);
  string _tmpS;
  string srun(abs(nchar),' ');
  _tmpS = trun;
  if (nchar>0) srun.replace(nchar-_tmpS.size(), _tmpS.size(), trun);
  else         srun.replace(0.,                 _tmpS.size(), trun);
  return srun;
}

// convert a double to a justified [nchar] fluka card  
string d2fc(double d, int nchar){
  char trun[10];
  sprintf(trun,"%.4e",d);
  string _tmpS;
  string srun(abs(nchar),' ');
  _tmpS = trun;
  if (nchar>0) srun.replace(nchar-_tmpS.size(), _tmpS.size(), trun);
  else         srun.replace(0.,                 _tmpS.size(), trun);
  return srun;
}

// convert a double to a justified [nchar] fluka card  
string g2fc(double d, int nchar){
  char trun[10];
  sprintf(trun,"%.4g",d);
  string _tmpS;
  string srun(abs(nchar),' ');
  _tmpS = trun;
  if (nchar>0) srun.replace(nchar-_tmpS.size(), _tmpS.size(), trun);
  else         srun.replace(0.,                 _tmpS.size(), trun);
  return srun;
}

// convert a double to a justified [nchar] fluka card  
string e2fc(double d, int nchar){
  char trun[10];
  sprintf(trun,"%.4e",d);
  string _tmpS;
  string srun(abs(nchar),' ');
  _tmpS = trun;
  if (nchar>0) srun.replace(nchar-_tmpS.size(), _tmpS.size(), trun);
  else         srun.replace(0.,                 _tmpS.size(), trun);
  return srun;
}

// convert a double to a justified [nchar] fluka card  
string f2fc(double d, int nchar){
  char trun[10];
  sprintf(trun,"%.3f",d);
  string _tmpS;
  string srun(abs(nchar),' ');
  _tmpS = trun;
  if (nchar>0) srun.replace(nchar-_tmpS.size(), _tmpS.size(), trun);
  else         srun.replace(0.,                 _tmpS.size(), trun);
  return srun;
}


// convert a string to a justified [nchar] fluka card  
string s2fc(string s, int nchar){
  string srun(abs(nchar),' ');
  if (nchar>0) srun.replace(nchar-s.size(), s.size(), s.c_str());
  else         srun.replace(0.,             s.size(), s.c_str());
  return srun;
}

int ReadDensityTable(std::string fname){
  cout << " opening " << fname <<endl;
  // Each element has a constant mass
  ifstream inputfile;
  inputfile.open(fname.c_str());
  
  vector<vector<vector<double> > > VOXEL;
  VOXEL.resize(MAX_BINX);
  for (int i = 0; i < MAX_BINX; ++i) {
    VOXEL[i].resize(MAX_BINY);

    for (int j = 0; j < MAX_BINY; ++j)
      VOXEL[i][j].resize(MAX_BINZ);
  }

  // double VOXEL[MAX_BINX][MAX_BINY][MAX_BINZ];
  // double cmX[MAX_BINX][MAX_BINY][MAX_BINZ];
  // double cmY[MAX_BINX][MAX_BINY][MAX_BINZ];
  // double cmZ[MAX_BINX][MAX_BINY][MAX_BINZ];
  double tmpX,tmpY,tmpZ;

  double TargetMass = 0.0;
  int elements = 0;

  double rho_step  = RHO_STEPS;
  double rho_steps = MAX_MATERIALS; 
  double rho_max   = _rho0 + rho_steps*rho_step/2.;
  double rho_min   = _rho0 - rho_steps*rho_step/2.;
  cout << "> Density level calculation " << endl
       << ">  Max number of materials: " << rho_steps << endl
       << ">  Density : - Step:        " << rho_step  << endl
       << ">            - Lower edge:  " << rho_min   << endl
       << ">            - Higher edge: " << rho_max   << endl;
  
  
  string ResultFileName;
  ResultFileName = fname + ".root";
  TFile *ResultFile = new TFile (ResultFileName.c_str(),"recreate");

  ResultFile->cd();
  TH1D *density   = new TH1D("density","density",rho_steps,rho_min,rho_max);
  TH2D *density2D = new TH2D("density2D","density2D",
                             var_BinNumber_X,-var_BinNumber_X/2.,var_BinNumber_X/2.,
                             var_BinNumber_Y,-var_BinNumber_Y/2.,var_BinNumber_Y/2.);
  TH3D *density3D = new TH3D("density3D","density3D",
                             var_BinNumber_X,-var_BinNumber_X/2.,var_BinNumber_X/2.,
                             var_BinNumber_Y,-var_BinNumber_Y/2.,var_BinNumber_Y/2.,
                             var_BinNumber_Z,-var_BinNumber_Z/2.,var_BinNumber_Z/2.);

  // count bin over 0 and define the density
 
  _loadedMaterials = 0;
  uint MaterialIDX = 0;
  //  uint MaterialIDX = 0;
  Double_t minDensity = 20;
  Double_t maxDensity = 0;

  if ( inputfile.is_open() ){
    //while(inputfile.isgood()){
    for (int k=0;k<(var_BinNumber_Z);k++)
      for (int j=0;j<var_BinNumber_Y;j++)
        for (int i=0;i<var_BinNumber_X;i++)
          {
            if ( i < var_Offset_X || i >= (var_BinNumber_X - var_Offset_X) || 
                 j < var_Offset_Y || j >= (var_BinNumber_Y - var_Offset_Y) || 
                 k < var_Offset_Z || k >= (var_BinNumber_Z - var_Offset_Z)    ) {
	      VOXEL[i][j][k]=0.;
	      _VOXEL_IDX[i][j][k]=0;
	    }
	    else{
	      inputfile >> tmpX >> tmpY >> tmpZ >> VOXEL[i][j][k];
	      // cmX[i][j][k]  >> cmY[i][j][k] >> cmZ[i][j][k] >> VOXEL[i][j][k];
	      elements ++;
	      
	      TargetMass += var_BinSize_X * var_BinSize_Y * var_BinSize_Z * VOXEL[i][j][k];
              
              _VOXEL_IDX[i][j][k] = density->GetXaxis()->FindBin(VOXEL[i][j][k]/1000.);
              if ( density->GetBinContent(_VOXEL_IDX[i][j][k]) == 0 ) {
                _rho[_loadedMaterials] = density->GetBinCenter(_VOXEL_IDX[i][j][k]);
                cout << " M="   << _loadedMaterials+1 
                     << " B="   << _VOXEL_IDX[i][j][k]
                     << " rho=" << _rho[_loadedMaterials] << endl;
                
                _loadedMaterials++;
              }
              if ( VOXEL[i][j][k]/1000. < minDensity) minDensity = VOXEL[i][j][k]/1000.;
              if ( VOXEL[i][j][k]/1000. > maxDensity) maxDensity = VOXEL[i][j][k]/1000.;
              
              density->Fill(VOXEL[i][j][k]/1000. );
              density2D->Fill((double)i -var_BinNumber_X/2.,
                              (double)j -var_BinNumber_Y/2.,
                              VOXEL[i][j][k]/(1000.*var_BinNumber_Z) );
              density3D->SetBinContent(i+1,j+1,k+1,VOXEL[i][j][k]/1000.);
              
              // if (_VOXEL_IDX[i][j][k] != 251) cout << _VOXEL_IDX[i][j][k] << endl;
              // if (VOXEL[i][j][k]/1000. < CriticalDensity )
              //     cout << i << ", " << j <<  ", " << k << ", "  << VOXEL[i][j][k]/1000. << endl;
            }
          }
  }
  // cout.precision(9);
  // density3D->GetMinimum()
  // density3D->GetMaximum()
  cout << "> Density: Min= " << minDensity
       << " Max= "           << maxDensity << endl;
  
  inputfile.close();
  double OriginalMass = var_BinSize_X * var_BinSize_Y * var_BinSize_Z * (var_BinNumber_X - 2*var_Offset_X) * (var_BinNumber_Y - 2*var_Offset_Y) * (var_BinNumber_Z - 2*var_Offset_Z) * _rho0 * 1000.;
  cout << "> The original mass of the block was " << OriginalMass << " kg" << endl;
  cout << "> The total    mass of the block is  " <<   TargetMass << " kg" << endl;
  
  density->Draw();
  ResultFile->Write();
  ResultFile->Close();
 
  // density2D->Draw("COLZ");
  
  //   for (int nbin=0;nbin<rho_steps;nbin++){
  //     //  cout << " X=" << density->GetBinCenter(nbin+1) << " Y=" << density->GetBinContent(nbin+1)<< endl;
  //     if ( density->GetBinContent(nbin+1) > 0 ) {
  //       _rho[_loadedMaterials] = density->GetBinCenter(nbin+1);
  //       _loadedMaterials++;
  //     }
  //   }
  
  cout << "> Only " <<  _loadedMaterials << " density levels are used" << endl;
  return elements;
  
}

int CountValues(string fname){
  // Each element has a constant mass
  ifstream inputfile;
  inputfile.open(fname.c_str());
 
  int elements = 0;
  double tmp;  
  
  if ( inputfile.is_open() ){
    while(inputfile.good() && !inputfile.eof()){
      inputfile >> tmp;
      elements++;
    }
  }
  
  inputfile.close();
  elements--;
  cout << "> " << elements << " loaded" << endl;
  return elements;
}

int CreateStartingBlock(string fname,
			 uint minVOXELx, uint minVOXELy, uint minVOXELz,
			 uint maxVOXELx, uint maxVOXELy, uint maxVOXELz)
{
  // Create a full block 
  
  if ( minVOXELx >= var_BinNumber_X || maxVOXELx >= var_BinNumber_X ||   
       minVOXELy >= var_BinNumber_Y || maxVOXELy >= var_BinNumber_Y ||   
       minVOXELz >= var_BinNumber_Z || maxVOXELz >= var_BinNumber_Z   ){
    cout << "ERROR" << endl;
    return 0;   
  }
  else {
    ofstream fileTarget;
    fileTarget.open(fname.c_str());
    if ( fileTarget.is_open() ){
      cout << "Creating file " << fname << endl;
      for ( int binZ=0;binZ<var_BinNumber_Z;binZ++){
	for ( int binY=0;binY<var_BinNumber_Y;binY++){
	  for ( int binX=0;binX<var_BinNumber_X;binX++){
	    if ( (binX >= minVOXELx ) && (binX < maxVOXELx) && 
		 (binY >= minVOXELy ) && (binY < maxVOXELy) &&
		 (binZ >= minVOXELz ) && (binZ < maxVOXELz)    ){           
	      
	      fileTarget << 1 << " ";
	    }
	    else {
	      fileTarget << 0 << " ";
	    }
	  }
	}
      }
    }
    fileTarget.close();

    _loadedMaterials = 1;    
    _rho[0] = _rho0;
    return _loadedMaterials; // Return the number of "voxel" tissue created 
  }
}

int CreateBlock(string fname,
                uint minVOXELx, uint minVOXELy, uint minVOXELz,
                uint maxVOXELx, uint maxVOXELy, uint maxVOXELz)
{
  // Create 
  
  if ( minVOXELx > var_BinNumber_X || maxVOXELx > var_BinNumber_X ||   
       minVOXELy > var_BinNumber_Y || maxVOXELy > var_BinNumber_Y ||   
       minVOXELz > var_BinNumber_Z || maxVOXELz > var_BinNumber_Z   ){
    cout << "ERROR" << endl;
    return 0;   
  }
  else {
    ofstream fileTarget;
    fileTarget.open(fname.c_str());
    if ( fileTarget.is_open() ){
      cout << "Creating file " << fname << endl;
      for ( int binZ=0;binZ<var_BinNumber_Z;binZ++){
	for ( int binY=0;binY<var_BinNumber_Y;binY++){
	  for ( int binX=0;binX<var_BinNumber_X;binX++){
	    if ( (binX >= minVOXELx ) && (binX < maxVOXELx) && 
		 (binY >= minVOXELy ) && (binY < maxVOXELy) &&
		 (binZ >= minVOXELz ) && (binZ < maxVOXELz)    ){           
	      
	      fileTarget << _VOXEL_IDX[binX][binY][binZ] << " ";
	    }
	    else {
	      fileTarget << 0 << " ";
	    }
	  }
	}
      }
    }
    fileTarget.close();

    return _loadedMaterials; // Return the number of "voxel" tissue created 
  }
}


string Material(double rho, int voxel_reg, string matbasename){
  string outstring;
  string regname;
  string matname;
  matname = matbasename + m2s(voxel_reg+2);
  regname = "VOXEL" + m2s(voxel_reg+2);

  if (rho==19.3){
    outstring  = "*...+....1....+....2....+....3....+....4....+....5....+....6....+....7....+....\n";
    outstring += "ASSIGNMA    TUNGSTEN" + s2fc(regname,10)  + "\n";
    
  }
  else{
    outstring  = "*...+....1....+....2....+....3....+....4....+....5....+....6....+....7....+....\n";
    outstring += "MATERIAL         74.          " + f2fc(rho,10) + s2fc(" ",30) +       s2fc(matname,-10) + "\n"; 
    outstring += "LOW-MAT   " + s2fc(matname,10) + 
                                     "       74.       -2.      296.        0.        0.TUNGSTEN\n";  
    outstring += "ASSIGNMA  " + s2fc(matname,10) + s2fc(regname,10)  + "\n";
  }
  return outstring;
}

bool WriteMaterials( ){
  
  // double rho_step = 0.1;
  // TH1F *density = new TH1F("density","density",MAX_MATERIALS,0 -rho_step/2., MAX_MATERIALS +rho_step/2.);
  // count bin over 0 and define the density
  
  string   mat_filename="target_block.mat";
  ofstream fileMaterial;
  fileMaterial.open(mat_filename.c_str());
  
  if ( fileMaterial.is_open() ){
    for (int i=0;i<_loadedMaterials;i++){
      // Case of tungsten
      //_rho[i] = 19.3;//i*rho_step;
      // cout         << Material(_rho[i],i,"W_vx");
      fileMaterial << Material(_rho[i],i,"W_vx");
      
    }
  }
  fileMaterial.close();
}
bool WriteScoring( ){
  
  // double rho_step = 0.1;
  // TH1F *density = new TH1F("density","density",MAX_MATERIALS,0 -rho_step/2., MAX_MATERIALS +rho_step/2.);
  // count bin over 0 and define the density
  
  string   scoring_filename="target_block.scoring";
  ofstream fileScoring;
  fileScoring.open(scoring_filename.c_str());
  string usrbin = "* ..+....1....+....2....+....3....+....4....+....5....+....6....+....7...\n";
  usrbin +=       "USRBIN           10.    ENERGY      -31."
    +f2fc((var_BinNumber_X-2*var_Offset_X)*var_BinSize_X*100./2.,10)  
    +f2fc((var_BinNumber_Y-2*var_Offset_Y)*var_BinSize_Y*100./2.,10) 
    +f2fc((var_BinNumber_Z-2*var_Offset_Z)*var_BinSize_Z*100.,10) + "Energy\n";
  usrbin +=       "USRBIN    " 
    +f2fc(-(var_BinNumber_X-2*var_Offset_X)*var_BinSize_X*100./2.,10) 
    +f2fc(-(var_BinNumber_Y-2*var_Offset_Y)*var_BinSize_Y*100./2.,10) 
    +f2fc(0,10) 
    +f2fc(var_BinNumber_X-2*var_Offset_X,10)  
    +f2fc(var_BinNumber_Y-2*var_Offset_Y,10) 
    +f2fc(var_BinNumber_Z-2*var_Offset_Z,10) + "&\n";
  fileScoring << usrbin;
  
  fileScoring.close();
}

// *
// * ..+....1....+....2....+....3....+....4....+....5....+....6....+....7...
// USRBIN           10.    ENERGY      -30.      1.55      2.25     100.5EnergyAll
// USRBIN         -1.55     -2.25      -0.5      155.      225.     404.0&
// USRBIN           10.    ENERGY      -31.      1.05      1.75     100.0Energy
// USRBIN         -1.05     -1.75        0.      105.      175.     400.0&
// * ..+....1....+....2....+....3....+....4....+....5....+....6....+....7...


int main(int argc, char **argv)
{
  char *tmp(0);
  int cycle;

  if (argc == 3 || argc == 4){
    cycle  = strtol(argv[1], &tmp,0); 
    cout << "> Running cycle n. " << cycle << endl;

    if(argc == 3){
      string type = argv[2];
      SetMesh(type);
      
      if (cycle == 0){
	cout << "> Creating " << type << "mesh on ascii file " << cycle << endl;
	CreateStartingBlock("target_block.ascii",var_Offset_X,var_Offset_Y,var_Offset_Z,
			    var_BinNumber_X-var_Offset_X,var_BinNumber_Y-var_Offset_Y,var_BinNumber_Z-var_Offset_Z);
	WriteMaterials();
	WriteScoring();
      }
      else {
	cout << "> ERROR: must specify input file name if cycle is bigger then 0" << endl;
	return 1;
      }
    }
    else if (argc == 4){
      string filename  = argv[2];  
      string type      = argv[3];
      SetMesh(type);
      
      cout << "> Loading " << type << " mesh from " << filename << endl;
      cout << "> " << ReadDensityTable(filename) << " values read from file" << endl;
      cout << "> Creating block" << endl;
      CreateBlock("target_block.ascii",
		  var_Offset_X,
		  var_Offset_Y,
		  var_Offset_Z,
		  var_BinNumber_X - var_Offset_X,
		  var_BinNumber_Y - var_Offset_Y,
		  var_BinNumber_Z - var_Offset_Z);
      //            var_Offset_X,             var_Offset_Y,             var_Offset_Z,
      //            var_BinNumber_X -var_Offset_X,   var_BinNumber_Y -var_Offset_Y,   var_BinNumber_Z -var_Offset_Z);
      WriteMaterials();
      WriteScoring();
    }
  }
  else {
    cout << "> ERROR: " << endl 
	 << "> writeTarget [cycle] [mesh_type]" <<endl
	 << "> writeTarget [cycle] [density_finename] [mesh_type]" << endl
	 << "> where [mesh_type] can be fine, intermediate or coarse" <<endl;
    return 1;
  }
  
  //  writeTarget();
  return 0;
}


