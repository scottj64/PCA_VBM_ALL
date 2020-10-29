function [U,K,F]=BodyTestPlot(F1,F2,F3,F4,F5,F6,F7)
%==========================================================================
% clear
% close all
% clc
% Set tetrahedrical mesh===================================================
load('p.mat')
load('t.mat')
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% % Plot Meshing:
% % -------------
% figure
% plot3(p(:,1),p(:,2),p(:,3),'o')
% hold on
% for o=1:length(p)
% text(p(o,1),p(o,2),p(o,3),int2str(o),'fontsize',14,'color','k');
% end
% hold on
% for s=1:length(t)
%     x1=p(t(s,1),1); 
%     y1=p(t(s,1),2);
%     z1=p(t(s,1),3);
%     x2=p(t(s,2),1); 
%     y2=p(t(s,2),2);
%     z2=p(t(s,2),3);
%     x3=p(t(s,3),1); 
%     y3=p(t(s,3),2);
%     z3=p(t(s,3),3);
%     x4=p(t(s,4),1); 
%     y4=p(t(s,4),2);
%     z4=p(t(s,4),3);
%     line([x1,x2],[y1,y2],[z1,z2]);
%     line([x2,x3],[y2,y3],[z2,z3]);
%     line([x3,x4],[y3,y4],[z3,z4]);
%     line([x4,x1],[y4,y1],[z4,z1]);
%     line([x1,x3],[y1,y3],[z1,z3]);
%     line([x2,x4],[y2,y4],[z2,z4]);
% %     text((x1+x2+x3+x4)/4,(y1+y2+y3+y4)/4, ...
% %         (z1+z2+z3+z4)/4,int2str(s),'fontsize',13,'color','r');
% end
% axis equal
% view(30,20)
% rotate3d on;
% title('3D Beam with Tetrahedron Elements', 'fontsize',20,'color','r') ;
% hold off
%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
% Set parameters===========================================================
E=4e3;
nu=0.33;
% List of p with a Dirichlet boundary condition (prescribed displacement)
NodesDirichlet=find(p(:,3)==0);
DegreesOfFreedomDirichlet=reshape([3*NodesDirichlet-2;3*NodesDirichlet-1; ...
    3*NodesDirichlet],1,3*numel(NodesDirichlet));
