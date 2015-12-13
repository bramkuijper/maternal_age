#include <ctime>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <cmath>
#include <cassert>
#include <gsl/gsl_roots.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_eigen.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_sf_erf.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_multiroots.h>
#include "bramauxiliary.h"

using namespace std;

string filename("iter_fecmort");
string filename_new(create_filename(filename));
ofstream DataFile(filename_new.c_str());  // output file 

double E = exp(1.0);

struct rparams
{
    // VARS
};


double bound0(double val)
{
    if (val < 0)
    {
        return(0.0001);
    }

    return(val);
}

double bound01(double val)
{
    val = val < 0 ? 0.0001 : val > 1.0 ? 0.9999 : val;

    return(val);
}

// recursions of all the patch frequencies
int psys_recur(void *params, gsl_vector *f)
{
    //VARFUNC
    
    
    // PATCHRECUR
    
    return GSL_SUCCESS;
}


// reproductive values
int rvsys_recur(void *params, gsl_vector *f)
{
    //VARFUNC
    
    
    // REPVALRECUR
    
}

// relatedness coefficients
int relcoeff_recur(void *params, gsl_vector *f)
{
    //VARFUNC
   
    // RELRECUR 
    
}

void selgrads(void *params, gsl_vector *f)
{
    // VARFUNC

    // SELGRADS

}

void write_params(void *params)
{
    // VARFUNC


    // WRITEPARAMS
}


void write_data(void *params, int time)
{
    // VARFUNC


    if (time < 0)
    {
        // HEADERWRT
    }

    // WRITEDATA
}

