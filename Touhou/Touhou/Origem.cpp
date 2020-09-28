#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image/stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include "shader.h"
#include "texture.h"
#include "resource_manager.h"
#include "layer.h"

#include <string>
#include <iostream>
#include <cstdlib>
#include <ctime>

// GLFW function declerations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void processInput(GLFWwindow* window);

//User Functions
void drawObject(Shader ourShader, glm::mat4 trans, Texture2D texture, float offSetX, float layerZ);
void drawObjectAndTranlate(Shader ourShader, glm::mat4 trans, Texture2D texture, float x, float y);
bool collisionDetection(float objectAx, float objectAy, float objectBx, float ObcjectBy, float sizeObjectA, float sizeObjectB);

// The Width of the screen
const unsigned int SCREEN_WIDTH = 1200;
// The height of the screen
const unsigned int SCREEN_HEIGHT = 600;

bool start = false;
bool gameOver = false;
bool gameWon = false;

int playerHealth = 3;
int bossHealth = 20;

float spaceshipX;
float spaceshipY;

bool shot = false;


int main(int argc, char* argv[])
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_RESIZABLE, false);

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Breakout", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Shader ourShader = ResourceManager::LoadShader("shader.vs", "shader.fs", nullptr, "ourShader");

    float spaceshipM[] = {
        // positions         // texture coords
         0.06f,  0.06f, 0.0f,  1.0f, 1.0f, // top right
         0.06f, -0.06f, 0.0f,  1.0f, 0.0f, // bottom right
        -0.06f, -0.06f, 0.0f,  0.0f, 0.0f, // bottom left
        -0.06f,  0.06f, 0.0f,  0.0f, 1.0f  // top left
    };
    float screen[] = {
        // positions         // texture coords
         1.0f,  1.0f, 0.0f,  1.0f, 1.0f, // top right
         1.0f, -1.0f, 0.0f,  1.0f, 0.0f, // bottom right
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, // bottom left
        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f  // top left
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    unsigned int VBO[2], VAO[2], EBO;
    glGenVertexArrays(2, VAO);
    glGenBuffers(2, VBO);
    glGenBuffers(1, &EBO);

    for (int i = 0; i < 2; i++) {
        glBindVertexArray(VAO[i]);
        if (i == 0) {
            glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(spaceshipM), spaceshipM, GL_STATIC_DRAW);
        }
        else {
            glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(screen), screen, GL_STATIC_DRAW);
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // texture coord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }


    //screens
    Texture2D startScreen = ResourceManager::LoadTexture("layers/startScreen.png", false, "startScreen");
    Texture2D gameOverScreen = ResourceManager::LoadTexture("layers/gameOverScreen.jpg", false, "gameOverScreen");
    Texture2D gameWonScreen = ResourceManager::LoadTexture("layers/gameWonScreen.png", false, "gameWonScreen");

    //background layers
    Texture2D background = ResourceManager::LoadTexture("layers/background.png", false, "background");
    Texture2D stars = ResourceManager::LoadTexture("layers/stars.png", true, "stars");
    Texture2D spacialTrash = ResourceManager::LoadTexture("layers/spacialTrash.png", true, "spacialTrash");
    Texture2D farPlanets = ResourceManager::LoadTexture("layers/far-planets2.png", true, "farPlanets");
    Texture2D ringPlanet = ResourceManager::LoadTexture("layers/ring-planet.png", true, "ringPlanet");
    Texture2D bigPlanet = ResourceManager::LoadTexture("layers/big-planet2.png", true, "bigPlanet");

    //frontal-objects
    Texture2D spaceship = ResourceManager::LoadTexture("layers/spaceship.png", true, "spaceship");
    Texture2D laserBullet = ResourceManager::LoadTexture("layers/laserBullet2.png", true, "laserBullet");
    Texture2D mage = ResourceManager::LoadTexture("layers/mage3.png", true, "mage");
    Texture2D mageShot = ResourceManager::LoadTexture("layers/mageShot2.png", true, "mageShot");
    Texture2D heart = ResourceManager::LoadTexture("layers/heart.png", true, "heart");
    Texture2D bossHealthTile = ResourceManager::LoadTexture("layers/bossHealth.png", true, "bossHealth");
    Texture2D shield = ResourceManager::LoadTexture("layers/shield.png", true, "shield");

    //background textures array
    Texture2D textureArr[] = { bigPlanet, ringPlanet, spacialTrash, farPlanets, stars, background };
    //background textures Z positions
    float zValues[] = { -0.55f, -0.54f, -0.53f, -0.52f, -0.51f, -0.5f };

    srand(static_cast <unsigned> (time(0)));

    ourShader.Use();
    ourShader.SetInteger("textures", 0);

    //shot parameters
    float shotX = 0.0f;
    float shotY = 0.0f;
    
    //mage parameters
    float mageY = 1.0f;
    bool top = true;
    bool goDown = false;
    bool bottom = false;
    bool goUp = true;

    //mage shot parameters
    float mageShotX = 0.7f;

    //shield parameters 
    int timer = 1;
    bool extraHeartDescend = false;
    float extraHeartY = -1.0f;
    float extraHeartX = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // manage user input
        processInput(window);

        // render
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // bind textures on corresponding texture units
        glBindVertexArray(VAO[1]);

        unsigned int transform = glGetUniformLocation(ourShader.ID, "transform");

        glm::mat4 trans = glm::mat4(1.0f);

        //game over screen
        if (playerHealth <= 0) {
            gameOver = true;
            drawObject(ourShader, trans, gameOverScreen, 0.0f, -1.0f);
        }
        //game won screen
        if (bossHealth <= 0) {
            gameWon = true;
            drawObject(ourShader, trans, gameWonScreen, 0.0f, -1.0f);
        }
        //start screen
        if (start == false && gameOver == false && gameWon == false) {
            drawObject(ourShader, trans, startScreen, 0.0f, -1.0f);
        }
        else if(start == true && gameOver == false && gameWon == false) {

            //background logic
            for (int i = 0; i < 6; i++) {
                drawObject(ourShader, trans, textureArr[i], (float)glfwGetTime() / (3 + i), zValues[i]);
            }

            ourShader.SetFloat("offsetx", 0.0f);
            ourShader.SetFloat("layer_z", -1.0f);
            glBindVertexArray(VAO[0]);

            //spaceship logic
            drawObjectAndTranlate(ourShader, trans, spaceship, spaceshipX, spaceshipY);

            //shot logic
            if (shot == true && shotX < 1.0f) {
                shotX += 0.01f;
                trans = glm::mat4(1.0f);
                trans = glm::translate(trans, glm::vec3(shotX, shotY, 0.0f));
                trans = glm::scale(trans, glm::vec3(0.5, 0.5, 1.0));
                ourShader.SetMatrix4("transform", trans);
                laserBullet.Bind();
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            }
            else {
                shotY = spaceshipY;
            }
            if (shotX >= 1.0f) {
                shot = false;
                shotX = spaceshipX;
            }

            //mage logic
            glBindVertexArray(VAO[1]);

            if (top == true && goUp == true) {
                top = goUp = false;
                goDown = true;
            }
            if (bottom == true && goDown == true) {
                bottom = goDown = false;
                goUp = true;
            }
            if (mageY >= 0.8f) {
                top = true;
            }
            if (mageY <= -0.9f) {
                bottom = true;
            }
            if (goUp == true) {
                mageY += 0.004f;
            }
            if (goDown == true) {
                mageY -= 0.004f;
            }

            trans = glm::mat4(1.0f);
            trans = glm::translate(trans, glm::vec3(0.8f, mageY, 0.0f));
            trans = glm::scale(trans, glm::vec3(0.2, 0.2, 1.0));
            ourShader.SetMatrix4("transform", trans);
            mage.Bind();
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            //mage bullet logic
            glBindVertexArray(VAO[0]);

            trans = glm::mat4(1.0f);
            trans = glm::translate(trans, glm::vec3(mageShotX, mageY, 0.0f));
            trans = glm::scale(trans, glm::vec3(0.6, 0.6, 1.0));
            ourShader.SetMatrix4("transform", trans);
            mageShot.Bind();
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            mageShotX -= 0.005f;
            if (mageShotX <= -1.0f) {
                mageShotX = 0.7f;
            }

            //shield logic

            int time = glfwGetTime();

            if (timer % 2700 == 0) 
                extraHeartDescend = true;
            if (extraHeartY >= -1.0f && extraHeartDescend) {
                
                drawObjectAndTranlate(ourShader, trans, heart, extraHeartX, extraHeartY);
                extraHeartY -= 0.003f;
            }
            else if (extraHeartY <= -1.0f) {
                int random = rand() % 2;
                if (random == 0)
                    extraHeartX = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 0.5));
                else
                    extraHeartX = (static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 0.9))) * -1;
                extraHeartDescend = false;
                extraHeartY = 1.0f;
            }
            timer += 1;

            //player health logic
            float heartX = -0.95f;
            for (int i = 0; i < playerHealth; i++) {
                drawObjectAndTranlate(ourShader, trans, heart, heartX, 0.9f);
                heartX += 0.066f;
            }

            //boss health logic
            float bossHealthX = -0.19f;
            for (int i = 0; i < bossHealth; i++) {
                drawObjectAndTranlate(ourShader, trans, bossHealthTile, bossHealthX, -0.9f);
                bossHealthX += 0.02f;
            }
        }

        if (collisionDetection(shotX, shotY, 0.70f, mageY, 0.12f, 0.2f)) {
            bossHealth--;
            shotX = 1.0f;
        }
        if (collisionDetection(spaceshipX, spaceshipY, mageShotX, mageY, 0.12f, 0.2f)) {
            playerHealth--;
            mageShotX = -1.0f;
        }
        if (collisionDetection(spaceshipX, spaceshipY, 0.8f, mageY, 0.12f, 0.2f)) {
            playerHealth = 0;
        }
        if (collisionDetection(spaceshipX, spaceshipY, extraHeartX, extraHeartY, 0.12f, 0.12f)) {
            extraHeartDescend = false;
            playerHealth += 1;
            extraHeartX = 2.0f;
        }
        glfwSwapBuffers(window);
    }

    // delete all resources as loaded using the resource manager
    ResourceManager::Clear();

    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    // when a user presses the escape key, we set the WindowShouldClose property to true, closing the application
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        spaceshipY += 0.04f * 2; // change this value accordingly (might be too slow or too fast based on system hardware)
        if (spaceshipY >= 1.0f)
            spaceshipY =  1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        spaceshipY -= 0.04f * 2; // change this value accordingly (might be too slow or too fast based on system hardware)
        if (spaceshipY <= -1.0f)
            spaceshipY =  -1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        spaceshipX += 0.04f; // change this value accordingly (might be too slow or too fast based on system hardware)
        if (spaceshipX >= 1.0f)
            spaceshipX =  1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        spaceshipX -= 0.04f; // change this value accordingly (might be too slow or too fast based on system hardware)
        if (spaceshipX <= -1.0f)
            spaceshipX =  -1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
    {
        shot = true;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
     {
        start = true;
     }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void drawObject(Shader ourShader, glm::mat4 trans, Texture2D texture, float offSetX, float layerZ) {
    ourShader.SetMatrix4("transform", trans);
    ourShader.SetFloat("offsetx", offSetX);
    ourShader.SetFloat("layer_z", layerZ);
    texture.Bind();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void drawObjectAndTranlate(Shader ourShader, glm::mat4 trans, Texture2D texture, float x, float y) {
    trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(x, y, 0.0f));
    ourShader.SetMatrix4("transform", trans);
    texture.Bind();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

bool collisionDetection(float objectAx, float objectAy, float objectBx, float ObcjectBy, float sizeObjectA, float sizeObjectB) {
    // collision x-axis?
    bool collisionX = objectAx + sizeObjectA >= objectBx &&
        objectBx + sizeObjectB >= objectAx;
    // collision y-axis?
    bool collisionY = objectAy + sizeObjectA >= ObcjectBy &&
        ObcjectBy + sizeObjectB >= objectAy;
    // collision only if on both axes
    return collisionX && collisionY;
}

