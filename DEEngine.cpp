/*
 *  DEEngine.cpp
 *  
 *
 *  Created by sunil lal on 8/21/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
*  Author: Sunil Lal
*  lal.sunil[at]gmail.com
*  University of the Ryukyus
 
 * Reference:
    Storn, Rainer, and Kenneth Price. Differential evolution-a simple and efficient adaptive scheme for global optimization over continuous spaces. Vol. 3. Berkeley: ICSI, 1995.
    http://www1.icsi.berkeley.edu/~storn/code.html
*
*/


#include "DEEngine.h"

DEEngine::DEEngine():	population(settings.POP_SIZE, vDouble(0)), 
						fitness(settings.POP_SIZE){
						

	int newSize;
	
	for(int i=0; i<settings.POP_SIZE; i++){
	
		if(settings.CLEN < 0){
			newSize = genrand_int32(settings.MIN_CHR_SIZE, settings.MAX_CHR_SIZE);
			newSize = newSize * settings.GLEN;
		}
		else
			newSize = settings.CLEN;
		
		for(int j=0; j<newSize; j++)
			population[i].push_back(genrand_real(settings.MIN_VECTOR[j], settings.MAX_VECTOR[j]));		
	}
	
	switch (settings.DE_STRATEGY)
	{		
		case Rand1Exp:
			calcTrialSolution = &DEEngine::stRand1Exp;
			break;
		
		case Best1Bin:
			calcTrialSolution = &DEEngine::stBest1Bin;
			break;

		case Rand1Bin:
			calcTrialSolution = &DEEngine::stRand1Bin;
			break;
	}
	

}


