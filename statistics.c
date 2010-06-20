/**********************************************************************

  statistics.c -

  $Authors: nobu, dcohen $
  created at: Tue Nov 03 09:42:52 JST 2009

  Copyright (C) 2009-2010 Nobuyoshi Nakada and Daniel Cohen
  
  With Special Thanks to Steven Cohen for stat_median assistance

**********************************************************************/

#include <ruby.h>
#include "ruby/config.h"
//Debug only
#include <stdio.h>
#include <math.h>

struct stat_data_args {
   double sum, sum2, product;
   long num;
};


static VALUE
stat_i(VALUE elem, VALUE arg, int argc, VALUE *argv)
{
   struct stat_data_args *a = (void *)arg;
   double x = NUM2DBL(elem);
   a->sum += x;
   a->sum2 += pow(x, 2);
   a->product *= x;
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
 *     Statistics.cdf(mean, standard deviation, x)   =>   double
 *
 *  Computes the Area under a Cumulative Distribution or Normal Distribution
 *  given the <i>mean</i>, <i>standard deviation</i> and an <i>x</i> value.
 */

static VALUE
stat_cdf(VALUE obj, VALUE mu, VALUE sig, VALUE x)
{
	double numer, denom;
	numer = NUM2DBL(x) -  NUM2DBL(mu);
	denom = sqrt(2*pow(NUM2DBL(sig),2));
	return rb_float_new(0.5*(1+erf(numer/denom)));
}

/*
 *	call-seq:
 *		Statistics.median(enum)   =>   double
 *
 *	Finds the median of an <i>enum</i>
 */


static VALUE
stat_median(VALUE obj, VALUE list)
{
	double median, middle;
	int len;
	
	list = rb_ary_sort(list);
   
	len = RARRAY_LEN(list);
    middle = (double) (len) / 2;

	if ((fmod(len,2)) == 0)
	{
		median = (NUM2DBL(RARRAY_PTR(list)[(int) middle]) + NUM2DBL(RARRAY_PTR(list)[(int) (middle - 1)])) / 2;
		return rb_float_new(median);
	}
	else 
	{
	return rb_float_new(NUM2DBL(RARRAY_PTR(list)[(int) (middle - 0.5)]));
	}
}

/*
 *	call-seq:
 *		Statistics.linreg2(enum, enum)	=> double
 *
 *	Returns the slope, intercept and correlation coefficient given equal arrays of
 *  <i>X-Values</i> and <i>Y-Values</i>
 */

static VALUE
stat_linreg2(VALUE obj, VALUE xvals, VALUE yvals)
{
    struct stat_data_args xargs;
    struct stat_data_args yargs;
	double meanx, meany, sumx, sumx2, sumxy, slope, intercept, R ,xstdev, ystdev;
	meanx = meany = sumx = sumx2 = sumxy = slope = intercept = R = 0;
	int i;
	VALUE result = rb_hash_new();
	rb_iterate(rb_each, xvals, stat_i, (VALUE)&xargs);
	rb_iterate(rb_each, yvals, stat_i, (VALUE)&yargs);

	meanx = NUM2DBL(stat_mean(0,xvals));
	meany = NUM2DBL(stat_mean(0,yvals));
	sumx = xargs.sum;
	sumx2 = xargs.sum2;
	
	for(i=0;i < RARRAY_LEN(xvals);i++)
	{
		sumxy += (NUM2DBL(RARRAY_PTR(xvals)[i]) * NUM2DBL(RARRAY_PTR(yvals)[i]));
	}
	
	slope = ((meany * sumx) - sumxy) / ((meanx * sumx) - sumx2);
	intercept = (-1 * slope * meanx) + meany;
	xstdev = NUM2DBL(stat_stddev(0,xvals));
	ystdev = NUM2DBL(stat_stddev(0,yvals));
	R = (slope * xstdev) / ystdev;
	// Shouldn't need this but otherwise it doesn't work!
	printf("", R);
	
	rb_hash_aset(result, rb_str_new2("m"), rb_float_new(slope));
	rb_hash_aset(result, rb_str_new2("b"), rb_float_new(intercept));
	rb_hash_aset(result, rb_str_new2("R"), rb_float_new(R));
	
	return result;
	
}

/*
 * call-seq:
 *	Statistics.geomean(enum)   => double
 *
 *	Returns the geometric mean given an <i>enum</i>
 */

static VALUE
stat_geomean(VALUE obj, VALUE list)
{
	double exp;
	struct stat_data_args args;
	args.product = 1;
	args.num = 0;
	rb_iterate(rb_each, list, stat_i, (VALUE)&args);
	
	return rb_float_new(pow(args.product, 1 / (double) (args.num)));
}
	
	
/*
 * This module provides statistics functions for the ruby language
 */

void
Init_statistics(void)
{
   VALUE mStatistics = rb_define_module("Statistics");
   rb_define_module_function(mStatistics, "stdev", stat_stddev, 1);
   rb_define_module_function(mStatistics, "sum", stat_sum, 1);
   rb_define_module_function(mStatistics, "sum2", stat_sum2, 1);
   rb_define_module_function(mStatistics, "stddevs", stat_stddev_s, 1);
   rb_define_module_function(mStatistics, "mean", stat_mean, 1);
   rb_define_module_function(mStatistics, "median", stat_median, 1);
   rb_define_module_function(mStatistics, "cdf", stat_cdf, 3);
   rb_define_module_function(mStatistics, "linreg2", stat_linreg2, 2);
   rb_define_module_function(mStatistics, "geomean", stat_geomean, 1);
}
