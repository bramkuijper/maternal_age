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
    gsl_vector *x_p = gsl_vector_alloc(8);
    gsl_vector *x_rv = gsl_vector_alloc(16);
    gsl_vector *x_rel = gsl_vector_alloc(8);
    gsl_vector *x_selgrad = gsl_vector_alloc(3);

    // initialize the struct with parameters
    struct rparams paramstruct; 
   
    // ARGINIT
    
    gsl_vector *p11_range = gsl_vector_alloc(10);
    gsl_vector *p10_range = gsl_vector_alloc(10);

    for (int ik = 0; ik < 10; ++ik)
    {
        gsl_vector_set(p11_range, ik, 0);        
        gsl_vector_set(p10_range, ik, 0);
    }

    write_data(&paramstruct,-1);

    // iterate
    int iter;
    for (iter = 0; iter < max_iter ; ++iter)
    {
        psys_recur(&paramstruct, x_p);

        paramstruct.f_1_0_0 = gsl_vector_get(x_p,0);
        paramstruct.f_1_0_1 = gsl_vector_get(x_p,1);
        paramstruct.f_1_1_0 = gsl_vector_get(x_p,2);
        paramstruct.f_1_1_1 = gsl_vector_get(x_p,3);
        paramstruct.f_2_0_0 = gsl_vector_get(x_p,4);
        paramstruct.f_2_0_1 = gsl_vector_get(x_p,5);
        paramstruct.f_2_1_0 = gsl_vector_get(x_p,6);
        paramstruct.f_2_1_1 = gsl_vector_get(x_p,7);

        // reproductive values
        rvsys_recur(&paramstruct, x_rv);

        paramstruct.vj_1_0_0 = gsl_vector_get(x_rv,0);
        paramstruct.vj_1_0_1 = gsl_vector_get(x_rv,1);
        paramstruct.vj_1_1_0 = gsl_vector_get(x_rv,2);
        paramstruct.vj_1_1_1 = gsl_vector_get(x_rv,3);
        paramstruct.vj_2_0_0 = gsl_vector_get(x_rv,4);
        paramstruct.vj_2_0_1 = gsl_vector_get(x_rv,5);
        paramstruct.vj_2_1_0 = gsl_vector_get(x_rv,6);
        paramstruct.vj_2_1_1 = gsl_vector_get(x_rv,7);
        paramstruct.vs_1_0_0 = gsl_vector_get(x_rv,8);
        paramstruct.vs_1_0_1 = gsl_vector_get(x_rv,9);
        paramstruct.vs_1_1_0 = gsl_vector_get(x_rv,10);
        paramstruct.vs_1_1_1 = gsl_vector_get(x_rv,11);
        paramstruct.vs_2_0_0 = gsl_vector_get(x_rv,12);
        paramstruct.vs_2_0_1 = gsl_vector_get(x_rv,13);
        paramstruct.vs_2_1_0 = gsl_vector_get(x_rv,14);
        paramstruct.vs_2_1_1 = gsl_vector_get(x_rv,15);

        // relatedness
        relcoeff_recur(&paramstruct, x_rel);

        paramstruct.r_aSaJ_1 = gsl_vector_get(x_rel,0);
        paramstruct.r_aSaJ_2 = gsl_vector_get(x_rel,1);
        paramstruct.r_mSaJ_1 = gsl_vector_get(x_rel,2);
        paramstruct.r_mSaJ_2 = gsl_vector_get(x_rel,3);
        paramstruct.r_aSmJ_1 = gsl_vector_get(x_rel,4);
        paramstruct.r_aSmJ_2 = gsl_vector_get(x_rel,5);
        paramstruct.r_mSmJ_1 = gsl_vector_get(x_rel,6);
        paramstruct.r_mSmJ_2 = gsl_vector_get(x_rel,7);


        // selection gradients
        selgrads(&paramstruct, x_selgrad);

        bool condition_p11 = (fabs(paramstruct.p11 - gsl_vector_get(x_selgrad, 0)) < 1e-7 || paramstruct.p11 >= 0.999) || paramstruct.p11 <= 0.001;
        bool condition_p10 = (fabs(paramstruct.p10 - gsl_vector_get(x_selgrad, 1)) < 1e-7 || paramstruct.p10 >= 0.999) || paramstruct.p10 <= 0.001;

        if (condition_p11 && condition_p10 && (
            fabs(paramstruct.mt - gsl_vector_get(x_selgrad, 2)) < 1e-07))
        {
            paramstruct.p11 = gsl_vector_get(x_selgrad, 0);
            paramstruct.p10 = gsl_vector_get(x_selgrad, 1);
            paramstruct.mt = gsl_vector_get(x_selgrad, 2);

            break;
        }
        paramstruct.p11 = gsl_vector_get(x_selgrad, 0);
        paramstruct.p10 = gsl_vector_get(x_selgrad, 1);
        paramstruct.mt = gsl_vector_get(x_selgrad, 2);
        
        if (iter > 50000)
        {
            bool found_in_range1 = false;
            bool found_in_range2 = false;

            bool done = false;

            for (int ik = 0; ik < 10; ++ik)
            {
//                cout << "previous t - " << ik << " is: " << paramstruct.p1 << " current p1: " << paramstruct.p1 << " compare: " << fabs(gsl_vector_get(p1_range, ik)-paramstruct.p1) << endl;
 //               cout << "previous t - " << ik << " is: " << paramstruct.p2 << " current p2: " << paramstruct.p2 << " compare: " << fabs(gsl_vector_get(p2_range, ik)-paramstruct.p2) << endl;
                if (!found_in_range1 && fabs(gsl_vector_get(p11_range, ik)-paramstruct.p11) < 1e-10)
                {
                    found_in_range1 = true;
                }

                if (!found_in_range2 && fabs(gsl_vector_get(p10_range, ik)-paramstruct.p10) < 1e-10)
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


        for (int ik = 9; ik > 0; --ik)
        {
            gsl_vector_set(p11_range, ik, gsl_vector_get(p11_range, ik - 1));
            gsl_vector_set(p10_range, ik, gsl_vector_get(p10_range, ik - 1));
        }

        gsl_vector_set(p11_range, 0, paramstruct.p11);
        gsl_vector_set(p10_range, 0, paramstruct.p10);

        if (iter % 100 == 0)
        {
            write_data(&paramstruct,iter);
        }
    }

    write_data(&paramstruct,iter);
    write_params(&paramstruct);


    gsl_vector_free(x_rel);
    gsl_vector_free(x_p);
    gsl_vector_free(x_rv);
    gsl_vector_free(x_selgrad);
}
