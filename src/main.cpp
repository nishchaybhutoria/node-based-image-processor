#include "imgui.h"
#include "imnodes.h"
#include "nodes/InputNode.h"
#include "nodes/OutputNode.h"
#include "nodes/BrightnessContrastNode.h"
#include <unordered_map>
#include <memory>

#include "../backends/imgui_impl_glfw.h"
#include "../backends/imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>
#include <iostream>

void glfw_error_callback(int error, const char* description) {
    std::cerr << "GLFW Error " << error << std::endl;
}

int main() {
    // Setup
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    bool fullscreen = true;

    GLFWmonitor* primary = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primary);

    GLFWwindow* window = glfwCreateWindow(
        fullscreen ? mode->width : 1280, 
        fullscreen ? mode->width : 720,
        "Node-Based Image Processor", 
        fullscreen ? primary : NULL, 
        NULL
    );
    
    if (!window) return -1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImNodes::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.Fonts->AddFontFromFileTTF("../assets/Inter_18pt-Regular.ttf", 16.0f);
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // 🔗 Shared node map
    std::unordered_map<int, std::shared_ptr<Node>> nodes;

    // 📥 Input
    auto inputNode = std::make_shared<InputNode>(1, "../assets/test.png");
    inputNode->process();
    nodes[1] = inputNode;

    // 🎚 Brightness/Contrast
    auto bcNode = std::make_shared<BrightnessContrastNode>(2);
    bcNode->setInput(inputNode->getOutput());
    bcNode->process();
    nodes[2] = bcNode;

    // 📤 Output
    auto outputNode = std::make_shared<OutputNode>(3);
    outputNode->setInput(bcNode->getOutput());
    outputNode->process();
    nodes[3] = outputNode;

    // Main Loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 🔁 Update pipeline before rendering
        bcNode->setInput(inputNode->getOutput());
        bcNode->process();

        outputNode->setInput(bcNode->getOutput());
        outputNode->process();

        // 🔲 Node Editor Window
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(io.DisplaySize);
        ImGui::Begin("Node Editor", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus);

        ImNodes::BeginNodeEditor();

        // 🧱 Input Node
        ImNodes::BeginNode(inputNode->id);
        ImGui::Text("%s", inputNode->name.c_str());
        if (GLuint tex = inputNode->getTextureID(); tex && glIsTexture(tex)) {
            ImGui::Image((ImTextureID)(intptr_t)tex, ImVec2(128, 128), ImVec2(1, 0), ImVec2(0, 1));
        }
        ImNodes::BeginOutputAttribute(inputNode->id + 100);
        ImGui::Text("Image Out");
        ImNodes::EndOutputAttribute();
        ImNodes::EndNode();

        // 🧱 Brightness/Contrast Node
        ImNodes::BeginNode(bcNode->id);
        ImGui::Text("%s", bcNode->name.c_str());
        bcNode->preview();
        ImNodes::BeginInputAttribute(bcNode->id + 100);
        ImGui::Text("In");
        ImNodes::EndInputAttribute();
        ImNodes::BeginOutputAttribute(bcNode->id + 200);
        ImGui::Text("Out");
        ImNodes::EndOutputAttribute();
        ImNodes::EndNode();

        // 🧱 Output Node
        ImNodes::BeginNode(outputNode->id);
        ImGui::Text("%s", outputNode->name.c_str());
        outputNode->preview();
        ImNodes::BeginInputAttribute(outputNode->id + 100);
        ImGui::Text("Final In");
        ImNodes::EndInputAttribute();
        ImNodes::EndNode();

        // 🔗 Links
        ImNodes::Link(1, inputNode->id + 100, bcNode->id + 100);
        ImNodes::Link(2, bcNode->id + 200, outputNode->id + 100);

        ImNodes::EndNodeEditor();

        // 🛠 Properties Panel
        ImGui::Begin("Properties");
        int selectedNodeId = -1;
        if (ImNodes::NumSelectedNodes() > 0) {
            ImNodes::GetSelectedNodes(&selectedNodeId);
            if (selectedNodeId != -1 && nodes.find(selectedNodeId) != nodes.end()) {
                // std::cout << "Rendering UI for node " << selectedNodeId << '\n';
                nodes[selectedNodeId]->renderPropertiesUI();
            }
            ImGui::Text("Node %d is selected.", selectedNodeId);
        } else {
            ImGui::Text("Select a node to view its properties.");
        }
        ImGui::End();
        
        ImGui::End();

        // 🔁 Render Frame
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
