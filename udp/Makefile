CXX=g++

CC=gcc

SERV_OBJ = echo_serv.o
CLIE_OBJ = client.o
RAW_CLIE_OBJ = raw_udp_client.o
CFLAG = -g3 -O0
CXXFLAG = -g3 -O0

LINK = -lpthread

SERV=s.out
CLIE=c.out
RAW_CLIE=rc.out

all: $(SERV) $(CLIE) $(RAW_CLIE)

$(SERV):$(SERV_OBJ)
	$(CXX) $< -o $@

$(CLIE):$(CLIE_OBJ)
	$(CXX) $< -o $@

$(RAW_CLIE):$(RAW_CLIE_OBJ)
	$(CXX) $< -o $@ $(LINK)

%.o:%.cpp
	$(CXX)$(CXXFLAG) $(CFLAGS) $(INC) -c $< -o $@

%.o:%.c
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

clean:
	rm -f $(SERV_OBJ) $(CLIE_OBJ) $(RAW_CLIE_OBJ)
	rm -f $(SERV) $(CLIE) $(RAW_CLIE)
	rm -f core
