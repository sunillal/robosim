/*
*  Author: Sunil Lal
*  lal.sunil[at]gmail.com
*  University of the Ryukyus
*/

#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "Controller.h"

extern setup settings;

template <class T, class S>
class Simulator{


	public:
	
		Simulator(): broken(settings.NUM_LEGS, vBool(settings.NUM_MODS)),
					 state(settings.NUM_LEGS, vector<T>(settings.NUM_MODS)),
					 ball(NUM_BALLS),
					 module(settings.NUM_LEGS, vector<simObj>(settings.NUM_MODS)),
					 ballJoint(NUM_BALLS),
					 motorJoint(settings.NUM_LEGS, vector<dJointID>(settings.NUM_MODS)){};
					 
		~Simulator(){destroySimObjs();}
		void setupSim(vBool, Controller<T,S> *);
		void setBrokenMod(vBool);
		void runVisualSim(int, int, char**);
		void runSim(int);
		static void simStep (int, void *);
		void getActualAngles(vector<vector<T> > &);
		void getBodyPosition(int, int, int, double&, double&, double&);
		int getCurrStateTrans();
		void printState();

	private:

		
		unsigned long int currentStep;
		int currentStateTrans;
		vBool2d broken;
		bool visualSim;
		int numStateTrans;
		vector<vector<T> > state;
		Controller<T,S> * motionController;
				
		dsFunctions fn;
		dWorldID world; //for kinematics calculation
		dSpaceID space; //space for collision detection
		dGeomID ground;	// geometry representing the ground
		dJointGroupID contactGroup; //for joints involved in collision
		simObj disc; 
		vector<simObj> ball;
		vector<vector<simObj> > module;
		vector<dJointID> ballJoint;
		vector<vector<dJointID> > motorJoint;
		
		
		void stateTransition();
		void prepDrawStuff();
		void createSimObjs();
		void destroySimObjs();
		static void start();
		void drawSimObjects();
		static void nearCallback(void *, dGeomID, dGeomID);
		bool control();
		double phaseToAngle (int);
		int angleToPhase(double);
		
				
		
};


//////////////////////////////////////////////////////////////////////////////////////////////
template <class T, class S>
void Simulator<T,S>::setupSim(vBool brokenMod, Controller<T,S> * ctrl){

	
	dInitODE();
	//create the world for kinetic calculations
	world = dWorldCreate();
	space = dHashSpaceCreate(0);
	contactGroup = dJointGroupCreate(0);
	
	dWorldSetGravity(world,0,0,GRAVITY);// set gravitational acceleration
	dWorldSetERP(world,ERP);
	dWorldSetCFM(world, CFM);
	
	//create the ground
	ground = dCreatePlane(space,0,0,1,0);
	
	//create the simualtion objects
	createSimObjs();
	
	//stepping related counters
	currentStep = 0;
	currentStateTrans = 0;
	
	setBrokenMod(brokenMod);
	
	motionController = ctrl;
	
	visualSim = false;
		
}	

template <class T, class S>
void Simulator<T,S>::setBrokenMod(vBool brokenMod){

	int count = 0;
	//initialize set of broken modules
	for(int i=0; i<settings.NUM_LEGS; i++)
		for(int j=0; j<settings.NUM_MODS; j++){
			broken[i][j] = brokenMod.at(count);
			count++;
		}

}

//////////////////////////////////////////////////////////////////////////////////////////////
template <class T, class S>
void Simulator<T,S>::runVisualSim(int numST, int ac, char**av){
	
	
	visualSim = true;
	numStateTrans = numST;
		

	prepDrawStuff();
	dsSimulationLoop(ac,av,640,480,&fn); // window size = 640*480


}	

//////////////////////////////////////////////////////////////////////////////////////////////
template <class T, class S>
void Simulator<T,S>::runSim(int numST){
	
	while(currentStateTrans <= numST)
		simStep(0,this);

}	


