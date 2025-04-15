#include "InputNode.h"
#include <opencv2/imgcodecs.hpp>
#include <iostream>
#include "../utils/TextureUtils.h"
#include "imgui.h"

InputNode::InputNode(int id, const std::string& defaultPath, const std::string& name) : Node(id, name), filepath(defaultPath) {}

void InputNode::process() {
    if (filepath.empty()) return;

    image = cv::imread(filepath);
    if (image.empty()) {
        std::cerr << "Failed to load image: " << filepath << std::endl;
        return;
    }

    if (textureID) glDeleteTextures(1, &textureID);
    textureID = matToTexture(image);
}

void InputNode::preview() {
    if (image.empty()) {
        ImGui::Text("No preview");
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

void InputNode::renderPropertiesUI() {
    ImGui::Text("Image Input");

    static char buf[256];
    strncpy(buf, filepath.c_str(), sizeof(buf));
    static std::string pendingPath;

    if (ImGui::InputText("Filepath", buf, IM_ARRAYSIZE(buf))) {
        pendingPath = std::string(buf);
    }

    if (ImGui::Button("Load")) {
        filepath = pendingPath;
        process();
    }

    if (!image.empty()) {
        ImGui::Separator();
        ImGui::Text("Metadata:");
        ImGui::Text("Dimensions: %d x %d", image.cols, image.rows);
        ImGui::Text("Channels: %d", image.channels());

        try {
            auto fsize = std::filesystem::file_size(filepath);
            ImGui::Text("File size: %.2f KB", fsize / 1024.0f);
        } catch (...) {
            ImGui::Text("File size: Unknown");
        }
    } else {
        ImGui::Text("No image loaded.");
    }
}


cv::Mat InputNode::getOutput() const {
    return image;
}