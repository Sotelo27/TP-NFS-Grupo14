.PHONY: all test clean editor client common server build

compile-debug:
	mkdir -p build/
	cmake -S . -B ./build -DCMAKE_BUILD_TYPE=Debug $(EXTRA_GENERATE)
	cmake --build  build/ $(EXTRA_COMPILE)

run-tests: compile-debug
	./build/taller_tests

run-server: compile-debug
	./build/taller_server 8080

run-client: compile-debug
	./build/taller_client 127.0.0.1 8080

run-editor: compile-debug
	./build/taller_editor

all: clean run-tests

clean:
	rm -Rf build/
