#include "BrightnessContrastNode.h"
#include <opencv2/imgcodecs.hpp>
#include <iostream>
#include "../utils/TextureUtils.h"
#include "imgui.h"
#include <vector>

BrightnessContrastNode::BrightnessContrastNode(int id, const std::string& name) : Node(id, name) {}

void BrightnessContrastNode::process() {
    if (inputImage.empty()) { // if there's no input
        // std::cerr << "BrightnessContrastNode: No input image.\n";
        return;
    }
    outputImage = inputImage.clone();
    outputImage.convertTo(outputImage, -1, contrast, brightness);
}

void BrightnessContrastNode::preview() {
    if (inputImage.empty()) {
        ImGui::Text("No input");
        return;
    }

    if (textureID) glDeleteTextures(1, &textureID); // cleanup old
    textureID = matToTexture(outputImage);
    
    if (textureID && glIsTexture(textureID)) {
        ImGui::Text("Preview:");
        ImGui::Image(
            (ImTextureID)(intptr_t)textureID,
            ImVec2(128, 128),
            ImVec2(1, 0), ImVec2(0, 1)
        );
    }
}

void BrightnessContrastNode::renderPropertiesUI() {
    ImGui::Text("Brightness/Contrast");

    if (inputImage.empty()) {
        ImGui::Text("No input image yet.");
        return;
    }

    bool updated = false;

    updated |= ImGui::SliderFloat("Brightness", &brightness, -100.0f, 100.0f);
    ImGui::SameLine();
    if (ImGui::Button("Reset##Brightness")) {
        brightness = 0.0f;
        updated = true;
    }

    updated |= ImGui::SliderFloat("Contrast", &contrast, 0.0f, 3.0f);
    ImGui::SameLine();
    if (ImGui::Button("Reset##Contrast")) {
        contrast = 1.0f;
        updated = true;
    }

    if (updated) {
        process();
    }
}

void BrightnessContrastNode::setInputs(const std::vector<cv::Mat>& input) {
    if (!input.empty()) {
        inputImage = input[0].clone();
    } else {
        inputImage.release();
    }
}

cv::Mat BrightnessContrastNode::getOutput() const {
    return outputImage;
}