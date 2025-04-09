#include "OutputNode.h"
#include <opencv2/imgcodecs.hpp>
#include <iostream>
#include "../utils/TextureUtils.h"

OutputNode::OutputNode(int id, const std::string& name) : Node(id, name) {}

void OutputNode::process() {
    return;
}

void OutputNode::setInput(const cv::Mat& input) {
    image = input;
    
    if (textureID) glDeleteTextures(1, &textureID); // cleanup old
    textureID = matToTexture(image);
}

cv::Mat OutputNode::getOutput() const {
    return image;
}