#ifndef OBS_PSI_H
#define OBS_PSI_H

#include <vector>
#include <set>

#include "spin.h"
#include "utils.h"


class PsiConfig
{
protected:

    const parameters::FileNames fnames;
    const parameters::SimulationParameters params;
    FILE *outfile;
    const SpinSystem* spin_system_ptr;
    
    // The (roughly) maximum timestep on the counter. It's padded at the end,
    // and is taken care of during post processing.
    long long _max_counter;
    std::vector<long long> _counter;

    // Keep track internally of the waiting time for both the standard
    // trajectory and the inherent structure
    long double _waiting_time = 0.0;

    int _out_of_counter = 0;

public:

    PsiConfig(const parameters::FileNames fnames, const parameters::SimulationParameters params, const SpinSystem& spin_system);
    void step(const double current_waiting_time);
    ~PsiConfig();

};

struct _PsiBasinObjects
{
    FILE* outfile;

    std::vector<long long> _counter;
    std::vector<long long> _counter_unique_configs_per_basin;
    std::set<std::string> _tmp_unique_configs_in_basin;
    
    double threshold;
    bool threshold_valid = true;

    long double _waiting_time = 0.0;
};

class PsiBasin
{
protected:

    // TODO- refeactor so these inherit from a base class
    const parameters::FileNames fnames;
    const parameters::SimulationParameters params;
    const SpinSystem* spin_system_ptr;
    long long _max_counter;
    _PsiBasinObjects E_objects, S_objects;

    void _init_E_objects();
    void _init_S_objects();
    _PsiBasinObjects* _get_psi_basin_object_pointer(const std::string which);
    void _step(const double current_waiting_time, const std::string which);
    void _dump_outfile(const std::string which);
    

public:
    PsiBasin(const parameters::FileNames fnames, const parameters::SimulationParameters params, const SpinSystem& spin_system);
    void step(const double current_waiting_time);
    ~PsiBasin();
};



#endif
