CXX		= g++ -std=c++11
CXXFLAGS	= -g -Wall
OBJS		= lexer.o parser.o checker.o Type.o Symbol.o Scope.o
PROG		= scc

all:		$(PROG)

$(PROG):	$(OBJS)
		$(CXX) -o $(PROG) $(OBJS)

clean:;		$(RM) -f $(PROG) core *.o
