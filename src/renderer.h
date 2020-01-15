#pragma once

#include "map_with_tuple_keys.h"

#include <tuple>
#include <unordered_map>
#include <unordered_set>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/scalar_multiplication.hpp>

#include "camera.h"
#include "chunk.h"
#include "shader.h"

typedef std::tuple<int, int, int> xyz;

class Renderer {
  private:
    float xAxisVertices[6] {
      -512.0f, 0.0f, 0.0f,
      512.0f, 0.0f, 0.0f,
    };
    unsigned int xAxisVAO {};
    unsigned int xAxisVBO {};

    float yAxisVertices[6] {
      0.0f, -512.0f, 0.0f,
      0.0f, 512.0f, 0.0f
    };
    unsigned int yAxisVAO {};
    unsigned int yAxisVBO {};

    float zAxisVertices[6] {
      0.0f, 0.0f, -512.0f,
      0.0f, 0.0f, 512.0f,
    };
    unsigned int zAxisVAO {};
    unsigned int zAxisVBO {};

    Shader blockShader { Shader("./src/shaders/block.vert", "./src/shaders/block.frag") };
    Shader lineShader { Shader("./src/shaders/line.vert", "./src/shaders/line.frag") };

    bool wireMode { false };

    Camera camera { Camera() };
    glm::ivec3 lastCameraChunkPosition {};

    std::unordered_map<xyz, Chunk, hash_tuple::hash<xyz>> chunks {};
    int viewingDistance { 2 };
    std::unordered_set<xyz, hash_tuple::hash<xyz>> lastAreaOfInterest {};

    // FPS
    // https://stackoverflow.com/a/4687507
    int framesThisSecond { -1 };
    float currentFPS { 0 };
    float avgFPS { 60.0f };
    float fpsDecay { 0.9f };
    double lastSecond { glfwGetTime() };
    void calculateFPS();

    // Coordinate lines
    bool showCoordinateLines { true };
    void renderCoordinateLines();

    // Overlay
    bool showOverlay { true };
    void renderOverlay();
  public:
    Renderer();
    void render(double dt);
    void processInput(GLFWwindow* window, float dt);
    void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    void windowSizeCallback(GLFWwindow* window, int width, int height);
};
