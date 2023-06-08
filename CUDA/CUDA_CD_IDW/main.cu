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


// structure definition
typedef struct locality{
    int row;
    int col;
    int num_local;
}locality;

// Methods definition
void parseParameters(int argc, const char** argv);
void RunCDIDW(string frictionMap, string demmandFile, string locsMap, string scenario);
float* importRaster(string name, int &rows, int &cols, float &scale, int &cell_null);
vector<pair<string, vector<float>>> loadDemmand(string name);
int readLocalities(float *map_local, int rows, int cols, map<int,locality> &local_ord, int cell_null);
float* resetMatrix(int rows,  int cols, float val1);

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

/*
 * This function start the CD and IDW algorithm for a given scenario.
 * Input: str <friction file tiff>, str <demmand file csv>
 */
void RunCDIDW(string frictionMap, string demmandFile, string locsMap, string scenario){
    //Object creation
    //Raster objrast;

    // Variable declaration
    int rows, cols, nullValue = 0, locsNum= 0;
    float scale;// map scale
    vector<pair<string, vector<float>>> demmand;// Vector to store demand of all years
    map<int, locality> localities;// map of localities
    const int moves[2][8]={{1, 1, 0, -1, -1, -1, 0,  1},{0, 1, 1, 1,  0,  -1, -1, -1}}; // all possible combinations of movements in a map including diagonals


    //Import friction matrix
    float* fric_matrix, *locsMatrix, *IDW_matrix; //matrices to store input/output data
    IDW_matrix = resetMatrix(rows, cols, 0);

    fric_matrix = importRaster(frictionMap, rows, cols, scale, nullValue);

    // Localities map
    locsMatrix = importRaster(locsMap, rows, cols, scale, nullValue);

    //cout <<"Loading demmand..." << endl;
    // Load demmand per year
    demmand = loadDemmand(demmandFile);


    // count the number of locs
    locsNum = readLocalities(locsMatrix, rows, cols, localities, nullValue);
    cout << "Total number of localities " << locsNum << endl;

    // Biomass requirement
    map<int, float> requiredBiomass;
    for(int loc=0; loc < demmand[0].second.size();loc++){ // Tamaño de localidades
        requiredBiomass.insert(pair<int, float>(int(demmand[0].second[loc]), float(demmand[year].second[loc]))); //load demmand in tons
        cout<< requiredBiomass
    }



    // TODO: start the parallel calculation of CD



}

/*
 * This function initializes a matrix with a specific value.
 * Input: Rows, cols and value
 * Output: Matrix with the givens size and value
 */
float* resetMatrix(int rows, int cols, float val1){
    int row,col;
    float *IDW = new float [rows * cols];//matriz IDW para todas las localidades
    for(row=0; row<rows; row++)
        for(col=0; col<cols; col++)
            IDW[(cols*row)+col]=val1;
    return(IDW);
}

/*
 * This function counts the number of localities in a map
 * TODO: this might be unnecesary in future releases.
 */
int readLocalities(float *map_local, int rows, int cols, map<int,locality> &local_ord, int cell_null) {
    //cout << "Enter to readLocs" << endl;
    locality array;
    int countLoc = 0;
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            if (map_local[(cols * row) + col] != cell_null) {
                //if (countLoc == 1)
                //    break;
                array.row = row;
                array.col = col;
                local_ord[(int) map_local[(cols * row) + col]] = array;
                countLoc++;
            }
        }
    }
    return countLoc;
}

/*
 * This function reads localities from a CSV file and stores their demmand per year.
 * Input: CSV filename
 * Output: a vector with ID and demmand per year.
 */
vector<pair<string, vector<float>>> loadDemmand(string name){
    vector<pair<string, vector<float>>> result;
    // Create an input filestream
    std::ifstream myFile(name);

    // Make sure the file is open
    if(!myFile.is_open())
        throw runtime_error("Could not open file");


    // Helper vars
    string line, colname, value;
   // float val;

    // Read the column names
    if(myFile.good())
    {
        // Extract the first line in the file
        getline(myFile, line);

        // Create a stringstream from line
        stringstream ss(line);

        // Extract each column name // This while are goint to finish once all titles have completed
        while(getline(ss, colname, ',')){

            colname.erase(remove(colname.begin(), colname.end(), '"'), colname.end());
            // Initialize and add <colname, int vector> pairs to result
            result.push_back({colname, vector<float> {}});
        }
    }
    else
    {
        cout << "An error occurred while reading the CSV file!" << endl;
        exit(0);
    }

    // Read data, row by row
    while(getline(myFile, line))
    {
        // Create a stringstream of the current line
        stringstream ss(line);

        // Keep track of the current column index
        int colIdx = 0;

        while(getline(ss, value, ',')){
            cout << value << endl;
            // Convert first value to numeric
            if(colIdx == 0)
                value.erase(remove(value.begin(), value.end(), '"'), value.end()); // remove special " char.
            // Add value to vector
            result.at(colIdx).second.push_back(stof(value));
            // If the next token is a comma, ignore it and move on
            if(ss.peek() == ',') ss.ignore();
            // Increment the column index
            colIdx++;

        }
    }

    // Close file
    myFile.close();

    return result;
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