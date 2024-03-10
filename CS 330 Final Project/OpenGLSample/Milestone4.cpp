#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cstdlib>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

#include "shader.h"
#include "camera.h"

#include <iostream>

using namespace std; // Standard namespace
bool perspective = true; // Perspective instance boolean.

/*Shader program Macro*/
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version " core \n" #Source
#endif

// Unnamed namespace
namespace
{
    const char* const WINDOW_TITLE = "Rowan Iruegas"; // Macro for window title

    // Variables for window width and height
    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;

    // Stores the GL data relative to a given mesh
    struct GLMesh
    {
        GLuint vao;         // Handle for the vertex array object
        GLuint vbo;         // Handle for the vertex buffer object
        GLuint nVertices;   // Number of indices of the mesh
    };

    // Main GLFW window
    GLFWwindow* gWindow = nullptr;
    // Mesh Data
    GLMesh gMesh, gKeyboardMesh, gPlaneMesh, gBookMesh, 
            gWallMesh, gMouseMesh, gCylinderMesh, gScrollMesh,
            gLimbMesh, gAstroMesh, gHelmMesh;
    // Texture Ids
    GLuint gTextureId, gMonitorTex, gScreen1Tex, 
            gScreen2Tex, gKeyboardTex, gBookTex, 
            gWallTex, gMouseTex, gCanTex, gAstroTex,
            gHelmTex;

    glm::vec2 gUVScale(5.0f, 5.0f);

    // LIGHT
    GLuint gLampProgramId;
    GLuint gCubeProgramId;
  
    // camera
    Camera gCamera(glm::vec3(0.0f, 0.0f, 3.0f));
    float gLastX = WINDOW_WIDTH / 2.0f;
    float gLastY = WINDOW_HEIGHT / 2.0f;
    bool gFirstMouse = true;

    // timing
    float gDeltaTime = 0.0f; // time between current frame and last frame
    float gLastFrame = 0.0f;

    // Shape Positions
    glm::vec3 gPosition[] = {
        glm::vec3(5.67f, 0.0f, 1.3f),   // Monitor 2 Position 0
        glm::vec3(1.0f, 0.0f, 0.0f),    // Monitor 1 Position 1
        glm::vec3(2.0f, -0.001f, 1.6f), // Desk Position 2
        glm::vec3(0.7f, 0.0f, 1.7f),    // Keyboard 3
        glm::vec3(5.7f, 0.0f, -0.2f),   // Book 4
        glm::vec3(3.5f, 0.0f, 1.9f),    // Mouse 5
        glm::vec3(3.54f, 0.16f, 2.07f), // Scroll 6
        glm::vec3(-0.3f, 0.0f, 0.9f),   // Cylinder 7
        glm::vec3(2.3f, 0.0f, 0.8f),    // Astronaut 8
        //LIMBS
        glm::vec3(2.15f, 0.5f, 0.95f),  //9
        glm::vec3(2.45f, 0.5f, 0.95f),  //10
        glm::vec3(2.25f, 0.25f, 0.95f), //11
        glm::vec3(2.35f, 0.25f, 0.95f)  //12
    };

    // Shape Scales
    glm::vec3 gScale[] = {
        glm::vec3(0.85f), // 0
        glm::vec3(1.0f),  // 1
        glm::vec3(3.0f),  // 2
        glm::vec3(0.25f), // 3
        glm::vec3(0.1f),  // 4
        glm::vec3(0.07f), // 5
        glm::vec3(0.06f)  // 6
    };

    glm::vec3 gRotation[] = {
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f), // Y Rotation
        glm::vec3(0.0f, 0.0f, 1.0f)
    };

    // Cube and light color
    glm::vec3 gObjectColor(0.6f, 0.5f, 0.75f);
    glm::vec3 gLightColor(1.0f, 1.0f, 1.0f);

    // Light position and scale
    glm::vec3 gLightPosition(2.5f, 2.0f, 3.0f);
    glm::vec3 gLightPosition2(0.0f, 7.0f, 0.0f);
    glm::vec3 gLightScale(0.3f);

}

/* User-defined Function prototypes to:
 * initialize the program, set the window size,
 * redraw graphics on the window when resized,
 * and render graphics on the screen
 */
bool UInitialize(int, char* [], GLFWwindow** window);
void UResizeWindow(GLFWwindow* window, int width, int height);
void UProcessInput(GLFWwindow* window);
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos);
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void UCreateMesh(GLMesh& mesh);
void UKeyboardCreateMesh(GLMesh& mesh);
void UBookCreateMesh(GLMesh& mesh);
void UPlaneCreateMesh(GLMesh& mesh);
void UWallCreateMesh(GLMesh& mesh);
void UMouseCreateMesh(GLMesh& mesh);
void UScrollCreateMesh(GLMesh& mesh);
void UCylinderCreateMesh(GLMesh& mesh);
void ULimbCreateMesh(GLMesh& mesh);
void UAstroCreateMesh(GLMesh& mesh);
void UHelmCreateMesh(GLMesh& mesh);
void UDestroyMesh(GLMesh& mesh);
bool UCreateTexture(const char* filename, GLuint& textureId);
void UDestroyTexture(GLuint textureId);
void URender();
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId);
void UDestroyShaderProgram(GLuint programId);

// Images are loaded with Y axis going down, but OpenGL's Y axis goes up, so let's flip it
void flipImageVertically(unsigned char* image, int width, int height, int channels)
{
    for (int j = 0; j < height / 2; ++j)
    {
        int index1 = j * width * channels;
        int index2 = (height - 1 - j) * width * channels;

        for (int i = width * channels; i > 0; --i)
        {
            unsigned char tmp = image[index1];
            image[index1] = image[index2];
            image[index2] = tmp;
            ++index1;
            ++index2;
        }
    }
}


