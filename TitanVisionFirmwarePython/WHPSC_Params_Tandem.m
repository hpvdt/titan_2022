%% get the bike Mass and geometry parameters
Rf = 0.255;
Rr = 0.255;
Mf = 2.5;
Mr = 2.5;
If = 0.0197492333611; % (Mf-0.200)*(Rf-25e-3)^2
Ir = 0.0197492333611; %
Mframe = 40-Mf-Mr; % everything except the wheels
Mrider = 150;
M = Mf+Mr+Mframe+Mrider;
M_Valkyrie = 60/2.2 + Mrider;
RefLen = 3.4;

%% Get Bike Drag Parameters
Cd_flatplate = @(Re) min(0.074*Re.^(-0.2),0.01); % turb flat plate. Anderson page 840
rho = 0.95;
mu = 1.983e-5; % dynamic viscosity of air
xpRef = 130/3.6; % reference velocity for CdA_Ref
CdA_Valkyrie = 0.014*1.7; % [m^2] 
CdA_Ref = 1.0*CdA_Valkyrie;
ReRef = xpRef*rho*RefLen/mu;
CdA_Fcn = @(Re) CdA_Ref* (Cd_flatplate(Re)/Cd_flatplate(ReRef)); % this allows the Cd to vary approx accurately with Re but be correct at a given speed
CdA_Fcn_Valkyrie = @(Re) CdA_Valkyrie* (Cd_flatplate(Re)/Cd_flatplate(ReRef)); 
Crr1 = 0.0023; %0.0023 * 0.4 + 0.0039 * 0.6; 
Crr2 = 0.00001; %0.000064 * 0.6; %0.000064 for Pro One, 0 for GP Custom
Crr = @(xp) Crr1 + xp * Crr2;
eta = 0.97;

% plot CdA
if false%ToPlot
    figure
    subplot(1,2,1)
    xp = linspace(0,70/3.6);
    plot(xp*3.6,CdA_Fcn(xp*rho*RefLen/mu))
    xlim([0 max(xp*3.6)])
    title('C_dA vs Speed')
    xlabel('Speed (km/hr)')
    ylabel('Drag Area (m^2)');
    subplot(1,2,2)
    PowerLoss = [M*9.81*Crr*xp; CdA_Fcn(xp*rho*RefLen/mu)*0.5*rho.*xp.^3];
    PowerLoss(3,:) = sum(PowerLoss,1);
    plot(xp*3.6,PowerLoss);
    axis tight
    legend('Rolling Power','Aero Power','Total Power','location','northwest');
    title('Power Loss vs Speed')
    xlabel('Speed (km/hr)')
    ylabel('Power Loss (W)')
end

%% Get some rider and simulation parameters
MaxLeanAng = 40;
BrakeAccel = 2.5;
xpTol = 1e-9;
Pthresh = 350;
StoppedTime = 2;
stepDuration = 0.005; %s

%% Regenerative Braking Parameters
RegenCap = 2000; %J
RegenPower = 700; %W
RegenEta = 0.6; %unitless
RegenBaseMass = 1.0; %kg
RegenStorageMass = 1.0/1000; %kg/J
RegenMass = RegenBaseMass + RegenStorageMass*RegenCap;

