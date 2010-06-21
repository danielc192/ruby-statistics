/**********************************************************************

  statistics.h -

  $Authors: dcohen, dkivanac $
  created at: Tue Nov 03 09:42:52 JST 2009

  Copyright (C) 2009-2010 Didem Kivanac and Daniel Cohen
  
**********************************************************************/


#include <ruby.h>
#include "ruby/config.h"
#include <math.h>

double iter(double x, double y) {
  double z;
  /*  Two steps of Newton-Raphson correction to full accuracy. */
  /*  Without these steps, erfinv(y) would be about 3 times    */
  /*  faster to compute, but accurate to only about 6 digits.  */
  z = x - (erf(x) - y)/(2.00/sqrt(M_PI) * exp(-x*x));
  z = z - (erf(z) - y)/(2.00/sqrt(M_PI) * exp(-z*z));
  return z;
}



double erfinv(double y) {
  double y0, x, z;
  double a[4] = { 0.886226899,-1.645349621, 0.914624893,-0.140543331};
  double b[4] = {-2.118377725, 1.442710462,-0.329097515, 0.012229801};
  double c[4] = {-1.970840454,-1.624906493, 3.429567803, 1.641345311};
  double d[4] = { 3.543889200, 1.637067800, 0.00, 0.00};

  y0 = 0.700L;
  if ((y <= y0)&&(y >= -y0)) {
    z = y*y;
    x = y*(((a[3]*z+a[2])*z+a[1])*z+a[0])/
             ((((b[3]*z+b[2])*z+b[1])*z+b[0])*z+1);
    x = iter(x, y);
    x = iter(x, y);
  } else if ((y > y0) && (y < 1.00)) {  /* Near end points of range. */
    z = sqrt(-log((1-y)/2.00));
    x = (((c[3]*z+c[2])*z+c[1])*z+c[0]) / ((d[1]*z+d[0])*z+1.00);
    x = iter(x, y);
    x = iter(x, y);
  } else if ((y < -1.00*y0) && (y > -1.00)) {
    z = sqrt(-log((1+y)/2.00));
    x = -(((c[3]*z+c[2])*z+c[1])*z+c[0]) / ((d[1]*z+d[0])*z+1.00);
    x = iter(x, y);
    x = iter(x, y);
  } else if (abs(y) >= 1) {
    x = MAXFLOAT;
  }
  return x;
}