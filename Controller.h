/*
 *  Author: Sunil Lal
 *  lal.sunil[at]gmail.com
 *  University of the Ryukyus
 *
 */
#include "constants.h"

#ifndef CONTROLLER_H
#define CONTROLLER_H

extern setup settings;

template <class T, class S>   //T is simulator type (int/double); S is the vector paramter type (bool/double) 
class Controller{


	public:
	
		Controller();
		virtual ~Controller(){};
		void setBrokenState(vBool2d);
		void setInitialState(vector<vector<T> >);
		void setParameter(S);
		
		virtual void getNextState(int, vector<vector<T> > &, vector<vector<T> >) = 0;

	protected:

		vBool2d brokenState;
		vector<vector<T> > initialState;
		S parameter;

		int boolToInt(const vBool, unsigned int, unsigned int);

};


//////////////////////////////////////////////////////////////////////////////////////////////
template <class T, class S>
Controller<T,S>::Controller():brokenState(settings.NUM_LEGS, vBool(settings.NUM_MODS)),
							 initialState(settings.NUM_LEGS, vector<T>(settings.NUM_MODS)){
	
	for(int i=0; i<settings.NUM_LEGS; i++)
		for(int j=0; j<settings.NUM_MODS; j++){
			
			brokenState[i][j] = false;
			
			if(sizeof(T) > sizeof(int))
				initialState[i][j] = 0;
			else{
				if((j==2) || (j==3))
					initialState[i][j] = 8;
				else
					initialState[i][j] = 7;
				
			}
		}

}


//////////////////////////////////////////////////////////////////////////////////////////////
template <class T, class S>
void Controller<T,S>::setBrokenState(vBool2d brokenMod){

	for(int i=0; i<settings.NUM_LEGS; i++)
		for(int j=0; j<settings.NUM_MODS; j++)
			brokenState[i][j] = brokenMod[i][j];

}


//////////////////////////////////////////////////////////////////////////////////////////////
template <class T, class S>
void Controller<T,S>::setInitialState(vector<vector<T> > init){
	
	for(int i=0; i<settings.NUM_LEGS; i++)
		for(int j=0; j<settings.NUM_MODS; j++)
			initialState[i][j] = init[i][j];
}
	


//////////////////////////////////////////////////////////////////////////////////////////////
template <class T, class S>
void Controller<T,S>::setParameter(S p){

	parameter = p;
	
}


///////////////////////////////////////////////////////////////////////////////////////////////
template <class T, class S>
int Controller<T,S>::boolToInt(const vBool p, unsigned int a, unsigned int b){

	int power = b - a;
	int sum = 0;
	
	for(int i = a; i <= b; i++){
		sum+=p.at(i)*pow(2,power);
		power--;
	}
	
	return sum;

}



#endif