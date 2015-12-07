/*
*  Author: Sunil Lal
*  lal.sunil[at]gmail.com
*  University of the Ryukyus
*/

#include "Controller.h"

#ifndef NNCONTROLLER_H
#define NNCONTROLLER_H

extern setup settings;

template <class T, class S>
class NNController: public Controller<T,S> {
	
	
	public:
	
		NNController(){};
		~NNController(){};

		void getNextState(int, vInt2d &, vInt2d);
		float actFunc(float);
		float phaseToAngle(int);
		int angleToPhase(float);
				
		
};


/////////////////////////////////////////////////////////////////////////////////////////////
template <class T, class S>
void NNController<T,S>::getNextState(int sn, vInt2d & st, vInt2d ast) {

	int maxStates, index;
	
	maxStates = this->boolToInt(this->parameter,0,3);  //first four bits hold the maximum number of state transitions

	if((sn == 0) || sn >= (maxStates+1)){
		for(int i=0; i<settings.NUM_LEGS; i++)
			for(int j=0; j<settings.NUM_MODS; j++)
				st[i][j] = this->initialState[i][j];
	}	
	else if(sn==1){
		for(int i=0; i<settings.NUM_LEGS; i++)
			for(int j=0; j<settings.NUM_MODS; j++){
				
				if(j==0){
					index = i*settings.GLEN + settings.NUM_BITS;
					st[i][j] = this->boolToInt(this->parameter,index,index + settings.NUM_BITS-1);
				}else
					st[i][j] = ast[i][j];
					
			}
	}
	else{
	
		for(int i=0; i<settings.NUM_LEGS; i++){
			for(int j=0; j<settings.NUM_MODS; j++){
				
				if(j==0)
					st[i][j] = (ast[i][j]+1)%NUM_STATES;
				else if(j==1)
					st[i][j] = angleToPhase( actFunc ( this->parameter[settings.GLEN*i+8] * phaseToAngle(st[i][0]) ) );
				else if(j==2)
					st[i][j] = angleToPhase( actFunc ( this->parameter[settings.GLEN*i+9] * phaseToAngle(st[i][0]) + this->parameter[settings.GLEN*i+11] * phaseToAngle(st[i][1]) ) );
				else if(j==3)
					st[i][j] = angleToPhase( actFunc ( this->parameter[settings.GLEN*i+10] * phaseToAngle(st[i][0]) + this->parameter[settings.GLEN*i+12] * phaseToAngle(st[i][1]) + this->parameter[settings.GLEN*i+13] * phaseToAngle(st[i][2]) ) );
			}
		}

	}

}

/////////////////////////////////////////////////////////////////////////////////////////////
template <class T, class S>
float NNController<T,S>::actFunc(float x){

	float y;
	
	if(settings.CONTROLLER == NNsin)
		y = (M_PI/3)*sin(x);
	else
		y = (2*M_PI/3) / (1 + exp(-x)) - M_PI/3;
		
	return y;

}

//////////////////////////////////////////////////////////////////////////////////////////////
//converts phase into corresponding angle (radians)
template <class T, class S>
float NNController<T,S>::phaseToAngle (int p){

	
	return (ANG_DIV*p + ANG_MIN);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//converts angle in radians to phase number
template <class T, class S>
int NNController<T,S>::angleToPhase (float a){

	return ( ((int)(round((a - ANG_MIN)/ANG_DIV))) % NUM_STATES  );
}



#endif
