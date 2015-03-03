CXX=g++

CC=gcc

SERV_OBJ = raw_recv.o
CLIE_OBJ = raw_send.o

CFLAG = -g -O0
CXXFLAG = -g -O0

LINK = -lpthread

SERV=s.out
CLIE=c.out

all: $(SERV) $(CLIE)

$(SERV):$(SERV_OBJ)
	$(CXX) $< -o $@

$(CLIE):$(CLIE_OBJ)
	$(CXX) $< -o $@

%.o:%.cpp
	$(CXX)$(CXXFLAG) $(CFLAGS) $(INC) -c $< -o $@

%.o:%.c
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

clean:
	rm -f $(SERV_OBJ) $(CLIE_OBJ)
	rm -f $(SERV) $(CLIE)
	rm -f core
