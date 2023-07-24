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
#include <unordered_map>


// structure definition
typedef struct locality{
    int row;
    int col;
    int ID;
    float demand;
}locality;

typedef struct localities{
    int year;
    locality * locsArray;
}localities;


// Methods definition
void parseParameters(int argc, const char** argv);
void RunCDIDW(string frictionMap, string demmandFile, string locsMap, string scenario);
float* importRaster(string name, int &rows, int &cols, float &scale, int &cell_null);
float* importLocsRaster(std::string name, int &rows, int &cols, float &scale, int &cell_null, long long int &countLocs, std::map<int, std::pair<int, int>> &matrixMap);
void loadCSVDemmand(string name, float *locsMatrix, unordered_map<int, vector<float>> &demand);
int generateLocsStruct(float *map_local, int rows, int cols, localities &locs, int cell_null, unordered_map<int, vector<float>> &demand, std::map<int, std::pair<int, int>> &matrixMap);
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
    unordered_map<int, vector<float>> demand;// Vector to store demand of all years
    //map<int, locality> localities;// map of localities
    localities *locs;
    const int moves[2][8]={{1, 1, 0, -1, -1, -1, 0,  1},{0, 1, 1, 1,  0,  -1, -1, -1}}; // all possible combinations of movements in a map including diagonals


    //Import friction matrix
    float* fric_matrix, *locsMatrix, *IDW_matrix; //matrices to store input/output data

    // Import friction raster
    fric_matrix = importRaster(frictionMap, rows, cols, scale, nullValue);

    // Initializar IDWMatrix with 0's
    IDW_matrix = resetMatrix(rows, cols, 0);

    // Import Localities map
    long long int numLocs=0;
    // TODO: store locs ands its position into a struct
    std::map<int, std::pair<int, int>> matrixMap;
    locsMatrix = importLocsRaster(locsMap, rows, cols, scale, nullValue, numLocs, matrixMap);

    //cout << "number of locs in map: " << matrixMap.size() << endl;
    //cout << "Number of locs in  tiff: " << numLocs << endl;

    cout <<"Loading demand..." << endl;
    // Load demand per year
    loadCSVDemmand(demmandFile, locsMatrix, demand);


    // count the number of localities
    locsNum = generateLocsStruct(locsMatrix, rows, cols, locs, nullValue, demand, matrixMap);
    cout << "Total number of localities " <<" " << locsNum << endl;


    int sizeLocs = locsNum * sizeof(localities);

    cout << locsNum <<endl;

    for (int i = 0; i < locsNum; i++) {
        localities* currentLocs = &locs[i];
        int year = currentLocs->year;
        locality* locsArray = currentLocs->locsArray;

        // Iterate over the localities within the current localities struct
        for (int j = 0; j < locsNum; j++) {
            locality* currentLocality = &locsArray[j];
            // Access and work with the current locality
            int row = currentLocality->row;
            int col = currentLocality->col;
            int ID = currentLocality->ID;
            float demand = currentLocality->demand;

            cout << ID << "=>" << demand << endl;

            // ... do something with row, col, ID, and demand
        }
    }
    // Biomass requirement
    /*map<int, float> requiredBiomass;
    for(int year= 1; year < demand.size(); year++){
        for(int loc=0; loc < demand[0].second.size(); loc++){ // Tamaño de localidades
            requiredBiomass.insert(pair<int, float>(int(demand[0].second[loc]), float(demand[year].second[loc])));//load demand in tons
        }
    }*/

    // 1) Declare host variables
    float* d_fric_matrix, * d_locsMatrix, *d_IDW_matrix;

    size_t  size = rows * cols *sizeof(float);


    // 2) Allocate device memory
    cudaError_t cudaStatus = cudaMalloc((void**)&d_fric_matrix, size);
    cudaStatus = cudaMalloc((void**)&d_locsMatrix, size);
    cudaStatus = cudaMalloc((void**)&d_IDW_matrix, size);
    //cudaError_t cudaStatus = cudaMalloc((void**)&d_IDW_matrix, size);

    //TODO: validate each device memory allocation
    if (cudaStatus != cudaSuccess) {
        cout<< "Error in cuda memory allocation: " << cudaStatus << endl;
    }

    // 3) Copy data from host  to devide IDW, fric, locs
    cudaMemcpy(d_fric_matrix, fric_matrix, size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_fric_matrix, d_locsMatrix, size, cudaMemcpyHostToDevice); // Not necessary
    cudaMemcpy(d_fric_matrix, d_IDW_matrix, size, cudaMemcpyHostToDevice);


    // 4) Instantiate the Kernel

    // 5) Copy memory from device

    // 6) free cuda mem

    cudaFree(d_fric_matrix);
    cudaFree(d_locsMatrix);
    cudaFree(d_IDW_matrix);

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
 * This function builds a struct of localities with all data
 */
