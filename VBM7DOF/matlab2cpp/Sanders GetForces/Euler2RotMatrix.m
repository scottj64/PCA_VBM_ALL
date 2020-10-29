function Rot=Euler2RotMatrix(angles,n)

roll=angles(1);
yaw=angles(2);
pitch=angles(3);

switch n
    case 1
        %rotation about y-axis
        P=[cos(-pitch) 0 sin(-pitch);0 1 0;-sin(-pitch) 0 cos(-pitch)];
        %rotation about x-axis
        Y=[1 0 0;0 cos(-yaw) -sin(-yaw);0 sin(-yaw) cos(-yaw)];
        %rotation about z-axis
        R=[cos(-roll),-sin(-roll),0;sin(-roll),cos(-roll),0;0,0,1];
    case 2
        %rotation about y-axis
        P=[cos(pitch) 0 sin(pitch);0 1 0;-sin(pitch) 0 cos(pitch)];
        %rotation about x-axis
        Y=[1 0 0;0 cos(yaw) -sin(yaw);0 sin(yaw) cos(yaw)];
        %rotation about z-axis
        R=[cos(-roll),-sin(-roll),0;sin(-roll),cos(-roll),0;0,0,1];
    case 3
        %rotation about y-axis
        P=[cos(-pitch) 0 sin(-pitch);0 1 0;-sin(-pitch) 0 cos(-pitch)];
        %rotation about z-axis
        Y=[cos(-yaw) -sin(-yaw) 0;sin(-yaw) cos(-yaw) 0;0 0 1];
        %rotation about x-axis
        R=[1 0 0;0 cos(-roll) -sin(-roll);0 sin(-roll) cos(-roll)];
    case 4
        %rotation about x-axis
        P=[1 0 0;0 cos(pitch) -sin(pitch);0 sin(pitch) cos(pitch)];
        %rotation about z-axis
        Y=[cos(-yaw) -sin(-yaw) 0;sin(-yaw) cos(-yaw) 0;0 0 1];
        %rotation about y-axis
        R=[cos(roll) 0 sin(roll);0 1 0;-sin(roll) 0 cos(roll)];
    case 5
        %rotation about y-axis
        P=[cos(pitch) 0 sin(pitch);0 1 0;-sin(pitch) 0 cos(pitch)];
        %rotation about z-axis
        Y=[cos(-yaw) -sin(-yaw) 0;sin(-yaw) cos(-yaw) 0;0 0 1];
        %rotation about x-axis
        R=[1 0 0;0 cos(-roll) -sin(-roll);0 sin(-roll) cos(-roll)];
end
%Pitch, row, yaw order
Rot=P*Y*R;
        