CXX      := g++
CXXFLAGS := -W -Wall -g
LIBS     := libmkvparser.a libmkvmuxer.a
PARSEOBJ := mkvparser.o mkvreader.o
MUXEROBJ := mkvmuxer.o mkvmuxerutil.o mkvwriter.o
OBJECTS1 := $(PARSEOBJ) sample.o
OBJECTS2 := $(PARSEOBJ) $(MUXEROBJ) sample_muxer/sample_muxer.o
EXES     := samplemuxer sample

all: $(EXES)

sample: $(OBJECTS1) libmkvparser.a
	$(CXX) $(OBJECTS1) -L./ -lmkvparser -o sample

samplemuxer: $(OBJECTS2) $(LIBS)
	$(CXX) $(OBJECTS2) -L./ -lmkvparser -l mkvmuxer -o samplemuxer

libmkvparser.a: $(PARSEOBJ)
	$(AR) rcs libmkvparser.a $(PARSEOBJ)

libmkvmuxer.a: $(MUXEROBJ)
	$(AR) rcs libmkvmuxer.a $(MUXEROBJ)

mkvmuxer.o: mkvmuxer.cpp
	$(CXX) -c $(CXXFLAGS) mkvmuxer.cpp -o mkvmuxer.o

mkvmuxerutil.o: mkvmuxerutil.cpp
	$(CXX) -c $(CXXFLAGS) mkvmuxerutil.cpp -o mkvmuxerutil.o

mkvwriter.o: mkvwriter.cpp
	$(CXX) -c $(CXXFLAGS) mkvwriter.cpp -o mkvwriter.o

mkvparser.o: mkvparser.cpp
	$(CXX) -c $(CXXFLAGS) mkvparser.cpp -o mkvparser.o

mkvreader.o: mkvreader.cpp
	$(CXX) -c $(CXXFLAGS) mkvreader.cpp -o mkvreader.o

sample.o: sample.cpp
	$(CXX) -c $(CXXFLAGS) sample.cpp -o sample.o

sample_muxer/sample_muxer.o: sample_muxer/sample_muxer.cpp
	$(CXX) -c $(CXXFLAGS) -I. sample_muxer/sample_muxer.cpp  -o sample_muxer/sample_muxer.o

clean:
	rm -rf $(OBJECTS2) sample.o $(LIBS) $(EXES) Makefile.bak
