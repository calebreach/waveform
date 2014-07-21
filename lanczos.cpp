#include <gsl/gsl_sf.h>
#include <math.h>
#include <assert.h>

double lanczos(double t) {
  if (t < -3) t = -3;
  if (t >  3) t =  3;

  return gsl_sf_sinc(t)*gsl_sf_sinc(t/3);
}

double lanczosIntegral(double t) {
  if (t < -3) t = -3;
  if (t >  3) t =  3;

  // Integrate[Sinc[Pi*x]*Sinc[Pi*x/3], {x, -3, t}, Assumptions -> -3 < t < 3]
  return (
      -3*cos((2*M_PI*t)/3.)
    +  3*cos((4*M_PI*t)/3.)
    -  2*M_PI*t*(
            gsl_sf_Si(2*M_PI)          
         -  2*gsl_sf_Si(4*M_PI)        
         +  gsl_sf_Si((2*M_PI*t)/3.)  
         -  2*gsl_sf_Si((4*M_PI*t)/3.)
    )
  )/((2.*M_PI*M_PI)*t);
}
