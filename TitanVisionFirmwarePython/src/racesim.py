'''
RaceSim is based off of our 'RacesimV3_WHPSC.m' file

This basically determines if we are facing more or less resistance than expected.

The purpose of this module is to find what our hypothetical performance
would be given our input power and speed in a predetermined 'base' case
and then mpare that hypothetical outcome to reality. This will tell us
if we are performing above or below expectations required of a record run.

The performance factor we use is acceleration. We store what the hypothetical
acceleration is and compare it to the actual acceleration later on.

The real acceleration is determined from the difference in speed every second.
The speed is an average from a rotating buffer of measurements.

The performance will only be updated every few calls as to avoid temporary jerks.
'''
from config import UPDATE_INTERVAL # Used to determine the period used between calls
from racesim_config import *    # Import all parameters from config file

BUFFER_TIME = 1         # The duration of the speed buffer (seconds)
NUMBER_INTERVALS = int(BUFFER_TIME / UPDATE_INTERVAL) # Number of calls between returning a new percentage value
NUMBER_INTERVALS = max(NUMBER_INTERVALS, 1) # Ensure there is always atleast one

TIME_BETWEEN_UPDATES = NUMBER_INTERVALS * UPDATE_INTERVAL # Records actual spacing
curStep = 0 # Used to keep track of calls

# Variables to carry between calls
percentage = 0
deltaSpeed = 1
previousSpeed = 1

# Need an array for the speed entries
speedList = []
for i in range(NUMBER_INTERVALS):
    speedList.append(0) # Makes a list of zeros

def getPerformancePercentage(fPower,rPower, speed, distance):
    speedList[curStep] = speed # Updates speed buffer
    curStep = curStep + 1 #Increment counter

    if curStep == NUMBER_INTERVALS:
        '''
        Update the percentage!
        
        Get current average, compare to previous using deltaSpeed.
        Get new percentage.
        Save the current average speed over the old one
        '''
        currentAverage = 0 # Used to calculate average speed in the buffer
        for x in speedList:
            currentAverage = currentAverage + x
        currentAverage = currentAverage / NUMBER_INTERVALS

        # Percentage is the acceleration achieved compared to the theoretical amount
        percentage = (currentAverage - previousSpeed) / deltaSpeed
        percentage = round(percentage * 100, 1) # Converts to %, rounds

        previousSpeed = currentAverage # Updates speed value

        '''
        Prepare for the next call

        Calculate hypothetical power losses.
        Find the current net power of the bike.
        Determine and record the acceleration.

        This assumes the conditions do not vary significantly between cycles.
        '''
        
        ### Get power of the bike
        Prider = fPower + rPower # Total input powers
        
        Pslope = -Slope(Section(distance))*speed*W  # slope power
        Prolling = - Crr(speed)*M*sqrt(g^2+speed^4/TurnRad(Section(distance))^2)*speed # rolling power
        Paero = - 0.5*rho*speed^3*CdA_Fcn(Re(speed)) # aero power

        Ppassive = Pslope + Prolling + Paero # Passive losses
        Pnet = Prider + Ppassive # Total power going into bike

        ### Calculate acceleration
        acc = Pnet / M
        deltaSpeed = acc * TIME_BETWEEN_UPDATES
        # Finds the expected change in velocity

        curStep = 0 # Reset counter
    return percentage
    
