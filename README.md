# VoxSandbox

A Minecraft-style voxel world built with C++ and raylib.

**▶ [Play in browser](https://davlwe.github.io/VoxSandbox/)**

## Controls

| Key | Action |
|-----|--------|
| WASD | Move |
| Mouse | Look around |
| Space | Jump |
| 1–4 | Select block (Grass / Dirt / Stone / Wood) |
| Left click | Break block |
| Right click | Place block |
| ESC | Return to menu |

## Project Structure

```
src/
├── main.cpp            Entry point
├── Game.h / .cpp       State machine (Menu / Playing)
├── Menu.h / .cpp       Main menu UI
├── Player.h / .cpp     First-person controller + collision
├── World.h / .cpp      Voxel storage, generation, rendering
├── VoxelRaycast.h/.cpp DDA raycast for block interaction
├── Block.h / .cpp      Block registry + procedural textures
├── UI.h / .cpp         Crosshair + hotbar
└── Constants.h         World size, block types enum
```

## Native Build (macOS)

**Prerequisites:**
- clang (Xcode Command Line Tools)
- [raylib](https://www.raylib.com/) 5.x (`brew install raylib`)

```bash
make          # build
make run      # build & run
make clean    # remove build artifacts
```

Or manually:

```bash
clang++ -std=c++17 -O2 src/*.cpp -o game \
  -I/opt/homebrew/opt/raylib/include \
  -L/opt/homebrew/opt/raylib/lib \
  -lraylib \
  -framework OpenGL -framework Cocoa -framework IOKit \
  -framework CoreVideo -framework CoreFoundation
```

## Web Build (Emscripten)

**Prerequisites:**
- [Emscripten SDK](https://emscripten.org/docs/getting_started/downloads.html)
- raylib source

```bash
# Clone raylib (one time)
git clone https://github.com/raysan5/raylib.git

# Build
make web
# Output → web/index.html
```

Serve locally to test:

```bash
cd web && python3 -m http.server 8000
# Open http://localhost:8000
```

## Adding Block Types

1. Add a value to the `BlockType` enum in `src/Constants.h`
2. Add a row to `BLOCK_DEFS[]` in `src/Block.cpp`:
   ```cpp
   { BlockType::NewBlock, "NewBlock", {R,G,B,A}, "assets/textures/newblock.png" }
   ```
3. (Optional) Add a procedural texture generator in `InitBlocks()` — or drop a 16×16 PNG in `assets/textures/`
4. The hotbar auto-populates with all non-Air blocks from `BLOCK_DEFS`

## GitHub Pages Deployment

Push to `main` and the GitHub Actions workflow automatically:

1. Checks out the repository
2. Sets up Emscripten
3. Clones raylib 5.5
4. Compiles the game to WebAssembly
5. Deploys to the `gh-pages` branch

The deployed site will be available at:

```
https://<your-username>.github.io/VoxSandbox/
```

Enable GitHub Pages in your repo settings:  
**Settings → Pages → Source: Deploy from branch → gh-pages**

## License

MIT
