#include "common.h"
#include "Methods_IDW_CD.h"
#include "Raster.h"
#include <ctime>
#include <chrono>
#include <string.h>
#include <tclap/CmdLine.h>


using namespace std;

void parseParameters(int argc, const char** argv);
void RunCDIDW(string frictionMap, string demmandFile, string locsMap, string scenario);

//Global variable definition
string frictionMapW, demmandFileW, locsMapW, frictionMapV, demmandFileV, locsMapV;
int isFriccRelative = 1, numThreads = 0, hours = 12, year = 1;
float exponent = 1.005;//exponent for IDW calculation

int main(int argc, const char** argv) {

    // Parse parameters and assigna variables
    parseParameters(argc, argv);

    // Run CDIDW for walking Scenario
    RunCDIDW(frictionMapW, demmandFileW, locsMapW, "walking");

    // Run CDIDW for vehicle Scenario
    RunCDIDW(frictionMapV, demmandFileV, locsMapV, "vehicle");

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
        TCLAP::ValueArg<std::string>sYears("y","year","number of specific year to run -y <int>",true,"number of specific year to run -y <int>","string");

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
        cmd.add(sYears);


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
        year = stoi(sYears.getValue());

    }
    catch (TCLAP::ArgException &e)  // catch any exceptions
    {
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
        exit(0);
    }
}


