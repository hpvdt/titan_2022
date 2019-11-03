'''
Configuration of all the parameters used for the racesim.py script

Rf,Rr       - radii of the front and rear wheels (m)
If,Ir       - Inertia of the front and rear wheels (kg*m^2)
M           - mass of the bike/rider system (kg)
PFcn        - power as a function of x (position on the course) (W)
rho         - air density (kg/m^3)
RefLen      - length of the vehicle used for Re calcs (m)
CdA_Fcn     - drag area as  a function of Re (m^2)
Crr         - rolling resistance coeff.
eta         - drivetrain efficiency
MaxLeanAng  - maximum lean angle in turns (deg.)
TimeDurationGuess - estimate of longest time it would take to complete
              the course (be conservative) (sec.)
BrakeAccel  - acceleration to use when braking (m/s^2)
xp          - Speed
xpTol       - tolerance to use for speed for the simulation. Typically global
              tolerance = xpTol/1000 so keep this in mind (m/s)
xpStart     - start speed (or estimated start speed if periodic) (m/s)
StoppedTime - amount of time to spend when bike comes to a stop (sec.)
ToPlot      - true if you want to plot

RegenCap    - Energy capacity of regen braking system
RegenPower  - Average power returned until depletion
RegenEta    - efficiency of regeneration, output/input

Ppassive = -Slope(Sec)*xp*W                            ... slope power
           -Crr(xp)*M*sqrt(g^2+xp^4/TurnRad(Sec)^2)*xp ... rolling power
           -0.5*rho*xp^3*CdA_Fcn(Re(xp));              % aero power

'''
import math

### Bike Mass and geometry parameters
Rf = 0.255
Rr = 0.255
Mf = 2.5
Mr = 2.5
If = 0.0197492333611 #(Mf-0.200)*(Rf-25e-3)^2
Ir = 0.0197492333611
Mframe = 40-Mf-Mr #everything except the wheels
Mrider = 150
M = Mf+Mr+Mframe+Mrider
M_Valkyrie = 60/2.2 + Mrider
RefLen = 3.4

g = 9.81            # Gravitational acceleration (m/s^2)
W = g*M             # Weight (N)

### Get Bike Drag Parameters
def Cd_flatplate(re):
    return min(0.074*re**(-0.2),0.01) # turb flat plate. Anderson page 840
rho = 0.95
mu = 1.983e-5   # dynamic viscosity of air
nu = mu / rho   # Kinematic viscosity
xpRef = 130/3.6 # reference velocity for CdA_Ref
CdA_Valkyrie = 0.014*1.7 # [m^2] 
CdA_Ref = 1.0*CdA_Valkyrie
ReRef = xpRef*rho*RefLen/mu
L2nu = RefLen / nu  # Used in Reynolds

    
def Re(velocity):
    # Get Reynold's number
    return velocity * L2nu

def CdA_Fcn(Re):
    return CdA_Ref* (Cd_flatplate(Re)/Cd_flatplate(ReRef)) #this allows the Cd to vary approx accurately with Re but be correct at a given speed
    
# Roling resistance parameters/function
Crr1 = 0.0023 #0.0023 * 0.4 + 0.0039 * 0.6; 
Crr2 = 0.00001 #0.000064 * 0.6; %0.000064 for Pro One, 0 for GP Custom
def Crr(xp):
    return Crr1 + xp * Crr2

eta = 0.97 # Drive train efficiency



# Get some rider and simulation parameters
MaxLeanAng = 40
BrakeAccel = 2.5
xpTol = 1e-9
Pthresh = 350
StoppedTime = 2
stepDuration = 0.005

# Regenerative Braking Parameters
RegenCap = 2000 #J
RegenPower = 700 #W
RegenEta = 0.6 #unitless
RegenBaseMass = 1.0 #kg
RegenStorageMass = 1.0/1000 #kg/J
RegenMass = RegenBaseMass + RegenStorageMass*RegenCap

# Course info load in
import csv

checkPoints = [] # Stores the distance along the track a section ends
distances = [] # Length of each section
TurnRad = []
Slope = []

# File for course data
trackFilename = '/home/pi/Desktop/TitanVisionFirmware/src/WHPSC.csv' 

with open(trackFilename) as csvDataFile:
    csvReader = csv.reader(csvDataFile)
    cumDist = 0 #Stores cumulative distance
    for row in csvReader:
        if row[0].isdigit(): # Skip header row
            # Length of section
            distances.append(int(row[0]))
            cumDist = cumDist + int(row[0])
            checkPoints.append(cumDist)
            
            TurnRad.append(float(row[1]))

            Slope.append(float(row[4]))
    

def Section(dist):
    # Returns the index of which section they are in given distance along track
    index = -1
    for val in checkPoints:
        index = index + 1
        if dist < val:
            break
    return index
