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

#include "GAEngine.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////
GAEngine::GAEngine():population(settings.POP_SIZE){

	problemType = settings.PROBLEM_TYPE;

	for(int i=0; i<settings.POP_SIZE; i++)
		population[i].initialize(settings.CLEN);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void GAEngine::run(){
	
	vBool soln, bestSol;
	
	settings.PARAM_FILE.open(settings.PARAM_FILENAME.c_str(),ios::out);
	settings.RESULT_FILE.open(settings.RESULT_FILENAME.c_str(),ios::out);	
	
	if(settings.PARAM_FILE.fail() || settings.RESULT_FILE.fail()){
		cout<<"Error opening parameter or result file\n";
		exit(1);
	}	
	
	settings.RESULT_FILE<<"#GEN\tAVG_FIT\tBEST_FIT\n";
	
	for(int i=0; i<=settings.MAX_GEN; i++){
		
		for(int j=0; j<settings.POP_SIZE; j++){
			getSolutions(soln, j);
			setFitness(j, evaluateFitness(soln));
		}
		
		collateResults();
		settings.RESULT_FILE<<i<<"\t"<<getAvgFitness()<<"\t"<<getBestChromosome().getFitness()<<endl;
		
		getBestSolution(bestSol);
		
		//save the best solution
		settings.PARAM_FILE<<endl;
		for(int k=0; k<bestSol.size(); k++)
			settings.PARAM_FILE<<bestSol[k]<<" ";
		settings.PARAM_FILE<<endl;
		
		selection();
		crossover();
		mutation();
		
	}
	
	settings.RESULT_FILE.close();
	settings.PARAM_FILE.close();

}



//////////////////////////////////////////////////////////////////////////////////////////////////////
void GAEngine::selection(){

	if (problemType == minimize)
		selectionMin();
	else
		selectionMax();

}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void GAEngine::selectionMin(){

	double sum=0, r, border;
	int count,min=0;
	vector<Chromosome> newPopl(settings.POP_SIZE);
	
	//carry forward the best sol in a given generation
	for(int i=0; i<settings.POP_SIZE; i++){
		if(population[i].getFitness()<population[min].getFitness())
			min = i;
	}
	newPopl[0] = population[min];
	
	//using roulette wheel selection method pick individuals for performing crossover operation
	for(int i=0; i<settings.POP_SIZE; i++)
		sum += (-1 * population[i].getFitness() + settings.OFFSET) ;
	
	for(int i=1; i<settings.POP_SIZE; i++){

		r = sum * genrand_real1();
		count=0;
		border = (-1 * population[0].getFitness() + settings.OFFSET);

		while(border<r){
			count++;
			border += (-1*population[count].getFitness() + settings.OFFSET);
		}

		newPopl[i] = population[count];

	}

	for(int i=0; i<settings.POP_SIZE; i++)
		population[i] = newPopl[i];



}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void GAEngine::selectionMax(){

	double sum=0, r, border;
	int count,max=0;
	vector<Chromosome> newPopl(settings.POP_SIZE);
	
	//carry forward the best sol in a given generation
	for(int i=0; i<settings.POP_SIZE; i++){
		if(population[i].getFitness()>population[max].getFitness())
			max = i;
	}
	newPopl[0] = population[max];
	
	//using roulette wheel selection method pick individuals for performing crossover operation
	for(int i=0; i<settings.POP_SIZE; i++)
		sum += population[i].getFitness();
	
	for(int i=1; i<settings.POP_SIZE; i++){

		r = sum * genrand_real1();
		count=0;
		border = population[0].getFitness();

		while(border<r){
			count++;
			border += population[count].getFitness();
		}

		newPopl[i] = population[count];

	}

	for(int i=0; i<settings.POP_SIZE; i++)
		population[i] = newPopl[i];

}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void GAEngine::crossover(){

	double r;
	Chromosome offSpring1, offSpring2;

	for(int i=1; i<settings.POP_SIZE-1; i+=2){ //starts from 1 so as to preserve the best solution
		r = genrand_real1();

		if(r <= settings.CR){
			population[i].crossover(population[i+1],offSpring1,offSpring2);
			population[i]=offSpring1;
			population[i+1]=offSpring2;
			
			offSpring1.clearGene();
			offSpring2.clearGene();
			
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void GAEngine::mutation(){


	for(int i=1; i<settings.POP_SIZE; i++){  //starts from 1 so as to preserve the best solution
		population[i].mutate();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void GAEngine::collateResults(){

	double sum=0;
	int best=0;

	for(int i=0; i<settings.POP_SIZE; i++){
		sum+=population[i].getFitness();
		
		if( (problemType == minimize) && (population[i].getFitness() < population[best].getFitness()) )
			best = i;
		else if( (problemType == maximize) && (population[i].getFitness() > population[best].getFitness()) )	
			best = i;
	}

	avgFitness = sum/settings.POP_SIZE;

	bestChromosome = population[best];
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
double GAEngine::getAvgFitness(){

	return avgFitness;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
Chromosome GAEngine::getBestChromosome(){

	return bestChromosome;
}			 

//////////////////////////////////////////////////////////////////////////////////////////////////////
void GAEngine::getBestSolution(vBool &s){

	bestChromosome.getSolution(s);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void GAEngine::print(){

	cout<<"---\n";
	for(int i=0; i<settings.POP_SIZE; i++)
		population[i].print();

	cout<<"---"<<endl;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void GAEngine::getSolutions(vBool &s,int indv){

	population[indv].getSolution(s);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void GAEngine::setFitness(int indv, double fit){

	population[indv].setFitness(fit);
}			


		