#include "common.h"
#include "Methods_IDW_CD.h"
#include "Raster.h"
#include <ctime>
#include <chrono>
#include <string.h>
#include <tclap/CmdLine.h>


// Methods definition
void parseParameters(int argc, const char** argv);

//Global variable definition
        string frictionMapW, demmandFileW, locsMapW, frictionMapV, demmandFileV, locsMapV;
int isFriccRelative = 1, numThreads = 0, hours = 12;
float exponent = 1.005;


int main(int argc, const char** argv) {
    std::cout << "Hello, World!" << std::endl;

    // Parse parameters from command line
    parseParameters(argc, argv);

    return 0;
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