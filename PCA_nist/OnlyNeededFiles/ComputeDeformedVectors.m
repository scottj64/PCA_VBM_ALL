function zd_hat=ComputeDeformedVectors(a,b,c,Rot,p)

%initialization
v=p(b,:)-p(a,:);
v_hat=v'/norm(v,2);
w=p(c,:)-p(a,:);
w_hat=w'/norm(w,2);
%Calculate deformed vectors based on rotation matrices
vd_hat=Rot*v_hat;
wd_hat=Rot*w_hat;
zd_hat=[vd_hat;wd_hat];
%solve for set of forces iternatively


