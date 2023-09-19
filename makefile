all:
	mkdir -p join
	mkdir -p table
	nvcc -O3 --compiler-options -Wall -Xptxas -v big2.cu -o big2 -lcurses
setup:
	mkdir -p join
	mkdir -p table
big2:
	nvcc -O3 --compiler-options -Wall -Xptxas -v big2.cu -o big2 -lcurses
delete:
	perl del.pl
clean:
	rm big2