// Author: Ulises Olivares
// uolivares@unam.mx
// June 8, 2023


#include "common.h"
#include "Methods_IDW_CD.h"
#include "Raster.h"
#include <ctime>
#include <chrono>
#include <string.h>
#include <tclap/CmdLine.h>
#include <omp.h>
#include <sstream>
#include <iostream>
#include <fstream>

// Methods definition
void parseParameters(int argc, const char** argv);
void RunCDIDW(string frictionMap, string demmandFile, string locsMap, string scenario);
float* importRaster(string name, int &rows, int &cols, float &scale, int &cell_null);

//Global variable definition
string frictionMapW, demmandFileW, locsMapW, frictionMapV, demmandFileV, locsMapV;
int isFriccRelative = 1, numThreads = 0, hours = 12;
float exponent = 1.005;

// Raster variables
const char *projection;
double adfGeoTransform[6];


int main(int argc, const char** argv) {
    // Parse parameters from command line
    parseParameters(argc, argv);

    // Run CDIDW for walking Scenario
    RunCDIDW(frictionMapW, demmandFileW, locsMapW, "walking");

    return 0;
}

void RunCDIDW(string frictionMap, string demmandFile, string locsMap, string scenario){
    //Object creation
    //Raster objrast;

    // Variable declaration
    int rows, cols;// size of matrix
    int nullValue;// null value
    float scale;// map scale

    //Import friction matrix
    float* fric_matrix, *locsMatrix, *IDW_matrix; //matrices to store input/output data

    fric_matrix = importRaster(frictionMap, rows, cols, scale, nullValue);

    



    /*Methods objMeth;
    //Variable definition and start timers
    std::string str;
    //double start2;
    //double end2;
    cout << "----------------------------------------------------------" << endl;
    cout << "Runing parallel CD and IDW for " << scenario << " scenario" << endl;
    //start2 = omp_get_wtime();

    // Variable definition
    int rows, cols;// size of matrix
    int nullValue;// null value
    float scale;// map scale

    int locCount = 0;// count localities
    position array; //store movements in the matrix
    int i;
    vector<pair<string, vector<float>>> demmand;// Vector to store demand of all years

    // matrices
    float* fric_matrix; //matrix to store the friction map
    float* locsMatrix;//matrix to store localities
    float* IDW_matrix; // matrix to store final IDW

    // maps to speed up opperations
    map<int, Raster::local> localidades;// map of localities

    //estructura local contiene el no. de comunidad y su (x,y)
    std::map<int, Raster::local>::iterator ubicacion; //iterados mapa ubicacion localidades
    std::map<int, float>::iterator biomass; //iterador mapa requisitos localidad

    // friction map
    fric_matrix = objrast.importRaster(frictionMap, rows, cols, scale, nullValue);
    int tmpNull = 0;

    // Localities map
    locsMatrix = objrast.importRaster(locsMap, rows, cols, scale, tmpNull);

    //get the number os locs
    //locsNum = objrast.countCommunities(locsMatrix, rows, cols, nullValue);

    // Store requisites of communities
    //Load demmnad from multiple years
    demmand = objrast.loadDemmand(demmandFile);
    // TODO: write a method to directly import demmand from a BaU file and avoid separate V and W files.

    // Store locs numbere
    int locsNum = objrast.readLocalities(locsMatrix, rows, cols, localidades, nullValue);

    cout << "Total number of localities " << locsNum << endl;*/


}

float* importRaster(std::string name, int &rows, int &cols, float &scale, int &cell_null){
    int row,col;//iteradores matriz
    GDALDataset *dataset;
    GDALAllRegister();
    string ds = name;
    dataset = (GDALDataset *) GDALOpen(ds.c_str(), GA_ReadOnly);
    GDALRasterBand  *poBand;

    poBand = dataset->GetRasterBand(1);
    dataset->GetGeoTransform( adfGeoTransform );

    projection = dataset->GetProjectionRef();

    cols = poBand->GetXSize();
    rows= poBand->GetYSize();
    scale = adfGeoTransform[1];

    dataset->GetGeoTransform( adfGeoTransform );
    scale = adfGeoTransform[1];
    cell_null = poBand->GetNoDataValue();  //read null value of cell
    float *matrix = new float[rows * cols];
    float *pBuf = new float[rows * cols];

    if (poBand->RasterIO(GF_Read, 0, 0, cols, rows, pBuf, cols, rows, GDT_Float32, 0, 0) == 0)
        cout << name << " raster imported sucessfully!" << endl;
    else
        cout << "An error occurred during the importation process! " << endl;

    int location;
    for (row = 0; row < rows; row++)
        for ( col = 0; col < cols; col++){
            location = (cols * (row)) + col;
            matrix[(cols*row)+col] = *(pBuf+location);
        }
    //cout<<"valor nulo: "<< cell_null<< endl;
    return matrix;

}


