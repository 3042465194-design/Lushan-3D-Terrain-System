#include "Common.h"
#include "ModelNode.h"
#include "Camera.h"
#include "Renderer.h"
#include "ShaderManager.h"
#include "InputHandler.h"
#include "PlaneNode.h"
#include "SkyDomeNode.h"
#include "SimpleMinimap.h"
#include <iostream>
#include <memory>
#include "stb_image.h"

//加载2D纹理
GLuint loadTexture2D(const std::string& path) {
    int w, h, ch;
    unsigned char* data = stbi_load(path.c_str(), &w, &h, &ch, 0);
    if (!data) {
        std::cerr << "Failed to load texture: " << path << std::endl;
        return 0;
    }
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    GLenum fmt = (ch == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_image_free(data);
    return tex;
}

int main() {
    // 初始化 GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Lushan - Third Person Flight", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to init GLAD" << std::endl;
        return -1;
    }

    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    glViewport(0, 0, fbWidth, fbHeight);
    glEnable(GL_DEPTH_TEST);

    // 加载地形模型
    auto terrainNode = std::make_shared<ModelNode>("lushan.obj", "lushan_imagery.jpg");
    if (!terrainNode->isValid()) {
        std::cerr << "Terrain model loading failed!" << std::endl;
        return -1;
    }
    float minH = terrainNode->getMinHeight();
    float maxH = terrainNode->getMaxHeight();
    std::cout << "Terrain height range: " << minH << " ~ " << maxH << std::endl;

    // 地形平面范围（用于小地图）
    glm::vec2 worldMin(terrainNode->getMinX(), terrainNode->getMinZ());
    glm::vec2 worldMax(terrainNode->getMaxX(), terrainNode->getMaxZ());

    // 天空穹顶
    auto skyDome = std::make_shared<SkyDomeNode>(5000.0f, 48, 96);
    ShaderManager skyDomeShader;
    if (!skyDomeShader.loadShader("sky_dome.vert", "sky_dome.frag")) {
        std::cerr << "Sky dome shader failed" << std::endl;
        return -1;
    }

    // 飞机模型
    float planeScale = 0.001f;
    auto planeNode = std::make_shared<PlaneNode>("assets/plane.STL", planeScale);
    if (planeNode->getVertexCount() == 0) {
        std::cerr << "Plane model empty, check STL file" << std::endl;
    }
    glm::vec3 planePos(0.0f, maxH + 80.0f, 0.0f);
    planeNode->setPosition(planePos);

    ShaderManager planeShader;
    if (!planeShader.loadShader("plane.vert", "plane.frag")) {
        std::cerr << "Plane shader failed" << std::endl;
        return -1;
    }

    // 相机
    Camera camera;
    glm::vec3 cameraOffset(0.0f, 40.0f, 80.0f); 

    // 渲染器和地形着色器
    Renderer renderer;
    renderer.initialize(fbWidth, fbHeight);
    renderer.setClearColor(0.2f, 0.3f, 0.5f, 1.0f);

    ShaderManager terrainShader;
    if (!terrainShader.loadShader("terrain.vert", "terrain.frag")) {
        std::cerr << "Terrain shader failed" << std::endl;
        return -1;
    }

    SimpleMinimap minimap;
    minimap.setBounds(worldMin, worldMax);
    minimap.setRect(20.0f, 20.0f, 200.0f, 150.0f);
    GLuint miniMapTex = loadTexture2D("lushan_map.jpg");
    if (miniMapTex) minimap.setBackgroundTexture(miniMapTex);

    float moveSpeed = 2000.0f;
    float mouseSensitivity = 0.0002f;
    float planeYaw = 0.0f;
    float planePitch = 0.0f;
    bool useElevation = true;

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    double lastMouseX, lastMouseY;
    bool firstMouse = true;
    float lastFrame = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        float now = glfwGetTime();
        float dt = now - lastFrame;
        lastFrame = now;

        // 键盘输入
        glm::vec3 moveDir(0.0f);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) moveDir.z -= 1.0f;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) moveDir.z += 1.0f;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) moveDir.x += 1.0f;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) moveDir.x -= 1.0f;
        if (glm::length(moveDir) > 0.0f) moveDir = glm::normalize(moveDir);

        // 鼠标控制飞机方向
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        if (firstMouse) {
            lastMouseX = mouseX;
            lastMouseY = mouseY;
            firstMouse = false;
        }
        double dx = mouseX - lastMouseX;
        double dy = mouseY - lastMouseY;
        lastMouseX = mouseX;
        lastMouseY = mouseY;

        planeYaw += dx * mouseSensitivity;
        planePitch += dy * mouseSensitivity;
        if (planePitch > glm::radians(80.0f)) planePitch = glm::radians(80.0f);
        if (planePitch < glm::radians(-80.0f)) planePitch = glm::radians(-80.0f);

        // 计算飞机的局部轴
        glm::vec3 forward;
        forward.x = cos(planePitch) * sin(planeYaw);
        forward.y = sin(planePitch);
        forward.z = cos(planePitch) * cos(planeYaw);
        forward = glm::normalize(forward);
        glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));

        // 移动飞机
        glm::vec3 velocity = moveDir.x * right + moveDir.z * forward;
        planePos += velocity * moveSpeed * dt;
        if (planePos.y < minH + 10.0f) planePos.y = minH + 10.0f;
        planeNode->setPosition(planePos);

        // 第三人称相机跟随
        glm::vec3 localOffset(0.0f, 45.0f, 100.0f); 
        glm::vec3 worldOffset = forward * localOffset.z
            + right * localOffset.x
            + glm::vec3(0.0f, 1.0f, 0.0f) * localOffset.y;
        glm::vec3 cameraPos = planePos + worldOffset;
        glm::vec3 lookAhead = planePos + forward * 200.0f;
        camera.setFirstPerson(cameraPos, lookAhead - cameraPos);

        // 切换高程渲染（E键）
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
            static bool last = false;
            if (!last) {
                useElevation = !useElevation;
                std::cout << "Elevation color: " << (useElevation ? "ON" : "OFF") << std::endl;
            }
            last = true;
        }
        else {
            static bool last = false;
            if (last) last = false;
        }

        // 窗口大小变化
        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        if (w != fbWidth || h != fbHeight) {
            fbWidth = w; fbHeight = h;
            renderer.resize(fbWidth, fbHeight);
            glViewport(0, 0, fbWidth, fbHeight);
        }
        float aspect = (float)fbWidth / fbHeight;

        // 渲染场景
        renderer.beginFrame();

        // 1.天空盒
        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);
        skyDomeShader.use();
        // 计算 modelViewProj 矩阵：projection * (view without translation) * model
        glm::mat4 viewNoTranslate = glm::mat4(glm::mat3(camera.getViewMatrix()));
        glm::mat4 mvp = camera.getProjectionMatrix(aspect) * viewNoTranslate * glm::mat4(1.0f);
        skyDomeShader.setMat4("modelViewProj", mvp);
        skyDome->draw(&renderer);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);

        //2.渲染地形
        terrainShader.use();
        terrainShader.setMat4("view", camera.getViewMatrix());
        terrainShader.setMat4("projection", camera.getProjectionMatrix(aspect));
        terrainShader.setMat4("model", glm::mat4(1.0f));
        terrainShader.setInt("texSampler", 0);
        terrainShader.setInt("useElevationColor", useElevation ? 1 : 0);
        terrainShader.setFloat("minHeight", minH);
        terrainShader.setFloat("maxHeight", maxH);
        terrainNode->draw(&renderer);
        //3.渲染飞机模型
        planeShader.use();
        planeShader.setMat4("view", camera.getViewMatrix());
        planeShader.setMat4("projection", camera.getProjectionMatrix(aspect));
        glm::mat4 model = glm::translate(glm::mat4(1.0f), planePos);
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, planeYaw, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, planePitch, glm::vec3(1.0f, 0.0f, 0.0f));
        planeShader.setMat4("model", model);
        planeShader.setVec3("lightPos", glm::vec3(0.0f, maxH + 500.0f, 0.0f));
        planeShader.setVec3("viewPos", camera.getPosition());
        planeNode->draw(&renderer);

        renderer.endFrame();
        // 绘制小地图
        minimap.draw(fbWidth, fbHeight, planePos);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteTextures(1, &miniMapTex);
    glfwTerminate();
    return 0;
}
