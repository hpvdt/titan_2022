% INPUT:
% CourseName - string specifying name of csv file in RaceCourses
%       folder (omit '.csv')
% Periodic - true if BC's should be periodic (ie. lap course)
% Rf,Rr - radii of the front and rear wheels (m)
% If,Ir - Inertia of the front and rear wheels (kg*m^2)
% M - mass of the bike/rider system (kg)
% PFcn - power as a function of x (position on the course) (W)
% rho - air density (kg/m^3)
% RefLen - length of the vehicle used for Re calcs (m)
% CdA_Fcn - drag area as  a function of Re (m^2)
% Crr - rolling resistance coeff.
% eta - drivetrain efficiency
% MaxLeanAng - maximum lean angle in turns (deg.)
% TimeDurationGuess - estimate of longest time it would take to complete
%       the course (be conservative) (sec.)
% BrakeAccel - acceleration to use when braking (m/s^2)
% xpTol - tolerance to use for speed for the simulation. Typically global
%       tolerance = xpTol/1000 so keep this in mind (m/s)
% xpStart - start speed (or estimated start speed if periodic) (m/s)
% StoppedTime - amount of time to spend when bike comes to a stop (sec.)
% ToPlot - true if you want to plot
%
% RegenCap - Energy capacity of regen braking system
% RegenPower - Average power returned until depletion
% RegenEta - efficiency of regeneration, output/input
% 
% OUTPUT:
% tEnd - amount of time to complete the course or a lap of the course (sec)
% xpEnd - vehicle speed at the end of the course (m/s)
% 
% Calvin Moes 2016/03/29
%
% Based on code by Trefor Evans