int generateLocsStruct(float *map_local, int rows, int cols, localities &locs, int cell_null, unordered_map<int, vector<float>> &demand, std::map<int, std::pair<int, int>> &matrixMap) {
    int countLoc = matrixMap.size(); // Size of locs stored in the CSV
    //locs = (localities*)malloc(demand.size() * sizeof(localities));
    int size = matrixMap.size() - 2;
    locs = new localities[size];
    for (int year = 0; year < demand.size(); year++) {
        locs[year].year = year;
        locs[year].locsArray = new locality[size];
        int locTmp = 0;

        for (const auto &entry: matrixMap) {
            locs[year].locsArray[locTmp].ID = entry.first;
            locs[year].locsArray[locTmp].row = entry.second.first;
            locs[year].locsArray[locTmp].col = entry.second.second;
            auto it = demand.find(entry.first);
            locs[year].locsArray[locTmp].demand = it->second[year];
            locTmp++;
        }
    }
    return countLoc;
}


/*
 * This function reads localities from a CSV file and stores their demmand per year.
 * Input: CSV filename
 * Output: a vector with ID and demmand per year.
 */
void loadCSVDemmand(string name, float *locsMatrix, unordered_map<int, vector<float>> &demand){
    //localities * demand = new ;
    //vector<pair<string, vector<float>>> result;
    //unordered_map<int, vector<float>> demand;
    // Create an input filestream
    std::ifstream myFile(name);
    // Make sure the file is open
    if(!myFile.is_open())
        throw runtime_error("Could not open file");
    // Helper variables
    string line, colname, value;
   // float val;
    // Step 1) Read the column names ignore this information
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
            //result.push_back({colname, vector<float> {}});
            //demand[colname] = std::vector<float>{};
        }
    }
    else
    {
        cout << "An error occurred while reading the CSV file!" << endl;
        exit(0);
    }

    // Step 2) Read data, row by rows
    while(getline(myFile, line))
    {
        // Create a stringstream of the current line
        stringstream ss(line);
        // Keep track of the current column index
        int colIdx = 0, id = 0;
        while(getline(ss, value, ',')){
            //cout << value << endl;
            // Convert first value to numeric
            if(colIdx == 0){
                value.erase(remove(value.begin(), value.end(), '"'), value.end()); // remove special " char.
                id = stoi(value);
                demand[id] = std::vector<float>{};// TODO:
            }
            else
                demand[id].push_back(stof(value));// Remove values only for the first element

            // If the next token is a comma, ignore it and move on
            if(ss.peek() == ',') ss.ignore();
            // Increment the column index
            colIdx++;

        }
    }
    // Close file
    myFile.close();

    //return result;
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

float* importLocsRaster(std::string name, int &rows, int &cols, float &scale, int &cell_null, long long int &countLocs, std::map<int, std::pair<int, int>> &matrixMap){
    int row,col;//iteradores matriz
    //std::map<int, std::pair<int, int>> matrixMap; // map to store
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
            if (matrix[(cols*row)+col] != cell_null && matrix[(cols*row)+col] != 0){
                matrixMap[matrix[(cols*row)+col]] = std::make_pair(row, col);
                countLocs++;
            }

        }
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
        TCLAP::ValueArg<std::string>locsv("5","locsVehicle","Absolutse path to locs.tif for vehicle scenario",true,"/path/to/locs.tif","string");
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