function Fest=GetForces(IMUEulerOutput,BIMUnode,NumofBodyIMUs,p,GMapping,Fest_prev,parametersM)
%check orientation condition
if all(abs(IMUEulerOutput)<=1.2)

%% Data Processing
%Convert to FEM orientation
    BROT=zeros(3,3,NumofBodyIMUs);
    BROT(:,:,1)=Euler2RotMatrix(IMUEulerOutput(1,1:3),1);
    BROT(:,:,2)=Euler2RotMatrix(IMUEulerOutput(1,4:6),1);
    BROT(:,:,3)=Euler2RotMatrix(IMUEulerOutput(1,7:9),1);
    BROT(:,:,4)=Euler2RotMatrix(IMUEulerOutput(1,10:12),5);
    BROT(:,:,5)=Euler2RotMatrix(IMUEulerOutput(1,13:15),2);
    
%     boolean=1;
%     zd_hat=zeros(NumofBodyIMUs*6,1);

    %% Body Bending Computation
    %calculate set of estimated body forces
    Error = zeros(30,1);
    for n=1:NumofBodyIMUs
        Error((n-1)*6+1:n*6,1) = ComputeDeformedVectors(BIMUnode(1,n),BIMUnode(2,n),BIMUnode(3,n),BROT(:,:,n),p);
    end
%     Error

     %deformed vectors
    %  Error = zeros(30,1);
    %  for i=0:4
    %      Error(i*6+1:(i+1)*6,1) = zd_hat{i+1,1};
    %  end
%      Error=zd_hat;
    %Use Ordinary Least Squares to estimate the set of virtual forces

    Fin=GMapping*Error

    %send to scaling
%     Fest=PostCalc(F,parametersM);

    Fest=zeros(1,7);

    if Fin(1)>parametersM(1)
        Fin(1)=Fin(1)-parametersM(1);
    elseif Fin(1)<-parametersM(1)
        Fin(1)=Fin(1)+parametersM(1);
    else
        Fin(1)=0;
    end

    if Fin(2)>parametersM(3)
        Fin(2)=Fin(2)-parametersM(3);
    elseif Fin(2)<-parametersM(3)
        Fin(2)=Fin(2)+parametersM(3);
    else
        Fin(2)=0;
    end

    if Fin(3)>parametersM(5)
        Fin(3)=Fin(3)-parametersM(5);
    elseif Fin(3)<-parametersM(5)
        Fin(3)=Fin(3)+parametersM(5);
    else
        Fin(3)=0;
    end

    if Fin(4)>parametersM(7)
        Fin(4)=Fin(4)-parametersM(7);
    elseif Fin(4)<-parametersM(7)
        Fin(4)=Fin(4)+parametersM(7);
    else
        Fin(4)=0;
    end

    if Fin(5)>parametersM(9)
        Fin(5)=Fin(5)-parametersM(9);
    elseif Fin(5)<-parametersM(9)
        Fin(5)=Fin(5)+parametersM(9);
    else
        Fin(5)=0;
    end

    if Fin(6)>parametersM(11)
        Fin(6)=Fin(6)-parametersM(11);
    elseif Fin(6)<-parametersM(11)
        Fin(6)=Fin(6)+parametersM(11);
    else
        Fin(6)=0;
    end

    if Fin(7)>parametersM(13)
        Fin(7)=Fin(7)-parametersM(13);
    elseif Fin(7)<-parametersM(14)
        Fin(7)=Fin(7)+parametersM(14);
    else
        Fin(7)=0;
    end

    Fest(1)=Fin(1)*parametersM(2);
    Fest(2)=Fin(2)*parametersM(4);
    Fest(3)=Fin(3)*parametersM(6);
    Fest(4)=Fin(4)*parametersM(8);
    Fest(5)=Fin(5)*parametersM(10);
    Fest(6)=Fin(6)*parametersM(12);
    Fest(7)=Fin(7)*parametersM(15);
    
    
else
    Fest=Fest_prev;
%     boolean=0;
end
