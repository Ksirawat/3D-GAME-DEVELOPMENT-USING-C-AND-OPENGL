#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>
#include <vector>

// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// Settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Game State
glm::vec3 playerPosition = glm::vec3(0.0f, 0.0f, 5.0f);
float playerSpeed = 3.0f;
float playerRotationY = 0.0f;

// Struct for collectible items
struct CollectibleItem {
    glm::vec3 position;
    bool isCollected = false;
};

std::vector<CollectibleItem> items;
int itemsCollectedCount = 0;
float itemScale = 0.25f; // Scale for the backpacks


int main()
{
    // --- Initialize GLFW and GLAD ---
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Simple 3D Game (Credit obj from kenney.nl)", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // --- Build Shaders and Load Models ---
    Shader ourShader("1.model_loading.vs", "1.model_loading.fs");


	//credit from https://kenney.nl/assets/mini-market
    Model playerModel(FileSystem::getPath("resources/objects/hw3/OBJ format/character-employee.obj"));
    Model itemModel(FileSystem::getPath("resources/objects/backpack/backpack.obj"));
    Model floorModel(FileSystem::getPath("resources/objects/hw3/OBJ format/floor.obj"));
    Model wallModel(FileSystem::getPath("resources/objects/hw3/OBJ format/wall.obj"));
    Model shelfModel(FileSystem::getPath("resources/objects/hw3/OBJ format/shelf-boxes.obj"));
    Model registerModel(FileSystem::getPath("resources/objects/hw3/OBJ format/cash-register.obj"));

    // Add items to the scene
    items.push_back(CollectibleItem{ glm::vec3(5.0f, 0.25f, -5.0f) });
    items.push_back(CollectibleItem{ glm::vec3(-5.0f, 0.25f, -5.0f) });
    items.push_back(CollectibleItem{ glm::vec3(0.0f, 0.25f, 8.0f) });

    // Lighting
    glm::vec3 lightDirection = glm::vec3(0.0f, -0.5f, -1.0f);

    // --- Render Loop ---
    while (!glfwWindowShouldClose(window))
    {
        // Timing
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Input
        processInput(window);

        // Game Logic (Check collisions)
        float collisionDistance = 1.5f;
        for (auto& item : items)
        {
            if (!item.isCollected && glm::distance(playerPosition, item.position) < collisionDistance)
            {
                item.isCollected = true;
                itemsCollectedCount++;
                std::cout << "Item Collected! Total: " << itemsCollectedCount << "/" << items.size() << std::endl;
            }
        }

        // --- Render ---
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ourShader.use();

        // View/Projection (Camera)
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        ourShader.setMat4("projection", projection);

        // 3rd Person Camera Logic
        glm::vec3 cameraPos = playerPosition;
        cameraPos.x += 8 * sin(glm::radians(playerRotationY));
        cameraPos.z += 8 * cos(glm::radians(playerRotationY));
        cameraPos.y += 3.0f;
        glm::mat4 view = glm::lookAt(cameraPos, playerPosition, glm::vec3(0.0f, 1.0f, 0.0f));
        ourShader.setMat4("view", view);

        // Send Lighting Info to Shader
        ourShader.setVec3("light.direction", lightDirection);
        ourShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
        ourShader.setVec3("light.diffuse", 0.8f, 0.8f, 0.8f);
        ourShader.setVec3("light.specular", 0.5f, 0.5f, 0.5f);
        ourShader.setVec3("viewPos", cameraPos);


        // --- Render Models ---
        glm::mat4 model = glm::mat4(1.0f);

        // Render Floor
        model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(20.0f, 1.0f, 20.0f));
        ourShader.setMat4("model", model);
        floorModel.Draw(ourShader);

        // Render Wall
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -10.0f));
        ourShader.setMat4("model", model);
        wallModel.Draw(ourShader);

        // Render Shelf
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-8.0f, 0.0f, 0.0f));
        ourShader.setMat4("model", model);
        shelfModel.Draw(ourShader);

        // Render Cash Register
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(7.0f, 0.0f, 0.0f));
        ourShader.setMat4("model", model);
        registerModel.Draw(ourShader);


        // Render the Player
        model = glm::mat4(1.0f);
        model = glm::translate(model, playerPosition);
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(playerRotationY), glm::vec3(0.0f, 1.0f, 0.0f));
        ourShader.setMat4("model", model);
        playerModel.Draw(ourShader);

        // Render the Items
        for (const auto& item : items)
        {
            if (!item.isCollected)
            {
                model = glm::mat4(1.0f);
                model = glm::translate(model, item.position);
                model = glm::scale(model, glm::vec3(itemScale));
                ourShader.setMat4("model", model);
                itemModel.Draw(ourShader);
            }
        }

        // Swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Terminate
    glfwTerminate();
    return 0;
}

// --- Function Definitions ---

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float speed = playerSpeed * deltaTime;
    float rotSpeed = 100.0f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        playerPosition.x -= sin(glm::radians(playerRotationY)) * speed;
        playerPosition.z -= cos(glm::radians(playerRotationY)) * speed;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        playerPosition.x += sin(glm::radians(playerRotationY)) * speed;
        playerPosition.z += cos(glm::radians(playerRotationY)) * speed;
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        playerRotationY += rotSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        playerRotationY -= rotSpeed;

    // Simple floor collision
    playerPosition.y = 0.0f;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