int main(int argc, char* argv[])
{
    if (!UInitialize(argc, argv, &gWindow))
        return EXIT_FAILURE;

    // Create the mesh
    UCreateMesh(gMesh); // Calls the function to create the Vertex Buffer Object
    UKeyboardCreateMesh(gKeyboardMesh);
    UPlaneCreateMesh(gPlaneMesh);
    UBookCreateMesh(gBookMesh);
    UWallCreateMesh(gWallMesh);
    UMouseCreateMesh(gMouseMesh);
    UCylinderCreateMesh(gCylinderMesh);
    UScrollCreateMesh(gScrollMesh);
    ULimbCreateMesh(gLimbMesh);
    UAstroCreateMesh(gAstroMesh);
    UHelmCreateMesh(gHelmMesh);
        
    // Load texture
    const char* texFilename = "../OpenGLSample/textures/monitor_1.jpg";
    if (!UCreateTexture(texFilename, gTextureId))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }

    texFilename = "../OpenGLSample/textures/desk.jpg";
    if (!UCreateTexture(texFilename, gMonitorTex))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }

    texFilename = "../OpenGLSample/textures/screen_1.jpg";
    if (!UCreateTexture(texFilename, gScreen1Tex))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }

    texFilename = "../OpenGLSample/textures/screen_2.jpg";
    if (!UCreateTexture(texFilename, gScreen2Tex))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }

    texFilename = "../OpenGLSample/textures/keyboard.jpg";
    if (!UCreateTexture(texFilename, gKeyboardTex))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }

    texFilename = "../OpenGLSample/textures/book.jpg";
    if (!UCreateTexture(texFilename, gBookTex))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }

    texFilename = "../OpenGLSample/textures/Wall.jpg";
    if (!UCreateTexture(texFilename, gWallTex))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }

    texFilename = "../OpenGLSample/textures/mouse.jpg";
    if (!UCreateTexture(texFilename, gMouseTex))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }

    texFilename = "../OpenGLSample/textures/can.jpg";
    if (!UCreateTexture(texFilename, gCanTex))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }

    texFilename = "../OpenGLSample/textures/astro.jpg";
    if (!UCreateTexture(texFilename, gAstroTex))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }

    texFilename = "../OpenGLSample/textures/helm.jpg";
    if (!UCreateTexture(texFilename, gHelmTex))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }


    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    glUseProgram(gCubeProgramId);

    // We set the texture as texture unit 0
    glUniform1i(glGetUniformLocation(gCubeProgramId, "uTexture"), 0);

    // Sets the background color of the window to black (it will be implicitely used by glClear)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(gWindow))
    {
        // per-frame timing
        // --------------------
        float currentFrame = glfwGetTime();
        gDeltaTime = currentFrame - gLastFrame;
        gLastFrame = currentFrame;

        // input
        // -----
        UProcessInput(gWindow);

        // Render this frame
        URender();

        glfwPollEvents();
    }

    // Release mesh data
    UDestroyMesh(gMesh);
    UDestroyMesh(gKeyboardMesh);
    UDestroyMesh(gPlaneMesh);
    UDestroyMesh(gBookMesh);
    UDestroyMesh(gWallMesh);
    UDestroyMesh(gMouseMesh);
    UDestroyMesh(gScrollMesh);
    UDestroyMesh(gLimbMesh);
    UDestroyMesh(gAstroMesh);
    UDestroyMesh(gHelmMesh);

    // Release texture
    UDestroyTexture(gTextureId);
    UDestroyTexture(gMonitorTex);
    UDestroyTexture(gScreen1Tex);
    UDestroyTexture(gScreen2Tex);
    UDestroyTexture(gKeyboardTex);
    UDestroyTexture(gBookTex);
    UDestroyTexture(gWallTex);
    UDestroyTexture(gMouseTex);
    UDestroyTexture(gCanTex);
    UDestroyTexture(gAstroTex);
    UDestroyTexture(gHelmTex);

    // Release shader program
    UDestroyShaderProgram(gCubeProgramId);
    //UDestroyShaderProgram(gLampProgramId);

    exit(EXIT_SUCCESS); // Terminates the program successfully
}


