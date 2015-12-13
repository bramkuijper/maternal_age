// Gillespie simulation 
// for the evolution of age-dependent nongenetic effects

#include <ctime>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <cmath>
#include <cassert>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include "bramauxiliary.h"

//#define NDEBUG

using namespace std;


// random number generator 
// see http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Generation.html#Random-Number-Generation 
gsl_rng_type const * T; // gnu scientific library rng type
gsl_rng *r; // gnu scientific rng 

const size_t Npatch = 4000; 
const size_t numgen = 80000;
int sample = 20;
double d0 = 0.01; //some starting values
double h0 = 0.01;

int seed = -1; // the random seed
time_t total_time;  // track the total time the simulation is running
size_t generation = 0; // track the current generation (for debugging only)

size_t skip2 = numgen; // intervals of data output

double envt_switch[2] = {0,0}; // patch state switch rate
double k = 0.001; // cost of dispersal

// mortality of maladapted junior breeders in both envts
double c_young[2] = {0,0}; 

// mortality of adapted junior breeders in both envts
double C_young[2] = {0,0}; 

// mortality of maladapted senior breeders in both envts
double c_old[2] = {0,0};

// mortality of adapted senior breeders in both envts
double C_old[2] = {0,0};

// mutation rates
double mu_h = 0.01;
double sdmu_h = 0.01;

// diploid individuals
struct Individual
{
    double d[2]; // one locus coding for migration
    double p1[2]; // proportion z1 offspring by z1 junior
    double p2[2]; // proportion z1 offspring by z2 junior
    bool z; // state of the individual wrt its environment, 0, 1

    bool is_senior;
};

struct Patch
{
    Individual localJun; // the local junior
    Individual localSen; // the local senior
    bool state; //environmental state of the patch

};

Patch MetaPop[Npatch];


// statistics for sampling
// number of patches in environment one or two
// with senior maladapted/adapted and junior maladapted/adapted
// for example Npatches[0][1][0] == 50 implies that there are 50
// patches in envt e1 with an adapted senior and a maladapted junior
size_t Npatches[2][2][2];
// next to counts store lists with ids
size_t patch_ids[2][2][2][Npatch];

// give the outputfile a unique name
string filename("sim_px");
string filename_new(create_filename(filename));
ofstream DataFile(filename_new.c_str());  // output file 

// initialize the command line arguments to vary 
// the parameters
void init_arguments(int argc, char *argv[])
{
    envt_switch[0] = atof(argv[1]);
    envt_switch[1] = atof(argv[2]);
    c_young[0] = atof(argv[3]);
    c_young[1] = atof(argv[4]);
    C_young[0] = atof(argv[5]);
    C_young[1] = atof(argv[6]);
    c_old[0] = atof(argv[7]);
    c_old[1] = atof(argv[8]);
    C_old[0] = atof(argv[9]);
    C_old[1] = atof(argv[10]);
    k = atof(argv[11]);
    mu_h = atof(argv[12]);
    sdmu_h = atof(argv[13]);
    d0 = atof(argv[14]);
    h0 = atof(argv[15]);
}

