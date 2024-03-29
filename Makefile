CXX = g++

EXE = checker
OBJ_DIR = ./build/
MAIN = main.cpp
SRCS = checker.cpp docker.cpp generator.cpp utils.cpp
OBJS = $(addprefix $(OBJ_DIR), $(addsuffix .o, $(basename $(notdir $(SRCS)))))
CXXFLAGS = -std=c++17 -Wall -Wformat
LIBS = -lcurl

all: $(EXE)
	@echo build complete

$(EXE): $(OBJS) $(MAIN)
	$(CXX) -o $@ $(MAIN) $(OBJS) $(CXXFLAGS) $(LIBS)

$(OBJ_DIR)checker.o : checker.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJ_DIR)docker.o : docker.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJ_DIR)generator.o : generator.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJ_DIR)utils.o : utils.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

build :
	mkdir $(OBJ_DIR)

clean:
	rm -rf $(OBJS)