// Initialize GLFW, GLAD, and create a window
bool UInitialize(int argc, char* argv[], GLFWwindow** window)
{
    // GLFW: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // GLFW: window creation
    // ---------------------
    * window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
    if (*window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(*window);
    glfwSetFramebufferSizeCallback(*window, UResizeWindow);
    glfwSetCursorPosCallback(*window, UMousePositionCallback);
    glfwSetScrollCallback(*window, UMouseScrollCallback);
    glfwSetMouseButtonCallback(*window, UMouseButtonCallback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Displays GPU OpenGL version
    cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << endl;

    return true;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void UProcessInput(GLFWwindow* window)
{
    static const float cameraSpeed = 3.0f;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        gCamera.ProcessKeyboard(FORWARD, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        gCamera.ProcessKeyboard(BACKWARD, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        gCamera.ProcessKeyboard(LEFT, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        gCamera.ProcessKeyboard(RIGHT, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        gCamera.ProcessKeyboard(UP, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        gCamera.ProcessKeyboard(DOWN, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        perspective = !perspective; // Switches perspective boolean to reflect ortho or perspective projection
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void UResizeWindow(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (gFirstMouse)
    {
        gLastX = xpos;
        gLastY = ypos;
        gFirstMouse = false;
    }

    float xoffset = xpos - gLastX;
    float yoffset = gLastY - ypos; // reversed since y-coordinates go from bottom to top

    gLastX = xpos;
    gLastY = ypos;

    gCamera.ProcessMouseMovement(xoffset, yoffset);
}


// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    gCamera.ProcessMouseScroll(yoffset);
}

// glfw: handle mouse button events
// --------------------------------
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    switch (button)
    {
    case GLFW_MOUSE_BUTTON_LEFT:
    {
        if (action == GLFW_PRESS)
            cout << "Left mouse button pressed" << endl;
        else
            cout << "Left mouse button released" << endl;
    }
    break;

    case GLFW_MOUSE_BUTTON_MIDDLE:
    {
        if (action == GLFW_PRESS)
            cout << "Middle mouse button pressed" << endl;
        else
            cout << "Middle mouse button released" << endl;
    }
    break;

    case GLFW_MOUSE_BUTTON_RIGHT:
    {
        if (action == GLFW_PRESS)
            cout << "Right mouse button pressed" << endl;
        else
            cout << "Right mouse button released" << endl;
    }
    break;

    default:
        cout << "Unhandled mouse button event" << endl;
        break;
    }
}


//// Functioned called to render a frame
void URender()
{
    Shader lightingShader("shaderfiles/6.multiple_lights.vs", "shaderfiles/6.multiple_lights.fs");
    Shader lightCubeShader("shaderfiles/6.light_cube.vs", "shaderfiles/6.light_cube.fs");

    // Enable z-depth
    glEnable(GL_DEPTH_TEST);

    // Clear the frame and z buffers
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set the shader to be used
    glUseProgram(gCubeProgramId);
    
    glm::mat4 projection;

    if (perspective)
    {
        projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
    }
    else
    {
        projection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 100.0f);
    }

    // camera/view transformation
    glm::mat4 view = gCamera.GetViewMatrix();
    
    // be sure to activate shader when setting uniforms/drawing objects
    lightingShader.use();
    lightingShader.setVec3("viewPos", gCamera.Position);
    lightingShader.setFloat("material.shininess", 32.0f);

    // Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
    // the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
    // by defining light types as classes and set their values in there, or by using a more efficient uniform approach
    // by using 'Uniform buffer objects', but that is something we'll discuss in the 'Advanced GLSL' tutorial.

    // directional light
    lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
    lightingShader.setVec3("dirLight.ambient", 0.1f, 0.1f, 0.1f);
    lightingShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
    lightingShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
    // point light 1
    lightingShader.setVec3("pointLights[0].position", gLightPosition);
    lightingShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
    lightingShader.setVec3("pointLights[0].diffuse", 0.0f, 0.0f, 0.0f);
    lightingShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
    lightingShader.setFloat("pointLights[0].constant", 1.0f);
    lightingShader.setFloat("pointLights[0].linear", 0.09);
    lightingShader.setFloat("pointLights[0].quadratic", 0.032);
    // point light 2
    lightingShader.setVec3("pointLights[1].position", gLightPosition);
    lightingShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
    lightingShader.setVec3("pointLights[1].diffuse", 0.0f, 0.0f, 0.0f);
    lightingShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
    lightingShader.setFloat("pointLights[1].constant", 1.0f);
    lightingShader.setFloat("pointLights[1].linear", 0.09);
    lightingShader.setFloat("pointLights[1].quadratic", 0.032);
    // point light 3
    lightingShader.setVec3("pointLights[2].position", gLightPosition2);
    lightingShader.setVec3("pointLights[2].ambient", 0.5f, 0.5f, 0.5f);
    lightingShader.setVec3("pointLights[2].diffuse", 2.6f, 2.6f, 2.0f);
    lightingShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
    lightingShader.setFloat("pointLights[2].constant", 1.0f);
    lightingShader.setFloat("pointLights[2].linear", 0.09);
    lightingShader.setFloat("pointLights[2].quadratic", 0.032);
    // point light 4
    lightingShader.setVec3("pointLights[3].position", gLightPosition);
    lightingShader.setVec3("pointLights[3].ambient", 0.4f, 0.4f, 0.1f);
    lightingShader.setVec3("pointLights[3].diffuse", 0.0f, 0.0f, 1.5f);
    lightingShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
    lightingShader.setFloat("pointLights[3].constant", 1.0f);
    lightingShader.setFloat("pointLights[3].linear", 0.09);
    lightingShader.setFloat("pointLights[3].quadratic", 0.032);
    // spotLight
    lightingShader.setVec3("spotLight.position", gCamera.Position);
    lightingShader.setVec3("spotLight.direction", gCamera.Front);
    lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
    lightingShader.setVec3("spotLight.diffuse", 0.0f, 0.0f, 0.0f);
    lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
    lightingShader.setFloat("spotLight.constant", 1.0f);
    lightingShader.setFloat("spotLight.linear", 0.09);
    lightingShader.setFloat("spotLight.quadratic", 0.032);
    lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
    lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

    lightingShader.setMat4("projection", projection);
    lightingShader.setMat4("view", view);

    // Monitor_1
    glm::mat4 model = glm::translate(gPosition[1]) * glm::scale(gScale[1]) * glm::rotate(0.0f, gRotation[2]);
    lightingShader.setMat4("model", model);
    // Activate the VBOs contained within the mesh's VAO
    glBindVertexArray(gMesh.vao);
    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureId);
    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, gMesh.nVertices - 6); // Draws the triangle

    // Screen texture and drawing
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gScreen1Tex);
    glDrawArrays(GL_TRIANGLES, gMesh.nVertices - 6, gMesh.nVertices); // Draws the triangle

    glBindVertexArray(0);


    //Monitor_2
    model = glm::translate(gPosition[0]) * glm::scale(gScale[1]) * glm::rotate(12.0f, gRotation[1]);
    lightingShader.setMat4("model", model);
    // Activate the VBOs contained within the mesh's VAO
    glBindVertexArray(gMesh.vao);
    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureId);
    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, gMesh.nVertices - 6); // Draws the triangle

    // Screen texture and drawing
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gScreen2Tex);
    glDrawArrays(GL_TRIANGLES, gMesh.nVertices - 6, gMesh.nVertices); // Draws the triangle

    glBindVertexArray(0);


    // Desk
    glBindVertexArray(gPlaneMesh.vao);

    model = glm::translate(gPosition[2]) * glm::scale(gScale[1]) * glm::rotate(0.0f, gRotation[1]);
    lightingShader.setMat4("model", model);

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gMonitorTex);
    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, gPlaneMesh.nVertices); // Draws the triangle
    glBindVertexArray(0);


    // Draw Walls
    glBindVertexArray(gWallMesh.vao);

    model = glm::translate(gPosition[2]) * glm::scale(gScale[1]) * glm::rotate(0.0f, gRotation[1]);
    lightingShader.setMat4("model", model);

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gWallTex);
    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, gWallMesh.nVertices); // Draws the triangle
    glBindVertexArray(0);


    // Draw Keyboard
    glBindVertexArray(gKeyboardMesh.vao);

    model = glm::translate(gPosition[3]) * glm::scale(gScale[1]) * glm::rotate(0.0f, gRotation[1]);
    lightingShader.setMat4("model", model);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureId);
    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, gKeyboardMesh.nVertices - 6); // Draws the triangle

    // Screen texture and drawing
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gKeyboardTex);
    glDrawArrays(GL_TRIANGLES, gKeyboardMesh.nVertices - 6, gKeyboardMesh.nVertices); // Draws the triangle
    glBindVertexArray(0);




    // Draw Mouse
    glBindVertexArray(gMouseMesh.vao);

    model = glm::translate(gPosition[5]) * glm::scale(gScale[1]) * glm::rotate(0.0f, gRotation[1]);
    lightingShader.setMat4("model", model);

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gMouseTex);
    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, gMouseMesh.nVertices); // Draws the triangle
    glBindVertexArray(0);

    // Draw Scroll Wheel
    glBindVertexArray(gScrollMesh.vao);

    model = glm::translate(gPosition[6]) * glm::scale(gScale[4]) * glm::rotate(1.6f, gRotation[2]);
    lightingShader.setMat4("model", model);

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gMouseTex);
    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, gScrollMesh.nVertices); // Draws the triangle
    glBindVertexArray(0);


    // Draw Astronaut
    glBindVertexArray(gAstroMesh.vao);

    model = glm::translate(gPosition[8]) * glm::scale(gScale[1]) * glm::rotate(0.0f, gRotation[1]);
    lightingShader.setMat4("model", model);

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gMouseTex);
    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, gAstroMesh.nVertices - 36); // Draws the triangle

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gAstroTex);
    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, (gAstroMesh.nVertices - 36), gAstroMesh.nVertices); // Draws the triangle
    glBindVertexArray(0);

    // Draw Helmet
    glBindVertexArray(gHelmMesh.vao);

    model = glm::translate(gPosition[8]) * glm::scale(gScale[1]) * glm::rotate(0.0f, gRotation[1]);
    lightingShader.setMat4("model", model);

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gHelmTex);
    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, gHelmMesh.nVertices); // Draws the triangle
    glBindVertexArray(0);

    // Draw Limbs
    glBindVertexArray(gLimbMesh.vao);

    model = glm::translate(gPosition[9]) * glm::scale(gScale[5]) * glm::rotate(0.0f, gRotation[1]);
    lightingShader.setMat4("model", model);

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gAstroTex);
    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, gLimbMesh.nVertices); // Draws the triangle
    glBindVertexArray(0);

    glBindVertexArray(gLimbMesh.vao);

    model = glm::translate(gPosition[10]) * glm::scale(gScale[5]) * glm::rotate(0.0f, gRotation[1]);
    lightingShader.setMat4("model", model);

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gAstroTex);
    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, gLimbMesh.nVertices); // Draws the triangle
    glBindVertexArray(0);

    glBindVertexArray(gLimbMesh.vao);

    model = glm::translate(gPosition[11]) * glm::scale(gScale[6]) * glm::rotate(0.0f, gRotation[1]);
    lightingShader.setMat4("model", model);

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gAstroTex);
    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, gLimbMesh.nVertices); // Draws the triangle
    glBindVertexArray(0);

    glBindVertexArray(gLimbMesh.vao);

    model = glm::translate(gPosition[12]) * glm::scale(gScale[6]) * glm::rotate(0.0f, gRotation[1]);
    lightingShader.setMat4("model", model);

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gAstroTex);
    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, gLimbMesh.nVertices); // Draws the triangle
    glBindVertexArray(0);


    // Draw Book
    glBindVertexArray(gBookMesh.vao);

    model = glm::translate(gPosition[4]) * glm::scale(gScale[1]) * glm::rotate(0.0f, gRotation[2]);
    lightingShader.setMat4("model", model);

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gBookTex);
    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, gBookMesh.nVertices); // Draws the triangle
    glBindVertexArray(0);

    // Draw Can
    glBindVertexArray(gCylinderMesh.vao);

    model = glm::translate(gPosition[7]) * glm::scale(gScale[3]) * glm::rotate(0.0f, gRotation[1]);
    lightingShader.setMat4("model", model);

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gCanTex);
    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, gCylinderMesh.nVertices); // Draws the triangle
    glBindVertexArray(0);




    // Deactivate Program
    glUseProgram(0);

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    glfwSwapBuffers(gWindow);    // Flips the the back buffer with the front buffer every frame.
}

