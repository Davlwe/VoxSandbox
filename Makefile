# VoxSandbox Makefile
# ====================
# make          → native macOS build (clang + homebrew raylib)
# make run      → build and run
# make web      → Emscripten build (requires emsdk + raylib source)
# make clean    → remove build artifacts

CXX      := clang++
CXXFLAGS := -std=c++17 -O2
RAYLIB   := /opt/homebrew/opt/raylib
INCLUDES := -I$(RAYLIB)/include
LDFLAGS  := -L$(RAYLIB)/lib -lraylib \
            -framework OpenGL -framework Cocoa -framework IOKit \
            -framework CoreVideo -framework CoreFoundation
TARGET   := game
SOURCES  := src/*.cpp

.PHONY: native run web clean

native: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $@ $(INCLUDES) $(LDFLAGS)

run: native
	./$(TARGET)

# ---------------------------------------------------------------------------
# Web build (Emscripten)
# ---------------------------------------------------------------------------
RAYLIB_SRC ?= raylib/src

web: $(SOURCES) index.html
	@test -f $(RAYLIB_SRC)/rcore.c || ( \
	  echo "raylib source not found at $(RAYLIB_SRC)/" && \
	  echo "Clone it:  git clone https://github.com/raysan5/raylib.git" && \
	  exit 1 )
	@mkdir -p web
	emcc -o web/index.html $(SOURCES) \
	  $(RAYLIB_SRC)/rcore.c \
	  $(RAYLIB_SRC)/rshapes.c \
	  $(RAYLIB_SRC)/rtextures.c \
	  $(RAYLIB_SRC)/rtext.c \
	  $(RAYLIB_SRC)/rmodels.c \
	  $(RAYLIB_SRC)/utils.c \
	  -I$(RAYLIB_SRC) \
	  -I$(RAYLIB_SRC)/external/glfw/include \
	  -DPLATFORM_WEB \
	  -s USE_GLFW=3 \
	  -s ASYNCIFY \
	  -s ALLOW_MEMORY_GROWTH=1 \
	  -s TOTAL_STACK=64MB \
	  --shell-file index.html \
	  -O2
	@echo "Web build → web/index.html"

# ---------------------------------------------------------------------------
clean:
	rm -f $(TARGET)
	rm -rf web/
