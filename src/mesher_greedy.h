#pragma once

#include <vector>

#include "chunk.h"
#include "quad.h"
#include "side.h"

class MesherGreedy {
  public:
    static std::vector<float> computeChunkMesh(const Chunk &chunk);

  private:
    static void loadQuadIntoMesh(const quad& quad, const BlockType& blockType, const Side& side, std::vector<float>& mesh);
};
