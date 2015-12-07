/*
 *  DEEngine.h
 *  
 *
 *  Created by sunil lal on 8/21/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 *
 *  Author: Sunil Lal
 *  lal.sunil[at]gmail.com
 *  University of the Ryukyus
 *
 */
 

#include "constants.h"

#ifndef DEENGINE_H
#define DEENGINE_H

extern setup settings;

class DEEngine;

typedef void (DEEngine::*StrategyFunction)(int);

class DEEngine {

	public: 
	
		DEEngine();
		virtual ~DEEngine(){};
		virtual void run();
		virtual double evaluateFitness(vDouble,bool&) = 0;

		void collateResults();
		double getAvgFitness();
		vDouble getBestVector();
		void print();
		void getSolutions(vDouble &,int);
		void setFitness(int,double);


	protected:
	
		
		vDouble2d population; //same
		vDouble fitness;    
		vDouble bestVector;
		vDouble trialVector;
		double avgFitness;
		double trialFitness;
		double bestFitness;		

		StrategyFunction calcTrialSolution;
	
		void SelectSamples(int candidate,int *r1,int *r2=0,int *r3=0);
		void stBest1Bin(int candidate);
		void stRand1Bin(int candidate);
		void stRand1Exp(int candidate);	
				



};


#endif