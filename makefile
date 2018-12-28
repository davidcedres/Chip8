CXX = clang++
COMPILE_FLAGS = -c -std=c++11	
SFML_LINK = -lsfml-system -lsfml-window -lsfml-graphics

all: core.o main.o io.o
	$(CXX) $^ -o app $(SFML_LINK)

core.o: core.cpp
	$(CXX) $(COMPILE_FLAGS) $< -I.

main.o: main.cpp
	$(CXX) $(COMPILE_FLAGS) $< -I.

io.o: io.cpp
	$(CXX) $(COMPILE_FLAGS) $< -I.

clean:
	$(RM) core.o main.o io.o app