int main (int argc, char **argv)
{
    int max_iter = atoi(argv[1]);

    // initialize the vectors that contain the variables
    // functions solve for
    gsl_vector *x_p = gsl_vector_alloc(6);
    gsl_vector *x_rv = gsl_vector_alloc(8);
    gsl_vector *x_rel = gsl_vector_alloc(6);
    gsl_vector *x_selgrad = gsl_vector_alloc(4);

    // initialize the struct with parameters
    struct rparams paramstruct; 
    
    // ARGINIT
    //
//    gsl_vector *p1_range = gsl_vector_alloc(5);
//    gsl_vector *p2_range = gsl_vector_alloc(5);
//
//    for (int ik = 0; ik < 5; ++ik)
//    {
//        gsl_vector_set(p1_range, ik, 0);        
//        gsl_vector_set(p2_range, ik, 0);
//    }


    write_data(&paramstruct,-1);
    // iterate
    for (int iter = 0; iter < max_iter ; ++iter)
    {
        psys_recur(&paramstruct, x_p);
        
        paramstruct.fa0 = gsl_vector_get(x_p,0);
        paramstruct.fa1 = gsl_vector_get(x_p,1);
        paramstruct.fa2 = gsl_vector_get(x_p,2);
        paramstruct.fb0 = gsl_vector_get(x_p,3);
        paramstruct.fb1 = gsl_vector_get(x_p,4);
        paramstruct.fb2 = gsl_vector_get(x_p,5);

        // reproductive values
        rvsys_recur(&paramstruct, x_rv);

        paramstruct.v100 = gsl_vector_get(x_rv,0);
        paramstruct.v101 = gsl_vector_get(x_rv,1);
        paramstruct.v110 = gsl_vector_get(x_rv,2);
        paramstruct.v111 = gsl_vector_get(x_rv,3);
        paramstruct.v200 = gsl_vector_get(x_rv,4);
        paramstruct.v201 = gsl_vector_get(x_rv,5);
        paramstruct.v210 = gsl_vector_get(x_rv,6);
        paramstruct.v211 = gsl_vector_get(x_rv,7);

        // relatedness
        relcoeff_recur(&paramstruct, x_rel);

        paramstruct.raa12 = gsl_vector_get(x_rel, 0);
        paramstruct.raa22 = gsl_vector_get(x_rel, 1);
        paramstruct.ram11 = gsl_vector_get(x_rel, 2);
        paramstruct.ram21 = gsl_vector_get(x_rel, 3);
        paramstruct.rmm10 = gsl_vector_get(x_rel, 4);
        paramstruct.rmm20 = gsl_vector_get(x_rel, 5);

        // selection gradients
        selgrads(&paramstruct, x_selgrad);

        bool condition_p1 = (fabs(paramstruct.p1 - gsl_vector_get(x_selgrad, 0)) < 1e-10 || paramstruct.p1 >= 0.999) || paramstruct.p1 <= 0.001;
        bool condition_p2 = (fabs(paramstruct.p2 - gsl_vector_get(x_selgrad, 1)) < 1e-10 || paramstruct.p2 >= 0.999) || paramstruct.p2 <= 0.001;
        bool condition_i = (fabs(paramstruct.i - gsl_vector_get(x_selgrad, 3)) < 1e-10 || paramstruct.i >= 0.999) || paramstruct.i <= 0.001;

        if (condition_i && condition_p1 && condition_p2 &&
            fabs(paramstruct.mt - gsl_vector_get(x_selgrad, 2)) < 1e-07)
        {
            paramstruct.p1 = gsl_vector_get(x_selgrad, 0);
            paramstruct.p2 = gsl_vector_get(x_selgrad, 1);
            paramstruct.mt = gsl_vector_get(x_selgrad, 2);
            paramstruct.i = gsl_vector_get(x_selgrad, 3);

            write_data(&paramstruct,iter);
            break;
        }

        paramstruct.p1 = gsl_vector_get(x_selgrad, 0);
        paramstruct.p2 = gsl_vector_get(x_selgrad, 1);
        paramstruct.mt = gsl_vector_get(x_selgrad, 2);
        paramstruct.i = gsl_vector_get(x_selgrad, 3);

        if (iter > 50000)
        {
            bool found_in_range1 = false;
            bool found_in_range2 = false;

            bool done = false;

            for (int ik = 0; ik < 5; ++ik)
            {
//                cout << "previous t - " << ik << " is: " << paramstruct.p1 << " current p1: " << paramstruct.p1 << " compare: " << fabs(gsl_vector_get(p1_range, ik)-paramstruct.p1) << endl;
 //               cout << "previous t - " << ik << " is: " << paramstruct.p2 << " current p2: " << paramstruct.p2 << " compare: " << fabs(gsl_vector_get(p2_range, ik)-paramstruct.p2) << endl;
                if (!found_in_range1 && fabs(gsl_vector_get(p1_range, ik)-paramstruct.p1) < 1e-10)
                {
                    found_in_range1 = true;
                }

                if (!found_in_range2 && fabs(gsl_vector_get(p2_range, ik)-paramstruct.p2) < 1e-10)
                {
                    found_in_range2 = true;
                }

                if (found_in_range1 && found_in_range2)
                {
                    done = true;
                    break;
                }
            }

            if (done)
            {
                break;
            }

        // another thing: if p1 = 0.999 or p2 = 0.000
        // it may take long for p2 and p1 (respectively) to converge
        // we'll allow p2 and p1 to take on the same value and see whether they converge
        
        //    if (paramstruct.p1 >= 0.999 && paramstruct.p2 > 0.5 && paramstruct.p2 < 0.999)
        //    {
        //        paramstruct.p2 = paramstruct.p1;
        //    } else if (paramstruct.p2 <= 0.0001 && paramstruct.p1 < 0.5 && paramstruct.p1 > 0.001)
        //    {
        //        paramstruct.p1 = paramstruct.p2;
        //    }
        }


        for (int ik = 4; ik > 0; --ik)
        {
            gsl_vector_set(p1_range, ik, gsl_vector_get(p1_range, ik - 1));
            gsl_vector_set(p2_range, ik, gsl_vector_get(p2_range, ik - 1));
        }

        gsl_vector_set(p1_range, 0, paramstruct.p1);
        gsl_vector_set(p2_range, 0, paramstruct.p2);

        if (isnan(paramstruct.mt) != 0)
        {
            write_data(&paramstruct,iter);
            break;
        }

        if (iter % 100 == 0)
        {
            write_data(&paramstruct,iter);
        }
    }

    write_params(&paramstruct);


    gsl_vector_free(x_rel);
    gsl_vector_free(x_p);
    gsl_vector_free(x_rv);
    gsl_vector_free(x_selgrad);
}
