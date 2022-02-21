
all:
	g++ main.cpp -lboost_program_options -g
	./a.out --help
	./a.out ls --help
