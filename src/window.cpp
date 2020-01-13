#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "renderer.h"
#include "window.h"

void errorCallback(int error, const char* description) {
  std::cerr << "Error: " << error << ", Description: " << description << std::endl;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  // std::cout
    // << "key: " << key
    // << " scancode: " << scancode
    // << " action: " << action
    // << " mods: " << mods << std::endl;
  if (action == GLFW_PRESS) {
    switch (key) {
      case GLFW_KEY_ESCAPE:
      case GLFW_KEY_Q:
        glfwSetWindowShouldClose(window, 1);
        break;
      case GLFW_KEY_M:
        if (Window::isFocusedInGame(window)) {
          Window::focusInGUI(window);
        } else {
          Window::focusInGame(window);
        }
        break;
    }
  }
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
  // std::cout << "xoffset: " << xoffset << " yoffset: " << yoffset << std::endl;
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
  if (Window::isFocusedInGame(window)) {
    Renderer* rendererPtr = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
    rendererPtr->mouseCallback(window, xpos, ypos);
  }
  // std::cout << "xpos: " << xpos << " ypos: " << ypos << std::endl;
}
#pragma GCC diagnostic pop

Window::Window() {
  std::cout << "Window created" << std::endl;
}

int Window::show() {
  glfwSetErrorCallback(errorCallback);

  glfwInit();
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(Window::WIDTH, Window::HEIGHT, "Mutiny :)", NULL, NULL);
  if (window == NULL) {
    std::cerr << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  glfwSetCursorPosCallback(window, mouseCallback);
  glfwSetKeyCallback(window, keyCallback);
  glfwSetScrollCallback(window, scrollCallback);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  const char* glsl_version = "#version 130";
  ImGui_ImplOpenGL3_Init(glsl_version);
  bool show_demo_window = true;

  glViewport(0, 0, Window::WIDTH, Window::HEIGHT);
  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

  focusInGame(window);

  Renderer renderer;
  glfwSetWindowUserPointer(window, &renderer);

  glfwSetTime(0.0);
  double lastTime = 0.0;
  while(!glfwWindowShouldClose(window)) {
    double time = glfwGetTime();
    double dt = time - lastTime;
    lastTime = time;

    glfwPollEvents();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Show a demo window
    ImGui::ShowDemoWindow(&show_demo_window);

    if (isFocusedInGame(window)) {
      renderer.processInput(window, dt);
    }
    renderer.render(dt);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwTerminate();
  return 0;
}

bool Window::isFocusedInGame(GLFWwindow* window) {
  // We only want to process in-game input when we are actually "in the game".
  //
  // Now that we have a GUI we want to interact with, we need to know whether
  // we are focused in game or focused in the gui, and only do input for one or
  // the other.
  int cursorMode = glfwGetInputMode(window, GLFW_CURSOR);
  return cursorMode == GLFW_CURSOR_DISABLED;
}

bool Window::isFocusedInGUI(GLFWwindow* window) {
  return !isFocusedInGame(window);
}

void Window::focusInGame(GLFWwindow* window) {
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Hide cursor

  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NoMouse;            // Disable Mouse
  io.ConfigFlags &= ~ImGuiConfigFlags_NavEnableKeyboard; // Disable Keyboard
}

void Window::focusInGUI(GLFWwindow* window) {
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); // Show cursor

  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;          // Enable Mouse
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard
}
