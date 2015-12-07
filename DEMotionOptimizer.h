/*
 *  GAEngine.h
 *  CA brittle star
 *
 *  Created by sunil lal on 6/24/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 *
 *  Author: Sunil Lal
 *  lal.sunil[at]gmail.com
 *  University of the Ryukyus
 *
 */
 

#include "DEEngine.h"
#include "MotionEvaluator.h"

#ifndef DEMotionOptimizer_H
#define DEMotionOptimizer_H




template <class C>

class DEMotionOptimizer:  public MotionEvaluator<double, vDouble, C>, public DEEngine{

	public:
			


		double evaluateFitness(vDouble parameter, bool &reachSoln){return (this->evaluateMotion(parameter));};

};

#endif
