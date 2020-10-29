% % clc
% % clear
% 
% load('p.mat');
% load('t.mat');
% [BIMUnode,NumofBodyIMUs,p,GMapping,Fest]=ModelInitialization(p,t);
% 
% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
% % Set parameters===========================================================
% E=4e3;
% nu=0.33;
% % List of p with a Dirichlet boundary condition (prescribed displacement)
% NodesDirichlet=find(p(:,3)==0);
% DegreesOfFreedomDirichlet=reshape([3*NodesDirichlet-2;3*NodesDirichlet-1; ...
%     3*NodesDirichlet],1,3*numel(NodesDirichlet));
% % =========================================================================
% TotalNumberOfNodes=size(p,1);
% NumberOfElements=size(t,1);
% % Linear elastic isotropic material========================================
% D=E/((1+nu)*(1-2*nu))*[1-nu nu      nu      0 0 0
%                        nu   1-nu    nu      0 0 0
%                        nu   nu      1-nu    0 0 0
%                         0 0 0 (1-2*nu)/2    0           0
%                         0 0 0 0             (1-2*nu)/2  0
%                         0 0 0 0             0           (1-2*nu)/2];
% % Initialisation of K, M and F=============================================
% K=zeros(3*TotalNumberOfNodes,3*TotalNumberOfNodes);
% F=zeros(3*TotalNumberOfNodes,1);
% %==========================================================================
% for e=1:NumberOfElements
%     NodesOfElement=t(e,:); % row containing nodenumbers of element e
%     DegreesOfFreedom=reshape([3*NodesOfElement-2; ...
%         3*NodesOfElement-1;3*NodesOfElement],1,3*numel(NodesOfElement));
%     X=[ones(1,4);p(NodesOfElement,:)'];
%     VolumeOfElement=abs(det(X))/6; % volume of element e
%     Coeffs=inv(X);
%     diffPhi=Coeffs(:,2:end);
% %     B{e}=[];
%     B=[];
%     for i=1:4
%         Bsmall=[
%                 diffPhi(i,1)    0               0;
%                 0               diffPhi(i,2)    0;
%                 0               0               diffPhi(i,3);
%                 diffPhi(i,2)    diffPhi(i,1)    0;
%                 0               diffPhi(i,3)    diffPhi(i,2);
%                 diffPhi(i,3)    0               diffPhi(i,1)];
% %         B{e}=[B{e},Bsmall];
%         B=[B,Bsmall];
%     end
%     % Computation of 12*12 Ke
% %     Ke=B{e}'*D*B{e}*VolumeOfElement;
%     Ke=B'*D*B*VolumeOfElement;
%     K(DegreesOfFreedom,DegreesOfFreedom)=K(DegreesOfFreedom,DegreesOfFreedom)+Ke; 
% end
% %%%%%%%%%
% %==========================================================================
% % Apply Forces:  *********************************************
% NodesOfElement_F=[240 249 237 251 234 258 205 200 170 167 178 189 224 229 65 67 95 93]; % row containing nodenumbers of element e
%     DegreesOfFreedom_F=reshape([3*NodesOfElement_F-2; ...
%         3*NodesOfElement_F-1;3*NodesOfElement_F], ...
%         1,3*numel(NodesOfElement_F)); 
% % body bending:
% 
% K(DegreesOfFreedomDirichlet,:)=0; % put zeros in boundary rows of K and F
% K(DegreesOfFreedomDirichlet,DegreesOfFreedomDirichlet)= ...
%     eye(numel(DegreesOfFreedomDirichlet));

% save('preComputedValues.mat','BIMUnode','DegreesOfFreedom_F','GMapping','K','NumberOfElements','NumOfBodyIMUs','p','t')
load('preComputedValues.mat')

%% animation
close all
tic


% load('data061217.mat');
% data = IMUDATA;
load('PCA_data.mat')
scaler = -1:.1:1;

minXview = -25;
minYview = -25;
minZview = 0;
maxXview = 55;
maxYview = 65;
maxZview = 90;


close all
f1 = figure('Units','inches','Position',[0 0 5 6],'PaperPositionMode','auto');
view(131,7)
xlim([minXview maxXview])
ylim([minYview maxYview])
zlim([minZview maxZview])
axis equal
axis off
hold on

f2 = figure('Units','inches','Position',[0 0 5 6],'PaperPositionMode','auto');
view(131,7)
xlim([minXview maxXview])
ylim([minYview maxYview])
zlim([minZview maxZview])
axis equal
axis off
hold on

data = [];

for PCAnum = 1:7
    PCAvect = transpose(PCA_data(PCAnum,1:end-1));
    data = [data;transpose([PCAvect*(scaler/max(abs(PCAvect))*pi/5)])];%,PCAvect*-scaler]);
