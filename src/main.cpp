#include "imgui.h"
#include "imnodes.h"
#include "core/Graph.h"
#include "nodes/InputNode.h"
#include "nodes/OutputNode.h"
#include "nodes/BrightnessContrastNode.h"
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
        fullscreen ? mode->height : 720,
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

    Graph graph;

    auto inputNode = std::make_shared<InputNode>(0, "../assets/test.png"); // id will be overwritten anyway
    int inputId = graph.addNode(inputNode);
    inputNode->process();

    auto bcNode = std::make_shared<BrightnessContrastNode>(0); // name is optional
    int bcId = graph.addNode(bcNode);
    bcNode->setInput(inputNode->getOutput());
    bcNode->process();

    auto outputNode = std::make_shared<OutputNode>(0);
    int outId = graph.addNode(outputNode);
    outputNode->setInput(bcNode->getOutput());
    outputNode->process();

    // 🔗 Hardcoded links
    graph.addLink(inputId, inputId + 100, bcId, bcId + 100);
    graph.addLink(bcId, bcId + 200, outId, outId + 100);

    int selectedNodeId = -1;

    // Main Loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        graph.evaluate();

        // 🧠 Node Editor
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(io.DisplaySize);
        ImGui::Begin("Node Editor", nullptr,
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoNavFocus
        );

        ImNodes::BeginNodeEditor();

        // 🧱 Draw all nodes
        for (auto& [id, node] : graph.nodes) {
            ImNodes::BeginNode(id);
            ImGui::Text("%s", node->name.c_str());

            node->preview();

            if (dynamic_cast<InputNode*>(node.get())) {
                ImNodes::BeginOutputAttribute(id + 100);
                ImGui::Text("Output");
                ImNodes::EndOutputAttribute();
            } else if (dynamic_cast<OutputNode*>(node.get())) {
                ImNodes::BeginInputAttribute(id + 100);
                ImGui::Text("Input");
                ImNodes::EndInputAttribute();
            } else {
                ImNodes::BeginInputAttribute(id + 100);
                ImGui::Text("In");
                ImNodes::EndInputAttribute();
                ImNodes::BeginOutputAttribute(id + 200);
                ImGui::Text("Out");
                ImNodes::EndOutputAttribute();
            }

            ImNodes::EndNode();
        }

        // 🔗 Draw links
        for (const auto& link : graph.links) {
            ImNodes::Link(link.id, link.fromAttr, link.toAttr);
        }

        ImNodes::EndNodeEditor();
        ImGui::End(); // Node Editor

        // 🛠 Properties
        ImGui::Begin("Properties");
        int tempSelected = -1;
        if (ImNodes::NumSelectedNodes() > 0) {
            ImNodes::GetSelectedNodes(&tempSelected);
            if (tempSelected != -1) {
                selectedNodeId = tempSelected;
            }
        }

        if (selectedNodeId != -1 && graph.nodes.count(selectedNodeId)) {
            graph.nodes[selectedNodeId]->renderPropertiesUI();
        } else {
            ImGui::Text("Select a node to view its properties.");
        }
        ImGui::End();

        // 🎨 Render Frame
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
