#include "common.h"

class Raster {
public:
    typedef struct local{
        int row;
        int col;
        int num_local;
    }local;

    float* read_tif_matrix(string name,int &rows, int &cols, float &scale,int &cell_null);

    void print_raster(float *matrix, int rows, int cols);
    int contar_comunidades(float *mapa_local, int rows, int cols,int cell_null);
    int readLocalities(float *map_local, int rows, int cols, map<int,local> &local_ord, int cell_null);
    int no_row(string name);
    string readFileIntoString(const string& path);
    vector<pair<string, vector<float>>> loadDemmand(string name);
    void matrix_to_tiff(float *output_raster, int rows, int cols, int count, string name, int nullValue);
    
private:
    const char *projection;
};
