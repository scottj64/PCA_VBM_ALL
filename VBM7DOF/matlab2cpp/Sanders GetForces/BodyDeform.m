function [vd_out,wd_out]=BodyDeform(F1,F2,F3,F4,F5,F6,F7,a,b,c,p,Kinv,DegreesOfFreedom_F)
% Initialization
TotalNumberOfNodes=size(p,1);
F=zeros(3*TotalNumberOfNodes,1);
% Apply Forces:  *********************************************
%==========================================================================
%Torso bending: 240,249,237,251;Torso moment: 234,258;Head bending forward:
%205,210,200,198; Head bending sideway: 178,189; head twisting: 178,189
%Scrapular: 224,229,65,67 (93,95 for front){opp} 
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
else
    F(DegreesOfFreedom_F(1,43),1)=0;
    F(DegreesOfFreedom_F(1,46),1)=0;
    F(DegreesOfFreedom_F(1,49),1)=0;
    F(DegreesOfFreedom_F(1,52),1)=0;
end
% Solve system=============================================================
U=Kinv*F;
Displacements=[U(1:3:end),U(2:3:end),U(3:3:end)];
Endpositions=p+Displacements;
%%%%%%%%%
%Compute Deformed vectors==================================================
vdm=Endpositions(b,:)-Endpositions(a,:);
vd_out=vdm'/norm(vdm,2);
wdm=Endpositions(c,:)-Endpositions(a,:);
wd_out=wdm'/norm(wdm,2);
% %==========================================================================