#include "imgui.h"
#include "imnodes.h"
#include "nodes/InputNode.h"
#include "nodes/OutputNode.h"

#include "../backends/imgui_impl_glfw.h"
#include "../backends/imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>
#include <iostream>

void glfw_error_callback(int error, const char* description) {
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

int main() {
    // Setup GLFW
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Node-Based Image App", NULL, NULL);
    if (!window) return -1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui and ImNodes
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImNodes::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    std::string imgPath = "../assets/test.png"; // Update if needed
    InputNode inputNode(1, imgPath);
    inputNode.process(); // Load and create texture

    OutputNode outputNode(2, "Output.png");
    outputNode.setInput(inputNode.getOutput()); 

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Node Editor Window
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
        ImGui::Begin("Node Editor", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

        ImNodes::BeginNodeEditor();

        ImNodes::BeginNode(inputNode.id);
        ImGui::Text("%s", inputNode.name.c_str());

        GLuint tex = inputNode.getTextureID();
        if (tex != 0 && glIsTexture(tex)) {
            ImGui::Image(
                (ImTextureID)(intptr_t)inputNode.getTextureID(),
                ImVec2(128, 128),
                ImVec2(1, 0), ImVec2(0, 1)
            );
        }

        ImNodes::BeginOutputAttribute(inputNode.id + 100);
        ImGui::Text("Input");
        ImNodes::EndOutputAttribute();

        ImNodes::EndNode();

        ImNodes::BeginNode(outputNode.id);
        ImGui::Text("%s", outputNode.name.c_str());

        tex = outputNode.getTextureID();
        if (tex != 0 && glIsTexture(tex)) {
            ImGui::Image(
                (ImTextureID)(intptr_t)inputNode.getTextureID(),
                ImVec2(128, 128),
                ImVec2(0, 1), ImVec2(1, 0)
            );
        }

        ImNodes::BeginInputAttribute(outputNode.id + 100);
        ImGui::Text("Output");
        ImNodes::EndOutputAttribute();

        ImNodes::EndNode();

        ImNodes::Link(1, inputNode.id + 100, outputNode.id + 100);

        ImNodes::EndNodeEditor();
        ImGui::End();

        // Render
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImNodes::DestroyContext();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
