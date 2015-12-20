#pragma once

namespace r3dStats
{
	// confidence interval (in %)
	enum Confidence
	{
		C90,
		C95,
		C99,
		C99p9,
		ConfidenceMax
	};

	// "mean" value
	// simple average of the numbers
	template< typename Real >
	Real CalcAverage ( Real* data, int count );

	// "variance"
	// for each number: subtract the Mean and square the result (the squared difference).
	// work out the average of those squared differences.
	template< typename Real >
	Real CalcVariance ( Real * data, Real average, int count );

	// "standard deviation"
	// its a square root of variance
	template< typename Real >
	Real CalcStd ( Real variance );

	// given value, returns if it needs to be kicked out, because its outside of confidence interval
	template< typename Real >
	bool CalcIsValueOutlier( Real value, unsigned count, Real average, Real std, Confidence conf);

	// given accuarate average and variance, give confidence interval (this is where your values are)
	template< typename Real >
	void CalcConfidenceInterval( Real average, Real variance, Confidence conf, Real* minV, Real* maxV );

	// one function to run them all
	template< typename Real >
	Real CalcGoodAverage ( Real * values, int count, Confidence conf, Real * std, Real * minV, Real * maxV );
}