void RunCDIDW(string frictionMap, string demmandFile, string locsMap, string scenario){
    //Object creation
    Raster objrast;
    Methods objMeth;
    //Variable definition and start timers
    std::string str;
    double start2;
    double end2;
    cout << "----------------------------------------------------------" << endl;
    cout << "Runing parallel CD and IDW for " << scenario << " scenario" << endl; 
    start2 = omp_get_wtime();

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

    /* Store requisites of communities
    Load demmnad from multiple years */
    demmand = objrast.loadDemmand(demmandFile);
    // TODO: write a method to directly import demmand from a BaU file and avoid separate V and W files.

    // Store locs numbere
    int locsNum = objrast.readLocalities(locsMatrix, rows, cols, localidades, nullValue);

    cout << "Total number of localities " << locsNum << endl;

    // Biomass requirement
    map<int, float> requiredBiomass;

    // Iterate over demmand for each year
    double locTimerStart, locTimerEnd;

    
        cout << "\nProcessing year " << year << " ... " << endl;

        auto givenTime = chrono::system_clock::to_time_t(chrono::system_clock::now());
        
        
        cout <<"Started at: "<< ctime(&givenTime);
        locTimerStart = omp_get_wtime();

        IDW_matrix = objMeth.reset_Matrix(rows, cols, 0); //llena la matriz inicial del valor indicado

        // Use the same format as before
        //cout << "size: " << demmand[0].second.size()<< endl;
        for(int loc=0; loc < demmand[0].second.size();loc++){ // Tamaño de localidades
            requiredBiomass.insert(pair<int, float>(int(demmand[0].second[loc]), float(demmand[year].second[loc]/1000))); // tons
            //cout << demmand[0].second[loc] << " year: " << year <<endl;  
        }

        //inicia calculo modelos
        biomass = requiredBiomass.begin();
        int start =int(biomass->first);
        biomass = --requiredBiomass.end();
        int end =int(biomass->first);

        const int mov[2][8]={{1,1,0,-1,-1,-1,0,1},{0,1,1,1,0,-1,-1,-1}};

        // Specify a number of threads
        if (numThreads!= 0){
            omp_set_num_threads(numThreads);
        }

        //cout << "Scale: "<< scale << endl;
        
        #pragma omp parallel for private(ubicacion,biomass,array)
        for(i=start;i<=end;i++) {
            //int iteration = 0;
            if (requiredBiomass.find(i) != requiredBiomass.end()) {//Is the community ID in the array?
                biomass = requiredBiomass.find(i);
                if (biomass->second > 0){//requisitos distintos a cero
                    if (localidades.find(i) != localidades.end()) { //existe la comunidad con ese numero?
                        ubicacion = localidades.find(biomass->first);//buscar ubicacion de la localidad
                        //ubicaion inicial
                        array.row = ubicacion->second.row;
                        array.col = ubicacion->second.col;
                        array.val_fricc = 0;
                        array.key=0;
                        locCount++;//localidades calculadas
                        float *CD_matrix = new float[rows*cols];

                        // Init CD_matrix with infinite values
                        //fill_n(CD_matrix, rows*cols, );
                        fill_n(CD_matrix, rows*cols, std::numeric_limits<float>::max());

                        priority_queue<position> CD_costos;
                        int key = 1;
                        int row_temp,col_temp,h;

                        position inicial;
                        CD_costos.push(array);
                        // inicia calculo

                        //while(!CD_costos.empty() | )
                        // Limit CD calculation
                        float tmpCost = 0.0;
                        float timeLimit = hours * 3600; // 6 hours
                        
                        while(tmpCost<= timeLimit  && !CD_costos.empty()){
                            inicial=CD_costos.top();
                            tmpCost = inicial.val_fricc;
                            //iteration++;
                            //cout<< "iteration: " << iteration << " single_fricc " << (inicial.single_fricc/scale) << " Cummulated " << cummulatedTime << endl;
                            //cout << "Iter: " << iteration << " Current: " <<  fric_matrix[(inicial.col*inicial.row)+inicial.col] << " Cummulated Time: " << cummulatedTime <<endl;
                            CD_costos.pop();

                            // Calculate cost of all neighbours
                            for(h=1;h<9;h++){
                                row_temp = mov[1][h - 1] + inicial.row;
                                col_temp = mov[0][h - 1] + inicial.col;

                                if(row_temp < rows && row_temp >= 0 && col_temp < cols && col_temp >= 0 && fric_matrix[(cols*row_temp)+col_temp]>0.0) { // Inside valid cells and friction > 0.0
                                    if (h % 2 != 0){//Lateral movement
                                        if(isFriccRelative == 1){
                                            array.val_fricc = (inicial.val_fricc) + ((fric_matrix[(cols * row_temp) + col_temp])*scale);
                                        }
                                        else{
                                            array.val_fricc = ((inicial.val_fricc) + ((fric_matrix[(cols * row_temp) + col_temp])));
                                        }
                                    }
                                    else {//Diagonal movement
                                        if( isFriccRelative == 1 ){
                                            array.val_fricc = (inicial.val_fricc) + (sqrt(2) * ((fric_matrix[(cols * row_temp) + col_temp])*scale));
                                        }
                                        else{
                                            array.val_fricc = (inicial.val_fricc) + (sqrt(2) * ((fric_matrix[(cols * row_temp) + col_temp])));
                                        }
                                        
                                    }//se busca el menor valor de CD, es posible que se escriba varias veces en una celda
                                    
                                    if (CD_matrix[(cols*row_temp)+col_temp] > array.val_fricc ){
                                        array.row = row_temp;
                                        array.col = col_temp;
                                        array.key=key;
                                        key++;
                                        CD_matrix[(cols*row_temp)+col_temp] = array.val_fricc;
                                        //tmpCost = CD_matrix[(cols*row_temp)+col_temp];
                                        CD_costos.push(array);
                                    }
                                        
                                }
                            }
                        }
                        
                        //---------------IDW
                        int row,col;
                        for(row = 0; row < rows; row++) {
                            for (col = 0; col < cols; col++) {
                                //if(CD_matrix[(cols * row) + col] == numeric_limits<float>::max()){ // null valu
                                //float temp = fric_matrix[(cols * row) + col];
                                if (fric_matrix[(cols * row) + col] < 0.0) {// Null value
                                    //cout << "Write mull value: " << IDW_matrix[(cols * row) + col] << " CD " <<  CD_matrix[(cols * row) + col] << endl;
                                    IDW_matrix[(cols * row) + col] = nullValue;
                                } 
                                else{
                                        #pragma omp atomic
                                        //IDW_matrix[(cols * row) + col] +=( biomass->second *1000) / pow(CD_matrix[(cols * row) + col], exponent);
                                        IDW_matrix[(cols * row) + col] +=( biomass->second) / pow(CD_matrix[(cols * row) + col], exponent);     
                                }
                            }
                        }
                        // Free memory on each iteration CD_matrix
                        delete CD_matrix;
                    }
                }
            }
        }
        // ---------------agregar valores nulos en la ubicacion de las localidade
        /*ubicacion = localidades.begin();
        while (ubicacion != localidades.end()) {
            IDW_matrix[(cols * ubicacion->second.row)+ubicacion->second.col] = 0;
            ubicacion++;
        }*/


        string fileName = "";
        if(year>9)
            fileName = "IDW_C++_" + demmand[year].first.substr(5, 4) + to_string(year);
        else
            fileName = "IDW_C++_" + demmand[year].first.substr(5, 4) +"0" + to_string(year);;

        // export image
        objrast.exportRaster(IDW_matrix, rows, cols, locsNum, fileName, nullValue);//crea tiff de IDW de todas las localidades calculadas

        //-----------liberar memoria

        //IDW_matrix.clear();



        // Clear biomass requirements
        requiredBiomass.clear();

        // End timer
        locTimerEnd = omp_get_wtime();
        cout << "Year " << year << " finished!" <<endl;
        cout << "Total elapsed time for year: " << year << " was: " <<  ((locTimerEnd - locTimerStart) / 3600) << " hours" << endl;
        cout << "Estimated remaining time: " << ((locTimerEnd - locTimerStart) / 3600) * ((demmand.size()-1) - year)<< " hours" << endl;

    end2 = omp_get_wtime();
    double duration = (end2 - start2);//calculate global execution time
    printf("Global time: %lf hours \n", (duration/3600));
    cout << scenario <<" scenario sucessfully finished "<< endl;
    cout << "----------------------------------------------------------" << endl;

    delete IDW_matrix;
    delete fric_matrix;
    delete locsMatrix;
}