/*
 *  GAEngine.cpp
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


#include "Simulator.h"
#include "FSMController.h"
#include "CAController.h"
#include "NNController.h"

#ifndef MOTIONEVALUATOR_H
#define MOTIONEVALUATOR_H

template <class T, class S, class C>
class MotionEvaluator{

	public :
	
		virtual ~MotionEvaluator(){};
		double evaluateMotion(S parameter);
		double evalLinearMotion(S param);
		double evalDirectionalMotion(S param);
		double evalAngularMotion(S param);
		
		void rectToPolar(double x,double y, double & r, double & t);
		
		
		
};

template <class T, class S, class C>
double MotionEvaluator<T,S,C>::evaluateMotion(S parameter){

	double fit = 0;
	
	if(settings.FITNESS_EVAL == linear)
		fit = evalLinearMotion(parameter);
	else if (settings.FITNESS_EVAL == directional)
		fit = evalDirectionalMotion(parameter);
	else if (settings.FITNESS_EVAL == angular)
		fit = evalAngularMotion(parameter);

	return fit;
	
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
template <class T, class S, class C>
double MotionEvaluator<T,S,C>::evalLinearMotion(S param){

	Controller<T, S> *ctrl = new C;
	Simulator<T, S> * sim = new Simulator<T,S>();
	
	int currST = 0,numStateTrans;
	double dispX, dispY, dispZ;
	double fitness;
	
	ctrl->setParameter(param);
	sim->setupSim(settings.BROKEN, ctrl);
	
	
	numStateTrans = (settings.NUM_STATE_TRANS<0) ? (param.size()/settings.GLEN + 2) : settings.NUM_STATE_TRANS;
	
	while(currST <= numStateTrans){
		
		sim->simStep(0,sim);
		currST = sim->getCurrStateTrans();
		
	}

	sim->getBodyPosition(0,0,0,dispX, dispY, dispZ);
	
	fitness = 1000 * sqrt(pow(dispX,2) + pow(dispY,2));
	
	delete ctrl;
	delete sim;
	
	return fitness;

}

template <class T, class S, class C>
double MotionEvaluator<T,S,C>::evalDirectionalMotion(S param){

	Controller<T, S> *ctrl = new C;
	Simulator<T, S> * sim = new Simulator<T,S>();
	
	int currST = 0,numStateTrans;
	double dispX, dispY, dispZ, sway, targetProx,l, phi, maxSway, effStates;
	double fitness;
	
	maxSway = 0;
	
	ctrl->setParameter(param);
	sim->setupSim(settings.BROKEN, ctrl);
	
	
	numStateTrans = (settings.NUM_STATE_TRANS<0) ? (param.size()/settings.GLEN + 2) : settings.NUM_STATE_TRANS;
	
	while(currST <= numStateTrans){
		
		sim->simStep(0,sim);
		currST = sim->getCurrStateTrans();
		
		sim->getBodyPosition(0,0,0,dispX, dispY, dispZ);

		rectToPolar(dispX,dispY,l,phi);
	
		sway = l*sin(phi - settings.TARGET_THETA);

		if(sway < 0)
			sway = sway * -1;

		if (sway > maxSway)
			maxSway = sway;
		
	}

	sim->getBodyPosition(0,0,0,dispX, dispY, dispZ);
	

	targetProx = sqrt( pow((settings.TARGETX - dispX), 2) + pow((settings.TARGETY - dispY), 2)  );
	
	effStates = (numStateTrans - 2 - settings.MIN_CHR_SIZE);
	
	fitness = targetProx*1000 + maxSway*1000 + settings.ALPHA*effStates;

	
	delete ctrl;
	delete sim;
	
	return fitness;

}

//////////////////////////////////////////////////////////////////////////////////////////////////////
template <class T, class S, class C>
double MotionEvaluator<T,S,C>::evalAngularMotion(S param){

	Controller<T, S> *ctrl = new C;
	Simulator<T, S> * sim = new Simulator<T,S>();
	
	int currST = 0, numStateTrans;
	double dispX, dispY, dispZ, dispX1, dispY1, dispZ1, centerDev, angDev, l, phi, effStates;
	double fitness;
	
	ctrl->setParameter(param);
	sim->setupSim(settings.BROKEN, ctrl);
	
	numStateTrans = (settings.NUM_STATE_TRANS<0) ? (param.size()/settings.GLEN + 2) : settings.NUM_STATE_TRANS;
	
	while(currST <= numStateTrans){
		
		sim->simStep(0,sim);
		currST = sim->getCurrStateTrans();
		
	}

	sim->getBodyPosition(0,0,0,dispX, dispY, dispZ); //get position of center of disc
	
	centerDev = 1000 * sqrt(pow(dispX,2) + pow(dispY,2));
	
	sim->getBodyPosition(1,0,0,dispX1, dispY1, dispZ1); // get position of center of first module
	
	dispX1 = dispX1 - dispX;
	dispY1 = dispY1 - dispY;
	
	rectToPolar(dispX1,dispY1,l,phi);
	
	angDev = settings.TARGET_THETA - phi;
	
	if (angDev < 0)
		angDev = angDev * -1;
		
	if((settings.TARGET_THETA == 0) && ((M_PI*2 - phi) < angDev) )
		angDev = M_PI*2 - phi;
	
	
	effStates = (numStateTrans - 2 - settings.MIN_CHR_SIZE);
	
	fitness = centerDev + angDev + settings.ALPHA*effStates;
	
	delete ctrl;
	delete sim;
	
	return fitness;

}


/*
//////////////////////////////////////////////////////////////////////////////////////////////////////
template <class T, class S, class C>
double MotionEvaluator<T,S,C>::evaluateMotion(S param){

	Controller<T, S> *ctrl = new C;
	Simulator<T, S> * sim = new Simulator<T,S>();
	
	int currST = 0,numStateTrans;
	double dispX, dispY, dispZ, sway, targetProx,l, phi, maxSway, effStates;
	double fitness;
	
	maxSway = 0;
	
	ctrl->setParameter(param);
	sim->setupSim(settings.BROKEN, ctrl);
	
	
	numStateTrans = (settings.NUM_STATE_TRANS<0) ? (param.size()/settings.GLEN + 2) : settings.NUM_STATE_TRANS;
	
	while(currST <= numStateTrans){
		
		sim->simStep(0,sim);
		currST = sim->getCurrStateTrans();
		
		if(settings.PROBLEM_TYPE==minimize){
			sim->getBodyPosition(0,0,0,dispX, dispY, dispZ);

			rectToPolar(dispX,dispY,l,phi);
		
			sway = l*sin(phi - settings.TARGET_THETA);
	
			if(sway < 0)
				sway = sway * -1;
	
			if (sway > maxSway)
				maxSway = sway;
		}
	}

	sim->getBodyPosition(0,0,0,dispX, dispY, dispZ);
	
	if(settings.PROBLEM_TYPE==minimize){
		targetProx = sqrt( pow((settings.TARGETX - dispX), 2) + pow((settings.TARGETY - dispY), 2)  );
		
		effStates = (numStateTrans - 2 - settings.MIN_CHR_SIZE);
		
		fitness = targetProx*1000 + maxSway*1000 + settings.ALPHA*effStates;
	}
	else
		fitness = 1000 * sqrt(pow(dispX,2) + pow(dispY,2));
	
	
	delete ctrl;
	delete sim;
	
	return fitness;

}

*/
//////////////////////////////////////////////////////////////////////////////////////////////
template <class T, class S, class C>
void MotionEvaluator<T,S,C>::rectToPolar(double x,double y, double & r, double & t){

	r = sqrt(x*x + y*y);
	
	if( (x > 0) && (y >=0) )
		t = atan(y/x);
	else if ( (x > 0) && (y < 0) )
		t = atan(y/x) + 2*M_PI;
	else if ( x < 0 )
		t = atan(y/x) + M_PI;	
	else if ( (x == 0) && (y > 0) )
		t = M_PI/2;
	else if ( (x == 0) && (y < 0) )
		t = 3*M_PI/2;
	else
		t = 0;

}

#endif
