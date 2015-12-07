/*
 *  Author: Sunil Lal
 *  lal.sunil[at]gmail.com
 *  University of the Ryukyus
 *
 */
#include "Controller.h"

#ifndef FSMCONTROLLER_H
#define FSMCONTROLLER_H

extern setup settings;

template <class T, class S>
class FSMController: public Controller<T,S> {
	
	
	public:
	
		FSMController(){};
		~FSMController(){};

		void getNextState(int, vInt2d &, vInt2d);
		void getNextState(int, vDouble2d &, vDouble2d);
			

};



/////////////////////////////////////////////////////////////////////////////////////////////
template <class T, class S>
void FSMController<T,S>::getNextState(int sn, vInt2d & st, vInt2d ast) {

	
	int pos1, pos2, size;
	
	size = (this->parameter.size()/settings.GLEN);
	
	if((sn == 0) || sn >= (size+1)){
		for(int i=0; i<settings.NUM_LEGS; i++)
			for(int j=0; j<settings.NUM_MODS; j++)
				st[i][j] = this->initialState[i][j];
	}	
	else{
		for(int i=0; i<settings.NUM_LEGS; i++)
			for(int j=0; j<settings.NUM_MODS; j++){
				pos1 = (sn-1)*settings.GLEN + i*(settings.GLEN/settings.NUM_LEGS) + j*settings.NUM_BITS;  // start position of state information for jth module in ith leg 
				pos2 = pos1+settings.NUM_BITS-1;      // finish position " " ... "
				st[i][j] = this->boolToInt(this->parameter, pos1, pos2);
			}
	}

}

/////////////////////////////////////////////////////////////////////////////////////////////
template <class T, class S>
void FSMController<T,S>::getNextState(int sn, vDouble2d & st, vDouble2d ast) {

	
	int pos, size;
	
	
	size = (this->parameter.size()/settings.GLEN);
	
	if((sn == 0) || sn >= (size+1)){
		for(int i=0; i<settings.NUM_LEGS; i++)
			for(int j=0; j<settings.NUM_MODS; j++)
				st[i][j] = this->initialState[i][j];
	}	
	else{
		for(int i=0; i<settings.NUM_LEGS; i++)
			for(int j=0; j<settings.NUM_MODS; j++){
				pos = (sn-1)*settings.GLEN + i*(settings.GLEN/settings.NUM_LEGS) + j;  // position of state information for jth module in ith leg 
				st[i][j] = this->parameter.at(pos);
			}
	}

}	

#endif