//////////////////////////////////////////////////////////////////////////////////////////////	
//the function which is invoked at each step of the simulation.
//kinematic calculations are carried out automatically
//unless drawing functions are invoked nothing will be drawn
template <class T, class S>
void Simulator<T,S>::simStep (int pause, void *obj){

	Simulator<T,S> *thisSim = (Simulator<T,S> *) obj;
	
	double posX, posY, posZ;
	
	
	if( (thisSim->currentStep%settings.TRANSITION_STEP==0) /*|| thisSim->control() */){

		if(thisSim->visualSim){
			thisSim->getBodyPosition(0, 0, 0, posX, posY, posZ);
			cout<<"state: "<<thisSim->currentStateTrans<<" pos: "<<posX<<'\t'<<posY<<endl;
		}
		
		thisSim->stateTransition();
		
				
		thisSim->currentStateTrans++;
		
		
	}
	
	thisSim->control();
	
	dSpaceCollide(thisSim->space,thisSim,&nearCallback); //collision detection code must be written on top
											//its detect which pairs of geoms in a space are potentially colliding
  
	dWorldStep(thisSim->world, settings.SIM_STEPSIZE); // set the simulation step size
  
	dJointGroupEmpty(thisSim->contactGroup); //destroy all the joint contacts involved in collision
	
	if(thisSim->visualSim){
		thisSim->drawSimObjects();
		if (thisSim->currentStateTrans > thisSim->numStateTrans)
			dsStop();
	}
	
		
	
	thisSim->currentStep++;

//	thisSim->getBodyPosition(0, 0, 0, posX, posY, posZ);
//	cout<<posX<<" "<<posY<<endl;

	
}  

template <class T, class S>
void Simulator<T,S>::stateTransition(){

	
	vector<vector<T> > aaState(settings.NUM_LEGS, vector<T>(settings.NUM_MODS));
	
	
	getActualAngles(aaState);
		
	motionController->getNextState(currentStateTrans, state, aaState);
		
}

template <class T, class S>
void Simulator<T,S>::getActualAngles(vector<vector<T> > & aa){

	dReal currAngle;
	
	//get the current actual angle of the modules
	for(int i=0; i<settings.NUM_LEGS; i++){
		for(int j=0; j<settings.NUM_MODS; j++){
			currAngle = dJointGetHingeAngle(motorJoint[i][j]);
			if(sizeof(T) > sizeof(int))
				aa[i][j] = currAngle;
			else
				aa[i][j] = angleToPhase(currAngle);
		}
	}

}


template <class T, class S>
int Simulator<T,S>::getCurrStateTrans(){

	return currentStateTrans;

}

template <class T, class S>
void Simulator<T,S>::printState(){
	
	for(int i=0; i<settings.NUM_LEGS; i++){
		for(int j=0; j<settings.NUM_MODS; j++)
			cout<<state[i][j]<<" ";
		cout<<endl;
	}
}

// i = 0-> disc, 1->module
//j & k represent module position
//x,y, and z are for respective positions of the individual body
template <class T, class S>
void Simulator<T,S>::getBodyPosition(int i, int j, int k, double &x, double &y, double &z){

	const dReal *p;
	
	if( i==0)
		p = dGeomGetPosition(disc.geom);
	else
		p = dGeomGetPosition(module[j][k].geom);
	
	x = p[0];
	y = p[1];
	z = p[2];
}
	

//////////////////////////////////////////////////////////////////////////////////////////////
template <class T, class S>
void Simulator<T,S>::prepDrawStuff(){
 
  fn.version =  DS_VERSION; //version of drawstuff
  fn.object = this;
  fn.start = &start; // function to call before simulation
  fn.step = &simStep; // function which is invoked at every step of the simulation
  fn.command = NULL; // responds to user commands during simulation
  fn.stop = NULL; //function to call at the end of simulation 
  fn.path_to_textures = "../drawstuff/textures"; // path to textures like sky etc
}  


