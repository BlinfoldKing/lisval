.PHONY: build run

build:
	@mkdir -p build
	@g++ -std=c++14 src/*.cpp src/lib/*.cpp -llinenoise -o build/app

run:
	@make build
	@./build/app
