/**********************************************************************

  statistics.c -

  $Authors: nobu, dcohen $
  created at: Tue Nov 03 09:42:52 JST 2009

  Copyright (C) 2009 Nobuyoshi Nakada and Daniel Cohen

**********************************************************************/

#include <ruby.h>
#include "ruby/config.h"
#include <math.h>

struct stat_data_args {
   double sum, sum2;
   long num;
};


double dblmod(double a, double b)
{
int result = (int) ( a / b );
return a - (double) ( result ) * b;
}


static VALUE
stat_i(VALUE elem, VALUE arg, int argc, VALUE *argv)
{
   struct stat_data_args *a = (void *)arg;
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
   struct stat_data_args args;
   double mean, stddev;

   args.sum = args.sum2 = 0.0;
   args.num = 0;
   rb_iterate(rb_each, list, stat_i, (VALUE)&args);
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
   struct stat_data_args args;
   
   args.sum = args.sum2 = 0.0;
   args.num = 0;
   rb_iterate(rb_each, list, stat_i, (VALUE)&args);
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
   struct stat_data_args args;

   args.sum = args.sum2 = 0.0;
   args.num = 0;
   rb_iterate(rb_each, list, stat_i, (VALUE)&args);
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
	struct stat_data_args args;
	double mean, stddevs;
	
	args.sum = args.sum2 = 0.0;
	args.num = 0;
	rb_iterate(rb_each, list, stat_i, (VALUE)&args);
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
	struct stat_data_args args;
	double mean;
	
	args.sum = args.sum2 = 0.0;
	args.num = 0;
	rb_iterate(rb_each, list, stat_i, (VALUE)&args);
	mean = args.sum / (double) args.num;
	return rb_float_new(mean);
}

/*
 *  call-seq:
 *     Statistics.cdf(enum, x)   =>   double
 *
 *  Computes the Area under a Cumulative Distribution or Normal Distribution
 *  given the <i>mean</i>, <i>standard deviation</i> and an <i>x</i> value.
 */

static VALUE
stat_cdf(VALUE obj, VALUE mu, VALUE sig, VALUE x)
{
	double numer, denom;
	numer = rb_float_new(x) - NUM2DBL(mu);
	denom = NUM2DBL(sig) * sqrt(2);
	return rb_float_new(0.5+0.5*erf(numer/denom));
}


/*static VALUE
stat_median(VALUE obj, VALUE list)
{
	struct stat_data_args args;
	double median, middle;
	
    middle = NUM2DBL(RARRAY_LEN(list))/ 2;

	if ((dblmod(middle,2)) == 0)
	{
		
		return rb_float_new(median);
	}
	else 
	{
	return rb_float_new(NUM2DBL(RARRAY_PTR(list)[(int) (middle + 0.5)]));
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
   //rb_define_module_function(mStatistics, "median", stat_median, 1);
   rb_define_module_function(mStatistics, "cdf", stat_cdf, 3);
}