end

[m,n]=size(data);
mov(m) = struct('cdata',[],'colormap',[]);

% RotAngles(1:m,1:n)=data(1:m,1:n);

load('mainViewNodesShowFaceTable.mat')
Faces=[1 1 2 3;2 2 3 1;3 4 4 4;1 1 2 3];

minX = 0;
maxX = 0;
minY = 0;
maxY = 0;
minZ = 0;
maxZ = 0;

parfor tstep=1:m
%     OutFo(tstep,:)=GetForces(RotAngles(tstep,:),BIMUnode,NumofBodyIMUs,p,GMapping);
    OutFo(tstep,:)=GetForces(data(tstep,:),BIMUnode,NumofBodyIMUs,p,GMapping);

    Force = OutFo(tstep,:);
    F1 = Force(1);
    F2 = Force(2);
    F3 = Force(3);
    F4 = Force(4);
    F5 = Force(5);
    F6 = Force(6);
    F7 = Force(7);

    F = zeros(849,1);

    F(DegreesOfFreedom_F(1,1))=F1/4;
    F(DegreesOfFreedom_F(1,2))=F2/4;
    F(DegreesOfFreedom_F(1,4))=F1/4;
    F(DegreesOfFreedom_F(1,5))=F2/4;
    F(DegreesOfFreedom_F(1,7))=F1/4;
    F(DegreesOfFreedom_F(1,8))=F2/4;
    F(DegreesOfFreedom_F(1,10))=F1/4;
    F(DegreesOfFreedom_F(1,11))=F2/4;
    % body moment: 
    F(DegreesOfFreedom_F(1,13))=F3;
    F(DegreesOfFreedom_F(1,16))=-F3;
    %head forward:
    F(DegreesOfFreedom_F(1,19))=F5/4;
    F(DegreesOfFreedom_F(1,22))=F5/4;
    F(DegreesOfFreedom_F(1,25))=-F5/4;
    F(DegreesOfFreedom_F(1,28))=-F5/4;
    %head sideway:
    F(DegreesOfFreedom_F(1,33))=F4;
    F(DegreesOfFreedom_F(1,36))=-F4;
    %head twist:
    F(DegreesOfFreedom_F(1,31))=F6/2;
    F(DegreesOfFreedom_F(1,34))=-F6/2;
    %scrapular motion:
    F(DegreesOfFreedom_F(1,37))=F7;
    F(DegreesOfFreedom_F(1,40))=F7;
    if F7>0
        F(DegreesOfFreedom_F(1,43))=-0.94*F7;
        F(DegreesOfFreedom_F(1,46))=-0.94*F7;
    elseif F7<0
        F(DegreesOfFreedom_F(1,49))=-0.94*F7;
        F(DegreesOfFreedom_F(1,52))=-0.94*F7;
    end

    % Solve system=============================================================
    U=K\F;
    Endpositions=p+[U(1:3:end),U(2:3:end),U(3:3:end)];

%     minX = min(minX,min(Endpositions(:,1)));
%     maxX = max(maxX,max(Endpositions(:,1)));
%     minY = min(minY,min(Endpositions(:,2)));
%     maxY = max(maxY,max(Endpositions(:,2)));
% %     minZ = min(minZ,min(Endpositions(:,3)));
%     maxZ = max(maxZ,max(Endpositions(:,3)));

    figNum = mod(tstep,2)+1;
    switch figNum
        case 1
            figure(f1)
        case 2
            figure(f2)
    end
    
    cla
    hold on
    xlim([minXview maxXview])
    ylim([minYview maxYview])
    zlim([minZview maxZview])
    
    for e=1:NumberOfElements
        for i=1:size(Faces,1)
            if showFace(e,i)==1
                C2=Endpositions(t(e,Faces(:,i)),:);
                fill3(C2(:,1),C2(:,2),C2(:,3),0,'FaceColor','w','EdgeColor','k');
            end
        end
    end
    mov(tstep)=getframe(gcf);
end

toc
close all
tic

for PCAnum = 1:7
    w = VideoWriter(['animatePCA',num2str(PCAnum),'.avi']);
    w.FrameRate = 5;
    open(w);
    mov_ = mov((PCAnum-1)*length(scaler)+1:(PCAnum)*length(scaler));
    writeVideo(w, [mov_, fliplr(mov_)]); %Write the frame to the movie file
    close(w);
end
toc
