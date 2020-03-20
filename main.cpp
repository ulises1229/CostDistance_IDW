#include "common.h"
#include "Tratamiento_datos.h"
#include "raster.h"
using namespace std;
int main() {
//------------------------------------------------------------------------matrices de entrada
    float **fricc ;
    float **biomass ;
//-------------------------------------------------------------------------variables iniciales
    short int x_init = 2, y_init = 3;
    short int ROWS;
    short int COLS;
    int requisitos = 20;
//------------------------------------------------------------------------objeto raster
    Raster objrast;
//-------------------------------------------------------------------------------------------------------carga de matriz biomasa
    string input_file ="/home/otavio/Documentos/IDW /Version_2/Haiti_biomass.tif";
     objrast.read_tif_matrix(input_file,biomass,ROWS, COLS);
    printf("\n Carga de mapa de biomasa completa");
//-------------------------------------------------------------------------------------------------------carga de matriz friccion
    input_file ="/home/otavio/Documentos/IDW /Version_2/Haiti_fricc.tif";
    objrast.read_tif_matrix(input_file,fricc,ROWS, COLS);
    printf("\n Carga de mapa de biomasa completa\n");
//-------------------------------------------------------------------------------------------------------generacion de matriz final
    float **final = new float *[ROWS];
    for (int i = 0; i < ROWS; i++)
        final[i] = new float[COLS];
//-------------------------------------------------------------------------------------------------------crear objeto
    Tratamiento_datos objTratamiento_datos(ROWS, COLS);
//---------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------inicio código
//---------------------------------------------------------------------------------------------------------------------------------
    objTratamiento_datos.reset_Matrix(final);//llena la matriz inicial de -1
    final[x_init][y_init]=-1000;//guardamos ubicacion de la aldea

    objTratamiento_datos.IDW(requisitos, fricc, biomass, final,x_init, y_init );

	return 0;
}

