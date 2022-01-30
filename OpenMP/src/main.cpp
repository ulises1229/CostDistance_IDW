#include "common.h"
#include "Metodos.h"
#include "Raster.h"
#include <array>
int main() {
    std::string str;
    double start2;
    double end2;
    start2 = omp_get_wtime();
    //------------------------------------------------------------------------objetos
    Raster objrast;
    metodos objMeth;
//------------------------------------------------------------------------variables
    int rows, cols;//tamaño matriz
    //int row,col;//iteradores matriz
    int cell_null;//valor nulo mapa
    float scale;//escala del mapa
    int num_com;//numero de localidades en el mapa
    float exp = 1.005;//exponente IDW
    int cont = 0;//numero de localidades a explorar en el ciclo while
    position array; //almacenar movimientos de CD
    int i;
// ------------------------------------------------------------------------matrices
    float* fric_matrix; //mapa friccion
    float* localidad_matrix;//mapa con localidades (ubicacion)
    float* IDW_matrix;
    int tmpNull;
// ------------------------------------------------------------------------mapas
    map<int, Raster::local> localidades;// mapa ubicacion localidades
    //estructura local contiene el no. de comunidad y su (x,y)
    std::map<int, Raster::local>::iterator ubicacion; //iterados mapa ubicacion localidades
    map<int, float> biomass_requerida;//mapa requisitos de localidades
    std::map<int, float>::iterator biomass; //iterador mapa requisitos localidad
    //---------------------mapa friccion
    //printf("----matriz friccion\n");
    fric_matrix = objrast.read_tif_matrix("/home/ulises/Kenya_full/fricc_w.tif", rows, cols, scale, cell_null);
    tmpNull = cell_null;
    //cout << "Cell_null"<< tmpNull << endl;
    //printf("Raster scale: %lf \n", scale);
    //---------------------mapa localidades
    //printf("----matriz localidades\n");
    localidad_matrix = objrast.read_tif_matrix("/home/ulises/Kenya_full/locs_c.tif", rows, cols, scale,cell_null);
    //obtenemos el numero de comunidades
    num_com = objrast.contar_comunidades(localidad_matrix, rows, cols, cell_null);
    //---------------------guardamos los requisitos de las comunidades en un mapa
    objrast.carga_requisitos("/home/ulises/Kenya_full/fwuse_W01.csv", biomass_requerida);

    // guardamos las localidades en un mapa para ordenarlas
    int numLoc = objrast.readLocalities(localidad_matrix, rows, cols, localidades, cell_null, num_com);
    cell_null = tmpNull;
    //cout << "Cell_null"<< cell_null << endl;
    cout << "Total number of localities "<< numLoc << endl;
    //valores iniciales
    IDW_matrix = objMeth.reset_Matrix(rows, cols, 0); //llena la matriz inicial del valor indicado

    //-------------------------------------------------------------------------------------------------------inicia calculo modelos
    biomass = biomass_requerida.begin();
    int start =int(biomass->first);
    biomass = --biomass_requerida.end();
    int end =int(biomass->first);

    const int mov[2][8]={{1,1,0,-1,-1,-1,0,1},{0,1,1,1,0,-1,-1,-1}};

    //omp_set_num_threads(1);
    #pragma omp parallel for private(ubicacion,biomass,array)
    for(i=start;i<=end;i++) {
        //int iteration = 0;
        if (biomass_requerida.find(i) != biomass_requerida.end()) {//existe la comunidad con ese numero?
            biomass = biomass_requerida.find(i);
            if (biomass->second != 0) {//requisitos distintos a cero
                if (localidades.find(i) != localidades.end()) { //existe la comunidad con ese numero?
                    ubicacion = localidades.find(biomass->first);//buscar ubicacion de la localidad
                    //ubicaion inicial
                    array.row = ubicacion->second.row;
                    array.col = ubicacion->second.col;
                    array.val_fricc = 0;
                    array.key=0;
                    cont++;//localidades calculadas
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
                                if (h % 2 != 0){//si es un movimiento lateral
                                    array.val_fricc = (inicial.val_fricc) + ((fric_matrix[(cols * row_temp) + col_temp])*scale);
                                    //array.single_fricc = ((fric_matrix[(cols * row_temp) + col_temp])*scale);
                                }else {//si es un movimiento diagonal
                                    array.val_fricc = (inicial.val_fricc) + sqrt(2) * ((fric_matrix[(cols * row_temp) + col_temp])*scale);
                                    //array.single_fricc =  sqrt(2) * ((fric_matrix[(cols * row_temp) + col_temp])*scale);
                                }//se busca el menor valor de CD, es posible que se escriba varias veces en una celda
                                if (CD_matrix[(cols*row_temp)+col_temp]>array.val_fricc ) {
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
                    //IDW_matrix_tmp=objMeth.IDW_test(biomass->second, CD_matrix, IDW_matrix_tmp, rows, cols, exp, cell_null);
                    //---------------IDW
                    int row,col;
                    for(row = 0; row < rows; row++) {
                        for (col = 0; col < cols; col++) {
                            if (CD_matrix[(cols * row) + col] <= 0) {
                                IDW_matrix[(cols * row) + col] = cell_null;
                            } else {
                                #pragma omp atomic
                                IDW_matrix[(cols * row) + col] += biomass->second / pow(CD_matrix[(cols * row) + col], exp);
                            }
                        }
                    }
                    // Free memory on each iteration
                    delete CD_matrix;
                    //free array;
                }
            }
        }
        //cout << "Finished" << i << " th locality" <<endl;
    }
    // ---------------agregar valores nulos en la ubicacion de las localidade
    ubicacion = localidades.begin();
    while (ubicacion != localidades.end()) {
        IDW_matrix[(cols * ubicacion->second.row)+ubicacion->second.col] = cell_null;
        ubicacion++;
    }
    objrast.matrix_to_tiff(IDW_matrix, rows, cols,numLoc,"IDW_C++_limit_CD");//crea tiff de IDW de todas las localidades calculadas
    end2 = omp_get_wtime();
    double duration = (end2 - start2);//calcula tiempo de ejecucion
    //-----------liberar memoria
    delete IDW_matrix;
    delete fric_matrix;
    delete localidad_matrix;
    printf("tiempo global: %lf segundos \n", duration);
    return 0;
}

