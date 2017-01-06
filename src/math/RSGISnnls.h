
/* When included within RSGISLib this code was downloaded from
 http://hesperia.gsfc.nasa.gov/~schmahl/nnls/ on June 6th 2012 */


#ifndef RSGISnnls_C
#define RSGISnnls_C

#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <math.h>

#include "common/RSGISCommons.h"

#define nnls_max(a,b) ((a) >= (b) ? (a) : (b))
#define nnls_abs(x) ((x) >= 0 ? (x) : -(x))

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#ifdef _MSC_VER
    #ifdef rsgis_maths_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis{namespace math{
        
    class DllExport RSGISNNLS
    {
    public:
        RSGISNNLS(){};
        int nnls_c(double* a, int* mda, int* m, int* n, double* b, 
                   double* x, double* rnorm, double* w, double* zz, int* index, 
                   int* mode);
        double d_sign(double *a, double *b);
        int g1_(double *a, double *b, double *cterm, double *sterm, double *sig);
        int h12_(int *mode, int *lpivot, int *l1, int *m, double *u, int *iue, double *up, double *c__, int *ice, int *icv, int *ncv);
        double diff_(double *x, double *y);
        ~RSGISNNLS(){};
    };

}}

#endif

