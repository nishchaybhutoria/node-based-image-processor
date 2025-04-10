#include "InputNode.h"
#include <opencv2/imgcodecs.hpp>
#include <iostream>
#include "../utils/TextureUtils.h"
#include "imgui.h"

InputNode::InputNode(int id, const std::string& path, const std::string& name) : Node(id, name.empty() ? std::filesystem::path(path).filename().string() : name), filepath(path) {}

void InputNode::process() {
    image = cv::imread(filepath);
    if (image.empty()) {
        std::cerr << "Failed to load image: " << filepath << std::endl;
    }

    if (textureID) glDeleteTextures(1, &textureID); // cleanup old
    textureID = matToTexture(image);
}

void InputNode::renderPropertiesUI() {
    ImGui::Text("Input");

    if (image.empty()) {
        ImGui::Text("No input image yet.");
        return;
    } else {
        ImGui::Text("Input exists.");
    }
}

cv::Mat InputNode::getOutput() const {
    return image;
}