// initialization function, runs at start of sim
// gives all the individuals some starting
// value
void init_pop()
{ 
    // start the time
    total_time = time(NULL);

    // obtain a seed from current nanosecond count
	seed = get_nanoseconds();

    // set the seed to the random number generator
    // stupidly enough, this can only be done by setting
    // a shell environment parameter
    stringstream s;
    s << "GSL_RNG_SEED=" << setprecision(10) << seed;
    putenv(const_cast<char *>(s.str().c_str()));

    // set up the random number generators
    // (from the gnu gsl library)
    gsl_rng_env_setup();
    T = gsl_rng_default;
    r = gsl_rng_alloc(T);

    // initialize patch state counters and set them to 0
    for (int envt_i = 0; envt_i < 2; ++envt_i)
    {
        for (int state_sen = 0; state_sen < 2; ++state_sen)
        {
            for (int state_jun = 0; state_jun < 2; ++state_jun)
            {
                Npatches[envt_i][state_sen][state_jun] = 0;            
            }
        }
    }

    // keep track of the number of adapted seniors and juniors
    // in each patch
    int N_adapted_sen = 0;
    int N_adapted_jun = 0;

    // initialize all the patches
    for (size_t i = 0; i < Npatch; ++i)
    {
        //patches are randomly put in env + or -
        MetaPop[i].state = gsl_rng_uniform(r) < 0.5; 

        // reset counters of number of adapted seniors
        // and juniors 
        N_adapted_sen = 0;
        N_adapted_jun = 0;

        // randomly assign initial phenotypes
        MetaPop[i].localJun.z = gsl_rng_uniform(r) < 0.5;
        MetaPop[i].localSen.z = gsl_rng_uniform(r) < 0.5;
        
        // state the obvious (for debugging)
        MetaPop[i].localSen.is_senior = true;
        MetaPop[i].localJun.is_senior = false;

        // initialize allelic values
        for (int allele = 0; allele < 2; ++allele)
        {
            MetaPop[i].localSen.d[allele] = d0;
            MetaPop[i].localSen.p1[allele] = h0;
            MetaPop[i].localSen.p2[allele] = h0;
            
            MetaPop[i].localJun.d[allele] = d0;
            MetaPop[i].localJun.p1[allele] = h0;
            MetaPop[i].localJun.p2[allele] = h0;
        }

        // update counters
        N_adapted_sen += MetaPop[i].localSen.z == MetaPop[i].state;
        N_adapted_jun += MetaPop[i].localJun.z == MetaPop[i].state;

        // increase the counter of patches of
        // (i) environmental state 'state'
        // (ii) containing a senior individual maladapted/adapted
        // (iii) containing a junior individual maladapted/adapted
        //
        size_t Npatches_of_this_type = Npatches[MetaPop[i].state][
            MetaPop[i].localSen.z == MetaPop[i].state
        ][
            MetaPop[i].localJun.z == MetaPop[i].state
        ];

        // append this patch to the corresponding stack of patch id's
        patch_ids[MetaPop[i].state][
                MetaPop[i].localSen.z == MetaPop[i].state
            ][
                MetaPop[i].localJun.z == MetaPop[i].state
            ][Npatches_of_this_type] = i;

        // update counter
        ++Npatches[MetaPop[i].state][
            MetaPop[i].localSen.z == MetaPop[i].state
        ][
            MetaPop[i].localJun.z == MetaPop[i].state
        ];
            
    }
}


double mut_h(double p)
{
    assert(p >= 0 && p <= 1);
    p+= gsl_rng_uniform(r) < mu_h ? gsl_ran_gaussian(r,sdmu_h) : 0;
    p = p < 0 ? 0 : p > 1 ? 1 : p;

    return(p);
}

// make a kid out of parent asexually
// add it either to local offspring pool
// or to pool of dispersers
void create_kid(Individual &mother, Individual &Kid)
{
    // if it is a phenotype 1 mother, 
    // we'll need to express p1
    // otherwise p2
    double expr_h;

    // mother's phenotype is one
    expr_h = mother.is_senior ? 0.5 * (mother.p2[0] + mother.p2[1]) 
                                :
                                0.5 * (mother.p1[0] + mother.p1[1]);

    // offspring gets z1 or z2 phenotype
    Kid.z = gsl_rng_uniform(r) < expr_h ? 0 : 1;

    // inherit alleles
    for (int allele_i = 0; allele_i < 2; ++allele_i)
    {
        Kid.d[allele_i] = mut_h(mother.d[allele_i]);
        Kid.p1[allele_i] = mut_h(mother.p1[allele_i]);
        Kid.p2[allele_i] = mut_h(mother.p2[allele_i]);
    }

    Kid.is_senior = false;
}

