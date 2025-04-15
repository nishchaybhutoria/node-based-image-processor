#include "OutputNode.h"
#include <opencv2/imgcodecs.hpp>
#include <iostream>
#include "../utils/TextureUtils.h"
#include "imgui.h"

OutputNode::OutputNode(int id, const std::string& name) : Node(id, name) {}

void OutputNode::process() {
    if (image.empty()) {
        std::cerr << "OutputNode: no input image\n";
        return;
    }
    
    if (textureID) glDeleteTextures(1, &textureID); // cleanup old
    textureID = matToTexture(image);
}

void OutputNode::setInput(const cv::Mat& input) {
    image = input.clone();

    if (textureID) glDeleteTextures(1, &textureID); // cleanup old
    textureID = matToTexture(image);
}

void OutputNode::preview() {
    if (image.empty()) {
        ImGui::Text("No input");
        return;
    }

    if (textureID && glIsTexture(textureID)) {
        ImGui::Text("Preview:");
        ImGui::Image(
            (ImTextureID)(intptr_t)textureID,
            ImVec2(128, 128),
            ImVec2(1, 0), ImVec2(0, 1)
        );
    }
}

void OutputNode::renderPropertiesUI() {
    ImGui::Text("Output");

    if (image.empty()) {
        ImGui::Text("No image yet.");
        return;
    } else {
        ImGui::Text("Image exists.");
    }
}

cv::Mat OutputNode::getOutput() const {
    return image;
}