void parseParameters(int argc, const char** argv){
    try {
        // Define the command line object, and insert a message
        // that describes the program. The "Command description message"
        // is printed last in the help text. The second argument is the
        // delimiter (usually space) and the last one is the version number.
        // The CmdLine object parses the argv array based on the Arg objects
        // that it contains.
        TCLAP::CmdLine cmd("Command description message", ' ', "1");

        // Parse the argv array.
        //cmd.parse( argc, argv );


        // Define a value argument and add it to the command line.
        // A value arg defines a flag and a type of value that it expects,
        // such as "-n Bishop".
        //TCLAP::ValueArg<std::string> nameArg("n","name","Name to print",true,"homer","string");
        TCLAP::ValueArg<std::string>frictionw("1","frictionWalking","Absolute path to friction.tif for walking scenario",true,"/path/to/friction.tif","string");
        TCLAP::ValueArg<std::string>locsw("2","locsWalking","Absolute path to locs.tif for walking scenario",true,"/path/to/locs.tif","string");
        TCLAP::ValueArg<std::string>demmandw("3","demmandWalking","Absolute path to demmand.csv for walking scenario",true,"/path/to/demmand.csv","string");

        TCLAP::ValueArg<std::string>frictionv("4","frictionVehicle","Absolute path to friction.tif for vehicle scenario",true,"/path/to/friction.tif","string");
        TCLAP::ValueArg<std::string>locsv("5","locsVehicle","Absolute path to locs.tif for vehicle scenario",true,"/path/to/locs.tif","string");
        TCLAP::ValueArg<std::string>demmandv("6","demmandVehicle","Absolute path to demmand.csv for vehicle scenario",true,"/path/to/demmand.csv","string");

        TCLAP::ValueArg<std::string>relative("r","relative","1 to friction relative, 0 otherwise",true,"1 to friction relative, 0 otherwise","string");
        TCLAP::ValueArg<std::string>processors("p","processors","Number of processors -p <int>",true,"Number of processors -p <int>","string");
        TCLAP::ValueArg<std::string>timeLimit("t","timeLimit","Time limit for Cost Distance exploration (hours) -t <int>",true,"Time limit for exploration -t <int>","string");
        TCLAP::ValueArg<std::string>expo("e","exponent","IDW exponent -e <float>",true,"IDW exponent -e <float>","string");
        // Add the argument nameArg to the CmdLine object. The CmdLine object
        // uses this Arg to parse the command line.


        cmd.add(frictionw);
        cmd.add(locsw);
        cmd.add(demmandw);

        cmd.add(frictionv);
        cmd.add(locsv);
        cmd.add(demmandv);

        cmd.add(relative);
        cmd.add(processors);
        cmd.add(timeLimit);
        cmd.add(expo);


        // Parse the argv array.
        cmd.parse( argc, argv );

        // Walking
        frictionMapW = frictionw.getValue();
        demmandFileW = demmandw.getValue();
        locsMapW = locsw.getValue();

        // Vehicle
        frictionMapV = frictionv.getValue();
        demmandFileV = demmandv.getValue();
        locsMapV = locsv.getValue();

        // user defined
        isFriccRelative = stoi(relative.getValue());
        numThreads = stoi(processors.getValue());
        hours = stoi(timeLimit.getValue());
        exponent = stof(expo.getValue());

    }
    catch (TCLAP::ArgException &e)  // catch any exceptions
    {
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
        exit(0);
    }
}