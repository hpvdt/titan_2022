#include "racesim.h"

/*INPUT:
Rf,Rr - radii of the front and rear wheels (m)
If,Ir - Inertia of the front and rear wheels (kg*m^2)
M - mass of the bike/rider system (kg)
PFcn - power as a function of x (position on the course) (W)
rho - air density (kg/m^3)
RefLen - length of the vehicle used for Re calcs (m)
CdA_Fcn - drag area as  a function of Re (m^2)
Crr - rolling resistance coeff.
eta - drivetrain efficiency
MaxLeanAng - maximum lean angle in turns (deg.)
TimeDurationGuess - estimate of longest time it would take to complete
      the course (be conservative) (sec.)
BrakeAccel - acceleration to use when braking (m/s^2)
xpTol - tolerance to use for speed for the simulation. Typically global
      tolerance = xpTol/1000 so keep this in mind (m/s)
xpStart - start speed (or estimated start speed if periodic) (m/s)


Ported and stripped down for C (from MATLAB) by Savo Bajic 2022/07/22
Calvin Moes 2016/03/29
Based on code by Trefor Evans
*/

// Function prototypes, just incase things get thrown out of order 
// (not put in header to keep them scoped to only this file)
float CdA_Fcn(float Re);
float Cd_flatplate(float re);
float Crr(float xp);
float PFcn(float dist);
float Re(float xp);
float slopePolynomial(float distance);

// Bike Mass and geometry parameters
float Rf;
float Rr;
float Mf;
float Mr;
float If; // (Mf-0.200)*(Rf-25e-3)^2;
float Ir;
float Mframe; // everything except the wheels
float Mrider;
float M;
float MI;
float RefLen;

const float g = 9.81;	// Gravitational acceleration (m/s^2)
float W;  	// Weight (N)

// Get Bike Drag Parameters
float rho;
float mu;   // dynamic viscosity of air
float nu;   // Kinematic viscosity
float xpRef; // reference velocity for CdA_Ref
float CdA_Valkyrie; // [m^2] 
float CdA_Ref;
float ReRef;
float L2nu;  // Used in Reynolds

float Re(float xp) {
	return (xp * L2nu);
}

float Cd_flatplate(float re) {
    // turb flat plate. Anderson page 840
    float cd = 0.074 * pow(re, -0.2);

    // Return minimum between cd and a threshold
    const float cdThreshold = 0.01;
    if (cd < cdThreshold) return cd;
    else return cdThreshold;
}

float CdA_Fcn(float Re) {
    return (CdA_Ref * (Cd_flatplate(Re) / Cd_flatplate(ReRef))); 
    //this allows the Cd to vary approx accurately with Re but be correct at a given speed
}

// Roling resistance parameters/function
float Crr1;
float Crr2;
float eta; // Drive train efficiency
float Crr(float xp) {
    return (Crr1 + xp * Crr2);
}

// Get some rider and simulation parameters
float stepDuration;
float distanceEnd; // Distance to end

// Power Setup
float p_runup; // W
float p_sprint; // W
float sprint_start_mark; // mile marker from finish
float sprint_start; // m

float PFcn(float dist) {
    // Return sprint power if in sprint region
    if (dist < sprint_start) return p_runup;
    else return p_sprint;
}


bool raceSimHasBeenSetup = false; // Keeps track of if variables have been set up

float slopePolynomial(float distance) {
    distance = distance / 1000.0; // Reduce differences in orders to keep things accurate
    float slope = -4.87E-06*pow(distance,4) + 0.0000643*pow(distance,3) - 0.0000185*pow(distance,2) - 0.0015*distance - 0.00427;
    return slope;
}

// Moved all variable/constant setting here so they could be calculated and set in one place
void setupRaceSim() {

    // Bike Mass and geometry parameters
    Rf = 0.255;
    Rr = 0.255;
    Mf = 2.5;
    Mr = 2.5;
    If = 0.0197492333611; // (Mf-0.200)*(Rf-25e-3)^2;
    Ir = 0.0197492333611;
    Mframe = 40 - Mf - Mr; // everything except the wheels
    Mrider = 145;
    M = Mf + Mr + Mframe + Mrider;
    MI = M + If / (Rf * Rf) + Ir / (Rr * Rr); // total mass and inertia
    RefLen = 3.4;

    W = g*M;  	// Weight (N)

    // Get some rider and simulation parameters
    stepDuration = 0.005;

    // Drag Parameters
    rho = 0.95;
    mu = 1.983e-5;   // dynamic viscosity of air
    nu = mu / rho;   // Kinematic viscosity
    xpRef = 130 / 3.6; // reference velocity for CdA_Ref
    CdA_Valkyrie = 0.014 * 1.7; // [m^2] 
    CdA_Ref = 1.0 * CdA_Valkyrie;
    ReRef = xpRef * rho * RefLen / mu;
    L2nu = RefLen / nu;  // Used in Reynolds

    // Rolling resistance parameters
    Crr1 = 0.0018; // 0.0023 * 0.4 + 0.0039 * 0.6; 
    Crr2 = 0.000005; // 0.000064 * 0.6; // 0.000064 for Pro One, 0 for GP Custom
    eta = 0.97; // Drive train efficiency

    // Power parameters
    p_runup = 290; // W
    p_sprint = 490; // W
    sprint_start_mark = 1.25; // mile marker from finish
    sprint_start = (5*1600)-(sprint_start_mark*1600); //m

    distanceEnd = 8000;


    raceSimHasBeenSetup = true; // Record setup
}

void RaceSimV3_WHPSC_complete(float initialSpeed, bool recordSimulation) {

    // Ensure proper input and other constants
    const float stepDuration = 0.005;
    
    if (raceSimHasBeenSetup == false) setupRaceSim();
    
    // Run time-marching algorithm
    // initialize X
    float currX[] = {0, initialSpeed}; // displacement velocity
    int step = 0; //keep track of steps
    
    FILE * logFile;
	if (recordSimulation == true) {
        logFile = fopen("./testlog.csv", "w+");
        fprintf(logFile, "Time,Position (m),Speed (m/s),Charge,Ppassive (w),Paero,Prolling,Pslope\n");
        fprintf(logFile, "%.3f,%f,%f,0,0,0,0,0\n", step*stepDuration, currX[0], currX[1]);
        fclose(logFile);
    }
    
    while (currX[0] < distanceEnd) {
        float x = currX[0]; //position
        float xp = currX[1]; //speed
        
        // figure out passive power loss
        float Paero = -0.5*rho*pow(xp,3)*CdA_Fcn(Re(xp));
        float Prolling = -Crr(xp)*M*sqrt(g*g)*xp;
        float Pslope = -slopePolynomial(x)*xp*W;
        
        float Ppassive = Pslope + Prolling + Paero;

        // Compute new position
        currX[0] = currX[0] + xp * stepDuration;

        // Compute new speed
        float acceleration = ((Ppassive + PFcn(x)*eta)/xp)/MI;
        if (acceleration > 4.5) acceleration = 4.5;
        currX[1] = currX[1] + acceleration * stepDuration; // min is just to stop the singularity at xp = zero
        step++;

        if (recordSimulation == true) {
            // Open and append data
            logFile = fopen("./testlog.csv", "a"); // Append
            fprintf(logFile, "%.3f,%f,%f,0,%f,%f,%f,%f\n", step*stepDuration, currX[0], currX[1], Ppassive, Paero, Prolling, Pslope);
            fclose(logFile);
        }
    }
    
    // Output stuff (I don't think any is particularly useful to us)
    const float tEnd = step * stepDuration;
    const float xpEnd = currX[1];
    
    return;
}
