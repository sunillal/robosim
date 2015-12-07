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
 
 
#include "Chromosome.h"

#ifndef GAENGINE_H
#define GAENGINE_H

extern setup settings;

class GAEngine {

	public:
			
		GAEngine(); //input chromosome size. -1 for variable length chromosome; minimization or maximization problem
		virtual ~GAEngine(){};
		virtual void run();
		virtual double evaluateFitness(vBool) = 0;
		void selection();
		void crossover();
		void mutation();	
		
		void collateResults();
		double getAvgFitness();
		Chromosome getBestChromosome();
		void print();
		void getSolutions(vBool &,int);
		void getBestSolution(vBool &);
		void setFitness(int,double);
				
	
	protected:
		
		vector<Chromosome> population; 
		Chromosome bestChromosome;
		double avgFitness;		
		T_PROBLEM problemType;
				
		void selectionMin();
		void selectionMax();

	
	

};

#endif