.PHONY: build

build:
	@mkdir -p build
	@g++ -std=c++14 src/*.cpp -o build/app

run:
	@make build
	@./build/app
