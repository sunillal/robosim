/*
*  Author: Sunil Lal
*  lal.sunil[at]gmail.com
*  University of the Ryukyus
*
*/
#include <ode/ode.h>
#include <drawstuff/drawstuff.h>
#include <iostream>
#include <math.h>
#include <fstream>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <string>
#include "mt19937ar.h"

using namespace std;

#ifdef dDOUBLE
#define dsDrawBox dsDrawBoxD
#define dsDrawCapsule dsDrawCapsuleD
#define dsDrawSphere dsDrawSphereD
#define dsDrawCylinder dsDrawCylinderD
#endif

#define MAX_CONTACTS 8		// maximum number of contact points per body  //was 8

#ifndef CONSTANTS
#define CONSTANTS

const dReal MOD_LENGTH = 0.09;
const dReal MOD_WIDTH = 0.07;
const dReal MOD_HEIGHT = 0.07;
const dReal MOD_MASS = 0.394;
const dReal MOD_SPACE = 0.02;
const dReal MOD_ANCHOR = 0.023;
const dReal DISC_RADIUS = 0.09;
const dReal DISC_HEIGHT = 0.04;
const dReal DISC_MASS = 0.958;
const dReal BALL_RADIUS = 0.01;   
const dReal BALL_HEIGHT = 0.03;
const dReal BALL_MASS = 0.051;      
const dReal BALL_PLACE = 0.06; 
const dReal ANG_VEL = 3.611;
const dReal MAX_TORQUE = 1.68;
const double ANG_MIN = -M_PI/3;
const double ANG_MAX = M_PI/3;
const int NUM_STATES = 16;
const double ANG_DIV = (ANG_MAX - ANG_MIN) / (NUM_STATES-1);

const double ANG_ACCURACY = 0.03; //accuracy of motor in radians   //was 0.01
const int NUM_BALLS = 5;

const dReal GRAVITY = -9.81;
const dReal ERP = 0.8;  // was 0.7
const dReal CFM = 1e-4;  // was 1e-5
const dReal FC_BEARING = 0.2;//0.2 tile //0.25 carpet //0.15 wood
const dReal FC_HMOTOR = 0.32;//0.32 tile// 0.12 carpet //0.12 wood
const dReal FC_VMOTOR = 0.43;//0.43 tile// 0.61 carpet //0.23 wood


typedef struct {
	dBodyID body;
	dGeomID geom;
} simObj;

typedef vector<int> vInt;
typedef vector<vInt> vInt2d;
typedef vector<bool> vBool;
typedef vector<vBool> vBool2d;
typedef vector<double> vDouble;
typedef vector<vDouble> vDouble2d;

enum T_CONTROL {CA1d, CA2dSing, CA2dDiff, NNsin, NNsig, FSMint, FSMdbl};
enum T_PROBLEM {minimize, maximize};
enum T_MODE {evolve, visual, nonvisual, robust1, robust2};
enum T_STRATEGY {Rand1Bin, Rand1Exp, Best1Bin};
enum T_FIT_EVAL {linear, directional, angular};

struct setup{

	int NUM_LEGS;
	int NUM_MODS;
	int NUM_BITS;
	double SIM_STEPSIZE;
	int TRANSITION_STEP;
	int NUM_STATE_TRANS;
	double TARGET_LEN;
	double TARGET_THETA;
	float ALPHA;
	double TARGETX; //calculated field, i.e not set by user
	double TARGETY; //calculated field, i.e not set by user
	T_MODE MODE;
	vector<bool> BROKEN;
	
	string RESULT_FILENAME;
	string PARAM_FILENAME;
	string IN_FILENAME;
	
	ofstream RESULT_FILE;
	ofstream PARAM_FILE;
	ifstream IN_FILE;
	
	//general GA paarmeters also used by DE
	int MAX_CHR_SIZE; //max no of genes in chromosome
	int MIN_CHR_SIZE; //min no of genes in chromosome
	int GLEN; //30 modules * 4 bits per module
	int CLEN;
	int POP_SIZE;
	int MAX_GEN;
	int SEED;
	float MR;   
	float CR;	
	T_CONTROL CONTROLLER;
	T_PROBLEM PROBLEM_TYPE;
	T_FIT_EVAL FITNESS_EVAL;
	double OFFSET;  //used in reducing a maximization problem to a minimization problem

	//CA2dDiff controller parameter
	int CLENCR;
	int CLENLR;
	
	//DE parameters
	vDouble MIN_VECTOR,MAX_VECTOR;
	T_STRATEGY DE_STRATEGY;
	float F;
	
	
};


#endif