//////////////////////////////////////////////////////////////////////////////////////////////
template <class T, class S>
void Simulator<T,S>::createSimObjs(){

	dMass m;
	dReal xd0 = 0.0, yd0 = 0.0, zd0 = BALL_HEIGHT+DISC_HEIGHT/2, modRadius, anchorRadius;
	dMatrix3 R;

    //create central disc
	disc.body = dBodyCreate(world);
	dMassSetZero(&m);
	dMassSetCylinderTotal(&m,DISC_MASS,3,DISC_RADIUS,DISC_HEIGHT);
	dBodySetMass(disc.body,&m);
	dBodySetPosition(disc.body,xd0,yd0,zd0);
	
	disc.geom = dCreateCylinder(space,DISC_RADIUS,DISC_HEIGHT);
	dGeomSetBody(disc.geom, disc.body);

	//create ball bearings
	dMassSetZero(&m);
	dMassSetSphereTotal(&m,BALL_MASS,BALL_RADIUS);
	for(int i=0; i<NUM_BALLS; i++){
		ball[i].body = dBodyCreate(world);
		dBodySetMass(ball[i].body,&m);
		dBodySetPosition(ball[i].body,BALL_PLACE*cos(i*2*M_PI/NUM_BALLS), BALL_PLACE*sin(i*2*M_PI/NUM_BALLS), BALL_RADIUS);
		
		ball[i].geom = dCreateSphere(space,BALL_RADIUS);
		dGeomSetBody(ball[i].geom, ball[i].body);
		
		//create ball joints
		ballJoint[i] = dJointCreateBall(world,0);
		dJointAttach(ballJoint[i],disc.body,ball[i].body);
		dJointSetBallAnchor (ballJoint[i], BALL_PLACE*cos(i*2*M_PI/NUM_BALLS), BALL_PLACE*sin(i*2*M_PI/NUM_BALLS), BALL_RADIUS);
	}

	
	
	//create modules
	dMassSetZero(&m);
	dMassSetBoxTotal(&m, MOD_MASS, MOD_LENGTH, MOD_WIDTH, MOD_HEIGHT);
	
	for(int i=0; i<settings.NUM_LEGS; i++){
	
		for(int j=0; j<settings.NUM_MODS; j++){
			module[i][j].body = dBodyCreate(world);
			dBodySetMass(module[i][j].body,&m);
			modRadius = DISC_RADIUS + MOD_SPACE + MOD_LENGTH/2 + j*(MOD_SPACE+MOD_LENGTH);
			dBodySetPosition(module[i][j].body, modRadius*cos(i*2*M_PI/settings.NUM_LEGS), modRadius*sin(i*2*M_PI/settings.NUM_LEGS), MOD_HEIGHT/2  );
			dRFromAxisAndAngle(R,0,0,1, i*2*M_PI/settings.NUM_LEGS);
			dBodySetRotation(module[i][j].body, R);
		
			module[i][j].geom = dCreateBox(space, MOD_LENGTH, MOD_WIDTH, MOD_HEIGHT);
			dGeomSetBody(module[i][j].geom, module[i][j].body);
			
			//setup hinge joints between the modules
			motorJoint[i][j] = dJointCreateHinge(world,0);
			
			if(j==0)
				dJointAttach(motorJoint[i][j],disc.body,module[i][j].body);
			else
				dJointAttach(motorJoint[i][j],module[i][j-1].body,module[i][j].body);
			
			anchorRadius = DISC_RADIUS + MOD_SPACE + MOD_ANCHOR + j*(MOD_SPACE + MOD_LENGTH);
			dJointSetHingeAnchor(motorJoint[i][j], anchorRadius*cos(i*2*M_PI/settings.NUM_LEGS), anchorRadius*sin(i*2*M_PI/settings.NUM_LEGS), MOD_HEIGHT/2);
			
			if(j%2 == 0)
				dJointSetHingeAxis(motorJoint[i][j],cos(M_PI/2 + i*2*M_PI/settings.NUM_LEGS), sin(M_PI/2 + i*2*M_PI/settings.NUM_LEGS), 0);
			else
				dJointSetHingeAxis(motorJoint[i][j],0, 0, 1);
				
			dJointSetHingeParam(motorJoint[i][j], dParamLoStop, M_PI/-3);
			dJointSetHingeParam(motorJoint[i][j], dParamHiStop,  M_PI/3);
			dJointSetHingeParam(motorJoint[i][j], dParamFMax, MAX_TORQUE);
		}
	}

} 

//////////////////////////////////////////////////////////////////////////////////////////////
template <class T, class S>
void Simulator<T,S>::destroySimObjs(){

	dJointGroupEmpty(contactGroup);
	dJointGroupDestroy(contactGroup);
	dSpaceDestroy(space);
	dWorldDestroy(world);
	dCloseODE();
}


//////////////////////////////////////////////////////////////////////////////////////////////
template <class T, class S>
void Simulator<T,S>::start(){

  //set the camera position

	float xyz[3] = {0, -1.6, 1};
	float hpr [3] = {90, -20, 0.0};
	dsSetViewpoint(xyz,hpr);
	dsSetSphereQuality(3);
	
}


