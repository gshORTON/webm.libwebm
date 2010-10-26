# source files.
SRC =  	webmmuxstreaminfo.cpp webmmuxebmlio.cpp webmmuxstream.cpp webmmuxstreamvideo.cpp webmmuxstreamvideovpx.cpp webmmuxstreamaudio.cpp webmmuxstreamaudiovorbis.cpp webmmuxstreamaudiovorbisogg.cpp webmmuxcontext.cpp

OBJ = $(SRC:.cpp=.o)

OUT = ../libwebm.a

# include directories
#INCLUDES = -I. -I../include/ -I/usr/local/include
INCLUDES = -I. -I./common/

# C++ compiler flags (-g -O2 -Wall)
CCFLAGS = -g -Wall

# compiler
#CXX = g++

# library paths
#LIBS = -L../ -L/usr/local/lib -lm
LIBS = -L../

# compile flags
LDFLAGS = -g

.SUFFIXES: .cpp

#default: dep $(OUT)
default: $(OUT)

.cpp.o:
	$(CXX) $(INCLUDES) $(CCFLAGS) -c $< -o $@

$(OUT): $(OBJ)
	ar rcs $(OUT) $(OBJ)

depend: dep

dep:
	makedepend -- $(CFLAGS) -- $(INCLUDES) $(SRC)

clean:
	rm -f $(OBJ) $(OUT) Makefile.bak 
