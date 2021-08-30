// check the biome at a block position
#include "finders.h"
#include <stdlib.h>

struct Condition {
    int structureType; // What Structure type to search for (eg. Village)
    uint32_t regionX; // What region file to look in (not user specified)
    uint32_t regionZ;
    uint32_t maxRadius; // What radius in chunks the structure can be away max.
    uint32_t chunkX; // Ideal chunk for the structure
    uint32_t chunkZ;
};

uint32_t distance(uint32_t py, uint32_t px, uint32_t jy, uint32_t jx) {
    return sqrtf((px - jx) * (px - jx) + (py - jy) * (py - jy));
}

int main(int argc, char** argv) { 
    /* Program Init */
    char *stopstring;
    uint64_t startingSeed = strtoull(argv[1], &stopstring, 10);
    uint64_t stoppingSeed = strtoull(argv[2], &stopstring, 10);
    uint64_t minecraftVersion = strtoull(argv[3], &stopstring, 10);
    printf("Starting Seed: %lu\nStopping Seed: %lu\nMinecraft Version: 1.%lu\n\nStarting now!\n\n", startingSeed, stoppingSeed, minecraftVersion);

    /* Searching Init */
    uint64_t conditionCount = strtoull(argv[4], &stopstring, 10);
    struct Condition conditions[conditionCount];
    for (uint64_t i = 0; i < conditionCount; i++) {
        int _type = strtoull(argv[5+i*4], &stopstring, 10);
        uint32_t _x  = strtoull(argv[5+i*4+1], &stopstring, 10);
        uint32_t _z = strtoull(argv[5+i*4+2], &stopstring, 10);
        uint32_t _radius = strtoull(argv[5+i*4+3], &stopstring, 10);
        uint32_t _regX = floor(_x / 32);
        uint32_t _regZ = floor(_z / 32);
        struct Condition c;
        c.structureType = _type;
        c.regionX = _regX;
        c.regionZ = _regZ;
        c.maxRadius = _radius;
        c.chunkX = _x;
        c.chunkZ = _z;
        conditions[i] = c;
        printf("Searching for structure %d at %d, %d (reg: %d, %d) with a max distance of %d\n", _type, _x, _z, _regX, _regZ, _radius);
    }
    printf("\n");

    /* Cubiomes Init */
    LayerStack g;
    Pos p;
    setupGenerator(&g, minecraftVersion);

    uint64_t successes = 0;

    /* Seedfinding with Cubiomes... */
    for (uint64_t structureSeed = startingSeed; structureSeed < stoppingSeed; structureSeed++) {
        char success = 1; // whether the structure seed succeeded


        for (size_t i = 0; i < conditionCount; i++) { // check conditions
            struct Condition c = conditions[i];
            // check if existant
            if (!getStructurePos(c.structureType, minecraftVersion, structureSeed, c.regionX, c.regionZ, &p)) {
                success = 0;
                break;
            }
            // check position
            if (distance((int) floor(p.x/16), (int) floor(p.z/16), c.chunkX, c.chunkZ) > c.maxRadius) {
                success = 0;
                break;
            }
        }

        if (success == 0) continue;
        //printf("Structure found: %lu\n", structureSeed);

        for (uint64_t biomeSeed = 0; biomeSeed < 65536; biomeSeed++) {
            uint64_t seed = structureSeed | (biomeSeed << 48);

            char success2 = 1; // whether the biome seed succeeded

            for (size_t i = 0; i < conditionCount; i++) { // check conditions
                struct Condition c = conditions[i];
                // check if it can spawn
                if (!isViableStructurePos(c.structureType, minecraftVersion, &g, seed, p.x, p.z)) {
                    success2 = 0;
                    break;
                }
            }

            if (success2 == 0) continue;
            successes++;

            //printf("Seed found: %lu (%lu | %lu)\n", seed, structureSeed, biomeSeed);
        }
    }
    printf("%lu seeds found!\n", successes);
    return 0;
}