// Implements the UCreateMesh function
void UPlaneCreateMesh(GLMesh& mesh)
{
    GLfloat verts[] = {
     //Plane
    -4.5f,  0.0f, -2.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
     4.5f,  0.0f, -2.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
     4.5f,  0.0f,  2.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
     4.5f,  0.0f,  2.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
    -4.5f,  0.0f,  2.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
    -4.5f,  0.0f, -2.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f
    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerColor = 2;

    mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerColor));

    glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vao);

    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerColor);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);
}

// Implements the UCreateMesh function
void UCreateMesh(GLMesh& mesh)
{
    GLfloat verts[] = {
        // positions          // normals           // texture coords
        // Back 
        -2.5f,  1.0f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         2.5f,  1.0f,  0.0f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         2.5f,  4.0f,  0.0f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         2.5f,  4.0f,  0.0f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -2.5f,  4.0f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -2.5f,  1.0f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
        // Front
        -2.5f,  1.0f,  0.1f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         2.5f,  1.0f,  0.1f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         2.5f,  4.0f,  0.1f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         2.5f,  4.0f,  0.1f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -2.5f,  4.0f,  0.1f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -2.5f,  1.0f,  0.1f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
        // Left
        -2.5f,  4.0f,  0.1f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -2.5f,  4.0f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -2.5f,  1.0f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -2.5f,  1.0f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -2.5f,  1.0f,  0.1f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -2.5f,  4.0f,  0.1f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        // Right
         2.5f,  4.0f,  0.1f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         2.5f,  4.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         2.5f,  1.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         2.5f,  1.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         2.5f,  1.0f,  0.1f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         2.5f,  4.0f,  0.1f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        // Bottom
        -2.5f,  1.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         2.5f,  1.0f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         2.5f,  1.0f,  0.1f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         2.5f,  1.0f,  0.1f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -2.5f,  1.0f,  0.1f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -2.5f,  1.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
        // Top
        -2.5f,  4.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         2.5f,  4.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         2.5f,  4.0f,  0.1f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         2.5f,  4.0f,  0.1f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -2.5f,  4.0f,  0.1f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -2.5f,  4.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,

        // STAND
        // Base
        -0.5f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.0f,  0.0f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
        // Front
        -0.5f,  0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
         0.0f,  2.5f,  0.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
        // Left
        -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.0f,  0.0f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.0f,  2.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        // Right
         0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.0f,  0.0f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.0f,  2.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        // SCREEN
        -2.45f,  1.1f,  0.101f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         2.45f,  1.1f,  0.101f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         2.45f,  3.95f,  0.101f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         2.45f,  3.95f,  0.101f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -2.45f,  3.95f,  0.101f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -2.45f,  1.1f,  0.101f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerColor = 2;

    mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerColor));

    glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vao);

    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerColor);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);
}

