clear;clc;

oPaths = {'001_MediumExamples/'};
rng(42);

for k=1:numel(oPaths)
  
  oPath = oPaths{k};
  if( ~exist(oPath,'dir') )
    mkdir(oPath);
  end
 
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
     
    SaveExample(c,w,w*x,sprintf('%s%03d_Example.txt',oPath,i));
    save([oPath sprintf('%03d_Example.mat',i)],'-struct','example');
  end
end