// do the stats on the data
void write_data()
{
    // get variance and means
    double p1,p2,d,vard,varp1,varp2;
    double meanp1 = 0;
    double meanp2 = 0;
    double ssp1 = 0;
    double ssp2 = 0;

    double meand = 0;
    double ssd = 0;

    // loop through patches and individuals
    // and get stats on genotypes and patch freqs.
    for (size_t i = 0; i < Npatch; ++i)
    {
        d = 0.5 * (MetaPop[i].localJun.d[0] + MetaPop[i].localJun.d[1]);
        p1 = 0.5 * (MetaPop[i].localJun.p1[0] + MetaPop[i].localJun.p1[1]);
        p2 = .5 * (MetaPop[i].localJun.p2[0] + MetaPop[i].localJun.p2[1]);

        meanp1+=p1;
        meanp2+=p2;
        meand+=d;
        ssp1+=p1*p1;
        ssp2+=p2*p2;
        ssd+=d*d;
        
        d = 0.5 * (MetaPop[i].localSen.d[0] + MetaPop[i].localSen.d[1]);
        p1 = 0.5 * (MetaPop[i].localSen.p1[0] + MetaPop[i].localSen.p1[1]);
        p2 = .5 * (MetaPop[i].localSen.p2[0] + MetaPop[i].localSen.p2[1]);

        meanp1+=p1;
        meanp2+=p2;
        meand+=d;
        ssp1+=p1*p1;
        ssp2+=p2*p2;
        ssd+=d*d;
    }

    DataFile << generation << ";"; 

    // write down all the patch frequency combinations
    for (size_t envt_i = 0; envt_i < 2; ++envt_i)
    {
        for (size_t state_sen = 0; state_sen < 2; ++state_sen)
        {
            for (size_t state_jun = 0; state_jun < 2; ++state_jun)
            {
                DataFile << setprecision(5) << (double)Npatches[envt_i][state_sen][state_jun] / Npatch << ";";
            }
        }
    }

    meanp1 /= Npatch * 2;
    meanp2 /= Npatch * 2;
    meand /= Npatch * 2;
    vard = ssd / (Npatch * 2) - meand*meand;
    varp1 = ssp1 / (Npatch * 2) - meanp1*meanp1;
    varp2 = ssp2 / (Npatch * 2) - meanp2*meanp2;

    DataFile 
        << setprecision(5) << meanp1 << ";"
        << setprecision(5) << meanp2 << ";"
        << setprecision(5) << varp1 << ";"
        << setprecision(5) << varp2 << ";"
        << setprecision(5) << meand << ";"
        << setprecision(5) << vard << endl;
}

// headers of the datafile
void write_data_headers()
{
    DataFile << "generation;";

    for (size_t envt_i = 0; envt_i < 2; ++envt_i)
    {
        for (size_t state_sen = 0; state_sen < 2; ++state_sen)
        {
            for (size_t state_jun = 0; state_jun < 2; ++state_jun)
            {
                DataFile << setprecision(5) << "f_" << (envt_i+1) << "_" << (state_sen + 1) << "_" <<  (state_jun + 1) << ";";
            }
        }
    }
    
    DataFile << "meanp1;meanp2;varp1;varp2;meand;vard" << endl;
}

// parameters at the end of the sim
void write_parameters()
{
    total_time = time(NULL) - total_time;
    DataFile << endl << endl << "d0;" << d0 << endl
                << "type;" << "phenotype-dependent" << endl
                << "p0;" << h0 << endl
                << "mu_h;" << mu_h << endl
                << "sdmu_h;" << sdmu_h << endl
                << "numgen;" << numgen << endl
                << "k;" << k << endl
                << "s1;" << envt_switch[0] << endl
                << "s2;" << envt_switch[1] << endl
                << "c1;" << c_young[0] << endl
                << "c2;" << c_young[1] << endl
                << "C1;" << C_young[0] << endl
                << "C2;" << C_young[1] << endl
                << "c3;" << c_old[0] << endl
                << "c4;" << c_old[1] << endl
                << "C3;" << C_old[0] << endl
                << "C4;" << C_old[1] << endl
                << "seed;" << seed << endl
                << "Npatches;" << Npatches << endl
                << "runtime;" << total_time << endl;
}

