clear;clc;

oPath = 'MiniExamples/';
if( ~exist(oPath,'dir') )
	mkdir(oPath);
end

param = cplexoptimset('cplex');
param.threads = 1;
param.output.clonelog = 0;
%param.display = 'on';

d = 8;
A = full(paralleltomo(d,[0:10:179],d,d));
A = A(sum(A>0,2) > 10,:);

B = zeros(size(A,1),5);

rng(42);
for i=1:size(A,1)
	w = A(i,:);
	w = w(w>0);
	n = numel(w);
	
	c = ones(n,1) + 0.2*randn(n,1);
	x = randi(2,n,1) - 1;
	
	example = struct;
	example.c = c;
	example.w = w;
	example.b = w*x;
	
	x_rel = cplexlp(c,[],[],w,w*x,zeros(n,1),ones(n,1),[],param);
	x_int = cplexbilp(c,[],[],w,w*x,[],param);
	
	example.opt_int = c'*x_int;
	example.opt_rel = c'*x_rel;
	example.gap = c'*x_int - c'*x_rel;
	
	B(i,:) = [i n example.opt_int example.opt_rel example.gap];
	
	SaveExample(c,w,w*x,sprintf('%s%03d_MiniExample.txt',oPath,i));
	save([oPath sprintf('%03d_MiniExample.mat',i)],'-struct','example');
end

dlmwrite([oPath 'table.txt'],B,'delimiter',' ','precision','%.5f');
