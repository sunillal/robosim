/*
 *  Chromosome.h
 *  CA brittle star
 *  Created by sunil lal on 6/24/06.
 *  Author: Sunil Lal
 *  lal.sunil[at]gmail.com
 *  University of the Ryukyus
 *  Revised: Nov 30, 2007
 */

#include "constants.h"

#ifndef _CHROMOSOME_H
#define _CHROMOSOME_H

extern setup settings;

class Chromosome {

	public:
	
	Chromosome(){};
	void initialize(int sz);
	void setGene(int, bool);
	bool getGene(int);
	void clearGene();
	int getSize();
	bool getRandomGene();
	void mutate();
	void crossover(Chromosome, Chromosome &, Chromosome &);
	void setFitness(double);
	double getFitness();
	void getSolution(vBool &);
	void print();
	
	
	private:
	
	vBool geneArray;
	double fitness;

};

#endif