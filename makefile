all:
	mkdir -p join
	mkdir -p table
	g++ -O3 big2.cpp -o big2 -lcurses
setup:
	mkdir -p join
	mkdir -p table
big2:
	g++ -O3 big2.cpp -o big2 -lcurses
delete:
	perl del.pl
clean:
	rm big2