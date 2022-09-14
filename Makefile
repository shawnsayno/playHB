
CC      = gcc
CXX     = g++

CFLAGS  =-g -O2 -std=c++11 -pthread -lstdc++ -lm -Wall -fpic

BASE_HOME       = src
HDR     =$(BASE_HOME)
SRC     =$(BASE_HOME)
OBJ     =obj
TARGET = bin/playHb

#VPATH  =-I$(HDR)
HDRS    =$(wildcard $(HDR)/*.h)
SRCS    =$(wildcard $(SRC)/*.c*)

OBJS    =$(patsubst %.cpp, $(OBJ)/%.o, $(notdir $(SRCS)))
DEPS    =$(patsubst %.o, %.d, $(OBJS))
INCLUDE =$(patsubst %, -I%, $(HDR)) 



all: $(TARGET)
$(TARGET):$(OBJS)
	$(CXX)  $^ $(CFLAGS) -o $@ 


-include $(DEPS)

$(OBJ)/%.o : $(SRC)/%.cpp
	$(CXX) $(CFLAGS) $(INCLUDE) -c -MMD $< -o $@

.PHONY: print
print:
	@echo HDRS = $(HDRS)
	@echo SRCS = $(SRCS)
	@echo OBJS = $(OBJS)
	@echo DEPS = $(DEPS)

.PHONY:clean
clean :    
	@rm -f $(TARGET) $(OBJS) $(DEPS)
