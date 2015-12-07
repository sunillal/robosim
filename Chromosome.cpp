/*
 *  Chromosome.cpp
 *  CA brittle star
 *
 *  Created by sunil lal on 6/24/06.
 *  Author: Sunil Lal
 *  lal.sunil[at]gmail.com
 *  University of the Ryukyus
 *  Revised: Nov 30, 2007
 */

#include "Chromosome.h"


void Chromosome::initialize(int size){
	
	int newSize;
	
	
	if(size < 0){
		newSize = genrand_int32(settings.MIN_CHR_SIZE,settings.MAX_CHR_SIZE);
		newSize = newSize * settings.GLEN;
	}
	else
		newSize = size;
	
	for(int i=0; i<newSize; i++)
		geneArray.push_back(getRandomGene());
			
}
		
void Chromosome::setGene(int i, bool val){

	if(i == -1)
		geneArray.push_back(val);
	else
		geneArray.insert(geneArray.begin()+i, val);
}

bool Chromosome::getGene(int i){

	return geneArray.at(i);
}


int Chromosome::getSize(){

	return geneArray.size();
}

bool Chromosome::getRandomGene(){

	return (genrand_int32()%2);
}

void Chromosome::clearGene(){

	geneArray.clear();
}

void Chromosome::mutate(){

	float r;
	int newSize, diff, geneBlocks, pos;

	for(unsigned int i=0; i<geneArray.size(); i++){
		
		r = genrand_real1();
		
			if(r <= settings.MR)
				geneArray[i]=!geneArray[i];
	}
		
	
	//for variable length chromosome randomly resize the length of chromosome
	r = genrand_real1();
	if((settings.CLEN < 0) && (r <= settings.MR)){
		
		do{
			newSize = genrand_int32(settings.MIN_CHR_SIZE,settings.MAX_CHR_SIZE);
			newSize = newSize * settings.GLEN;
			diff = newSize - geneArray.size();
		}while(diff == 0);
		
		while(diff != 0){
			geneBlocks = geneArray.size()/settings.GLEN;
			pos = (genrand_int32()%geneBlocks) * settings.GLEN;

			if(diff > 0){
				for(int i=0; i<settings.GLEN; i++)
					setGene(pos+i, getRandomGene());
			}
			else{
				for(int i=0; i<settings.GLEN; i++)
					geneArray.erase( geneArray.begin() + pos );
			}
			diff = newSize - geneArray.size();
		}
	
	}
	
	
}


void Chromosome::crossover(Chromosome p, Chromosome &o1, Chromosome &o2){

	int crosspos, basepos;

	Chromosome *small, *big;
	
	if(getSize() <= p.getSize()){
		small = this;
		big = &p;
	}
	else{
		small = &p;
		big = this;
	}
	
	crosspos = genrand_int32()%(small->getSize());
	basepos = big->getSize() - small->getSize() ;
	
	for(int i = 0; i<small->getSize(); i++){
		
		if(i <= crosspos)
			o1.setGene(-1,small->getGene(i));
		else{
			o2.setGene(-1,small->getGene(i));
			o1.setGene(-1, big->getGene(basepos+i));
		}
	}
			
	for(int i = 0; i<= (basepos+crosspos) ; i++)
		
		o2.setGene(i,big->getGene(i));
		
	
}
		

double Chromosome::getFitness(){

	return fitness;
}

void Chromosome::print(){

	for(unsigned int i=0; i<geneArray.size(); i++)
		cout<<geneArray[i]<<" ";
					
	cout<<"\nfitness: "<<fitness<<endl;
}


void Chromosome::getSolution(vBool & sol){   

		
	sol = geneArray;
		
}


void Chromosome::setFitness(double f){

	fitness = f;
}
	