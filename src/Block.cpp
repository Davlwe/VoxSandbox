#include "Block.h"
#include <cstdlib>   // rand

// ---------------------------------------------------------------------------
// Master block list — add new block types here
// ---------------------------------------------------------------------------
const BlockDef BLOCK_DEFS[] = {
    { BlockType::Air,   "Air",   BLANK,                        nullptr },
    { BlockType::Grass, "Grass", { 34,  139, 34,  255 }, "assets/textures/grass.png" },
    { BlockType::Dirt,  "Dirt",  { 139, 90,  43,  255 }, "assets/textures/dirt.png"  },
    { BlockType::Stone, "Stone", { 128, 128, 128, 255 }, "assets/textures/stone.png" },
    { BlockType::Wood,  "Wood",  { 160, 82,  45,  255 }, "assets/textures/wood.png"  },
    { BlockType::Leaves, "Leaves", { 34, 139, 34,  255 }, "assets/textures/leaves.png" },
};
const int BLOCK_DEF_COUNT = sizeof(BLOCK_DEFS) / sizeof(BLOCK_DEFS[0]);

// Per-block textured models + shared cube mesh (indexed by BlockType)
static Model  s_Models[BLOCK_DEF_COUNT];
static Mesh   s_CubeMesh;
static bool   s_Initialized = false;

// ---------------------------------------------------------------------------
// Procedural 16×16 textures (used when PNG files aren't found)
// ---------------------------------------------------------------------------

static Image GenGrassTexture() {
    Image img = GenImageColor(16, 16, Color{ 34, 139, 34, 255 });  // green base
    // Brown strip at the bottom (dirt edge)
    for (int y = 12; y < 16; ++y)
        for (int x = 0; x < 16; ++x)
            ImageDrawPixel(&img, x, y, Color{ 101, 67, 33, 255 });
    // Slightly darker patches for variation
    for (int i = 0; i < 20; ++i) {
        int x = rand() % 16, y = rand() % 10;
        ImageDrawPixel(&img, x, y, Color{ 20, 100, 20, 255 });
    }
    return img;
}

static Image GenDirtTexture() {
    Image img = GenImageColor(16, 16, Color{ 139, 90, 43, 255 });  // brown base
    for (int i = 0; i < 40; ++i) {
        int x = rand() % 16, y = rand() % 16;
        int shade = 80 + rand() % 40;
        ImageDrawPixel(&img, x, y, Color{ (unsigned char)shade, (unsigned char)(shade/2), 20, 255 });
    }
    return img;
}

static Image GenStoneTexture() {
    Image img = GenImageColor(16, 16, Color{ 128, 128, 128, 255 });  // gray base
    for (int i = 0; i < 30; ++i) {
        int x = rand() % 16, y = rand() % 16;
        int shade = 100 + rand() % 56;
        ImageDrawPixel(&img, x, y, Color{ (unsigned char)shade, (unsigned char)shade, (unsigned char)shade, 255 });
    }
    // Few darker "cracks"
    for (int i = 0; i < 8; ++i) {
        int x = rand() % 16, y = rand() % 16;
        ImageDrawPixel(&img, x, y, Color{ 60, 60, 60, 255 });
    }
    return img;
}

static Image GenWoodTexture() {
    // Plank base color
    Image img = GenImageColor(16, 16, Color{ 160, 120, 60, 255 });
    // Horizontal grain lines
    for (int y = 0; y < 16; ++y) {
        int shade = 140 + (y % 4) * 10;
        for (int x = 0; x < 16; ++x) {
            unsigned char r = (unsigned char)(shade);
            unsigned char g = (unsigned char)(shade * 3 / 4);
            unsigned char b = 40;
            ImageDrawPixel(&img, x, y, Color{ r, g, b, 255 });
        }
    }
    // Dark plank-separator lines
    for (int y = 3; y < 16; y += 4)
        for (int x = 0; x < 16; ++x)
            ImageDrawPixel(&img, x, y, Color{ 80, 50, 20, 255 });
    return img;
}

