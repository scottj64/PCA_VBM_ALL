% function [BIMUnode,NumofBodyIMUs,p,GMapping,Fest]=ModelInitialization(p,t)
%% Initialization
data1=load('p.mat');
data2=load('t.mat');
p=data1.p;
t=data2.t;

%Pre-Compute K^-1
[Kinv,DegreesOfFreedom_F]=CalcKinv(p,t);

%define IMU nodes (1) and adjacent nodes (2,3)
NumofBodyIMUs=5;
BIMUnode=zeros(3,NumofBodyIMUs);
%BodyIMU1
% BIMUnode(1,1)=243;
% BIMUnode(2,1)=242;
% BIMUnode(3,1)=96;
BIMUnode(1,1)=99;
BIMUnode(2,1)=98;
BIMUnode(3,1)=111;

%BodyIMU2
% BIMUnode(1,2)=239;
% BIMUnode(2,2)=238;
% BIMUnode(3,2)=68;
% BIMUnode(1,2)=70;
% BIMUnode(2,2)=71;
% BIMUnode(3,2)=83;

%BodyIMU3
BIMUnode(1,2)=242;
BIMUnode(2,2)=232;
BIMUnode(3,2)=22;

%BodyIMU4
BIMUnode(1,3)=256;
BIMUnode(2,3)=254;
BIMUnode(3,3)=43;

%BodyIMU5
%Head1
BIMUnode(1,4)=219;
BIMUnode(2,4)=221;
BIMUnode(3,4)=218;

%BodyIMU6
%Head2
BIMUnode(1,5)=217;
BIMUnode(2,5)=216;
BIMUnode(3,5)=194;

%Force Memory Initialization
% Fest=zeros(1,7);

%parameter initialization
% F1=0;
% F2=0;
% F3=0;
% F4=0;
% F5=0;
% F6=0;
% F7=0;
count=0;
dedF=zeros(6*NumofBodyIMUs,7);
for nn=1:NumofBodyIMUs
    dedF(count+1:count+1+5,:)=ComputeJacobian(F1,F2,F3,F4,F5,F6,F7,BIMUnode(1,nn),BIMUnode(2,nn),BIMUnode(3,nn),p,Kinv,DegreesOfFreedom_F);
    count=count+6;
end
DiffErrorMap=dedF;
DiffErrorMapT=transpose(DiffErrorMap);
GMapping=((DiffErrorMapT*DiffErrorMap)\DiffErrorMapT);


parametersM = [2500, 2.50E-05, 3000, 1.40E-05, 7500, 7.00E-06, 1000, 2.00E-04, 1000, 2.00E-04, 1000, 2.00E-04, 10000, 26500, 6.00E-06];
IMUEulerOutput = [-0.01086	-0.450859	-0.119547	0.029571	-0.522158	-0.272827	0.204297	-0.38194	0.082748	0.012609	-0.158873	0.019847	-0.987821	0.152327	0.031722];
Fest_prev = [0,0,0,0,0,0,0];
Fest=GetForces(IMUEulerOutput,BIMUnode,NumofBodyIMUs,p,GMapping,Fest_prev,parametersM)