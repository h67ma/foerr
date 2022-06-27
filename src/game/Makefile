LIBS=-lsfml-graphics -lsfml-window -lsfml-system
OBJS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))
HEADERS=*.h *.hpp
BIN_PATH=../../foerr
FLAGS=-g -rdynamic -Wall

%.o: %.cpp $(HEADERS)
	g++ $(FLAGS) -c $< -o $@

all: build

build: $(OBJS)
	g++ $(FLAGS) -o $(BIN_PATH) $(OBJS) $(LIBS)

opt_build: FLAGS+=-O3
opt_build: build

clean:
	rm -f *.o $(BIN_PATH)
