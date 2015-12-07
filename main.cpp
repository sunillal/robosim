/* Author: Sunil Lal (Nov 30, 2007) Revised: Oct 2008
 *  lal.sunil[at]gmail.com
 *  University of the Ryukyus
 *  usage: $target sfile
 *
*/

#include "GAMotionOptimizer.h"
#include "DEMotionOptimizer.h"

#include <sys/time.h>

setup settings;  //struct which contain all the essential tunable parameter settings


void setup(char *);

template <class T, class S, class C >
void visualSim();



//////////////////////////////////////////////////////////////////////////////////////////////
int main (int argc, char **argv){

	struct timeval t1, t2;
	
	
	gettimeofday(&t1,NULL);
		
	setup(argv[1]);		
	
	if(settings.SEED == -1)
		init_genrand(time(0));
	else
		init_genrand(settings.SEED);
		
		
	if(settings.MODE==visual){
		if(settings.CONTROLLER==FSMdbl)
			visualSim<double,vDouble,FSMController<double,vDouble> >();
		else
			visualSim<int,vBool, FSMController<int,vBool> >();
	}
	else if(settings.MODE == evolve){
		if(settings.CONTROLLER==FSMdbl){
			DEMotionOptimizer<FSMController<double,vDouble> > de;
			de.run();
		}
		else if (settings.CONTROLLER==FSMint){
			GAMotionOptimizer<FSMController<int,vBool> > ga;
			ga.run();
		}
		else if ((settings.CONTROLLER==CA1d) || (settings.CONTROLLER==CA2dSing) ){
			GAMotionOptimizer<CAController<int,vBool> > ca;
			ca.run();
		}
		else if ((settings.CONTROLLER==NNsin) || (settings.CONTROLLER==NNsig) ){
			GAMotionOptimizer<NNController<int,vBool> > nn;
			nn.run();
		}
		
	}	
	
	gettimeofday(&t2,NULL);
	
	cout<<"\nstart:\t"<<t1.tv_sec<<'\t'<<t1.tv_usec<<"\nstop:\t"<<t2.tv_sec<<'\t'<<t2.tv_usec<<endl;
	
	return 0;
	
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <class T, class S, class C>
void visualSim(){

	bool b;
	int numStates;
	S param;
	Controller<T, S> *ctrl = new C;
	Simulator<T, S> * sim = new Simulator<T, S>();


	settings.IN_FILE.open(settings.IN_FILENAME.c_str(),ios::in);
		
	if(settings.IN_FILE.fail()){
		cout<<"Error opening input file";
		exit(1);
	}
	
	while(!settings.IN_FILE.fail() && !settings.IN_FILE.eof()){
		settings.IN_FILE>>b;
		param.push_back(b);
	}
	
	settings.IN_FILE.close();
	
		
	ctrl->setParameter(param);
	
	sim->setupSim(settings.BROKEN, ctrl);
	numStates = (settings.NUM_STATE_TRANS<0) ? (param.size()/settings.GLEN + 2) : settings.NUM_STATE_TRANS;
	sim->runVisualSim( numStates,0,NULL);
	
	delete ctrl;
	delete sim;
	


}


//////////////////////////////////////////////////////////////////////////////////////////////
void setup(char *filename){

	ifstream sFile;
	string s;
	char c;
	int len;
	
	
	sFile.open(filename,ios::in);
	if(sFile.fail()){
		cout<<"Error opening settings file\n";
		exit(1);
	}
	
	sFile>>s;
	while(s != "END" && !sFile.eof() ){
	
		if (s.compare("NUM_LEGS")==0)
			sFile>>settings.NUM_LEGS;
		else if	(s.compare("NUM_MODS")==0)
			sFile>>settings.NUM_MODS;
		else if	(s.compare("NUM_BITS")==0)
			sFile>>settings.NUM_BITS;
		else if	(s.compare("SIM_STEPSIZE")==0)
			sFile>>settings.SIM_STEPSIZE;	
		else if	(s.compare("TRANSITION_STEP")==0)
			sFile>>settings.TRANSITION_STEP;	
		else if	(s.compare("NUM_STATE_TRANS")==0)
			sFile>>settings.NUM_STATE_TRANS;
		else if	(s.compare("TARGET_LEN")==0)
			sFile>>settings.TARGET_LEN;		
		else if	(s.compare("TARGET_THETA")==0)
			sFile>>settings.TARGET_THETA;	
		else if	(s.compare("ALPHA")==0)
			sFile>>settings.ALPHA;
		else if	(s.compare("MODE")==0){
			sFile>>s;
			if (s.compare("visual")==0)
				settings.MODE = visual;
			else if (s.compare("nonvisual")==0)
				settings.MODE = nonvisual;
			else if (s.compare("robust1")==0)
				settings.MODE = robust1;
			else if (s.compare("robust2")==0)
				settings.MODE = robust2;
			else if (s.compare("evolve")==0)
				settings.MODE = evolve;
			else
				cout<<"Unkown mode\n";
		}
		else if (s.compare("BROKEN")==0){
			sFile>>c;
			while(c != ';'){
				if(c=='0')
					settings.BROKEN.push_back(false);
				else
					settings.BROKEN.push_back(true);
				sFile>>c;
			}
		}	
		else if	(s.compare("RESULT_FILENAME")==0)
			sFile>>settings.RESULT_FILENAME;
		else if	(s.compare("PARAM_FILENAME")==0)
			sFile>>settings.PARAM_FILENAME;
		else if	(s.compare("IN_FILENAME")==0)
			sFile>>settings.IN_FILENAME;
		else if	(s.compare("MAX_CHR_SIZE")==0)
			sFile>>settings.MAX_CHR_SIZE;
		else if	(s.compare("MIN_CHR_SIZE")==0)
			sFile>>settings.MIN_CHR_SIZE;	
		else if	(s.compare("GLEN")==0)
			sFile>>settings.GLEN;		
		else if	(s.compare("CLEN")==0)
			sFile>>settings.CLEN;			
		else if	(s.compare("POP_SIZE")==0)
			sFile>>settings.POP_SIZE;			
		else if	(s.compare("MAX_GEN")==0)
			sFile>>settings.MAX_GEN;	
		else if	(s.compare("SEED")==0)
			sFile>>settings.SEED;
		else if	(s.compare("MR")==0)
			sFile>>settings.MR;					
		else if	(s.compare("CR")==0)
			sFile>>settings.CR;	
		else if	(s.compare("CONTROLLER")==0){
			sFile>>s;
			if(s.compare("CA1d")==0)
				settings.CONTROLLER = CA1d;
			else if (s.compare("CA2dSing")==0)
				settings.CONTROLLER = CA2dSing;
			else if (s.compare("CA2dDiff")==0)
				settings.CONTROLLER = CA2dDiff;
			else if (s.compare("NNSin")==0)
				settings.CONTROLLER = NNsin;
			else if (s.compare("NNSig")==0)
				settings.CONTROLLER = NNsig;
			else if (s.compare("FSMdbl")==0)
				settings.CONTROLLER = FSMdbl;
			else if (s.compare("FSMint")==0)
				settings.CONTROLLER = FSMint;
			else
				cout<<"Unknow controller name\n";
		}
		else if	(s.compare("PROBLEM_TYPE")==0){
			sFile>>s;
			if(s.compare("maximize")==0)
				settings.PROBLEM_TYPE = maximize;
			else if(s.compare("minimize")==0)
				settings.PROBLEM_TYPE = minimize;
			else
				cout<<"Unknown poblem type\n";
		}
		else if	(s.compare("FITNESS_EVAL")==0){
			sFile>>s;
			if(s.compare("linear")==0)
				settings.FITNESS_EVAL = linear;
			else if(s.compare("directional")==0)
				settings.FITNESS_EVAL = directional;
			else if(s.compare("angular")==0)
				settings.FITNESS_EVAL = angular;
			else
				cout<<"Unknow fitness evaluator\n";	
		}
		else if	(s.compare("OFFSET")==0)
			sFile>>settings.OFFSET;								
		else if	(s.compare("CLENCR")==0)
			sFile>>settings.CLENCR;															
		else if	(s.compare("CLENLR")==0)
			sFile>>settings.CLENLR;																													
		else if	(s.compare("DE_STRATEGY")==0){
			sFile>>s;
			if(s.compare("Rand1Bin")==0)
				settings.DE_STRATEGY = Rand1Bin;
			else if (s.compare("Rand1Exp")==0)
				settings.DE_STRATEGY = Rand1Exp;
			else if (s.compare("Best1Bin")==0)
				settings.DE_STRATEGY = Best1Bin;
			else
				cout<<"Unknown DE strategy\n";
		}
		else if	(s.compare("F")==0)
			sFile>>settings.F;
			
		sFile.ignore(300,'\n');
		sFile>>s;
	}
	
	settings.TARGETX = settings.TARGET_LEN*cos(settings.TARGET_THETA);
	settings.TARGETY = settings.TARGET_LEN*sin(settings.TARGET_THETA);
	
	settings.NUM_BITS = 4; // corresponding to NUM_STATES of 16
	
	if(settings.CLEN < 0)
		len = settings.MAX_CHR_SIZE*settings.GLEN;
	else
		len = settings.CLEN;
		
	for(int i=0; i<len; i++){
		settings.MIN_VECTOR.push_back(ANG_MIN);
		settings.MAX_VECTOR.push_back(ANG_MAX);
	}
}

