
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

//#include <GL/glew.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>
//#include <algorithm>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void mouse_callback(GLFWwindow *window, double xpos, double ypos);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void processInput(GLFWwindow *window);

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

unsigned int loadCubemap(vector<std::string> faces);

glm::mat4 updateModel(float time);
glm::mat4 updateModel2(float time);
float clamp(float value,float min,float max);
// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 800;

Camera camera(glm::vec3(0.0f,3.0f,30.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

float promena=1;

int main() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    //stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders

    // -----------------------------------------------------------------------------------------

    Shader ourShader("resources/shaders/2.model_lighting.vs", "resources/shaders/2.model_lighting.fs");
    Shader skyboxShader("resources/shaders/skybox.vs", "resources/shaders/skybox.fs");

    //skybox
    float skyboxVertices[] = {
            // positions
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f,  1.0f
    };

    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);

    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    //ubacivanje tekstura u sejdere koje sam iznad aktivirala

    vector<std::string> skyboxSides = {
            FileSystem::getPath("resources/textures/skybox2/right.jpg"),
            FileSystem::getPath("resources/textures/skybox2/left.jpg"),
            FileSystem::getPath("resources/textures/skybox2/top.jpg"),
            FileSystem::getPath("resources/textures/skybox2/bottom.jpg"),
            FileSystem::getPath("resources/textures/skybox2/front.jpg"),
            FileSystem::getPath("resources/textures/skybox2/back.jpg")
    };
    unsigned int cubemapTexture=loadCubemap(skyboxSides);


    // load models
    // --------------------------------------------------------------------------------------
    Model Model1("resources/objects/objekat2/untitled.obj");
    Model Model2("resources/objects/objekat3/objekat2.obj");
    Model1.SetShaderTextureNamePrefix("material.");


    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);
    float startTime=static_cast<float>(glfwGetTime());

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);


        // render
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        ourShader.use();
        //pointLight.position = glm::vec3(4.0 * cos(currentFrame), 4.0f, 4.0 * sin(currentFrame));
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
        //Directional light
        ourShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        ourShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        ourShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        ourShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

        //Pointlight
        ourShader.setVec3("pointLight.position", glm::vec3(4.0f, 4.0, 0.0));
        ourShader.setVec3("pointLight.ambient", glm::vec3(0.1, 0.1, 0.1));
        ourShader.setVec3("pointLight.diffuse", glm::vec3(0.6, 0.6, 0.6));
        ourShader.setVec3("pointLight.specular", glm::vec3(1.0, 1.0, 1.0));
        ourShader.setFloat("pointLight.constant", 1.0f);
        ourShader.setFloat("pointLight.linear", 0.14f);
        ourShader.setFloat("pointLight.quadratic", 0.07f);

        ourShader.setVec3("viewPosition", camera.Position);
        ourShader.setFloat("material.shininess", 32.0f);
        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
                                                (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        // render the loaded model

        //model svih balona

        float currentTime=static_cast<float>(glfwGetTime());
        float elapsedTime=currentTime-startTime;
        glm::mat4 model=updateModel(elapsedTime);
        //model=glm::rotate(model,glm::radians(180.0f),glm::vec3(1.0f,1.0f,0.0f));
        //glm::mat4 model=glm::mat4(1.0f);
        model = glm::translate(model,
                               glm::vec3(0.0f,-3.0f,-3.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f));    // it's a bit too big for our scene, so scale it down
        ourShader.setMat4("model", model);
        Model1.Draw(ourShader);

        //model jednog centralnog balona
        currentTime=static_cast<float>(glfwGetTime());
        elapsedTime=currentTime-startTime;
        glm::mat4 model2=updateModel2(elapsedTime);
        model2 = glm::translate(model2,
                               glm::vec3(10.0f,-3.0f,-20.0f)); // translate it down so it's at the center of the scene
        model2 = glm::scale(model2, glm::vec3(1.0f));    // it's a bit too big for our scene, so scale it down
        ourShader.setMat4("model", model2);
        Model2.Draw(ourShader);

        //render skybox cube
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
        /*model = glm::translate(model,
                              programState->backpackPosition);*/
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);


        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1,&skyboxVBO);


    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;


    camera.ProcessMouseMovement(xoffset*0.02, yoffset*0.02);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}

unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

float clamp(float value,float min,float max){
    if (value<min) return min;
    if(value>max) return max;
    return value;
}
glm::mat4 updateModel(float time){
    promena++;
    glm::mat4 model=glm::mat4(1.0f);


    float speed=0.4f;
    float amplitude=5.0f;
    float frequency=0.5f;
    //float amplitude=1.0f;

    //float angle=speed*time;

    float x=speed*time;
    float y=amplitude*sin(frequency*x);
    float z=amplitude*cos(frequency*x);

    //x=clamp(x,-10.0,10.0);
    //y=clamp(y,-10.0,10.0);
    //z=clamp(z,-10.0,10.0);
    glm::vec3 pos=glm::vec3(x,y,z);

    model=glm::translate(model,glm::vec3(x,y,z));
    glm::vec3 direction(cos(frequency * x), sin(frequency * x),z);
    /*
    pos+=(direction*speed*deltaTime);
    if(x<-10.0f || x>10.0f || y<-10.0f || y>10.0f || z<-10.0f || z>10.0f){
        promena=-promena;
    }
    if(promena>10000) {
        direction = -direction;
        promena = 0;
    }*/
    direction=direction*promena;
    float rotationAngle=atan2(direction.y/2.0f,direction.x/2.0f);
    model=glm::rotate(model,rotationAngle,glm::vec3(0.0f,1.0f,0.0f));

    return model;
}
glm::mat4 updateModel2(float time){

    glm::mat4 model=glm::mat4(1.0f);


    float speed=0.6f;
    float amplitude=5.0f;
    float frequency=0.5f;
    //float amplitude=1.0f;

    //float angle=speed*time;

    float x=speed*time;
    float y=amplitude*cos(frequency*x);
    float z=amplitude*sin(frequency*x);

    //x=clamp(x,-10.0,10.0);
    //y=clamp(y,-10.0,10.0);
    //z=clamp(z,-10.0,10.0);

    model=glm::translate(model,glm::vec3(x,y,z));
    glm::vec2 direction(-cos(frequency * x), sin(frequency * x));

    float rotationAngle=atan2(direction.y/2.0f,direction.x/2.0f);
    model=glm::rotate(model,rotationAngle,glm::vec3(0.0f,1.0f,0.0f));

    return model;
}

