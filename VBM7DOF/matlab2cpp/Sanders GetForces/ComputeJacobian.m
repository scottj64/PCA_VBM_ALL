function dedF=ComputeJacobian(F1,F2,F3,F4,F5,F6,F7,a,b,c,p,Kinv,DegreesOfFreedom_F)

NumofForces=7;
[vd_out,wd_out]=BodyDeform(F1,F2,F3,F4,F5,F6,F7,a,b,c,p,Kinv,DegreesOfFreedom_F);
error_vi=vd_out;
error_wi=wd_out;
dedF=zeros(6,7);
for k=1:NumofForces
    dedF(:,k)=NumDiff(F1,F2,F3,F4,F5,F6,F7,a,b,c,error_vi,error_wi,p,k,Kinv,DegreesOfFreedom_F);
end



