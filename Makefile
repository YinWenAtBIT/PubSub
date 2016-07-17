C=gcc
CXX=g++
CFLAGS= -g -D_DEBUG -fPIC -Wshadow -Wcast-qual -Wcast-align -Wwrite-strings -Wsign-compare -Winvalid-pch -fms-extensions -Wall -MMD
CPPFLAGS=$(CFLAGS) -Woverloaded-virtual -Wsign-promo -fno-gnu-keywords -std=c++11

MAIN_DIR=/home/yw/PubSub/build

OBJS := code.o hub.o sub.o pub.o

MUDUO_INCLUDE=/home/yw/muduo/muduo-master

LIBPATH=/home/yw/muduo/build/release/lib

LIB =  -lmuduo_net -lmuduo_base -lpthread -lrt

TARGET=hub sub pub



all : $(TARGET) 

check : $(TARGET)
		./$^

hub :  hub.o codec.o
		$(CXX) $(CPPFLAGS) -o $(MAIN_DIR)/$@ hub.o codec.o -I$(MUDUO_INCLUDE) -L$(LIBPATH) $(LIB)		


pub :  pub.o
		$(CXX) $(CPPFLAGS) -o $(MAIN_DIR)/$@ pub.o codec.o -I$(MUDUO_INCLUDE) -L$(LIBPATH) $(LIB)
	
sub :  sub.o
		$(CXX) $(CPPFLAGS) -o $(MAIN_DIR)/$@ sub.o codec.o -I$(MUDUO_INCLUDE) -L$(LIBPATH) $(LIB)			



codec.o: codec.cpp
		$(CXX) $(CPPFLAGS) -c codec.cpp -o ./$@  -I$(MUDUO_INCLUDE) -L$(LIBPATH)  $(LIB)	

hub.o: hub.cpp
		$(CXX) $(CPPFLAGS) -c hub.cpp -o ./$@  -I$(MUDUO_INCLUDE) -L$(LIBPATH)  $(LIB)	

pub.o: pub.cpp
		$(CXX) $(CPPFLAGS) -c pub.cpp -o ./$@  -I$(MUDUO_INCLUDE) -L$(LIBPATH)  $(LIB)	

sub.o: sub.cpp
		$(CXX) $(CPPFLAGS) -c sub.cpp -o ./$@  -I$(MUDUO_INCLUDE) -L$(LIBPATH)  $(LIB)	

clean:
		rm -rf .o *.d $(OBJS)  && cd ./build && rm $(TARGET)

