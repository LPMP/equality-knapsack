function []=SaveExample(c,w,b,name)
	
	assert(length(c) == length(w));
	
	if( size(c,1) > 1 )
		c = c';
	end
	if( size(w,1) > 1 )
		w = w';
	end
	
	f = fopen(name,'w');
	fprintf(f,'EKP\n');
	fprintf(f,'%d\n',length(c));
	fclose(f);
	
	dlmwrite(name,c,'-append','delimiter',' ','precision','%.5f');
	dlmwrite(name,w,'-append','delimiter',' ','precision','%.0f');
	dlmwrite(name,b,'-append','delimiter',' ','precision','%.0f');
	
end