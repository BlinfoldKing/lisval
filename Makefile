.PHONY: build run

build:
	@mkdir -p build
	@g++ -std=c++14 src/*.cpp src/lib/*.cpp -llinenoise -o build/app

run:
	@make build
	@./build/app

install/linenoise:
	@cd ext/linenoise && \
		rm -rf build && \
		mkdir -p build && \
		(cd build && cmake -DCMAKE_BUILD_TYPE=Release .. && make && sudo make install)

install:
	@git submodule update --init
	@make install/linenoise/nix
