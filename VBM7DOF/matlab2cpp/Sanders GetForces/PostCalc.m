function Fout=PostCalc(Fin,parametersM)

% F1Thresh=parametersM(1);
% F1Gain=parametersM(2);
% F2Thresh=parametersM(3);
% F2Gain=parametersM(4);
% F3Thresh=parametersM(5);
% F3Gain=parametersM(6);
% F4Thresh=parametersM(7);
% F4Gain=parametersM(8);
% F5Thresh=parametersM(9);
% F5Gain=parametersM(10);
% F6Thresh=parametersM(11);
% F6Gain=parametersM(12);
% F7ThreshClose=parametersM(13);
% F7ThreshOpen=parametersM(14);
% F7Gain=parametersM(15);


Fout=zeros(1,7);
% F1thres=5e3;
% F2thres=2e4;
% F3thres=1e4;
% F4thres=2e4;
% F5thres=1e3;

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
% Fout(1)=Fin(1)*1e-5;
% Fout(2)=Fin(2)*2.5e-6;
% Fout(3)=Fin(3)*6.67e-6;
% Fout(4)=Fin(4)*4e-6;
% Fout(5)=Fin(5)*6.67e-5;
Fout(1)=Fin(1)*parametersM(2);
Fout(2)=Fin(2)*parametersM(4);
Fout(3)=Fin(3)*parametersM(6);
Fout(4)=Fin(4)*parametersM(8);
Fout(5)=Fin(5)*parametersM(10);
Fout(6)=Fin(6)*parametersM(12);
Fout(7)=Fin(7)*parametersM(15);