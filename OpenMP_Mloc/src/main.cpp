#include "common.h"
#include "Methods_IDW_CD.h"
#include "Raster.h"
#include <ctime>
#include <chrono>

using namespace std;

int main() {
    //Object creation
    Raster objrast;
    Methods objMeth;
    //Variable definition and start timers
    std::string str;
    double start2;
    double end2;
    start2 = omp_get_wtime();

    //------------------------------------------------------------------------variables
    int rows, cols;// size of matrix
    int nullValue;// null value
    float scale;// map scale
    float exp = 1.005;//exponente IDW
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
    fric_matrix = objrast.importRaster("/home/ulises/haiti100m/fricc_v.tif", rows, cols, scale, nullValue);
    int tmpNull = 0;

    // Localities map
    locsMatrix = objrast.importRaster("/home/ulises/haiti100m/locs_c.tif", rows, cols, scale, tmpNull);


    //get the number os locs
    //locsNum = objrast.countCommunities(locsMatrix, rows, cols, nullValue);

    /* Store requisites of communities
    Load demmnad from multiple years */
    demmand = objrast.loadDemmand("/home/ulises/haiti100m/Haiti_vehicle.csv");

    // Store locs number
    int locsNum = objrast.readLocalities(locsMatrix, rows, cols, localidades, nullValue);

    cout << "Total number of localities " << locsNum << endl;

    // Biomass requirement
    map<int, float> requiredBiomass;

    // Iterate over demmand for each year
    double locTimerStart, locTimerEnd;
    for(int year = 1; year<=demmand.size()-1;year++){
    //for(int year = 26; year<=demmand.size()-1;year++){
        cout << "Processing year " << year << " ... " << endl;
        auto givenTime = chrono::system_clock::to_time_t(chrono::system_clock::now());
        //cout << "
        // Started at: " << givenTime << endl;
        cout <<"Started at: "<< ctime(&givenTime) << endl;
        locTimerStart = omp_get_wtime();

        IDW_matrix = objMeth.reset_Matrix(rows, cols, 0); //llena la matriz inicial del valor indicado
        // Use the same format as before
        for(int loc=0; loc < demmand[0].second.size();loc++){ // Tamaño de localidades
            requiredBiomass.insert(pair<int, float>(int(demmand[0].second[loc]), float(demmand[year].second[loc])));
        }
        //-------------------------------------------------------------------------------------------------------inicia calculo modelos
        biomass = requiredBiomass.begin();
        int start =int(biomass->first);
        biomass = --requiredBiomass.end();
        int end =int(biomass->first);

        const int mov[2][8]={{1,1,0,-1,-1,-1,0,1},{0,1,1,1,0,-1,-1,-1}};
        //omp_set_num_threads(1);
        #pragma omp parallel for private(ubicacion,biomass,array)
        for(i=start;i<=end;i++) {
            //int iteration = 0;
            if (requiredBiomass.find(i) != requiredBiomass.end()) {//existe la comunidad con ese numero?
                biomass = requiredBiomass.find(i);
                //if (biomass->second != 0) {//requisitos distintos a cero
                if (biomass->second != 0){//requisitos distintos a cero
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
                        fill_n(CD_matrix, rows*cols, std::numeric_limits<float>::max());

                        priority_queue<position> CD_costos;
                        int key = 1;
                        int row_temp,col_temp,h;

                        position inicial;
                        CD_costos.push(array);
                        //---------------------------------------------------------------inicia calculo

                        //while(!CD_costos.empty() | )
                        // Limit CD calculation
                        float tmpCost = 0.0;
                        float timeLimit = 12 * 3600; // 12 hours
                        //while(cummulatedTime <= timeLimit  && !CD_costos.empty()){
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
                                if (row_temp < rows && row_temp >= 0 && col_temp < cols && col_temp >= 0 && fric_matrix[(cols*row_temp)+col_temp]>0.0) {
                                    if (h % 2 != 0){//Lateral movement
                                        array.val_fricc = (inicial.val_fricc) + ((fric_matrix[(cols * row_temp) + col_temp])*scale);
                                        //array.single_fricc = ((fric_matrix[(cols * row_temp) + col_temp])*scale);
                                    }else {//Diagonal movement
                                        array.val_fricc = (inicial.val_fricc) + sqrt(2) * ((fric_matrix[(cols * row_temp) + col_temp])*scale);
                                        //array.single_fricc =  sqrt(2) * ((fric_matrix[(cols * row_temp) + col_temp])*scale);
                                    }//se busca el menor valor de CD, es posible que se escriba varias veces en una celda
                                    if (CD_matrix[(cols*row_temp)+col_temp]>array.val_fricc ){
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
                        //IDW_matrix_tmp=objMeth.IDW_test(biomass->second, CD_matrix, IDW_matrix_tmp, rows, cols, exp, nullValue);
                        //---------------IDW
                        int row,col;
                        for(row = 0; row < rows; row++) {
                            for (col = 0; col < cols; col++) {
                                //if(CD_matrix[(cols * row) + col] == numeric_limits<float>::max()){ // null valu
                                //float temp = fric_matrix[(cols * row) + col];
                                if (fric_matrix[(cols * row) + col] < 0.0) {// Null value
                                    //cout << IDW_matrix[(cols * row) + col] << endl;
                                    IDW_matrix[(cols * row) + col] = nullValue;

                                } else{
                                    #pragma omp atomic
                                    IDW_matrix[(cols * row) + col] += biomass->second / pow(CD_matrix[(cols * row) + col], exp);
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
            fileName = "IDW_C++_" + demmand[year].first + to_string(year);
        else
            fileName = "IDW_C++_" + demmand[year].first +"0" + to_string(year);;

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
    }

    end2 = omp_get_wtime();
    double duration = (end2 - start2);//calculate global execution time
    printf("Global time: %lf hours \n", (duration/3600));
    cout << "Process sucessfully finished "<< endl;

    delete IDW_matrix;
    delete fric_matrix;
    delete locsMatrix;

    return 0;
}
