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
    // ImNodes::EnableLinkDetachmentWithModifier(ImGuiMod_Alt);
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.Fonts->AddFontFromFileTTF("../assets/Inter_18pt-Regular.ttf", 16.0f);
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    Graph graph;

    auto inputNode = std::make_shared<InputNode>(0);
    int inputId = graph.addNode(inputNode);

    auto bcNode = std::make_shared<BrightnessContrastNode>(0);
    int bcId = graph.addNode(bcNode);

    auto outputNode = std::make_shared<OutputNode>(0);
    int outId = graph.addNode(outputNode);

    auto aNode = std::make_shared<BrightnessContrastNode>(0);
    int aId = graph.addNode(aNode);

    auto bNode = std::make_shared<BrightnessContrastNode>(0);
    int bId = graph.addNode(bNode);

    auto cNode = std::make_shared<BrightnessContrastNode>(0);
    int cId = graph.addNode(cNode);


    // Connect input -> brightness/contrast, and brightness/contrast -> output
    // graph.addLink(inputId, 0, bcId, 0);
    // graph.addLink(bcId, 0, outId, 0);

    int selectedNodeId = -1;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        graph.evaluate();

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

        if (graph.hasCycle) {
            ImGui::TextColored(ImVec4(1, 0.3f, 0.3f, 1), "Graph contains a cycle. Evaluation halted.");
        }

        ImNodes::BeginNodeEditor();

        for (auto& [id, node] : graph.nodes) {
            ImNodes::BeginNode(id);
            ImGui::Text("%s", node->name.c_str());

            node->preview();

            if (dynamic_cast<InputNode*>(node.get())) {
                ImNodes::BeginOutputAttribute(id * 1000 + 0);
                ImGui::Text("Output");
                ImNodes::EndOutputAttribute();
            } else if (dynamic_cast<OutputNode*>(node.get())) {
                ImNodes::BeginInputAttribute(id * 1000 + 0);
                ImGui::Text("Input");
                ImNodes::EndInputAttribute();
            } else {
                ImNodes::BeginInputAttribute(id * 1000 + 0);
                ImGui::Text("In");
                ImNodes::EndInputAttribute();
                ImNodes::BeginOutputAttribute(id * 1000 + 1);
                ImGui::Text("Out");
                ImNodes::EndOutputAttribute();
            }

            ImNodes::EndNode();
        }

        for (const auto& link : graph.links) {
            ImNodes::Link(link.id, link.fromAttr, link.toAttr);
        }

        ImNodes::EndNodeEditor();

        int hoveredLinkId;
        if (ImNodes::IsLinkHovered(&hoveredLinkId)) {

            ImGui::BeginTooltip();
            ImGui::Text("ALT + Click to delete link");
            ImGui::EndTooltip();


            if (ImGui::IsKeyDown(ImGuiKey_LeftAlt) && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                graph.removeLink(hoveredLinkId);
            }
        }

        int start_attr, end_attr;
        if (ImNodes::IsLinkCreated(&start_attr, &end_attr)) {
            int fromAttr = start_attr, toAttr = end_attr;
            if (fromAttr % 1000 < toAttr % 1000) std::swap(fromAttr, toAttr);

            int fromNode = fromAttr / 1000;
            int toNode = toAttr / 1000;

            bool addLink = true;

            if (fromNode == toNode) {
                std::cerr << "Self-links are not allowed\n";
                addLink = false;
            }
            
            for (const auto& link : graph.links) {
                if (link.fromAttr == fromAttr && link.toAttr == toAttr) {
                    std::cerr << "Duplicate link ignored\n";
                    addLink = false;
                }
            }
            
            int inputCount = 0; // assumes input is <= 1, change for blend node
            for (const auto& link : graph.getInputLinks(toNode)) {
                if (link.toAttr == toAttr) {
                    ++inputCount;
                }
            }
            if (inputCount > 0) {
                std::cerr << "Only one input allowed for this node\n";
                addLink = false;
            }
            
            if (addLink) {
                graph.addLink(fromNode, fromAttr % 1000, toNode, toAttr % 1000);
            }
        }

        int deletedLinkId;
        if (ImNodes::IsLinkDestroyed(&deletedLinkId)) {
            graph.removeLink(deletedLinkId);
        }

        ImGui::End();

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

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    ImNodes::DestroyContext();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
