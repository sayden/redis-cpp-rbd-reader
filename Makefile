CMAKE_TOOLCHAIN_FILE = -DCMAKE_TOOLCHAIN_FILE=/home/mcastro/software/vcpkg/scripts/buildsystems/vcpkg.cmake

compile:
	CURRENT_DIR=`pwd`
	cd build && cmake --build .
	cd "${CURRENT_DIR}"

all:
	@echo "All..."
	CURRENT_DIR=`pwd`
	rm -rf build && mkdir build && cd build && cmake $(CMAKE_TOOLCHAIN_FILE) .. && cmake --build . && ctest --verbose && valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -s tests
	cd "${CURRENT_DIR}"

test: compile
	@echo "Testing..."
	CURRENT_DIR=`pwd`
	cd build && ctest --verbose
	cd "${CURRENT_DIR}"

valgrind:
	@echo "Valgrinding tests..."
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -s tests
	@echo "Valgrinding binary..."
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -s rediscpp

massif:
	@echo "Creating massif file..."
