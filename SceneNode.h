#pragma once
#include "Common.h"

class SceneNode : public std::enable_shared_from_this<SceneNode> 
{
public:
    SceneNode();
    virtual ~SceneNode();
    void addChild(std::shared_ptr<SceneNode> child);
    void setTransform(const glm::mat4& transform);
    glm::mat4 getWorldTransform() const;
    virtual void update(float deltaTime) {}
    virtual void draw(class Renderer* renderer) = 0;

protected:
    glm::mat4 localTransform;
    std::weak_ptr<SceneNode> parent;
    std::vector<std::shared_ptr<SceneNode>> children;
};