//////////////////////////////////////////////////////////////////////////////////////////////
template <class T, class S>
void Simulator<T,S>::drawSimObjects(){

	const dReal *pos, *rot;
	const dReal sides[3] = {MOD_LENGTH,MOD_WIDTH,MOD_HEIGHT};
	
	
	dsSetColor (1.0,0,0); //set the colour of the object to red

	//draw disc
	pos = dGeomGetPosition(disc.geom);
	rot = dGeomGetRotation(disc.geom);
	dsDrawCylinder(pos,rot,DISC_HEIGHT,DISC_RADIUS);
	
	//draw ball joints
	for(int i=0; i<NUM_BALLS; i++){
		pos = dGeomGetPosition(ball[i].geom);  //get the position of the ball
		rot = dGeomGetRotation(ball[i].geom); //get the attitude/orientation of the ball
		dsDrawSphere(pos,rot,BALL_RADIUS); //draw the ball
	}
		
	//draw modules
	for(int i=0; i<settings.NUM_LEGS; i++)
		for(int j=0; j<settings.NUM_MODS; j++){
			
			if (j%2 == 0)
				dsSetColor (1.0,1.0,1.0);
			else
				dsSetColor (1.0,0,0);
					
			pos = dGeomGetPosition(module[i][j].geom);  
			rot = dGeomGetRotation(module[i][j].geom); 
			dsDrawBox(pos,rot,sides);
		}

}

//////////////////////////////////////////////////////////////////////////////////////////////
template <class T, class S>
void Simulator<T,S>::nearCallback(void *data, dGeomID o1, dGeomID o2){


	Simulator *thisSim = (Simulator *) data;
	dReal mu;
	
	// exit without doing anything if the two bodies are connected by a joint
	dBodyID b1 = dGeomGetBody(o1);
	dBodyID b2 = dGeomGetBody(o2);


	if (b1 && b2 && dAreConnected(b1, b2))
		return;
	
	mu = FC_BEARING;
	
	for(int i=0; i<settings.NUM_LEGS; i++)
		for(int j=0; j<settings.NUM_MODS; j++)
			if( (b1 == thisSim->module[i][j].body) || (b2 == thisSim->module[i][j].body) ){
				if(j%2 != 0)
					mu = FC_VMOTOR;
				else
					mu = FC_HMOTOR;
			}
			
				
	dContact contact[MAX_CONTACTS];   // up to MAX_CONTACTS contacts per box-box
  
	for (int i=0; i<MAX_CONTACTS; i++) {
		contact[i].surface.mode = dContactMu2;
		contact[i].surface.mu = mu;
		contact[i].surface.mu2 = mu;
	
	}
  
	if (int numc = dCollide (o1,o2,MAX_CONTACTS,&contact[0].geom, sizeof(dContact))) {

		for (int i=0; i<numc; i++) {
			dJointID c = dJointCreateContact (thisSim->world,thisSim->contactGroup,contact+i);
			dJointAttach (c,b1,b2);
		}
	} 
	
}


//////////////////////////////////////////////////////////////////////////////////////////////
template <class T, class S>
bool Simulator<T,S>::control(){	

	dReal currAngle, error, desiredAngle;
	bool finish = true;
	
	for(int i=0; i<settings.NUM_LEGS; i++)
		for(int j=0; j<settings.NUM_MODS; j++){
			
			if(!broken[i][j]){
			
				currAngle = dJointGetHingeAngle(motorJoint[i][j]);
				if (sizeof(T) > sizeof(int))
					desiredAngle = state[i][j];
				else
					desiredAngle = phaseToAngle(state[i][j]);
			
				error = desiredAngle - currAngle;
	
				if (error >= 0){
					if (error <= ANG_ACCURACY){
						dJointSetHingeParam(motorJoint[i][j], dParamVel, 0);
					}
					else{
						dJointSetHingeParam(motorJoint[i][j], dParamVel, ANG_VEL);
						finish = false;
					}
				}
				else {
					if (error >= -ANG_ACCURACY){
						dJointSetHingeParam(motorJoint[i][j], dParamVel, 0);
					}
					else{
						dJointSetHingeParam(motorJoint[i][j], dParamVel, -ANG_VEL);
						finish = false;
					}
				}
			}
		}
		
	return finish;
}


//////////////////////////////////////////////////////////////////////////////////////////////
//converts phase into corresponding angle (radians)
template <class T, class S>
double Simulator<T,S>::phaseToAngle (int p){

	
	return (ANG_DIV*p + ANG_MIN);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//converts angle in radians to phase number
template <class T, class S>
int Simulator<T,S>::angleToPhase (double a){

	return ( ((int)(round((a - ANG_MIN)/ANG_DIV))) % NUM_STATES  );
}



#endif

