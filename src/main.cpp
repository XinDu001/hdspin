#include <mpi.h>

#include "main_utils.h"
#include "CLI11/CLI11.hpp"


// All utilities in this main module come from main_utils
using namespace main_utils;


int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    utils::SimulationParameters p;

    // PARSER ----------------------------------------------------------------
    // -----------------------------------------------------------------------
    // -----------------------------------------------------------------------
    // Note that it appears this must be handled directly in main().
    CLI::App app{
        "hdspin is an application for simulating binary spin systems"
    };

    app.add_option(
        "-N, --N_spins", p.N_spins,
        "Number of binary spins to use in the simulation. Must be bounded "
        "between 1 and the PRECISON option set at compile time."
    )->check(CLI::Range(1, PRECISON))->required();

    app.add_option(
        "-l, --landscape", p.landscape,
        "Choice of landscape, either the exponential random energy model "
        "(EREM) or the Gaussian random energy model (GREM)."
    )->check(CLI::IsMember({"EREM", "GREM"}))->required();

    app.add_option(
        "-b, --beta", p.beta,
        "Inverse temperature."
    )->check(CLI::PositiveNumber)->required();

    app.add_option(
        "-t, --log10_N_timesteps", p.log10_N_timesteps,
        "The number of timesteps to run on a log10 scale. For example, if "
        "'t=7', then a simulation of length 10^7 will be run."
    )->check(CLI::PositiveNumber)->required();

    app.add_option(
        "-m, --memory", p.memory,
        "The size of the memory cache. If -1, will attempt to use a cache "
        "size equal to 2^N_spins, which might cause memory issues. The "
        "default is 2^25."
    )->check(CLI::PositiveNumber|CLI::IsMember({-1}));

    app.add_option(
        "-d, --dynamics", p.dynamics,
        "The type of dynamics to run. Defaults to 'auto'. Standard dynamics "
        "attempts to flip one spin every timestep, with the Metropolis "
        "acceptance/rejection criterion. Gillespie dynamics calculates all "
        "exit rates at once and flips a spin every iteration of the "
        "algorithm, but with a waiting time not necessarily equal to 1. The "
        "auto selection runs quick simulations of both types to see which "
        "is faster, and selects that one."
    )->check(CLI::IsMember({"standard", "gillespie", "auto"}));

    // YOU LEFT OFF HERE!!!!
    // Need to change this definition in the utils files
    app.add_option(
        "-n, --n_tracers", p.n_tracers,
        "The number of simulations to run in total, defaults to 100."
    )->check(CLI::PositiveNumber);

    app.add_option(
        "--seed", p.seed,
        "Seeds for the random number generators for reproducible runs. Leave "
        "unset for un-reproducible, random seeds."
    )->check(CLI::PositiveNumber);

    app.add_flag("--skip_IS{false}", p.calculate_inherent_structure_observables,
        "Providing the --skip_IS flag informs hdspin to skip all calculations of the inherent structure. This will speed up the simulation and use ~log N less memory in the cache.");

    CLI11_PARSE(app, argc, argv);
    // -----------------------------------------------------------------------
    // -----------------------------------------------------------------------
    // PARSER ----------------------------------------------------------------

    update_parameters_(&p);
    initialize_grids_and_config(p);
    auto_determine_dynamics_(&p);
    execute_process_pool(p);

    MPI_Finalize();
}
