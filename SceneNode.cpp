#include "SceneNode.h"

SceneNode::SceneNode() : localTransform(1.0f) {}

SceneNode::~SceneNode() {}

void SceneNode::addChild(std::shared_ptr<SceneNode> child) {
    child->parent = shared_from_this();
    children.push_back(child);
}

void SceneNode::setTransform(const glm::mat4& transform) {
    localTransform = transform;
}

glm::mat4 SceneNode::getWorldTransform() const {
    if (auto p = parent.lock()) {
        return p->getWorldTransform() * localTransform;
    }
    return localTransform;
}