% =========================================================================
TotalNumberOfNodes=size(p,1);
NumberOfElements=size(t,1);
% Linear elastic isotropic material========================================
D=E/((1+nu)*(1-2*nu))*[1-nu nu      nu      0 0 0
                       nu   1-nu    nu      0 0 0
                       nu   nu      1-nu    0 0 0
0 0 0 (1-2*nu)/2    0           0
0 0 0 0             (1-2*nu)/2  0
0 0 0 0             0           (1-2*nu)/2];
% Initialisation of K, M and F=============================================
K=zeros(3*TotalNumberOfNodes,3*TotalNumberOfNodes);
F=zeros(3*TotalNumberOfNodes,1);
%==========================================================================
for e=1:NumberOfElements
    NodesOfElement=t(e,:); % row containing nodenumbers of element e
    DegreesOfFreedom=reshape([3*NodesOfElement-2; ...
        3*NodesOfElement-1;3*NodesOfElement],1,3*numel(NodesOfElement));
    X=[ones(1,4);p(NodesOfElement,:)'];
    VolumeOfElement=abs(det(X))/6; % volume of element e
    Coeffs=inv(X);
    diffPhi=Coeffs(:,2:end);
    B{e}=[];
    for i=1:4
        Bsmall=[
                diffPhi(i,1)    0               0;
                0               diffPhi(i,2)    0;
                0               0               diffPhi(i,3);
                diffPhi(i,2)    diffPhi(i,1)    0;
                0               diffPhi(i,3)    diffPhi(i,2);
                diffPhi(i,3)    0               diffPhi(i,1)];
        B{e}=[B{e},Bsmall];
    end
    % Computation of 12*12 Ke
    Ke=B{e}'*D*B{e}*VolumeOfElement;
    K(DegreesOfFreedom,DegreesOfFreedom)=K(DegreesOfFreedom,DegreesOfFreedom)+Ke; 
end
%%%%%%%%%
%==========================================================================
% Apply Forces:  *********************************************
%==========================================================================
%Torso bending: 240,249,237,251;Torso moment: 234,258;Head bending forward:
%205,210,200,198; Head bending sideway: 178,189; head twisting: 178,189
%Scrapular: 224,229,65,67{opp} 
NodesOfElement_F=[240 249 237 251 234 258 205 200 170 167 178 189 224 229 65 67 95 93]; % row containing nodenumbers of element e
    DegreesOfFreedom_F=reshape([3*NodesOfElement_F-2; ...
        3*NodesOfElement_F-1;3*NodesOfElement_F], ...
        1,3*numel(NodesOfElement_F)); 
% body bending:
F(DegreesOfFreedom_F(1,1),1)=F1/4;
F(DegreesOfFreedom_F(1,2),1)=F2/4;
F(DegreesOfFreedom_F(1,4),1)=F1/4;
F(DegreesOfFreedom_F(1,5),1)=F2/4;
F(DegreesOfFreedom_F(1,7),1)=F1/4;
F(DegreesOfFreedom_F(1,8),1)=F2/4;
F(DegreesOfFreedom_F(1,10),1)=F1/4;
F(DegreesOfFreedom_F(1,11),1)=F2/4;
% body moment: 
F(DegreesOfFreedom_F(1,13),1)=F3;
F(DegreesOfFreedom_F(1,16),1)=-F3;
%head forward:
F(DegreesOfFreedom_F(1,19),1)=F5/4;
F(DegreesOfFreedom_F(1,22),1)=F5/4;
F(DegreesOfFreedom_F(1,25),1)=-F5/4;
F(DegreesOfFreedom_F(1,28),1)=-F5/4;
%head sideway:
F(DegreesOfFreedom_F(1,33),1)=F4;
F(DegreesOfFreedom_F(1,36),1)=-F4;
%head twist:
F(DegreesOfFreedom_F(1,31),1)=F6/2;
F(DegreesOfFreedom_F(1,34),1)=-F6/2;
%scrapular motion:
F(DegreesOfFreedom_F(1,37),1)=F7;
F(DegreesOfFreedom_F(1,40),1)=F7;
if F7>0
    F(DegreesOfFreedom_F(1,43),1)=-0.94*F7;
    F(DegreesOfFreedom_F(1,46),1)=-0.94*F7;
    F(DegreesOfFreedom_F(1,49),1)=0;
    F(DegreesOfFreedom_F(1,52),1)=0;
elseif F7<0
    F(DegreesOfFreedom_F(1,49),1)=-0.94*F7;
    F(DegreesOfFreedom_F(1,52),1)=-0.94*F7;
    F(DegreesOfFreedom_F(1,43),1)=0;
    F(DegreesOfFreedom_F(1,46),1)=0;
end
%==========================================================================
K(DegreesOfFreedomDirichlet,:)=0; % put zeros in boundary rows of K and F
K(DegreesOfFreedomDirichlet,DegreesOfFreedomDirichlet)= ...
    eye(numel(DegreesOfFreedomDirichlet));
% Solve system=============================================================
U=K\F;
Displacements=[U(1:3:end),U(2:3:end),U(3:3:end)];
Endpositions=p+Displacements;
%%%%%%%%%
%Compute Deformed vectors==================================================
%==========================================================================
% figure
% set(gcf,'color','w')
% set(gca,'Projection','perspective')
% view(-120,50)
% axis vis3d
figure('Units','inches',...
'Position',[0 0 5 6],'PaperPositionMode','auto');
view(131,7)
axis equal

axis off
hold on
% xlabel('x')
% ylabel('y')
% zlabel('z')
% handle=colorbar;
% ylabel(handle,'U')
%==========================================================================
Faces=[1 1 2 3;2 2 3 1;3 4 4 4;1 1 2 3];
for e=1:NumberOfElements
    for i=1:size(Faces,1)
        C1=p(t(e,Faces(:,i)),:);
        C2=Endpositions(t(e,Faces(:,i)),:);
        % Show original mesh
%        fill3(C1(:,1),C1(:,2),C1(:,3),0,'LineStyle','--','EdgeColor',[0.5,0.5,0.5],'FaceColor','none');
        % Show deformed mesh
        fill3(C2(:,1),C2(:,2),C2(:,3),Endpositions(t(e,Faces(:,i))),'FaceColor','w','EdgeColor',[0,0,0]);
    end
end
% axis equal
% Label the p with a Dirichlet condition===============================
% for i=1:numel(NodesDirichlet)
%    quiver3(Endpositions(NodesDirichlet(i),1),Endpositions(NodesDirichlet(i),2) ...
%        ,Endpositions(NodesDirichlet(i),3),5,0,0,'linewidth',1,'Color','m','MaxHeadSize',0.5);
%    quiver3(Endpositions(NodesDirichlet(i),1),Endpositions(NodesDirichlet(i),2) ...
%        ,Endpositions(NodesDirichlet(i),3),0,5,0,'linewidth',1,'Color','m','MaxHeadSize',0.5);
%    quiver3(Endpositions(NodesDirichlet(i),1),Endpositions(NodesDirichlet(i),2) ...
%        ,Endpositions(NodesDirichlet(i),3),0,0,5,'linewidth',1,'Color','m','MaxHeadSize',0.5);
% end
% for o=1:length(p)
% text(Endpositions(o,1),Endpositions(o,2),Endpositions(o,3),int2str(o),'fontsize',14,'color','k');
% end
% axis equal
rotate3d on;
%==========================================================================