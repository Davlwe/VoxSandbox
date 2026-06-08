# VoxSandbox Makefile
# ====================
# make          → native macOS build (CMake + homebrew raylib)
# make run      → build and run
# make web      → Emscripten build (CMake + FetchContent raylib)
# make clean    → remove build artifacts

.PHONY: native run web clean

native:
	cmake -B build -DCMAKE_BUILD_TYPE=Release
	cmake --build build --parallel
	@echo "Native build → build/VoxSandbox"

run: native
	./build/VoxSandbox

web:
	emcmake cmake -B build-web -DWEB_BUILD=ON -DCMAKE_BUILD_TYPE=Release
	cmake --build build-web --parallel
	@echo "Web build → build-web/VoxSandbox.html"

clean:
	rm -rf build/ build-web/ game
