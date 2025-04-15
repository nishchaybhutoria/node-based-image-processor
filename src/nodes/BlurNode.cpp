#include "BlurNode.h"
#include "../utils/TextureUtils.h"
#include "imgui.h"
#include <iostream>

BlurNode::BlurNode(int id, const std::string& name) : Node(id, name) {}

void BlurNode::setInputs(const std::vector<cv::Mat>& input) {
    if (!input.empty()) {
        inputImage = input[0].clone();
    } else {
        inputImage.release();
    }
}

void BlurNode::process() {
    if (inputImage.empty()) {
        outputImage.release();
        if (textureID) {
            glDeleteTextures(1, &textureID);
            textureID = 0;
        }
        return;
    }

    int ksize = blurRadius * 2 + 1;
    if (directional) {
        cv::GaussianBlur(inputImage, outputImage, cv::Size(ksize, 1), 0);
    } else {
        cv::GaussianBlur(inputImage, outputImage, cv::Size(ksize, ksize), 0);
    }
}

cv::Mat BlurNode::getOutput() const {
    return outputImage;
}

void BlurNode::preview() {
    if (outputImage.empty()) {
        ImGui::Text("No output");
        return;
    }

    if (textureID) glDeleteTextures(1, &textureID);
    textureID = matToTexture(outputImage);

    if (textureID && glIsTexture(textureID)) {
        ImGui::Text("Preview:");
        ImGui::Image((ImTextureID)(intptr_t)textureID, ImVec2(128, 128), ImVec2(1, 0), ImVec2(0, 1));
    }
}

void BlurNode::renderPropertiesUI() {
    ImGui::Text("Blur Settings");

    bool updated = false;

    updated |= ImGui::SliderInt("Radius", &blurRadius, 1, 20);
    updated |= ImGui::Checkbox("Directional (Horizontal Only)", &directional);

    if (ImGui::Button("Reset")) {
        blurRadius = 5;
        directional = false;
        updated = true;
    }

    if (updated) {
        process();
    }
}