void switch_patch_state(size_t patch_envt, size_t sen_adapted, size_t jun_adapted)
{
    // sample a random patch that fulfills the conditions
    size_t random_patch = 
        gsl_rng_uniform_int(r, 
            Npatches[patch_envt][sen_adapted][jun_adapted]
        );

    // get the patch_id 
    size_t random_patch_id = 
        patch_ids[patch_envt][sen_adapted][jun_adapted][random_patch];

    // error checking
    assert(MetaPop[random_patch_id].state == patch_envt);
    assert(sen_adapted ? 
            MetaPop[random_patch_id].localSen.z == patch_envt : 
            MetaPop[random_patch_id].localSen.z != patch_envt);
    assert(jun_adapted ? 
            MetaPop[random_patch_id].localJun.z == patch_envt : 
            MetaPop[random_patch_id].localJun.z != patch_envt);

    // update patch characteristics
    MetaPop[random_patch_id].state = !MetaPop[random_patch_id].state;

    size_t patch_envt_new = patch_envt == 0 ? 1 : 0; 
    size_t sen_adapted_new = sen_adapted == 0 ? 1 : 0;
    size_t jun_adapted_new = jun_adapted == 0 ? 1 : 0;

    
    // update statistics
    //
    // delete patch id in the corresponding stack
    // by replacing it with the last patch id in the stack
    // and then deleting the last element (by reducing the counter by 1)
    patch_ids[patch_envt][sen_adapted][jun_adapted][random_patch] =
        patch_ids[patch_envt][sen_adapted][jun_adapted][
            --Npatches[patch_envt][sen_adapted][jun_adapted] 
        ];

    // add patch id to the correct stack
    patch_ids[patch_envt_new][sen_adapted_new][jun_adapted_new][
        Npatches[patch_envt_new][sen_adapted_new][jun_adapted_new]++
    ] = random_patch_id;
    
//    cout << patch_envt_new << " " << patch_envt << endl 
//        << sen_adapted_new << " " << sen_adapted << endl 
//        << jun_adapted_new << " " << jun_adapted << endl
//        << MetaPop[random_patch_id].localJun.z << endl
//        << MetaPop[random_patch_id].localSen.z << endl
//        << MetaPop[random_patch_id].state << endl;
    
    assert(MetaPop[random_patch_id].state == patch_envt_new);
    assert(sen_adapted_new ? 
            MetaPop[random_patch_id].localSen.z == patch_envt_new : 
            MetaPop[random_patch_id].localSen.z != patch_envt_new);
    assert(jun_adapted_new ? 
            MetaPop[random_patch_id].localJun.z == patch_envt_new : 
            MetaPop[random_patch_id].localJun.z != patch_envt_new);
}

// mortality of an individual in one of the patches in state 
// patch_envt, sen_adapted, jun_adapted
//
// size_t patch_envt: the environment of the patch the mortality event took place in
// size_t sen_adapted: the state of the senior in the patch the mortality event took place in
// size_t jun_adapted: the state of the junior in the patch the mortality event took place in
// bool mortality_of_senior: whether the mortality affected someone senior (true) or junior)

