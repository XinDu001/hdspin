#include <sstream>  // oss
#include <algorithm>
#include <fstream>      // std::ofstream

#include "utils.h"
#include "ArbitraryPrecision/ap/ap.hpp"


namespace utils
{

json read_json(const std::string fname)
{
    std::ifstream f(fname);
    return json::parse(f);
}

double mean_vector(const std::vector<double> v)
{
    const double sum = std::accumulate(v.begin(), v.end(), 0.0);
    return sum / v.size();
}

double weighted_mean_vector(const std::vector<double> v, const std::vector<double> weights)
{
    double sum = 0.0;
    // std::cout << v.size() << " " << weights.size() << std::endl;
    assert(v.size() == weights.size());
    for (size_t ii=0; ii<v.size(); ii++)
    {
        sum += v[ii] * weights[ii];
    }
    double total_weight = std::accumulate(weights.begin(), weights.end(), 0.0);
    if (total_weight == 0.0) {total_weight = 1.0;}
    return sum / total_weight;
}

double variance_vector(const std::vector<double> v)
{
    const double mean = mean_vector(v);
    const double sq_sum = std::inner_product(v.begin(), v.end(), v.begin(), 0.0);
    return sq_sum / v.size() - mean * mean;
}

double median_vector(const std::vector<double> v)
{
    std::vector<double> v2 = v;
    std::sort(v2.begin(), v2.end());
    const size_t size = v2.size();
    if (v2.size() % 2 != 0){return v2[size / 2];}
    return (v2[size / 2] + v2[(size / 2) - 1]) / 2;
}

ap_uint<PRECISON> arbitrary_precision_integer_pow(
    const int base, const int exponent)
{
    if (exponent == 0)
    {
        ap_uint<PRECISON> val = 1;
        return val;   
    }
    ap_uint<PRECISON> val = base;
    for (int ii=0; ii<exponent - 1; ii++)
    {
        val *= base;
    }
    return val;
}

long long ipow(long long base, long long exp)
{
    assert(base > 0);
    assert(exp > 0);
    long long result = 1;
    for (;;)
    {
        if (exp & 1){result *= base;}
        exp >>= 1;
        if (!exp){break;}
        base *= base;
    }
    return result;
}


void get_neighbors_(ap_uint<PRECISON> *neighbors, ap_uint<PRECISON> n,
    const unsigned int bitLength)
{   
    for (int b=0; b<bitLength; b++)
    {
        ap_uint<PRECISON> one = 1;
        neighbors[bitLength - 1 - b] = n ^ (one << b);
    }
}

ap_uint<PRECISON> flip_bit(const ap_uint<PRECISON> state, const unsigned int k, const unsigned int bitLength)
{
    const int bit = bitLength - k;
    const ap_uint<PRECISON> one = 1;
    return (state ^ (one << (bit - 1)));
}

void arbitrary_precision_integer_from_int_array_(
    const unsigned int *config, const unsigned int N, ap_uint<PRECISON> &res)
{
    res = 0;
    for (int ii=N-1; ii>=0; ii--)
    {
        const ap_uint<PRECISON> config_val = config[N - ii - 1];
        const ap_uint<PRECISON> power_val = arbitrary_precision_integer_pow(2, ii);
        res += config_val * power_val;
    }
}

void int_array_from_arbitrary_precision_integer_(
    unsigned int *config, const unsigned int N, const ap_uint<PRECISON> &integer)
{
    ap_uint<PRECISON> _integer = integer;
    for (unsigned int ii=0; ii<N; ii++)
    {
        ap_uint<PRECISON> remainder = _integer % 2;
        _integer = _integer / 2;
        config[N - ii - 1] = int(remainder);
    }
}

std::string string_rep_from_arbitrary_precision_integer(const ap_uint<PRECISON> current_state, const unsigned int N)
{
    unsigned int* binary_array = 0;
    binary_array = new unsigned int [N];
    int_array_from_arbitrary_precision_integer_(binary_array, N, current_state);

    std::string s = "";
    for (int ii=0; ii<N; ii++)
    {
        s += std::to_string(binary_array[ii]);
    }

    delete[] binary_array;

    return s;
}

bool _key_exists(const json inp, const std::string key)
{
    const std::string_view string_key = key;
    if (inp.contains(std::string(string_key))){return true;}
    return false;
}

void print_json(const json jrep)
{
    std::cout << "hdspin - simulation parameters" << std::endl;
    std::cout << std::setw(4) << jrep << std::endl;
}


json simulation_parameters_to_json(const utils::SimulationParameters p)
{
    json j = {
        {"log10_N_timesteps", p.log10_N_timesteps},
        {"N_timesteps", p.N_timesteps},
        {"N_spins", p.N_spins},
        {"beta", p.beta},
        {"beta_critical", p.beta_critical},
        {"landscape", p.landscape},
        {"dynamics", p.dynamics},
        {"memory", p.memory},
        {"energetic_threshold", p.energetic_threshold},
        {"entropic_attractor", p.entropic_attractor},
        {"valid_entropic_attractor", p.valid_entropic_attractor},
        {"grid_size", p.grid_size},
        {"dw", p.dw},
        {"n_tracers", p.n_tracers},
        {"use_manual_seed", p.use_manual_seed},
        {"seed", p.seed},
        {"calculate_inherent_structure_observables", p.calculate_inherent_structure_observables},
        {"PRECISON", PRECISON}
    };

    return j;
}


utils::SimulationParameters json_to_simulation_parameters(const json j)
{
    utils::SimulationParameters p;
    p.log10_N_timesteps = j["log10_N_timesteps"];
    p.N_timesteps = j["N_timesteps"];
    p.N_spins = j["N_spins"];
    p.beta = j["beta"];
    p.beta_critical = j["beta_critical"];
    p.landscape = j["landscape"];
    p.dynamics = j["dynamics"];
    p.memory = j["memory"];
    p.energetic_threshold = j["energetic_threshold"];
    p.entropic_attractor = j["entropic_attractor"];
    p.valid_entropic_attractor = j["valid_entropic_attractor"];
    p.grid_size = j["grid_size"];
    p.dw = j["dw"];
    p.n_tracers = j["n_tracers"];
    p.use_manual_seed = j["use_manual_seed"];
    p.seed = j["seed"];
    p.calculate_inherent_structure_observables = j["calculate_inherent_structure_observables"];
    return p;
}

void json_to_file(const json jrep, const std::string& filename)
{
    std::ofstream outputFile(filename);
    if (outputFile.is_open())
    {
        outputFile << std::setw(4) << jrep;
        outputFile.close();
    } else {
        std::cerr << "Unable to open file: " << filename << std::endl;
    }
}


void json_to_file_no_format(const json jrep, const std::string& filename)
{
    std::ofstream outputFile(filename);
    if (outputFile.is_open())
    {
        outputFile << jrep;
        outputFile.close();
    } else {
        std::cerr << "Unable to open file: " << filename << std::endl;
    }
}


utils::FileNames get_filenames(const unsigned int ii)
{
    std::string ii_str = std::to_string(ii);
    ii_str.insert(ii_str.begin(), 8 - ii_str.length(), '0');

    utils::FileNames fnames;
    fnames.ii_str = ii_str;
    fnames.json_final = std::string(DATA_PATH) + ii_str + ".json";
    return fnames;
}



void make_directories()
{
    std::string command = "mkdir " + std::string(DATA_PATH);
    system(command.c_str());
    command = "mkdir " + std::string(GRID_PATH);
    system(command.c_str());
}


// Does nothing for now
void cleanup_directories()
{
    // std::string command = "rm -r " + std::string(GRID_PATH);
    // system(command.c_str());
}

void make_energy_grid_logspace(const size_t log10_timesteps, const size_t n_gridpoints)
{
    std::vector <double> v;
    const double delta = ((double) log10_timesteps) / ((double) n_gridpoints);
    for (size_t ii=0; ii<n_gridpoints + 1; ii++)
    {
        const double val = pow(10.0, ((double) ii * delta));
        v.push_back(val);
    }
    FILE* outfile = fopen(ENERGY_GRID_PATH, "w");
    for (size_t ii=0; ii<v.size(); ii++)
    {
        fprintf(outfile, "%e\n", v[ii]);
    }
    fclose(outfile);
}

void make_pi_grids(const size_t log10_timesteps, const double dw, const size_t n_gridpoints)
{
    std::vector <float> v1;
    std::vector <float> v2;
    const double nMC = pow(10, log10_timesteps);
    const double tw_max = nMC / (dw + 1.0);
    const double delta = log10(tw_max) / ((double) n_gridpoints);

    for (size_t ii=0; ii<n_gridpoints + 1; ii++)
    {
        const double _v1 = pow(10, ((double) ii * delta));
        v1.push_back(_v1);
    }

    const double dw_plus_1 = dw + 1.0;
    for (size_t ii=0; ii<v1.size(); ii++)
    {
        const double _v2 = v1[ii] * dw_plus_1;
        v2.push_back(_v2);
    }

    FILE* outfile1 = fopen(PI1_GRID_PATH, "w");
    FILE* outfile2 = fopen(PI2_GRID_PATH, "w");

    for (size_t ii=0; ii<v1.size(); ii++)
    {
        fprintf(outfile1, "%e\n", v1[ii]);
        fprintf(outfile2, "%e\n", v2[ii]);
    }

    fclose(outfile1);
    fclose(outfile2);
}

void load_grid_(std::vector<double> &grid, const std::string path)
{
    std::ifstream myfile (path);
    std::string line;
    if (myfile.is_open())
    {
        while (getline(myfile, line))
        {
            grid.push_back(stof(line));
        }
        myfile.close();
    }
}

std::string get_datetime()
{
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

double get_time_delta(const std::chrono::time_point<std::chrono::high_resolution_clock> start)
{
    std::chrono::time_point<std::chrono::high_resolution_clock> stop = std::chrono::high_resolution_clock::now();
    const auto dur = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    return std::chrono::duration<double>(dur).count();
}
}
