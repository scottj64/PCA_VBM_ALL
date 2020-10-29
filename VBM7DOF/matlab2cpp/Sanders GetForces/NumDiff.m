function dedF=NumDiff(F1,F2,F3,F4,F5,F6,F7,a,b,c,error_vi,error_wi,p,z,Kinv,DegreesOfFreedom_F)
%initial errors
dF=[200;2000;200;200;200;200;200];
switch z
    
    case 1
        %Change F1, keep the rest fixed
        F1c=F1+dF(1);
        [vd_out,wd_out]=BodyDeform(F1c,F2,F3,F4,F5,F6,F7,a,b,c,p,Kinv,DegreesOfFreedom_F);
        error_vf1=vd_out-error_vi;
        error_wf1=wd_out-error_wi;
        %differential error of v
        devdF=error_vf1/dF(1);

        %differential error of w
        dewdF=error_wf1/dF(1);
        
        dedF=[devdF;dewdF];

    case 2
        %Change F2, keep the rest fixed
        F2c=F2+dF(2);
        [vd_out,wd_out]=BodyDeform(F1,F2c,F3,F4,F5,F6,F7,a,b,c,p,Kinv,DegreesOfFreedom_F);
        error_vf2=vd_out-error_vi;
        error_wf2=wd_out-error_wi;

        %differential error of v
        devdF=error_vf2/dF(2);

        %differential error of w
        dewdF=error_wf2/dF(2);
        
        dedF=[devdF;dewdF];
        
    case 3
        %Change F3, keep the rest fixed
        F3c=F3+dF(3);
        [vd_out,wd_out]=BodyDeform(F1,F2,F3c,F4,F5,F6,F7,a,b,c,p,Kinv,DegreesOfFreedom_F);
        error_vf3=vd_out-error_vi;
        error_wf3=wd_out-error_wi;

        %differential error of v
        devdF=error_vf3/dF(3);

        %differential error of w
        dewdF=error_wf3/dF(3);
        
        dedF=[devdF;dewdF];
        
    case 4
        %Change F1, keep rest fixed
        F4c=F4+dF(4);
        [vd_out,wd_out]=BodyDeform(F1,F2,F3,F4c,F5,F6,F7,a,b,c,p,Kinv,DegreesOfFreedom_F);
        error_vf4=vd_out-error_vi;
        error_wf4=wd_out-error_wi;
        %differential error of v
        devdF=error_vf4/dF(4);

        %differential error of w
        dewdF=error_wf4/dF(4);
        
        dedF=[devdF;dewdF];
        
    case 5
        %Change F1, keep the rest fixed
        F5c=F5+dF(5);
        [vd_out,wd_out]=BodyDeform(F1,F2,F3,F4,F5c,F6,F7,a,b,c,p,Kinv,DegreesOfFreedom_F);
        error_vf5=vd_out-error_vi;
        error_wf5=wd_out-error_wi;
        %differential error of v
        devdF=error_vf5/dF(5);

        %differential error of w
        dewdF=error_wf5/dF(5);
        
        dedF=[devdF;dewdF];
    case 6
        %Change F1, keep the rest fixed
        F6c=F6+dF(6);
        [vd_out,wd_out]=BodyDeform(F1,F2,F3,F4,F5,F6c,F7,a,b,c,p,Kinv,DegreesOfFreedom_F);
        error_vf6=vd_out-error_vi;
        error_wf6=wd_out-error_wi;
        %differential error of v
        devdF=error_vf6/dF(6);

        %differential error of w
        dewdF=error_wf6/dF(6);
        
        dedF=[devdF;dewdF];
    case 7
        %Change F1, keep the rest fixed
        F7c=F7+dF(7);
        [vd_out,wd_out]=BodyDeform(F1,F2,F3,F4,F5,F6,F7c,a,b,c,p,Kinv,DegreesOfFreedom_F);
        error_vf7=vd_out-error_vi;
        error_wf7=wd_out-error_wi;
        %differential error of v
        devdF=error_vf7/dF(7);

        %differential error of w
        dewdF=error_wf7/dF(7);
        
        dedF=[devdF;dewdF];
end