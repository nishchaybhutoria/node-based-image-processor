#include "OutputNode.h"
#include <opencv2/imgcodecs.hpp>
#include <iostream>
#include "../utils/TextureUtils.h"
#include "imgui.h"
#include <vector>

OutputNode::OutputNode(int id, const std::string& name) : Node(id, name) {}

void OutputNode::process() {
    if (image.empty()) {
        std::cerr << "OutputNode: no input image\n";
        return;
    }
    
    if (textureID) glDeleteTextures(1, &textureID); // cleanup old
    textureID = matToTexture(image);
}

void OutputNode::setInputs(const std::vector<cv::Mat>& input) {
    if (!input.empty()) {
        image = input[0].clone();
    } else {
        image.release();
    }

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

void OutputNode::saveImage() {
    if (image.empty()) {
        std::cerr << "Cannot save: no image available\n";
        return;
    }

    std::vector<int> params;
    std::string fullFilename = filename;

    if (format == "JPG") {
        params.push_back(cv::IMWRITE_JPEG_QUALITY);
        params.push_back(jpgQuality);
        if (filename.find(".jpg") == std::string::npos) fullFilename += ".jpg";
    } else if (format == "PNG") {
        if (filename.find(".png") == std::string::npos) fullFilename += ".png";
    } else if (format == "BMP") {
        if (filename.find(".bmp") == std::string::npos) fullFilename += ".bmp";
    }

    if (!cv::imwrite(fullFilename, image, params)) {
        std::cerr << "Failed to save image\n";
    } else {
        std::cout << "Saved to " << fullFilename << "\n";
    }
}

void OutputNode::renderPropertiesUI() {
    ImGui::Text("Output Settings");

    char filenameBuffer[256];
    std::strncpy(filenameBuffer, filename.c_str(), sizeof(filenameBuffer));
    filenameBuffer[sizeof(filenameBuffer) - 1] = '\0';

    if (ImGui::InputText("Filename", filenameBuffer, sizeof(filenameBuffer))) {
        filename = filenameBuffer;
    }

    const char* formats[] = { "JPG", "PNG", "BMP" };
    static int currentFormat = 0;

    if (ImGui::Combo("Format", &currentFormat, formats, IM_ARRAYSIZE(formats))) {
        format = formats[currentFormat];
    }

    if (format == "JPG") {
        ImGui::SliderInt("JPG Quality", &jpgQuality, 0, 100);
    }

    if (ImGui::Button("Save Image")) {
        saveImage();
    }

    if (image.empty()) {
        ImGui::Text("No image yet.");
    } else {
        ImGui::Text("Image exists.");
    }
}

cv::Mat OutputNode::getOutput() const {
    return image;
}