void mortality(size_t patch_envt, size_t sen_adapted, size_t jun_adapted, bool mortality_of_senior)
{
    // sample a random patch that fulfills the conditions
    size_t random_patch = 
        gsl_rng_uniform_int(r, 
            Npatches[patch_envt][sen_adapted][jun_adapted]
        );

    // get the patch_id 
    size_t random_patch_id = 
        patch_ids[patch_envt][sen_adapted][jun_adapted][random_patch];

    // error checking
    assert(MetaPop[random_patch_id].state == patch_envt);
    assert(sen_adapted ? 
            MetaPop[random_patch_id].localSen.z == patch_envt : 
            MetaPop[random_patch_id].localSen.z != patch_envt);
    assert(jun_adapted ? 
            MetaPop[random_patch_id].localJun.z == patch_envt : 
            MetaPop[random_patch_id].localJun.z != patch_envt);

    // fecundity selection absent
    // so whether we get immigrant 
    // only depends on dispersal probability
    
    int sampled = 2 + sample;

    double choose_prob[sampled];
    int patch_origin[sampled];

    // array saying whether current parent is senior
    bool parent_is_sen[sampled];

    double sumprob = 0;
    double d = 0;

    // make cumulative distribution over all sampled remote offspring
    // mimicking competition for breeding spots
    //
    // offspring from junior
    d = 0.5 * (
            MetaPop[random_patch_id].localJun.d[0] + 
            MetaPop[random_patch_id].localJun.d[1]
            );

    patch_origin[0] = random_patch_id;
    parent_is_sen[0] = false;

    choose_prob[0] = 1 - d;
    sumprob = choose_prob[0];
            
    // offspring from senior
    d = 0.5 * (
            MetaPop[random_patch_id].localSen.d[0] + 
            MetaPop[random_patch_id].localSen.d[1]
            );

    patch_origin[1] = random_patch_id;
    parent_is_sen[1] = true;

    choose_prob[1] = sumprob + 1 - d;
    sumprob = choose_prob[1];

    // do some weighing of the dispersers
    double pi = 2.0 / sample;

    // make cumulative distribution over all sampled remote offspring
    // mimicking competition for breeding spots
    for (int i = 2; i < sampled; ++i)
    {
        patch_origin[i] = gsl_rng_uniform_int(r,Npatch);

        // let senior or junior parent reproduce
        parent_is_sen[i] = gsl_rng_uniform(r) < 0.5; 

        d = parent_is_sen[i] ?
                    0.5 * (
                        MetaPop[patch_origin[i]].localSen.d[0] + 
                        MetaPop[patch_origin[i]].localSen.d[1]
                        ) 
                    :
                    0.5 * (
                        MetaPop[patch_origin[i]].localJun.d[0] + 
                        MetaPop[patch_origin[i]].localJun.d[1]
                        );

        choose_prob[i] = sumprob + (1-k) * pi * d;

        sumprob = choose_prob[i];
    }

    double deviate = gsl_rng_uniform(r) * sumprob;

    Individual Kid;

    bool kid_created = false;

    for (int i = 0; i < sampled; ++i)
    {
        if (deviate <= choose_prob[i])
        {
            kid_created = true;

            if (parent_is_sen[i])
            {
                create_kid(
                        MetaPop[patch_origin[i]].localSen,
                        Kid);
            }
            else
            {
                create_kid(
                        MetaPop[patch_origin[i]].localJun,
                        Kid);
            }
            break;
        }
    }

    assert(kid_created);

    // keep track of new state variables
    // to update counters (see below)
    size_t sen_adapted_new = sen_adapted;

    // first we treat mortality of senior
    // when state of junior equal to that of senior
    // hence, nothing changes for the senior part
    // so just replace them and then act like this
    // is a juvenile mortality
    if (mortality_of_senior)
    {
        // check whether new senior 
        // has a different state
        sen_adapted_new = 
            MetaPop[random_patch_id].localJun.z == MetaPop[random_patch_id].state;

        // overwrite senior
        MetaPop[random_patch_id].localSen = MetaPop[random_patch_id].localJun;
        MetaPop[random_patch_id].localSen.is_senior = true;
    }

    // then mortality of junior
    size_t jun_adapted_new = Kid.z == MetaPop[random_patch_id].state;

    MetaPop[random_patch_id].localJun = Kid;
    MetaPop[random_patch_id].localJun.is_senior = false;

    // update statistics
    //
    // delete patch id in the corresponding stack
    // by replacing it with the last patch id in the stack
    // and then deleting the last element (by reducing the counter by 1)
    patch_ids[patch_envt][sen_adapted][jun_adapted][random_patch] =
        patch_ids[patch_envt][sen_adapted][jun_adapted][
            --Npatches[patch_envt][sen_adapted][jun_adapted] 
        ];

    // add patch id to the correct stack
    patch_ids[patch_envt][sen_adapted_new][jun_adapted_new][
        Npatches[patch_envt][sen_adapted_new][jun_adapted_new]++
    ] = random_patch_id;

    // we're done.
    // error checking
    assert(MetaPop[random_patch_id].state == patch_envt);
    assert(sen_adapted_new ? 
            MetaPop[random_patch_id].localSen.z == patch_envt : 
            MetaPop[random_patch_id].localSen.z != patch_envt);
    assert(jun_adapted_new ? 
            MetaPop[random_patch_id].localJun.z == patch_envt : 
            MetaPop[random_patch_id].localJun.z != patch_envt);
}