// Creates the mesh for the soda can
void UCylinderCreateMesh(GLMesh& mesh)
{
    GLfloat verts[] = {
        // positions          // normals           // texture coords
        // Bottom
         0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
         0.7f,  0.0f,  0.7f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
         0.7f,  0.0f,  0.7f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         1.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
         1.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.7f,  0.0f, -0.7f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
         0.7f,  0.0f, -0.7f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.0f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
         0.0f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.7f,  0.0f, -0.7f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.7f,  0.0f, -0.7f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.7f,  0.0f,  0.7f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.7f,  0.0f,  0.7f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
        //Top
         0.0f,  4.2f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
         0.7f,  4.2f,  0.7f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.0f,  4.2f,  1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.0f,  4.2f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
         0.7f,  4.2f,  0.7f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         1.0f,  4.2f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.0f,  4.2f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
         1.0f,  4.2f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.7f,  4.2f, -0.7f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.0f,  4.2f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
         0.7f,  4.2f, -0.7f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.0f,  4.2f, -1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.0f,  4.2f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
         0.0f,  4.2f, -1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.7f,  4.2f, -0.7f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.0f,  4.2f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.7f,  4.2f, -0.7f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -1.0f,  4.2f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.0f,  4.2f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -1.0f,  4.2f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.7f,  4.2f,  0.7f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.0f,  4.2f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.7f,  4.2f,  0.7f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.0f,  4.2f,  1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
        //1
         0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.0f,  4.2f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.7f,  0.0f,  0.7f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.7f,  4.2f,  0.7f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.0f,  4.2f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
         0.7f,  0.0f,  0.7f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
        //2
         1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         1.0f,  4.2f,  0.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.7f,  0.0f,  0.7f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.7f,  4.2f,  0.7f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         1.0f,  4.2f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
         0.7f,  0.0f,  0.7f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
        //3
         1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         1.0f,  4.2f,  0.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.7f,  0.0f, -0.7f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.7f,  4.2f, -0.7f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         1.0f,  4.2f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
         0.7f,  0.0f, -0.7f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
        //4
         0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.0f,  4.2f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.7f,  0.0f, -0.7f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.7f,  4.2f, -0.7f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.0f,  4.2f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
         0.7f,  0.0f, -0.7f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
        //5
         0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.0f,  4.2f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
        -0.7f,  0.0f, -0.7f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.7f,  4.2f, -0.7f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.0f,  4.2f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.7f,  0.0f, -0.7f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
        //6
        -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
        -1.0f,  4.2f,  0.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
        -0.7f,  0.0f, -0.7f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.7f,  4.2f, -0.7f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -1.0f,  4.2f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.7f,  0.0f, -0.7f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
        //7
        -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
        -1.0f,  4.2f,  0.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
        -0.7f,  0.0f,  0.7f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.7f,  4.2f,  0.7f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -1.0f,  4.2f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.7f,  0.0f,  0.7f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
        //8
         0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.0f,  4.2f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
        -0.7f,  0.0f,  0.7f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.7f,  4.2f,  0.7f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.0f,  4.2f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.7f,  0.0f,  0.7f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerColor = 2;

    mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerColor));

    glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vao);

    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerColor);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);
}

// Creates the mesh for the astronaut limbs
void ULimbCreateMesh(GLMesh& mesh)
{
    GLfloat verts[] = {
        // positions          // normals           // texture coords
        // Bottom
         0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
         0.7f,  0.0f,  0.7f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
         0.7f,  0.0f,  0.7f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         1.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
         1.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.7f,  0.0f, -0.7f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
         0.7f,  0.0f, -0.7f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.0f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
         0.0f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.7f,  0.0f, -0.7f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.7f,  0.0f, -0.7f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.7f,  0.0f,  0.7f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.7f,  0.0f,  0.7f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         //Top
         0.0f,  4.2f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
         0.7f,  4.2f,  0.7f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.0f,  4.2f,  1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.0f,  4.2f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
         0.7f,  4.2f,  0.7f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         1.0f,  4.2f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.0f,  4.2f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
         1.0f,  4.2f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.7f,  4.2f, -0.7f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.0f,  4.2f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
         0.7f,  4.2f, -0.7f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.0f,  4.2f, -1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.0f,  4.2f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
         0.0f,  4.2f, -1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.7f,  4.2f, -0.7f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.0f,  4.2f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.7f,  4.2f, -0.7f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -1.0f,  4.2f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.0f,  4.2f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -1.0f,  4.2f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.7f,  4.2f,  0.7f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.0f,  4.2f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.7f,  4.2f,  0.7f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.0f,  4.2f,  1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
        //1
         0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.0f,  4.2f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.7f,  0.0f,  0.7f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.7f,  4.2f,  0.7f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.0f,  4.2f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
         0.7f,  0.0f,  0.7f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
        //2
         1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         1.0f,  4.2f,  0.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.7f,  0.0f,  0.7f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.7f,  4.2f,  0.7f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         1.0f,  4.2f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
         0.7f,  0.0f,  0.7f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
        //3
         1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         1.0f,  4.2f,  0.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.7f,  0.0f, -0.7f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.7f,  4.2f, -0.7f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         1.0f,  4.2f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
         0.7f,  0.0f, -0.7f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
        //4
         0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.0f,  4.2f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.7f,  0.0f, -0.7f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.7f,  4.2f, -0.7f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.0f,  4.2f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
         0.7f,  0.0f, -0.7f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
        //5
         0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.0f,  4.2f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
        -0.7f,  0.0f, -0.7f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.7f,  4.2f, -0.7f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.0f,  4.2f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.7f,  0.0f, -0.7f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
       //6
        -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
        -1.0f,  4.2f,  0.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
        -0.7f,  0.0f, -0.7f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.7f,  4.2f, -0.7f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -1.0f,  4.2f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.7f,  0.0f, -0.7f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
       //7
        -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
        -1.0f,  4.2f,  0.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
        -0.7f,  0.0f,  0.7f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.7f,  4.2f,  0.7f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -1.0f,  4.2f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.7f,  0.0f,  0.7f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
       //8
         0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.0f,  4.2f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
        -0.7f,  0.0f,  0.7f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.7f,  4.2f,  0.7f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.0f,  4.2f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.7f,  0.0f,  0.7f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerColor = 2;

    mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerColor));

    glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vao);

    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerColor);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);
}

