.PHONY: build

build:
	@mkdir -p build
	@g++ -std=c++14 src/*.cpp src/lib/*.cpp -ledit -o build/app

run:
	@make build
	@./build/app