// main function body
int main(int argc, char **argv)
{
    init_arguments(argc, argv);
    init_pop();
    write_parameters();
    write_data_headers();

    // there are this number of n probabilities:
    // 1. switching
    // 2. mortality junior
    // 3. mortality senior
    //
    // these can affect 2 * 2 * 2 different patch types
    // i. 2 x envt, ii. 2 x state senior, iii. 2 x state junior
    size_t nprobs = 3 * 2 * 2 * 2 ;

    double probs[nprobs]; // vector with probabilities

    bool done; // variable to get out of nested loop
    double prob; // actual event probability sampled from 
                // the cumulative prob distribution

    // set up as a markovian process of numgen^2 timesteps
    // if we would have used a single variable then we would 
    // run into problems as the minimum specification of the maximum size
    // of size_t = ~65500
    for (generation = 0; generation < numgen; ++generation)
    {
        for (size_t generation2 = 0; generation2 < numgen; ++generation2)
        {
            // generate cumulative prob. distribution of possible
            // events
            double sum_probs = 0;
            size_t prob_count = 0;

            // loop through the different patch types 
            for (size_t patch_envt = 0; patch_envt < 2; ++patch_envt)
            {
                // senior adapted yes/no
                for (size_t sen_adapted = 0; sen_adapted < 2; ++sen_adapted)
                {
                    // junior adapted yes/no
                    for (size_t jun_adapted = 0; jun_adapted < 2; ++jun_adapted)
                    {
                        // probability that a patch switches state
                        sum_probs += envt_switch[patch_envt] 
                            * Npatches[patch_envt][sen_adapted][jun_adapted];

                        probs[prob_count++] = sum_probs; 


                        // mortality young individual
                        sum_probs += (
                                    jun_adapted == 0 ? 
                                        (1.0 / (1.0 - c_young[patch_envt])) 
                                        : 
                                        (1.0 / (1.0 - C_young[patch_envt]))
                                    ) 
                            * Npatches[patch_envt][sen_adapted][jun_adapted];
                        
                        probs[prob_count++] = sum_probs; 

                        // mortality old individual
                        sum_probs += (
                                    sen_adapted == 0 ? 
                                        (1.0 / (1.0 - c_old[patch_envt])) 
                                        : 
                                        (1.0 / (1.0 - C_old[patch_envt]))
                                    ) 
                            * Npatches[patch_envt][sen_adapted][jun_adapted];
                        
                        probs[prob_count++] = sum_probs; 

                        assert(prob_count <= nprobs);
                    }
                }
            }

            // decide which event will happen
            //
            // draw value from cumul distribution
            prob = gsl_rng_uniform(r) *  sum_probs;

            prob_count = 0;

            // variable to break out of all the loops
            done = false;

            // locate the event
            // in the cumulative distribution
            for (size_t patch_envt = 0; patch_envt < 2; ++patch_envt)
            {
                if (done)
                {
                    break;
                }
                for (size_t sen_adapted = 0; sen_adapted < 2; ++sen_adapted)
                {
                    if (done)
                    {
                        break;
                    }
                    for (size_t jun_adapted = 0; jun_adapted < 2; ++jun_adapted)
                    {
                        if (prob <= probs[prob_count])
                        {
                            switch_patch_state(patch_envt, sen_adapted, jun_adapted);
                            done = true;
                            break;
                        }

                        ++prob_count;

                        // mortality young individual
                        if (prob <= probs[prob_count])
                        {
                            mortality(patch_envt, sen_adapted, jun_adapted, false);
                            done = true;
                            break;
                        }

                        ++prob_count;

                        // mortality old individual
                        if (prob <= probs[prob_count])
                        {
                            mortality(patch_envt, sen_adapted, jun_adapted, true);
                            done = true;
                            break;
                        }
                        
                        ++prob_count;
                    }
                }
            } // end for patch_envt
    
            if (generation2 % skip2 == 0)
            {
                write_data();
            }
        } //end for size_t generation2
    } // end for size_t _generation

    write_data();

    exit(1);
}