//Creates the mesh for the scroll wheel
void UScrollCreateMesh(GLMesh& mesh)
{
    GLfloat verts[] = {
        // positions          // normals           // texture coords
        // Bottom
         0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
         0.7f,  0.0f,  0.7f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
         0.7f,  0.0f,  0.7f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         1.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
         1.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.7f,  0.0f, -0.7f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
         0.7f,  0.0f, -0.7f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.0f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
         0.0f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.7f,  0.0f, -0.7f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.7f,  0.0f, -0.7f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.7f,  0.0f,  0.7f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.7f,  0.0f,  0.7f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
        //Top
         0.0f,  0.8f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
         0.7f,  0.8f,  0.7f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.0f,  0.8f,  1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.0f,  0.8f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
         0.7f,  0.8f,  0.7f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         1.0f,  0.8f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.0f,  0.8f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
         1.0f,  0.8f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.7f,  0.8f, -0.7f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.0f,  0.8f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
         0.7f,  0.8f, -0.7f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.0f,  0.8f, -1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.0f,  0.8f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
         0.0f,  0.8f, -1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.7f,  0.8f, -0.7f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.0f,  0.8f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.7f,  0.8f, -0.7f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -1.0f,  0.8f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.0f,  0.8f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -1.0f,  0.8f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.7f,  0.8f,  0.7f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.0f,  0.8f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.7f,  0.8f,  0.7f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.0f,  0.8f,  1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
        //1
         0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.0f,  0.8f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.7f,  0.0f,  0.7f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.7f,  0.8f,  0.7f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.0f,  0.8f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
         0.7f,  0.0f,  0.7f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
        //2
         1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         1.0f,  0.8f,  0.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.7f,  0.0f,  0.7f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.7f,  0.8f,  0.7f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         1.0f,  0.8f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
         0.7f,  0.0f,  0.7f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
        //3
         1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         1.0f,  0.8f,  0.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.7f,  0.0f, -0.7f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.7f,  0.8f, -0.7f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         1.0f,  0.8f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
         0.7f,  0.0f, -0.7f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
        //4
         0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.0f,  0.8f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.7f,  0.0f, -0.7f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.7f,  0.8f, -0.7f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.0f,  0.8f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
         0.7f,  0.0f, -0.7f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
        //5
         0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.0f,  0.8f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
        -0.7f,  0.0f, -0.7f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.7f,  0.8f, -0.7f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.0f,  0.8f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.7f,  0.0f, -0.7f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
        //6
        -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
        -1.0f,  0.8f,  0.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
        -0.7f,  0.0f, -0.7f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.7f,  0.8f, -0.7f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -1.0f,  0.8f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.7f,  0.0f, -0.7f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
        //7
        -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
        -1.0f,  0.8f,  0.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
        -0.7f,  0.0f,  0.7f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.7f,  0.8f,  0.7f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -1.0f,  0.8f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.7f,  0.0f,  0.7f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
        //8
         0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.0f,  0.8f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
        -0.7f,  0.0f,  0.7f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.7f,  0.8f,  0.7f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.0f,  0.8f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.7f,  0.0f,  0.7f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerColor = 2;

    mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerColor));

    glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vao);

    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerColor);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);
}

// Implements the UCreateMesh function
void UKeyboardCreateMesh(GLMesh& mesh)
{
    GLfloat verts[] = {
        // positions          // normals           // texture coords
        // Back 
        -2.15f,  0.0f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         2.15f,  0.0f,  0.0f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         2.15f,  0.15f,  0.0f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         2.15f,  0.15f,  0.0f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -2.15f,  0.15f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -2.15f,  0.0f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
        // Front
        -2.15f,  0.0f,  1.3f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         2.15f,  0.0f,  1.3f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         2.15f,  0.15f,  1.3f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         2.15f,  0.15f,  1.3f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -2.15f,  0.15f,  1.3f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -2.15f,  0.0f,  1.3f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
        // Left
        -2.15f,  0.15f,  1.3f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -2.15f,  0.15f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -2.15f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -2.15f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -2.15f,  0.0f,  1.3f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -2.15f,  0.15f,  1.3f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        // Right
         2.15f,  0.15f,  1.3f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         2.15f,  0.15f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         2.15f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         2.15f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         2.15f,  0.0f,  1.3f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         2.15f,  0.15f,  1.3f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        // Bottom
        -2.15f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         2.15f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         2.15f,  0.0f,  1.3f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         2.15f,  0.0f,  1.3f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -2.15f,  0.0f,  1.3f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -2.15f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
       // Top
        -2.15f,  0.15f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         2.15f,  0.15f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         2.15f,  0.15f,  1.3f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         2.15f,  0.15f,  1.3f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -2.15f,  0.15f,  1.3f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -2.15f,  0.15f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerColor = 2;

    mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerColor));

    glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vao);

    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerColor);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);
}

