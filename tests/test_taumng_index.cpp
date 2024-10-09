
#include <index_taumng.h>
#include <util.h>
#include <omp.h>

void load_data(char* filename, float*& data, unsigned& num,
               unsigned& dim) {  // load data with sift10K pattern
    std::ifstream in(filename, std::ios::binary);
    if (!in.is_open()) {
        std::cout << "open file error" << std::endl;
        exit(-1);
    }
    in.read((char*)&dim, 4);
    in.seekg(0, std::ios::end);
    std::ios::pos_type ss = in.tellg();
    size_t fsize = (size_t)ss;
    num = (unsigned)(fsize / (dim + 1) / 4);
    data = new float[(size_t)num * (size_t)dim];

    in.seekg(0, std::ios::beg);
    for (size_t i = 0; i < num; i++) {
        in.seekg(4, std::ios::cur);
        in.read((char*)(data + i * dim), dim * 4);
    }
    in.close();
}

template <class T>
T* read_bin(const char* filename, uint32_t& npts, uint32_t& dim) {
    std::ifstream in(filename, std::ios::binary);
    if (!in.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        exit(EXIT_FAILURE);
    }
    in.read(reinterpret_cast<char*>(&npts), sizeof(uint32_t));
    in.read(reinterpret_cast<char*>(&dim), sizeof(uint32_t));
    std::cout << "Loading data from file: " << filename << ", points_num: " << npts << ", dim: " << dim << std::endl;
    size_t total_size = static_cast<size_t>(npts) * dim;  // notice if the space is exceed the bound size_t max
    if (total_size > std::numeric_limits<size_t>::max() / sizeof(T)) {
        std::cerr << "Requested size is too large." << std::endl;
        exit(EXIT_FAILURE);
    }
    T* data = new T[total_size];
    in.read(reinterpret_cast<char*>(data), total_size * sizeof(T));
    in.close();
    return data;
}

int main(int argc, char** argv) {
    if (argc != 9) {
        std::cout << argv[0] << " data_file nn_graph_path L R C tau ang save_graph_file "
                  << std::endl;
        exit(-1);
    }
    //float* data_load = NULL;
    unsigned points_num, dim;
    float* data_load = read_bin<float>(argv[1], points_num, dim);

    std::string nn_graph_path(argv[2]);
    unsigned L = (unsigned)atoi(argv[3]);
    unsigned R = (unsigned)atoi(argv[4]);
    unsigned C = (unsigned)atoi(argv[5]);
    float tau = atof(argv[6]);

    IndexTauMNG index(dim, points_num, INNER_PRODUCT, nullptr);

    auto s = std::chrono::high_resolution_clock::now();
    Parameters paras;
    paras.Set<unsigned>("L", L);
    paras.Set<unsigned>("R", R);
    paras.Set<unsigned>("C", C);
    paras.Set<std::string>("nn_graph_path", nn_graph_path);

    index.tau = tau;
    index.ang = 30; // not useful

    omp_set_num_threads(omp_get_num_procs());
    std::cout << "start build. " << std::endl;
    index.Build(points_num, data_load, paras);
    auto e = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = e - s;

    std::cout << "indexing time: " << diff.count() << std::endl;
    index.Save(argv[7]);

    return 0;
}
