function Fest=GetForces(IMUEulerOutput,BIMUnode,NumofBodyIMUs,p,GMapping)
%check orientation condition
% if all(abs(IMUEulerOutput)<=1.2)

%% Data Processing
%Convert to FEM orientation
    BROT=zeros(3,3,NumofBodyIMUs);
    BROT(:,:,1)=Euler2RotMatrix(IMUEulerOutput(1,1:3),1);
    BROT(:,:,2)=Euler2RotMatrix(IMUEulerOutput(1,4:6),1);
    BROT(:,:,3)=Euler2RotMatrix(IMUEulerOutput(1,7:9),1);
    BROT(:,:,4)=Euler2RotMatrix(IMUEulerOutput(1,10:12),5);
    BROT(:,:,5)=Euler2RotMatrix(IMUEulerOutput(1,13:15),2);
%     boolean=1;
    %% initial setup
    zd_hat=zeros(NumofBodyIMUs*6,1);

    %% Body Bending Computation
    %calculate set of estimated body forces
    count=0;
    %constructing the sensitivity matrix
    for n=1:NumofBodyIMUs
        zd_hat(count+1:count+1+5,1)=ComputeDeformedVectors(BIMUnode(1,n),BIMUnode(2,n),BIMUnode(3,n),BROT(:,:,n),p);
        count=count+6;
    end

     %deformed vectors
%      Error=zd_hat;
    %Use Ordinary Least Squares to estimate the set of virtual forces

%      Fest=GMapping*Error;
     Fest=GMapping*zd_hat;
%      F1=F(1);
%      F2=F(2);
%      F3=F(3);
%      F4=F(4);
%      F5=F(5);
%      F6=F(6);
%      F7=F(7);
% 
%     Fest=[F(1),F(2),F(3),F(4),F(5),F(6),F(7)];
    %send to scaling
%     Fest=PostCalc(Festb);
% else
%     Fest=Fest_prev;
%     boolean=0;
end
