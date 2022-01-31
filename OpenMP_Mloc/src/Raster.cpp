//
// Created by otavio on 05/03/20.
//
#include "Raster.h"
#include <sstream>
#include <iostream>
#include <fstream>

//-------------------------------------------------------------------------------------------------------------------------

double adfGeoTransform[6];
//-------------------------------------------------------------------------------------------------------------------------
float* Raster::read_tif_matrix(std::string file,int &rows, int &cols, float &scale,int &cell_null){
    int row,col;//iteradores matriz
    GDALDataset *dataset;
    GDALAllRegister();
    string ds = file;
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

    poBand->RasterIO(GF_Read, 0, 0, cols, rows, pBuf, cols, rows, GDT_Float32, 0, 0);

    int location;
    for (row = 0; row < rows; row++)
        for ( col = 0; col < cols; col++){
            location = (cols * (row)) + col;
            matrix[(cols*row)+col] = *(pBuf+location);
        }
    /*printf("Columnas: %d\n", cols);
    printf("Renglones: %d\n", rows);
    printf("valor nulo: %d\n", cell_null);*/
    return matrix;
}
//-------------------------------------------------------------------------------------------------------------------------
void Raster::print_raster(float *matrix, int rows, int cols) {
    printf("Imprimiento matriz \n");
    for(int row=0; row< rows; row++){
        for(int col =0; col < cols; col++){
            printf(" %f ",matrix[(cols*row)+col]);
        }
        printf("\n");
    }
}
//-------------------------------------------------------------------------------------------------------------------------
int Raster::contar_comunidades(float *mapa_local, int rows, int cols,int cell_null) {
    int count=0;
    for(int row=0;row<rows;row++)
        for(int col=0;col<cols;col++){
            if(mapa_local[(cols*row)+col] != cell_null) {
                count++;
            }
        }
    //printf("Total de comunidades es %d\n",count);
    return count;
}

//-------------------------------------------------------------------------------------------------------------------------
int Raster::readLocalities(float *map_local, int rows, int cols, map<int,local> &local_ord, int cell_null, int num_local) {
    local array;
    int countLoc=0;
    for(int row=0;row<rows;row++){
        for(int col=0;col<cols;col++){
            if(map_local[(cols*row)+col]  != cell_null) {
                //if (countLoc == 1)
                //    break;
                array.row = row;
                array.col = col;
                local_ord[(int)map_local[(cols*row)+col]] = array;
                countLoc ++ ;
            }
        }
    }
    return countLoc;
}

//-------------------------------------------------------------------------------------------------------------------------
int Raster::no_row(string name){
    int counter = 0;
    ifstream file(name.c_str());
    string data;
    while(getline(file,name))
        counter++;
    return counter;
}



string Raster::readFileIntoString(const string& path) {
    auto ss = ostringstream{};
    ifstream input_file(path);
    if (!input_file.is_open()) {
        cerr << "Could not open the file - '"
             << path << "'" << endl;
        exit(EXIT_FAILURE);
    }
    ss << input_file.rdbuf();
    return ss.str();
}


//-------------------------------------------------------------------------------------------------------------------------
vector<pair<string, vector<float>>> Raster::loadDemmand(string name){


    vector<pair<string, vector<float>>> result;


    // Create an input filestream
    std::ifstream myFile(name);

    // Make sure the file is open
    if(!myFile.is_open()) throw runtime_error("Could not open file");

    // Helper vars
    string line, colname;
    float val;

    // Read the column names
    if(myFile.good())
    {
        // Extract the first line in the file
        getline(myFile, line);

        // Create a stringstream from line
        stringstream ss(line);

        // Extract each column name
        while(getline(ss, colname, ',')){

            // Initialize and add <colname, int vector> pairs to result
            result.push_back({colname, vector<float> {}});
        }
    }

    // Read data, line by line
    while(getline(myFile, line))
    {
        // Create a stringstream of the current line
        stringstream ss(line);

        // Keep track of the current column index
        int colIdx = 0;

        // Extract each integer
        while(ss >> val){

            // Add the current integer to the 'colIdx' column's values vector
            result.at(colIdx).second.push_back(float(val));

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
//-------------------------------------------------------------------------------------------------------------------------
void Raster::matrix_to_tiff(float *output_raster, int rows, int cols, int count,string name) {
    int row,col;//iteradores
    GDALDataset *poDstDS;
    GDALDriver *poDriver;
    OGRSpatialReference oSRS;
    string proyeccion = "EPSG:21037";
    string fileName = name + to_string(count)+ ".tif";
    //cout << fileName << endl;
    poDriver = GetGDALDriverManager()->GetDriverByName("Gtiff");
    poDstDS = poDriver->Create( fileName.c_str(), cols, rows, 1, GDT_Float32, NULL);
    // set raster projection
    poDstDS->SetProjection(projection);
    poDstDS->SetGeoTransform(adfGeoTransform);
    oSRS.SetWellKnownGeogCS(proyeccion.c_str());
    //cout << "Projection: \n" << endl;

    GDALRasterBand *poBand;
    float *pBuf = new float[rows * cols], maxVal = 0;
    for(row= 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            pBuf[(row * cols) + col] = output_raster[(cols*row)+col];
            if(output_raster[(cols*row)+col] <= 0.0)
                pBuf[row * cols + col] = -9999;
            if(output_raster[(cols*row)+col] > maxVal)
                maxVal = output_raster[(cols*row)+col];
        }
    }

    poBand = poDstDS->GetRasterBand(1);
    poDstDS->GetRasterBand(1)->SetNoDataValue(-9999);
    poBand->RasterIO( GF_Write, 0, 0, cols, rows,pBuf, cols, rows, GDT_Float32, 0, 0 );
    GDALClose( (GDALDatasetH) poDstDS );

}
