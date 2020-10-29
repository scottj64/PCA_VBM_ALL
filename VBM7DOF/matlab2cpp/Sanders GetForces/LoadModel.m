function [p,t]=LoadModel()

data1=load('p.mat');
data2=load('t.mat');
p=data1.p;
t=data2.t;