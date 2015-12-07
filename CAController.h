//Author: Sunil Lal
//lal.sunil[at]gmail.com
//University of the Ryukyus
//Revised: Nov 30, 2007

#include "Controller.h"

#ifndef CACONTROLLER_H
#define CACONTROLLER_H

extern setup settings;

template <class T, class S>
class CAController: public Controller<T,S> {
	
	
	public:
	
		CAController(){};
		~CAController(){};

		void getNextState(int, vInt2d &, vInt2d);			

};


/////////////////////////////////////////////////////////////////////////////////////////////
template <class T, class S>
void CAController<T,S>::getNextState(int sn, vInt2d & st, vInt2d ast) {

	
	int maxStates,l,c,r, offset, offsetCR, index;
	unsigned int ruleNum;
	
	maxStates = this->boolToInt(this->parameter,0,3);  //first four bits hold the maximum number of state transitions
	
	if((sn == 0) || sn >= (maxStates+1)){
		for(int i=0; i<settings.NUM_LEGS; i++)
			for(int j=0; j<settings.NUM_MODS; j++)
				st[i][j] = this->initialState[i][j];
	}	
	else if(sn==1){
		for(int i=0; i<settings.NUM_LEGS; i++)
			for(int j=0; j<settings.NUM_MODS; j++){
				index = i*settings.NUM_MODS*settings.GLEN + j*settings.GLEN + settings.GLEN;
				st[i][j] = this->boolToInt(this->parameter, index, index+settings.GLEN-1);
			}
	}
	else{
		
		offset = settings.GLEN*(settings.NUM_LEGS*settings.NUM_MODS + 1); 
	
		for(int i=0; i<settings.NUM_LEGS; i++){
			for(int j=0; j<settings.NUM_MODS; j++){
				
				if( ((settings.CONTROLLER == CA2dSing) || (settings.CONTROLLER == CA2dDiff)) && (j==0) ){
					
					if(settings.CONTROLLER == CA2dDiff)
						offsetCR = settings.CLENLR - offset;
					else
						offsetCR = 0;
					
					l = i-1;
					c = i;
					r = i+1;
				
					if(l < 0)
						l = settings.NUM_LEGS+l;
					if(r >= settings.NUM_LEGS)
						r = r - settings.NUM_LEGS;
				
					ruleNum = settings.GLEN*(ast[l][j]*pow(2,settings.GLEN*2) + ast[c][j]*pow(2,settings.GLEN) + ast[r][j]) + offset + offsetCR; 
				
					st[i][j] = this->boolToInt(this->parameter,ruleNum,ruleNum+settings.GLEN-1);
				}
				
				else{
					l = j-1;
					c = j;
					r = j+1;
				
					if(l < 0)
						l = settings.NUM_MODS+l;
					if(r >= settings.NUM_MODS)
						r = r - settings.NUM_MODS;
				
					ruleNum = settings.GLEN*(ast[i][l]*pow(2,settings.GLEN*2) + ast[i][c]*pow(2,settings.GLEN) + ast[i][r]) + offset; 
				
					st[i][j] = this->boolToInt(this->parameter,ruleNum,ruleNum+settings.GLEN-1);
				}
				
			}
		}			
					
	}

}


#endif


/*
	
		if(settings.CONTROLLER == CA1d){
			offset = settings.GLEN; //only first four bits for max number of state transtions
			
			for(int i=0; i<settings.NUM_LEGS; i++){
				for(int j=0; j<settings.NUM_MODS; j++){
					
					l = j-1;
					c = j;
					r = j+1;
					
					if(l < 0)
						l = settings.NUM_MODS+l;
					if(r >= settings.NUM_MODS)
						r = r - settings.NUM_MODS;
					
					ruleNum = settings.GLEN*(ast[i][l]*pow(2,settings.GLEN*2) + ast[i][c]*pow(2,settings.GLEN) + ast[i][r]) + offset; 
					
					st[i][c] = this->boolToInt(this->parameter,ruleNum,ruleNum+settings.GLEN-1);
					
				}
			}
		}
		else if(settings.CONTROLLER == CA2dSing){
		
		//
			offset = settings.GLEN; //only first four bits for max number of state transtions
		
			for(int i=0; i<settings.NUM_LEGS; i++){
				for(int j=0; j<settings.NUM_MODS; j++){
					
					if(j==0){
						l = i-1;
						c = i;
						r = i+1;
					
						if(l < 0)
							l = settings.NUM_LEGS+l;
						if(r >= settings.NUM_LEGS)
							r = r - settings.NUM_LEGS;
					
						ruleNum = settings.GLEN*(ast[l][j]*pow(2,settings.GLEN*2) + ast[c][j]*pow(2,settings.GLEN) + ast[r][j]) + offset; 
					
						st[i][j] = this->boolToInt(this->parameter,ruleNum,ruleNum+settings.GLEN-1);
					}
					
					else{
						l = j-1;
						c = j;
						r = j+1;
					
						if(l < 0)
							l = settings.NUM_MODS+l;
						if(r >= settings.NUM_MODS)
							r = r - settings.NUM_MODS;
					
						ruleNum = settings.GLEN*(ast[i][l]*pow(2,settings.GLEN*2) + ast[i][c]*pow(2,settings.GLEN) + ast[i][r]) + offset; 
					
						sti][j] = this->boolToInt(this->parameter,ruleNum,ruleNum+settings.GLEN-1);
					}
					
				}
			}
		
		} // end else if
		
		else {
		
			//
			offset = settings.GLEN; //only first four bits for max number of state transtions
			offsetCR = settings.CLENLR;
			
			
			for(int i=0; i<settings.NUM_LEGS; i++){
				for(int j=0; j<settings.NUM_MODS; j++){
					
					if(j==0){
						l = i-1;
						c = i;
						r = i+1;
					
						if(l < 0)
							l = settings.NUM_LEGS+l;
						if(r >= settings.NUM_LEGS)
							r = r - settings.NUM_LEGS;
					
						ruleNum = settings.GLEN*(ast[l][j]*pow(2,settings.GLEN*2) + ast[c][j]*pow(2,settings.GLEN) + ast[r][j]) + offsetCR; 
					
						st[i][j] = this->boolToInt(this->parameter,ruleNum,ruleNum+settings.GLEN-1);
					}
					
					else{
						l = j-1;
						c = j;
						r = j+1;
					
						if(l < 0)
							l = settings.NUM_MODS+l;
						if(r >= settings.NUM_MODS)
							r = r - settings.NUM_MODS;
					
						ruleNum = settings.GLEN*(ast[i][l]*pow(2,settings.GLEN*2) + ast[i][c]*pow(2,settings.GLEN) + ast[i][r]) + offset; 
					
						sti][j] = this->boolToInt(this->parameter,ruleNum,ruleNum+settings.GLEN-1);
					}					
				}
			}

			
			
			//
		
		}

*/	
