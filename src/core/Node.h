#pragma once
#include <opencv2/opencv.hpp>
#include <string>

class Node {
public:
    int id;
    std::string name;

    Node(int id, const std::string& name) : id(id), name(name) {}

    virtual void process() = 0;
    virtual cv::Mat getOutput() const = 0;
    virtual void preview() {}
    virtual void renderPropertiesUI() {}
    virtual void setInput(const cv::Mat&) {}

    virtual ~Node() = default;
};