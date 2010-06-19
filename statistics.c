/**********************************************************************

  statistics.c -

  $Authors: nobu, dcohen $
  created at: Tue Nov 03 09:42:52 JST 2009

  Copyright (C) 2009 Nobuyoshi Nakada and Daniel Cohen

**********************************************************************/

#include <ruby.h>
#include "ruby/config.h"
#include <math.h>

struct stat_stdev_args {
   double sum, sum2;
   long num;
};



static VALUE
stdev_i(VALUE elem, VALUE arg, int argc, VALUE *argv)
{
   struct stat_stdev_args *a = (void *)arg;
   double x = NUM2DBL(elem);
   a->sum += x;
   a->sum2 += pow(x, 2);
   a->num++;
   return Qnil;
}	

/*
 *  call-seq:
 *     Statistics.stdev(enum)   =>   double
 *
 *  Computes the standard deviation of the elements in an <i>enum</i>
 *  If they are all numbers.
 */

static VALUE
stat_stddev(VALUE obj, VALUE list)
{
   long i;
   struct stat_stdev_args args;
   double mean, stddev;

   args.sum = args.sum2 = 0.0;
   args.num = 0;
   rb_iterate(rb_each, list, stdev_i, (VALUE)&args);
   mean = args.sum / args.num;
   stddev = sqrt(args.sum2 / args.num - mean*mean);
   return rb_float_new(stddev);
}

/*
 *  call-seq:
 *     Statistics.sum(enum)   =>   double
 *
 *  Computes the sum of the elements in an <i>enum</i>
 *  If they are all numbers.
 */


static VALUE
stat_sum(VALUE obj, VALUE list)
{
   struct stat_stdev_args args;
   
   args.sum = args.sum2 = 0.0;
   args.num = 0;
   rb_iterate(rb_each, list, stdev_i, (VALUE)&args);
   return rb_float_new(args.sum);
}

/*static VALUE
stat_sum(VALUE obj, VALUE ary)
{
	long i;
	double sum = 0.0;
	
	
	for (i=0; i<RARRAY_LEN(ary); i++) {
		sum += NUM2DBL(RARRAY_PTR(ary)[i]);
	}
	return rb_float_new(sum);
}*/

/*
 *  call-seq:
 *     Statistics.sum2(enum)   =>   double
 *
 *  Computes the sum of the square of each element in an <i>enum</i>
 *  If they are all numbers.
 */

static VALUE
stat_sum2(VALUE obj, VALUE list)
{
   struct stat_stdev_args args;

   args.sum = args.sum2 = 0.0;
   args.num = 0;
   rb_iterate(rb_each, list, stdev_i, (VALUE)&args);
   return rb_float_new(args.sum2);
}

/*
 *  call-seq:
 *     Statistics.stdevs(enum)   =>   double
 *
 *  Computes the n-1 or sample standard deviation of the elements in an <i>enum</i>
 *  If they are all numbers.
 */

static VALUE
stat_stddev_s(VALUE obj, VALUE list)
{
	struct stat_stdev_args args;
	double mean, stddevs;
	
	args.sum = args.sum2 = 0.0;
	args.num = 0;
	rb_iterate(rb_each, list, stdev_i, (VALUE)&args);
	mean = args.sum / args.num;
	stddevs = sqrt((args.sum2 - (pow(args.sum,2) / args.num)) / (args.num - 1)); 
	return rb_float_new(stddevs);
}

/*
 *  call-seq:
 *     Statistics.mean(enum)   =>   double
 *
 *  Computes the mean of the elements in an <i>enum</i>
 *  If they are all numbers.
 */

static VALUE
stat_mean(VALUE obj, VALUE list)
{
	struct stat_stdev_args args;
	double mean, stddevs;
	
	args.sum = args.sum2 = 0.0;
	args.num = 0;
	rb_iterate(rb_each, list, stdev_i, (VALUE)&args);
	mean = args.sum / (double) args.num;
	return rb_float_new(mean);
}

/*
 *  call-seq:
 *     Statistics.cdf(stdev, mean, x)   =>   double
 *
 *  Computes the Area under a Cumulative Distribution or Normal Distribution
 *  given the <i>standard deviation</i>, <i>mean</i> and <i>value</i> to compute 
 *  the area to the left of
 *  If they are all numbers.
 */

static VALUE
stat_cdf(VALUE obj, VALUE sig, VALUE mu, VALUE x)
{
	double numer, denom;
	numer = x - mu;
	denom = sig * sqrt(2);
	return rb_float_new(0.5+0.5*erf(numer/denom));
}


/*static VALUE
stat_median(VALUE obj, VALUE list)
{
	int i;
	for(i = 0;i<RARRAY_LEN(list);i++)
	{
		if ( i == (RARRAY_LEN(list) / 2)) {
			if((RARRAY_LEN(list) % 2) == 0) {
				return rb_float_new((NUM2DBL(RARRAY_PTR(list)[i]) + NUM2DBL(RARRAY_PTR(list)[i-1])) / 2);
			}
		}
		
		if (i == ((RARRAY_LEN(list) / 2) - 0.5)) {
			if((RARRAY_LEN(list) % 2) != 0) {
				return RARRAY_PTR(list)[i];
			}
		}
		
	}	
		
}*/

void
Init_statistics(void)
{
   VALUE mStatistics = rb_define_module("Statistics");
   rb_define_module_function(mStatistics, "stdev", stat_stddev, 1);
   rb_define_module_function(mStatistics, "sum", stat_sum, 1);
   rb_define_module_function(mStatistics, "sum2", stat_sum2, 1);
   rb_define_module_function(mStatistics, "stddevs", stat_stddev_s, 1);
   rb_define_module_function(mStatistics, "mean", stat_mean, 1);
   rb_define_module_function(mStatistics, "cdf", stat_cdf, 3);
}
