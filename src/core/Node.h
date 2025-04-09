#pragma once
#include <string>

class Node {
public:
    int id;
    std::string name;

    virtual void process() = 0;

    virtual ~Node() = default;
};