void DEEngine::run(){

	bool reachedSoln = false;
	int best=0;
	
	settings.PARAM_FILE.open(settings.PARAM_FILENAME.c_str(),ios::out);
	settings.RESULT_FILE.open(settings.RESULT_FILENAME.c_str(),ios::out);	
	
	if(settings.PARAM_FILE.fail() || settings.RESULT_FILE.fail()){
		cout<<"Error opening parameter or result file\n";
		exit(1);
	}	
	
	settings.RESULT_FILE<<"#GEN\tAVG_FIT\tBEST_FIT\n";
	
	//initialize; evaluate intial fitness
	for(int i=0; i<settings.POP_SIZE; i++){

		fitness[i] = evaluateFitness(population[i],reachedSoln);
			
		if( (settings.PROBLEM_TYPE == minimize) && (fitness[i] < fitness[best]) )
			best = i;
		else if( (settings.PROBLEM_TYPE == maximize) && (fitness[i] > fitness[best]) )	
			best = i;	
	}
	bestVector = population[best];
	bestFitness = fitness[best];
	collateResults();
	settings.RESULT_FILE<<'0'<<"\t"<<avgFitness<<"\t"<<bestFitness<<endl;
	//save the best solution
	settings.PARAM_FILE<<endl;
	for(unsigned int k=0; k<bestVector.size(); k++)
		settings.PARAM_FILE<<bestVector[k]<<" ";
	settings.PARAM_FILE<<endl;
	
	
	//iterate through DE generations
	for(int i=1; (i<=settings.MAX_GEN) && !reachedSoln; i++){
		
		for(int j=0; j<settings.POP_SIZE; j++){
			(this->*calcTrialSolution)(j);
			trialFitness = evaluateFitness(trialVector, reachedSoln);
			
			if( (settings.PROBLEM_TYPE == minimize) && (trialFitness < fitness[j]) ){
			
				fitness[j] = trialFitness;
				population[j] = trialVector;
				
				if(trialFitness < bestFitness){
					bestFitness = trialFitness;
					bestVector = trialVector;
				}
			}
			else if( (settings.PROBLEM_TYPE == maximize) && (trialFitness > fitness[j]) ){
			
				fitness[j] = trialFitness;
				population[j] = trialVector;
				
				if(trialFitness > bestFitness){
					bestFitness = trialFitness;
					bestVector = trialVector;
				}
			}
		}
		
		collateResults();
		settings.RESULT_FILE<<i<<"\t"<<avgFitness<<"\t"<<bestFitness<<endl;
		//save the best solution
		settings.PARAM_FILE<<endl;
		for(unsigned int k=0; k<bestVector.size(); k++)
			settings.PARAM_FILE<<bestVector[k]<<" ";
		settings.PARAM_FILE<<endl;

	}

}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void DEEngine::stRand1Bin(int candidate){

	int r1, r2, r3;
	int j;
	int randPos;
	int dSmall, dTrial, dCandidate = population[candidate].size();
	vDouble trialTemp(dCandidate);

	trialVector.clear();

	SelectSamples(candidate,&r1,&r2,&r3);
	
	dSmall = population[r1].size();	
	if(dSmall > population[r2].size())
		dSmall = population[r2].size();
	if(dSmall > population[r3].size())
		dSmall = population[r3].size();
	
	randPos = genrand_int32() % dSmall;
	j = genrand_int32()%dCandidate;
	
	for(int k=0; k<dCandidate; k++){
	
		if ( ((genrand_real1() < settings.CR) || (j == randPos) ) && (j<dSmall) )
			trialTemp[j] = population[r1][j] + settings.F*(population[r2][j] - population[r3][j]);
		else
			trialTemp[j] = population[candidate][j];
		
		j = (j+1)%dCandidate;
	}
	
	if(genrand_real1() < settings.MR){
		dTrial = genrand_int32(settings.MIN_CHR_SIZE, settings.MAX_CHR_SIZE) * settings.GLEN;
		trialVector.resize(dTrial);
		
		//note size of trial temp = size of candidate
		if(dTrial < dCandidate)
			dSmall = dTrial;
		else
			dSmall = dCandidate;
	
		randPos = genrand_int32() % dSmall;
		
		j = genrand_int32()%dTrial;
		
		for(int k=0; k<dTrial; k++){
			if( ((genrand_real1() < settings.CR) || (j == randPos) ) && (j<dSmall) )
				trialVector[j] = trialTemp[j];
			else
				trialVector[j] = genrand_real(settings.MIN_VECTOR[j], settings.MAX_VECTOR[j]);
			
			j = (j+1) % dTrial;
			
		}
	}
	else 
		trialVector = trialTemp;
		
		
	//check bound of trial vector
	for(unsigned int i=0; i<trialVector.size(); i++)
		if((trialVector[i] < settings.MIN_VECTOR[i]) || (trialVector[i] > settings.MAX_VECTOR[i]) )
			trialVector[i] = genrand_real(settings.MIN_VECTOR[i], settings.MAX_VECTOR[i]);
	
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void DEEngine::stRand1Exp(int candidate){

	int r1, r2, r3;
	int j;
	int dSmall, dTrial, dCandidate = population[candidate].size();
	vDouble trialTemp(dCandidate);

	trialVector.clear();

	SelectSamples(candidate,&r1,&r2,&r3);
	
	dSmall = population[r1].size();	
	if(dSmall > population[r2].size())
		dSmall = population[r2].size();
	if(dSmall > population[r3].size())
		dSmall = population[r3].size();
	
	j = genrand_int32()%dCandidate;
	
	trialTemp = population[candidate];
	
	for(int k=0; (genrand_real1()<settings.CR) && (k<dCandidate); k++){
	
		if (j<dSmall)
			trialTemp[j] = population[r1][j] + settings.F*(population[r2][j] - population[r3][j]);
	
			
		j = (j+1)%dCandidate;
	}
	
	if(genrand_real1() < settings.MR){
		dTrial = genrand_int32(settings.MIN_CHR_SIZE, settings.MAX_CHR_SIZE) * settings.GLEN;
		trialVector.resize(dTrial);
		
		//note size of trial temp = size of candidate
		if(dTrial < dCandidate)
			dSmall = dTrial;
		else
			dSmall = dCandidate;
	
	
		for(int i=0; i<dTrial; i++)
			trialVector[i] = genrand_real(settings.MIN_VECTOR[i], settings.MAX_VECTOR[i]);
					
		j = genrand_int32()%dTrial;
		
		for(int k=0; (genrand_real1()<settings.CR) && (k<dTrial); k++){
			
			if(j<dSmall) 
				trialVector[j] = trialTemp[j];
			
			j = (j+1) % dTrial;
			
		}
	}
	else 
		trialVector = trialTemp;
		
		
	//check bound of trial vector
	for(unsigned int i=0; i<trialVector.size(); i++)
		if((trialVector[i] < settings.MIN_VECTOR[i]) || (trialVector[i] > settings.MAX_VECTOR[i]) )
			trialVector[i] = genrand_real(settings.MIN_VECTOR[i], settings.MAX_VECTOR[i]);
	
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void DEEngine::stBest1Bin(int candidate){

	int r1, r2;
	int j;
	int randPos;
	int dSmall, dTrial, dCandidate = population[candidate].size();
	vDouble trialTemp(dCandidate);

	trialVector.clear();

	SelectSamples(candidate,&r1,&r2);
	
	dSmall = population[r1].size();	
	if(dSmall > population[r2].size())
		dSmall = population[r2].size();
	if(dSmall > bestVector.size())
		dSmall = bestVector.size();
	
	randPos = genrand_int32() % dSmall;
	j = genrand_int32()%dCandidate;
	
	for(int k=0; k<dCandidate; k++){
	
		if ( ((genrand_real1() < settings.CR) || (j == randPos) ) && (j<dSmall) )
			trialTemp[j] = bestVector[j] + settings.F*(population[r1][j] - population[r2][j]);
		else
			trialTemp[j] = population[candidate][j];
		
		j = (j+1)%dCandidate;
	}
	
	if(genrand_real1() < settings.MR){
		dTrial = genrand_int32(settings.MIN_CHR_SIZE, settings.MAX_CHR_SIZE) * settings.GLEN;
		trialVector.resize(dTrial);
		
		//note size of trial temp = size of candidate
		if(dTrial < dCandidate)
			dSmall = dTrial;
		else
			dSmall = dCandidate;
	
		randPos = genrand_int32() % dSmall;
		
		j = genrand_int32()%dTrial;
		
		for(int k=0; k<dTrial; k++){
			if( ((genrand_real1() < settings.CR) || (j == randPos) ) && (j<dSmall) )
				trialVector[j] = trialTemp[j];
			else
				trialVector[j] = genrand_real(settings.MIN_VECTOR[j], settings.MAX_VECTOR[j]);
			
			j = (j+1) % dTrial;
			
		}
	}
	else 
		trialVector = trialTemp;
		
		
	//check bound of trial vector
	for(unsigned int i=0; i<trialVector.size(); i++)
		if((trialVector[i] < settings.MIN_VECTOR[i]) || (trialVector[i] > settings.MAX_VECTOR[i]) )
			trialVector[i] = genrand_real(settings.MIN_VECTOR[i], settings.MAX_VECTOR[i]);
	
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
void DEEngine::SelectSamples(int candidate,int *r1,int *r2,int *r3)
{

	if (r1)
	{
		do
		{
			*r1 = genrand_int32()%settings.POP_SIZE;
		}
		while (*r1 == candidate);
	}

	if (r2)
	{
		do
		{
			*r2 = genrand_int32()%settings.POP_SIZE;
		}
		while ((*r2 == candidate) || (*r2 == *r1));
	}

	if (r3)
	{
		do
		{
			*r3 = genrand_int32()%settings.POP_SIZE;
		}
		while ((*r3 == candidate) || (*r3 == *r2) || (*r3 == *r1));
	}

}



//////////////////////////////////////////////////////////////////////////////////////////////////////
void DEEngine::collateResults(){

	double sum=0;

	for(int i=0; i<settings.POP_SIZE; i++)
		sum+=fitness[i];
		
	avgFitness = sum/settings.POP_SIZE;

}

//////////////////////////////////////////////////////////////////////////////////////////////////////
double DEEngine::getAvgFitness(){

	return avgFitness;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
vDouble DEEngine::getBestVector(){

	return bestVector;
}			 

//////////////////////////////////////////////////////////////////////////////////////////////////////
void DEEngine::print(){

	cout<<"---\n";
	for(int i=0; i<settings.POP_SIZE; i++){
		for(int j=0; j<population[i].size(); j++)
			cout<<population[i][j]<<" ";
		
		cout<<endl;
	}

	cout<<"---"<<endl;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void DEEngine::getSolutions(vDouble &s,int indv){

	s = population[indv];
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void DEEngine::setFitness(int indv, double fit){

	fitness[indv] = fit;
}			

