# VoxSandbox

A Minecraft-style voxel world built with C++ and raylib.

**▶ [Play in browser](https://davlwe.github.io/VoxSandbox/)**

## Controls

| Key | Action |
|-----|--------|
| WASD | Move |
| Mouse | Look around |
| Space | Jump |
| 1-9 | Select blocks |
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
- Xcode Command Line Tools
- CMake 3.15+
- [raylib](https://www.raylib.com/) 5.x (`brew install raylib`)

```bash
make          # CMake configure + build
make run      # build & run
make clean    # remove build artifacts
```

> The CMake build outputs to `build/VoxSandbox`. The VS Code task (`Cmd+Shift+B`) still works for quick iteration.
```

## Web Build (Emscripten)

**Prerequisites:**
- [Emscripten SDK](https://emscripten.org/docs/getting_started/downloads.html)
- CMake 3.15+

```bash
# Build (raylib is fetched automatically via CMake FetchContent)
make web
# Output → build-web/VoxSandbox.html
```

Serve locally to test:

```bash
cd build-web && python3 -m http.server 8000
# Open http://localhost:8000/VoxSandbox.html
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