static Image GenLeavesTexture() {
    // Leaves: green base with dappled light/dark patches and small gaps
    Image img = GenImageColor(16, 16, Color{ 34, 139, 34, 255 });

    // Scattered lighter patches (sunlit leaves)
    for (int i = 0; i < 40; ++i) {
        int x = rand() % 16;
        int y = rand() % 16;
        int shade = 100 + rand() % 80;
        ImageDrawPixel(&img, x, y, Color{
            (unsigned char)(shade / 2),
            (unsigned char)shade,
            (unsigned char)(shade / 3),
            255 });
    }

    // A few darker spots (shadow gaps between leaves)
    for (int i = 0; i < 15; ++i) {
        int x = rand() % 16;
        int y = rand() % 16;
        ImageDrawPixel(&img, x, y, Color{ 15, 60, 15, 255 });
    }

    // Tiny sky-visible gaps (semi-transparent / lighter)
    for (int i = 0; i < 8; ++i) {
        int x = rand() % 16;
        int y = rand() % 16;
        ImageDrawPixel(&img, x, y, Color{ 180, 220, 180, 255 });
    }

    return img;
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

void InitBlocks() {
    if (s_Initialized) return;
    s_Initialized = true;

    // One shared cube mesh for all block models
    s_CubeMesh = GenMeshCube(1.0f, 1.0f, 1.0f);

    for (int i = 0; i < BLOCK_DEF_COUNT; ++i) {
        const BlockDef& def = BLOCK_DEFS[i];
        s_Models[i] = { 0 };   // default: empty model

        Image img = { 0 };

        // 1 — Try loading texture file
        if (def.textureFile && def.textureFile[0] != '\0') {
            img = LoadImage(def.textureFile);
        }

        // 2 — Fall back to procedural generation
        if (img.data == nullptr) {
            switch (def.type) {
                case BlockType::Grass: img = GenGrassTexture(); break;
                case BlockType::Dirt:  img = GenDirtTexture();  break;
                case BlockType::Stone: img = GenStoneTexture(); break;
                case BlockType::Wood:  img = GenWoodTexture();  break;
                case BlockType::Leaves: img = GenLeavesTexture(); break;
                default: break;
            }
        }

        // 3 — Build a textured model from the image
        if (img.data != nullptr) {
            Texture2D tex = LoadTextureFromImage(img);
            UnloadImage(img);

            s_Models[i] = LoadModelFromMesh(s_CubeMesh);
            s_Models[i].materials[0].maps[MATERIAL_MAP_ALBEDO].texture = tex;
        }
    }
}

void UnloadBlocks() {
    if (!s_Initialized) return;
    s_Initialized = false;

    for (int i = 0; i < BLOCK_DEF_COUNT; ++i) {
        if (s_Models[i].meshes != nullptr) {
            // Unload the texture from the material first
            Texture2D tex = s_Models[i].materials[0].maps[MATERIAL_MAP_ALBEDO].texture;
            if (tex.id != 0) UnloadTexture(tex);
            UnloadModel(s_Models[i]);
            s_Models[i] = { 0 };
        }
    }
    // Don't unload s_CubeMesh — it was transferred to each model via LoadModelFromMesh
}

Color GetBlockColor(BlockType type) {
    int idx = (int)type;
    if (idx >= 0 && idx < BLOCK_DEF_COUNT)
        return BLOCK_DEFS[idx].fallbackColor;
    return BLANK;
}

Model GetBlockModel(BlockType type) {
    int idx = (int)type;
    if (idx >= 0 && idx < BLOCK_DEF_COUNT)
        return s_Models[idx];
    return { 0 };
}

bool BlockHasModel(BlockType type) {
    int idx = (int)type;
    return idx >= 0 && idx < BLOCK_DEF_COUNT && s_Models[idx].meshes != nullptr;
}
