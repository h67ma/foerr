LIBS=-lsfml-graphics -lsfml-window -lsfml-system
OBJS=main.o window_cursor.o custom_cursor.o log.o log_element_text.o fps_meter.o settings.o setting.o
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
