#pragma once
#include <queue>
#include <vector>
#include <unordered_map>
#include <memory>
#include "Node.h"
#include "../nodes/InputNode.h"

struct Link {
    int id;
    int fromNode, fromAttr;
    int toNode, toAttr;
};

class Graph {
private:
    int nextNodeId = 1;
    int nextLinkId = 2;
public:
    std::unordered_map<int, std::shared_ptr<Node>> nodes;
    std::vector<Link> links;
    bool hasCycle = false;
    
    int addNode(std::shared_ptr<Node> node) {
        node->id = nextNodeId;
        nodes[nextNodeId] = node;
        nextNodeId += 2;
        return node->id;
    }

    int addLink(int fromNode, int fromAttrIndex, int toNode, int toInputIndex) {
        int fromAttr = fromNode * 1000 + fromAttrIndex;
        int toAttr   = toNode   * 1000 + toInputIndex;

        for (const auto& link : links) {
            if (link.fromAttr == fromAttr && link.toAttr == toAttr) {
                return link.id; // already exists
            }
        }
        Link link {
            nextLinkId,
            fromNode, fromAttr,
            toNode, toAttr
        };
        links.push_back(link);
        nextLinkId += 2;
        return link.id;
    }
    
    void removeNode(int id) {
        nodes.erase(id);
        links.erase(std::remove_if(links.begin(), links.end(),
            [id](const Link& link) {
                return link.toNode == id || link.fromNode == id;
            }),
            links.end()
        );
    }

    void removeLink(int id) {
        links.erase(std::remove_if(links.begin(), links.end(),
            [id](const Link& link) {
                return link.id == id;
            }),
            links.end()
        );
    }

    std::vector<Link> getInputLinks(int id) {
        std::vector<Link> res;
        for (const Link& link : links) {
            if (link.toNode == id) {
                res.push_back(link);
            }
        }
        return res;
    }

    std::unordered_map<int, std::vector<int>> buildAdjacencyList() {
        std::unordered_map<int, std::vector<int>> adjacencyList;
        
        for (const auto& [id, ptr] : nodes) {
            adjacencyList[id] = {};
        }

        for (const auto& link : links) {
            adjacencyList[link.fromNode].push_back(link.toNode);
        }

        return adjacencyList;
    }

    std::vector<int> topologicalSort() {
        auto adjacencyList = buildAdjacencyList();
        std::unordered_map<int, int> indegree;
        std::vector<int> toposort;
        std::queue <int> q;

        for (const auto& [node, neighbours] : adjacencyList) {
            for (const auto& neighbour : neighbours) {
                ++indegree[neighbour];
            }
        }

        for (const auto& [node, neighbours] : adjacencyList) {
            if (indegree[node] == 0) {
                q.push(node);
            }
        }

        while (!q.empty()) {
            auto cur = q.front();
            q.pop();
            toposort.push_back(cur);

            for (const auto& next : adjacencyList[cur]) {
                if (--indegree[next] == 0) {
                    q.push(next);
                }
            }
        }

        if (toposort.size() != adjacencyList.size()) {
            toposort.clear();
            hasCycle = true;
        } else {
            hasCycle = false;
        }

        return toposort;
    }

    void evaluate() {
        auto toposort = topologicalSort();

        if (toposort.empty()) {
            // std::cerr << "Graph contains a cycle\n";
            return;
        }

        for (int nodeId : toposort) {
            auto node = nodes[nodeId];
            auto inputs = getInputLinks(nodeId);

            assert(inputs.size() <= 2);

            std::vector <cv::Mat> outputs;
            
            if (inputs.size() == 1 || inputs.size() == 2) {
                if (inputs.size() == 2 && (inputs[0].toAttr % 1000 > inputs[1].toAttr % 1000)) {
                    std::swap(inputs[0], inputs[1]);
                }
                for (const auto& link : inputs) {
                    outputs.push_back(nodes[link.fromNode]->getOutput());
                }
            }            

            if (!dynamic_cast<InputNode*>(node.get()) && !inputs.empty()) {
                node->setInputs(outputs);
            }

            node->process();
        }
    }
};