function [tEnd xpEnd raceData] = RaceSimV3_WHPSC(CourseName,Rf,Rr,If,Ir,M,PFcn,...
    rho,RefLen,CdA_Fcn,Crr,eta,MaxLeanAng,TimeDurationGuess,BrakeAccel,...
    ~,xpStart,StoppedTime,ToPlot,RegenCap,RegenPower,RegenEta,stepDuration)
    
    %% ensure proper input and other constants
    BrakeAccel = -abs(BrakeAccel);
    g = 9.81;
    W = g*M;
    MI = M+If/Rf^2+Ir/Rr^2; % total mass and inertia
    mu = 1.983e-5; % dynamic viscosity of air
    nu = mu/rho; % kinematic viscosity
    L2nu = RefLen/nu;
    Re = @(xp) xp*L2nu;
    
    % load data
    MyPath = strrep(fileparts(mfilename('fullpath')),'\','/');
    fid = fopen([MyPath '/RaceCourses/' CourseName '.csv']);
    data = textscan(fid,'%f %f %f %f %f %*s','Delimiter',',','EmptyValue',NaN,'HeaderLines',1);
    fclose(fid);
    SecLen = data{1};
    TurnRad = data{2};
    TurnAng = data{3}*pi/180;
    xpMax = data{4}/3.6;
    Slope = data{5};
    
    % calculate any missing data
    SecLen = max([SecLen TurnRad.*TurnAng],[],2);
    xpMax(xpMax~=Inf) = min([xpMax(xpMax~=Inf) sqrt(g*TurnRad(xpMax~=Inf)/tand(90-MaxLeanAng)) inf(size(xpMax(xpMax~=Inf)))],[],2);
    Slope(isnan(Slope)) = 0;
    TurnRad(isnan(TurnRad) | TurnRad == 0) = inf;
    
    % add the stop time for stopped sections
    StoppedDist = 0.1;
    SecLen(SecLen <= 0 | xpMax <= 0) = StoppedDist; % don't allow and 0 or neg length sections
    StoppedSpeed = StoppedDist/StoppedTime; % so you traverse StoppedDist in StoppedTime
    xpMax(xpMax <= 0) = StoppedSpeed; % don't allow full stops or neg speeds
    
    % get the breaks ( in the course
    xBreaks = cumsum([0;SecLen]);
    nBreaks = length(xBreaks);
    xEnd = max(xBreaks);
    
    
    %% Run time-marching algorithm
    %initialize X
    currX=[0 xpStart 0]; %displacement velocity charge
    step=1; %keep track of steps
    raceData=zeros(TimeDurationGuess/stepDuration,8); %initialize data array [time X Ppassive Paero Prolling Pslope]
    
    raceData(1,:)=[0 currX 0 0 0 0];
    
    while currX(1)<xEnd
        x = currX(1); %position
        xp = currX(2); %speed
        charge = currX(3); %regen charge
        
        % determine what section I'm in
        Sec = find(xBreaks <= x,1,'last');
        
        % figure out passive power loss
        Paero = -0.5*rho*xp^3*CdA_Fcn(Re(xp));
        Prolling = -Crr(xp)*M*sqrt(g^2+xp^4/TurnRad(Sec)^2)*xp;
        Pslope = -Slope(Sec)*xp*W;
        
        Ppassive = -Slope(Sec)*xp*W                        ... slope power
               -Crr(xp)*M*sqrt(g^2+xp^4/TurnRad(Sec)^2)*xp ... rolling power
               -0.5*rho*xp^3*CdA_Fcn(Re(xp));            % aero power
        
        % determine if I need to brake before the next section
        if Sec ~= nBreaks-1 && xpMax(Sec+1) < xp && (xpMax(Sec+1)^2 - xp^2) / (2*BrakeAccel) >= xBreaks(Sec+1)-x %if (not last Sec) && (above next speed limit) && (not enough distance to slow down)
            Dist2NextSec = xBreaks(Sec+1)-x;
            DistReq2Brake = (xpMax(Sec+1)^2 - xp^2) / (2*BrakeAccel); % dx = (xp2^2 - xp1^2)/(2*xpp)
            if Dist2NextSec == DistReq2Brake % then I'm on a good brake trajectory
                dXdt = [xp; BrakeAccel; 0]; % continue braking but my speed is good
                if charge < RegenCap
                    dXdt(3) = -1*BrakeAccel*MI*xp*RegenEta;
                end
            elseif Dist2NextSec < DistReq2Brake % then I have to force my speed to be slower
                dXdt = [sqrt(xpMax(Sec+1)^2-2*Dist2NextSec*BrakeAccel); % this is the speed that the vehicle must be at to slow down in time (dx = (xp2^2 - xp1^2)/(2*xpp) rearranged)
                        BrakeAccel; 0];
                if charge < RegenCap
                    dXdt(3) = -1*BrakeAccel*MI*xp*RegenEta;
                end
            end
        
        % determine if I'm at or above max speed allowed
        elseif xp >= xpMax(Sec)
            dXdt = [xpMax(Sec); 0; 0];
        
            
            
        else
            % otherwise I can put in power
            % figure out passive power loss
            Ppassive = -Slope(Sec)*xp*W                        ... slope power
                    -Crr(xp)*M*sqrt(g^2+xp^4/TurnRad(Sec)^2)*xp ... rolling power
                    -0.5*rho*xp^3*CdA_Fcn(Re(xp));            % aero power
        
            % figure out acceleration
            if charge > 0
                dXdt = [xp; min( ((Ppassive + PFcn(x)*eta + RegenPower)/xp)/MI, 4.5 ); -1*RegenPower];
            else
                dXdt = [xp; min( ((Ppassive + PFcn(x)*eta)/xp)/MI, 4.5 ); 0]; % min is just to stop the singularity at xp = zero
            end
        end
        
        %compute new X
        currX=currX+dXdt'.*stepDuration;
        step=step+1;
        
        if step>size(raceData,1)
            raceData=vertcat(raceData,zeros(1000,8));
            fprintf('.');
        end
        raceData(step,:)=[step*stepDuration currX Ppassive Paero Prolling Pslope];
    end
    
    %% Output stuff
    tEnd = step*stepDuration;
    xpEnd = currX(2);
    
    raceData = raceData(1:step,:);
    
    trapStart = find(raceData(:,2)>=(max(raceData(:,2))-200),1,'first');
    xpResult = mean(raceData(trapStart:end,3));
    
    if ToPlot        
        
        figure('units','normalized','outerposition',[0 0 1 1])
        subplot(1,2,1)
        plotyy(raceData(:,1),raceData(:,3)*3.6,raceData(:,1),raceData(:,5));
        xlabel('Time (s)');
        ylabel('Speed (km/hr)');
        title('Speed and Total Power Loss vs Time')
        ylim([0 max(raceData(:,3))*3.6+5])
        legend('Speed','Total Passive Power','location','best')
        set(gca,'YTick',0:10:floor(max(raceData(:,3))*3.6/10)*10)
        text(tEnd*0.4,xpEnd*3.6*0.1,[num2str(xpResult*3.6/1.609,4) ' MPH'],'Color','red','FontSize',30);
        
        subplot(1,2,2)
        plot(raceData(:,1),raceData(:,5),raceData(:,1),raceData(:,6),raceData(:,1),raceData(:,7),raceData(:,1),raceData(:,8));
        xlabel('Time (s)');
        ylabel('Power (W)');
        title('Power Loss vs Distance')
        hold on 
        legend('Total Dissipated Power','Aero','Rolling','Slope','Location','northwest')
        %text(tEnd*0.4,min(min(raceData(:,5:8)))+10,[num2str(xpEnd*3.6/1.609,4) ' MPH'],'Color','red','FontSize',20);
    end
    
    
    
    
    
    
    
    
    
    
    
    
    