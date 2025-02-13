libs_args = -lmingw32 -lSDL2main -lSDL2
c_files = $(wildcard src/*.cpp)
o_files = $(patsubst src/%.cpp,obj/%.o, $(c_files))

obj/%.o: src/%.cpp
	g++ -c $< -o $@ -Wall -Werror -Iincludes 

all: $(o_files)
	g++ -o app/app.exe $(o_files) -Llib $(libs_args) 