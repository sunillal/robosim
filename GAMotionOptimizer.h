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
 
 
#include "GAEngine.h"
#include "MotionEvaluator.h"


#ifndef GAMotionOptimizer_H
#define GAMotionOptimizer_H



template <class C>

class GAMotionOptimizer:  public MotionEvaluator<int, vBool, C>, public GAEngine{

	public:
			

		double evaluateFitness(vBool parameter){return (this->evaluateMotion(parameter));};

};

#endif
