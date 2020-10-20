#ifndef SPIN_GILLESPIE_H
#define SPIN_GILLESPIE_H

#include <random>

#include "Utils/structures.h"
#include "Spin/base.h"


class GillespieSpinSystem : public SpinSystem
{
private:
    // Pointer to the delta E and exit rates
    double *delta_E = 0;
    double *exit_rates = 0;

    // Fills the exit_rates and delta_E arrays and returns the total exit
    // rate.
    double _calculate_exit_rates();

public:
    GillespieSpinSystem(const RuntimeParameters);

    // Step computes the neighboring energies, delta E values and exit rates,
    // then based on that information, steps the spin configuration and
    // returns the waiting time. Note that a Gillespie step is always accepted.
    long double step_();

    ~GillespieSpinSystem();
};


#endif