// Implements the UCreateMesh function
void UBookCreateMesh(GLMesh& mesh)
{
    GLfloat verts[] = {
        // positions          // normals           // texture coords
        // Back 
        -1.2f,  0.0f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         1.2f,  0.0f,  0.0f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         1.2f,  0.5f,  0.0f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         1.2f,  0.5f,  0.0f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -1.2f,  0.5f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -1.2f,  0.0f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
        // Front
        -1.2f,  0.0f,  3.7f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         1.2f,  0.0f,  3.7f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         1.2f,  0.5f,  3.7f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         1.2f,  0.5f,  3.7f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -1.2f,  0.5f,  3.7f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -1.2f,  0.0f,  3.7f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
        // Left
        -1.2f,  0.5f,  3.7f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -1.2f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -1.2f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -1.2f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -1.2f,  0.0f,  3.7f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -1.2f,  0.5f,  3.7f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        // Right
         1.2f,  0.5f,  3.7f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         1.2f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         1.2f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         1.2f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         1.2f,  0.0f,  3.7f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         1.2f,  0.5f,  3.7f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        // Bottom
        -1.2f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         1.2f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         1.2f,  0.0f,  3.7f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         1.2f,  0.0f,  3.7f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -1.2f,  0.0f,  3.7f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -1.2f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
        // Top
        -1.2f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         1.2f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         1.2f,  0.5f,  3.7f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         1.2f,  0.5f,  3.7f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -1.2f,  0.5f,  3.7f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -1.2f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerColor = 2;

    mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerColor));

    glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vao);

    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerColor);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);
}

// Implements the UCreateMesh function
void UWallCreateMesh(GLMesh& mesh)
{
    GLfloat verts[] = {
        // Back Wall
        -4.5f,  0.0f, -2.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         4.5f,  0.0f, -2.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         4.5f,  6.0f, -2.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         4.5f,  6.0f, -2.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -4.5f,  6.0f, -2.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -4.5f,  0.0f, -2.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
        // Left Wall
        -4.5f,  6.0f,  2.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -4.5f,  6.0f, -2.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -4.5f,  0.0f, -2.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -4.5f,  0.0f, -2.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -4.5f,  0.0f,  2.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -4.5f,  6.0f,  2.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerColor = 2;

    mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerColor));

    glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vao);

    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerColor);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);
}

// Create the mesh for the mouse
void UMouseCreateMesh(GLMesh& mesh)
{
    GLfloat verts[] = {
        // positions          // normals           // texture coords
        // Back 
        -0.2f,  0.0f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.2f,  0.0f,  0.0f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.2f,  0.2f,  0.0f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.2f,  0.2f,  0.0f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.2f,  0.2f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.2f,  0.0f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
        // Front
        -0.2f,  0.0f,  0.8f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.2f,  0.0f,  0.8f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.2f,  0.2f,  0.8f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.2f,  0.2f,  0.8f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.2f,  0.2f,  0.8f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.2f,  0.0f,  0.8f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
        // Left
        -0.2f,  0.2f,  0.8f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.2f,  0.2f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.2f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.2f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.2f,  0.0f,  0.8f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.2f,  0.2f,  0.8f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        // Right
         0.2f,  0.2f,  0.8f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.2f,  0.2f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.2f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.2f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.2f,  0.0f,  0.8f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.2f,  0.2f,  0.8f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        // Bottom
        -0.2f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.2f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.2f,  0.0f,  0.8f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.2f,  0.0f,  0.8f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.2f,  0.0f,  0.8f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.2f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
        // Top
        -0.2f,  0.2f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.2f,  0.2f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.2f,  0.2f,  0.8f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.2f,  0.2f,  0.8f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.2f,  0.2f,  0.8f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.2f,  0.2f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerColor = 2;

    mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerColor));

    glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vao);

    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerColor);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);
}

// Create mesh for the astronaught torso and stand
void UAstroCreateMesh(GLMesh& mesh)
{
    GLfloat verts[] = {
        // positions          // normals           // texture coords
        // BASE **********************************************
        // Bottom
        -0.2f,  0.0f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.2f,  0.0f,  0.0f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.2f,  0.1f,  0.0f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.2f,  0.1f,  0.0f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.2f,  0.1f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.2f,  0.0f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
        // Front
        -0.2f,  0.0f,  0.4f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.2f,  0.0f,  0.4f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.2f,  0.1f,  0.4f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.2f,  0.1f,  0.4f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.2f,  0.1f,  0.4f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.2f,  0.0f,  0.4f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
        // Left
        -0.2f,  0.1f,  0.4f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.2f,  0.1f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.2f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.2f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.2f,  0.0f,  0.4f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.2f,  0.1f,  0.4f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        // Right
         0.2f,  0.1f,  0.4f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.2f,  0.1f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.2f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.2f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.2f,  0.0f,  0.4f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.2f,  0.1f,  0.4f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        // Bottom
        -0.2f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.2f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.2f,  0.0f,  0.4f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.2f,  0.0f,  0.4f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.2f,  0.0f,  0.4f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.2f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
        // Top
        -0.2f,  0.1f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.2f,  0.1f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.2f,  0.1f,  0.4f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.2f,  0.1f,  0.4f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.2f,  0.1f,  0.4f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.2f,  0.1f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,

        // HOLDER ********************************************
        // Bottom
        -0.05f,  0.0f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.05f,  0.0f,  0.0f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.05f,  0.8f,  0.0f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.05f,  0.8f,  0.0f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.05f,  0.8f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.05f,  0.0f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
        // Front
        -0.05f,  0.0f,  0.1f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.05f,  0.0f,  0.1f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.05f,  0.8f,  0.1f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.05f,  0.8f,  0.1f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.05f,  0.8f,  0.1f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.05f,  0.0f,  0.1f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
        // Left
        -0.05f,  0.8f,  0.1f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.05f,  0.8f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.05f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.05f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.05f,  0.0f,  0.1f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.05f,  0.8f,  0.1f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        // Right
         0.05f,  0.8f,  0.1f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.05f,  0.8f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.05f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.05f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.05f,  0.0f,  0.1f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.05f,  0.8f,  0.1f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        // Bottom
        -0.05f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.05f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.05f,  0.0f,  0.1f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.05f,  0.0f,  0.1f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.05f,  0.0f,  0.1f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.05f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
        // Top
        -0.05f,  0.8f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.05f,  0.8f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.05f,  0.8f,  0.1f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.05f,  0.8f,  0.1f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.05f,  0.8f,  0.1f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.05f,  0.8f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,

        // TORSO *********************************************
        // Bottom
        -0.1f,  0.5f,  0.1f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.1f,  0.5f,  0.1f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.1f,  0.8f,  0.1f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.1f,  0.8f,  0.1f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.1f,  0.8f,  0.1f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.1f,  0.5f,  0.1f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
        // Front
        -0.1f,  0.5f,  0.2f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.1f,  0.5f,  0.2f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.1f,  0.8f,  0.2f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.1f,  0.8f,  0.2f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.1f,  0.8f,  0.2f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.1f,  0.5f,  0.2f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
        // Left
        -0.1f,  0.8f,  0.2f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.1f,  0.8f,  0.1f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.1f,  0.5f,  0.1f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.1f,  0.5f,  0.1f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.1f,  0.5f,  0.2f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.1f,  0.8f,  0.2f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        // Right
         0.1f,  0.8f,  0.2f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.1f,  0.8f,  0.1f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.1f,  0.5f,  0.1f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.1f,  0.5f,  0.1f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.1f,  0.5f,  0.2f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.1f,  0.8f,  0.2f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        // Bottom
        -0.1f,  0.5f,  0.1f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.1f,  0.5f,  0.1f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.1f,  0.5f,  0.2f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.1f,  0.5f,  0.2f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.1f,  0.5f,  0.2f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.1f,  0.5f,  0.1f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
        // Top
        -0.1f,  0.8f,  0.1f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.1f,  0.8f,  0.1f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.1f,  0.8f,  0.2f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.1f,  0.8f,  0.2f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.1f,  0.8f,  0.2f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.1f,  0.8f,  0.1f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerColor = 2;

    mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerColor));

    glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vao);

    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerColor);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);
}

