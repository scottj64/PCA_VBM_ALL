function [Kinv,DegreesOfFreedom_F]=CalcKinv(p,t)
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
%==========================================================================
B=cell(1,833);
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

%==========================================================================
K(DegreesOfFreedomDirichlet,:)=0; % put zeros in boundary rows of K and F
K(DegreesOfFreedomDirichlet,DegreesOfFreedomDirichlet)= ...
    eye(numel(DegreesOfFreedomDirichlet));
Kinv=inv(K);
%==========================================================================
%Set up forces
%Torso bending: 240,249,237,251;Torso moment: 234,258;Head bending forward:
%205,210,200,198; Head bending sideway: 178,189; head twisting: 178,189
%Scrapular: 224,229,65,67{opp} 
NodesOfElement_F=[240 249 237 251 29 258 205 200 170 167 178 189 224 229 65 67 95 93]; % row containing nodenumbers of element e
    DegreesOfFreedom_F=reshape([3*NodesOfElement_F-2; ...
        3*NodesOfElement_F-1;3*NodesOfElement_F], ...
        1,3*numel(NodesOfElement_F)); 
