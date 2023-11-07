all: lemur test

lemur:
	g++ -g main/main.cpp src/*.cpp src/lib/*.cpp -o bin/lemur

test:
	g++ -g tests/main.cpp src/*.cpp src/lib/*.cpp -o bin/lemur-test

clean:
	rm bin/*