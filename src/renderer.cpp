#include <algorithm>
#include <iostream>

#include <imgui/imgui.h>
#include <stb/stb_image.h>

#include "memory_helper.h"
#include "renderer.h"
#include "shader.h"
#include "texture.h"

Renderer::Renderer() {
  std::cout << "Renderer created" << std::endl;

  stbi_set_flip_vertically_on_load(true);

  glEnable(GL_DEPTH_TEST);

  // Axes
  glGenVertexArrays(1, &xAxisVAO);
  glGenBuffers(1, &xAxisVBO);

  glGenVertexArrays(1, &yAxisVAO);
  glGenBuffers(1, &yAxisVBO);

  glGenVertexArrays(1, &zAxisVAO);
  glGenBuffers(1, &zAxisVBO);

  glActiveTexture(GL_TEXTURE0);
  Texture containerTexture("./assets/dirt.jpg");

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void Renderer::render(double dt) {
  MemoryHelper::print_mem_usage_if_changed();

  glClearColor(0.2f * dt, 0.3f, 0.3f, 0.1f); // Use dt here for now to get rid of the warning while I fill out the rest of the missing code. It's a nice color.
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  blockShader.use();
  blockShader.setInt("myTexture", 0);
  blockShader.setMat4("view", camera.getViewMatrix());
  blockShader.setMat4("projection", camera.getProjectionMatrix());

  glm::ivec3 cameraChunkPosition {};

  if (camera.position.x > 0) {
    cameraChunkPosition.x = floor((camera.position.x + Chunk::SIZE / 2) / Chunk::SIZE);
  } else {
    cameraChunkPosition.x = ceil((camera.position.x - Chunk::SIZE / 2) / Chunk::SIZE);
  }

  if (camera.position.z > 0) {
    cameraChunkPosition.z = floor((camera.position.z + Chunk::SIZE / 2) / Chunk::SIZE);
  } else {
    cameraChunkPosition.z = ceil((camera.position.z - Chunk::SIZE / 2) / Chunk::SIZE);
  }

  cameraChunkPosition.y = floor((camera.position.y) / Chunk::SIZE);

  std::unordered_set<xyz, hash_tuple::hash<xyz>> areaOfInterest {};

  for (auto ix = cameraChunkPosition.x - viewingDistance; ix <= cameraChunkPosition.x + viewingDistance; ix++) {
    for (auto iz = cameraChunkPosition.z - viewingDistance; iz <= cameraChunkPosition.z + viewingDistance; iz++) {
      for (auto iy = std::max(0, cameraChunkPosition.y - viewingDistance); iy <= cameraChunkPosition.y + viewingDistance; iy++) {
        xyz key = std::make_tuple(ix, iy, iz);

        areaOfInterest.insert(key);

        if (chunks.find(key) == chunks.end()) {
          Chunk &chunk = chunks.try_emplace(key, glm::vec3(ix, iy, iz)).first->second;
          chunk.render(blockShader);
        } else {
          Chunk &chunk = chunks.find(key)->second;
          chunk.render(blockShader);
        }
      }
    }
  }

  if (cameraChunkPosition != lastCameraChunkPosition) {
    std::cout << "\nChunk changed!!" << std::endl;
    std::cout << cameraChunkPosition.x << " " << cameraChunkPosition.y << " " << cameraChunkPosition.z << std::endl;

    // If the current AoI doesn't have a previous AoI key, then it is outside
    // of the AoI and we want to unload it.
    for (xyz oldKey : lastAreaOfInterest) {
      if (areaOfInterest.find(oldKey) == areaOfInterest.end()) {
        chunks.erase(oldKey);
      }
    }
    std::cout << "Total number of chunks loaded: " << chunks.size() << std::endl;
  }

  // Axes stuff
  glm::mat4 axisModel = glm::mat4(1.0f);
  lineShader.use();
  lineShader.setMat4("view", camera.getViewMatrix());
  lineShader.setMat4("projection", camera.getProjectionMatrix());
  lineShader.setMat4("model", axisModel);

  glBindVertexArray(xAxisVAO);
  glBindBuffer(GL_ARRAY_BUFFER, xAxisVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(xAxisVertices), xAxisVertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  lineShader.setVec3("color", glm::vec3(1.0f, 0.0f, 0.0f));
  glDrawArrays(GL_LINES, 0, 2);
  glDrawArrays(GL_POINTS, 0, 2);

  glBindVertexArray(yAxisVAO);
  glBindBuffer(GL_ARRAY_BUFFER, yAxisVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(yAxisVertices), yAxisVertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  lineShader.setVec3("color", glm::vec3(0.0f, 1.0f, 0.0f));
  glDrawArrays(GL_LINES, 0, 2);
  glDrawArrays(GL_POINTS, 0, 2);

  glBindVertexArray(zAxisVAO);
  glBindBuffer(GL_ARRAY_BUFFER, zAxisVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(zAxisVertices), zAxisVertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  lineShader.setVec3("color", glm::vec3(0.0f, 0.0f, 1.0f));
  glDrawArrays(GL_LINES, 0, 2);
  glDrawArrays(GL_POINTS, 0, 2);

  lastCameraChunkPosition = cameraChunkPosition;
  lastAreaOfInterest = areaOfInterest;

  // Do this at the end so that we have the most up-to-date info for this frame.
  showOverlay();
}

void Renderer::processInput(GLFWwindow* window, float dt) {
  camera.processInput(window, dt);
}

void Renderer::cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
  camera.cursorPosCallback(window, xpos, ypos);
}

void Renderer::windowSizeCallback(GLFWwindow* window, int width, int height) {
  camera.windowSizeCallback(window, width, height);
}

void Renderer::showOverlay() {
  ImGuiIO& io = ImGui::GetIO();

  const float DISTANCE = 10.0f;
  ImVec2 window_pos = ImVec2(DISTANCE, DISTANCE);
  ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always);
  ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background

  const auto flags =
    ImGuiWindowFlags_NoMove |
    ImGuiWindowFlags_NoDecoration |
    ImGuiWindowFlags_AlwaysAutoResize |
    ImGuiWindowFlags_NoSavedSettings |
    ImGuiWindowFlags_NoFocusOnAppearing |
    ImGuiWindowFlags_NoNav;

  if (ImGui::Begin("Info", &isOverlayOpen, flags)) {
    ImGui::Text("Debug Info");

    ImGui::Separator();

    ImGui::Text("Mouse:");
    if (ImGui::IsMousePosValid()) {
      ImGui::Text("(%.1f,%.1f)", io.MousePos.x, io.MousePos.y);
    } else {
      ImGui::Text("<invalid>");
    }
  }
  ImGui::End();
}