// Create the mesh for the astronaut helmet
void UHelmCreateMesh(GLMesh& mesh)
{
    GLfloat verts[] = {
    // HEAD **********************************************
    // Bottom
    -0.1f, 0.8f, 0.05f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        0.1f, 0.8f, 0.05f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
        0.1f, 1.0f, 0.05f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        0.1f, 1.0f, 0.05f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        -0.1f, 1.0f, 0.05f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
        -0.1f, 0.8f, 0.05f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        // Front
        -0.1f, 0.8f, 0.25f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.1f, 0.8f, 0.25f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        0.1f, 1.0f, 0.25f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.1f, 1.0f, 0.25f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        -0.1f, 1.0f, 0.25f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        -0.1f, 0.8f, 0.25f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        // Left
        -0.1f, 1.0f, 0.25f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.1f, 1.0f, 0.05f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -0.1f, 0.8f, 0.05f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.1f, 0.8f, 0.05f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.1f, 0.8f, 0.25f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        -0.1f, 1.0f, 0.25f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        // Right
        0.1f, 1.0f, 0.25f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.1f, 1.0f, 0.05f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.1f, 0.8f, 0.05f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.1f, 0.8f, 0.05f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.1f, 0.8f, 0.25f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.1f, 1.0f, 0.25f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        // Bottom
        -0.1f, 0.8f, 0.05f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
        0.1f, 0.8f, 0.05f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
        0.1f, 0.8f, 0.25f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        0.1f, 0.8f, 0.25f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        -0.1f, 0.8f, 0.25f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        -0.1f, 0.8f, 0.05f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
        // Top
        -0.1f, 1.0f, 0.05f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.1f, 1.0f, 0.05f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        0.1f, 1.0f, 0.25f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.1f, 1.0f, 0.25f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -0.1f, 1.0f, 0.25f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        -0.1f, 1.0f, 0.05f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerColor = 2;

    mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerColor));

    glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vao);

    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerColor);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);
}

// Release mesh
void UDestroyMesh(GLMesh& mesh)
{
    glDeleteVertexArrays(1, &mesh.vao);
    glDeleteBuffers(1, &mesh.vbo);
}

/*Generate and load the texture*/
bool UCreateTexture(const char* filename, GLuint& textureId)
{
    int width, height, channels;
    unsigned char* image = stbi_load(filename, &width, &height, &channels, 0);
    if (image)
    {
        flipImageVertically(image, width, height, channels);

        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);

        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        if (channels == 3)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        else if (channels == 4)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        else
        {
            cout << "Not implemented to handle image with " << channels << " channels" << endl;
            return false;
        }

        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(image);
        glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

        return true;
    }

    // Error loading the image
    return false;
}


void UDestroyTexture(GLuint textureId)
{
    glGenTextures(1, &textureId);
}


// Implements the UCreateShaders function
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId)
{
    // Compilation and linkage error reporting
    int success = 0;
    char infoLog[512];

    // Create a Shader program object.
    programId = glCreateProgram();

    // Create the vertex and fragment shader objects
    GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

    // Retrive the shader source
    glShaderSource(vertexShaderId, 1, &vtxShaderSource, NULL);
    glShaderSource(fragmentShaderId, 1, &fragShaderSource, NULL);

    // Compile the vertex shader, and print compilation errors (if any)
    glCompileShader(vertexShaderId); // compile the vertex shader
    // check for shader compile errors
    glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShaderId, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;

        return false;
    }

    glCompileShader(fragmentShaderId); // compile the fragment shader
    // check for shader compile errors
    glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShaderId, sizeof(infoLog), NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;

        return false;
    }

    // Attached compiled shaders to the shader program
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);

    glLinkProgram(programId);   // links the shader program
    // check for linking errors
    glGetProgramiv(programId, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(programId, sizeof(infoLog), NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;

        return false;
    }

    glUseProgram(programId);    // Uses the shader program

    return true;
}

// Release shader program
void UDestroyShaderProgram(GLuint programId)
{
    glDeleteProgram(programId);
}
