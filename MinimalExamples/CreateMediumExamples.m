clear;clc;

oPaths = {'001_MediumExamples/'};
rng(42);

for k=1:numel(oPaths)
  
  oPath = oPaths{k};
  if( ~exist(oPath,'dir') )
    mkdir(oPath);
  end
  
  param = cplexoptimset('cplex');
  param.threads = 1;
  param.output.clonelog = 0;
  param.mip.tolerances.integrality = 1e-10;
  param.display = 'on';
  
  d = 64;
  A = full(paralleltomo(d,0:10:179,d,d));
  A = A(sum(A>0,2) > 10,:);
  
  B = zeros(size(A,1),5);
  
  for i=1:size(A,1)
    w = A(i,:);
    w = w(w>0);
    n = numel(w);
    
    c = round(ones(n,1) + 0.2*randn(n,1),5);
    w = round(1e+5*w);
    
    assert( ~any(c <= 0) ); % cost vector is non negative
    
    x = randi(2,n,1) - 1;
    while( sum(x) == n || sum(x) == 0 )
      x = randi(2,n,1) - 1;
    end
    
    ggt = gcd(w(1),w(2));
    for j=3:n
      ggt = gcd(ggt,w(j));
    end
    ggt = gcd(ggt,w*x);
    w = w/ggt;
    b = w*x;
    
    example = struct;
    example.c = c;
    example.w = w;
    example.b = b;
    
    %x_rel = cplexlp(c,[],[],w,b,zeros(n,1),ones(n,1),[],param);
    %x_int = cplexbilp(c,[],[],w,b,[],param);
    
    %res = abs(abs(x_int - 0.5)-0.5);
    %assert(norm(res,'inf') < 1e-10); % integrality gap
    
    %assert(w*round(x_int) == b);
    %x_int = round(x_int);
    
    %example.x_int = x_int;
    %example.x_rel = x_rel;
    %example.opt_int = c'*x_int;
    %example.opt_rel = c'*x_rel;
    %example.gap = c'*x_int - c'*x_rel;
    
    %assert( example.opt_rel <= example.opt_int );
    
    %B(i,:) = [i n example.opt_rel example.opt_int example.gap];
    
    SaveExample(c,w,w*x,sprintf('%s%03d_Example.txt',oPath,i));
    save([oPath sprintf('%03d_Example.mat',i)],'-struct','example');
  end
  
%   f = fopen([oPath 'table.txt'],'w');
%   
%   fprintf(f,'idx vars rel int gap\n');
%   for i=1:size(B,1)
%     fprintf(f,'%02d %02d %.20f %.20f %.10f',B(i,:));
%     if( i < size(B,1) )
%       fprintf(f,'\n');
%     end
%   end
%   
%   fclose(f);
end

%dlmwrite([oPath 'table.txt'],B,'delimiter',' ','precision','%.10f','-append');