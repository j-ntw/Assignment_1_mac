////////////////////////////////////////////////////////////////////////
//
//
//  Assignment 1 of SUTD Course 50.017
//
//    Mesh Viewer
//
//
//
////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include "shaderSource.h"
#include "shader.h"

using namespace std;

#define MAX_BUFFER_SIZE 1024

#define _ROTATE_FACTOR 0.005f
#define _SCALE_FACTOR 0.01f
#define _TRANS_FACTOR 0.02f

#define _Z_NEAR 0.001f
#define _Z_FAR 100.0f

/***********************************************************************/
/**************************   global variables   ***********************/
/***********************************************************************/

// Window size
unsigned int winWidth = 800;
unsigned int winHeight = 600;

// Camera
glm::vec3 camera_position = glm::vec3(0.0f, 0.0f, 15.0f);
glm::vec3 camera_target = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 camera_up = glm::vec3(0.0f, 1.0f, 0.0f);
float camera_fovy = 45.0f;
glm::mat4 projection;

// Mouse interaction
bool leftMouseButtonHold = false;
bool isFirstMouse = true;
float prevMouseX;
float prevMouseY;
glm::mat4 modelMatrix = glm::mat4(1.0f);

// Mesh color table
glm::vec3 colorTable[4] =
    {
        glm::vec3(0.6, 1.0, 0.6),
        glm::vec3(1.0, 0.6, 0.6),
        glm::vec3(0.6, 0.6, 1.0),
        glm::vec3(1.0, 1.0, 0.6)};

// Mesh rendering color
int colorID = 0;
glm::vec3 meshColor;

// declaration
void processInput(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
void cursor_pos_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

/******************************************************************************/
/***************   Functions to be filled in for Assignment 1    **************/
/***************    IMPORTANT: you ONLY need to work on these    **************/
/***************                functions in this section        **************/
/******************************************************************************/

// TODO: insert your code in this function for Mesh Loading
//       1) store vertices and normals in verList with order (v.x, v.y, v.z, n.x, n.y, n.z)
//       2) store vertex indices of each triangle in triList
int LoadInput(vector<float> &verList, vector<unsigned> &triList)
{
    // read input file
    string inputfile;
    cout << "Please enter the input file name: ";
    cin >> inputfile;

    // concatenate the file path
    string filepath = "../data/" + inputfile;
    ifstream file(filepath);
    if (!file.is_open())
    {
        cout << "Error: File not found" << endl;
        return -1;
    }

    // print file content
    string line;
    vector<float> vList;              // internal storage for vertices
    vector<float> vnList;             // internal storage for normals
    unordered_map<int, int> v_vn_map; // internal hashmap for indices of vertices and normals
    while (getline(file, line))
    {
        // get normals in vnlist
        if (line[0] == 'v' && line[1] == 'n')
        {
            stringstream ss(line);
            string v;
            float x, y, z;
            ss >> v >> x >> y >> z;
            vnList.push_back(x);
            vnList.push_back(y);
            vnList.push_back(z);
        }
        // starts with v, push floats to vList
        else if (line[0] == 'v' && line[1] == ' ')
        {
            stringstream ss(line);
            string v;
            float x, y, z;
            ss >> v >> x >> y >> z;
            vList.push_back(x);
            vList.push_back(y);
            vList.push_back(z);
        }
        // starts with f, push indices to triList
        else if (line[0] == 'f' && line[1] == ' ')
        {
            // f a/b/c d/e/f g/h/i
            // we store a, d and g in triList
            // and store a: c, d: f, g: i in v_vn_map

            // remove f
            stringstream ss(line.substr(2));

            string abc;
            unsigned int a, b, c;
            char delimiter = '/';

            while (ss >> abc)
            {
                // get first number, ignore b, get third number
                stringstream(abc) >> a >> delimiter >> b >> delimiter >> c;
                triList.push_back(a - 1);
                v_vn_map[a - 1] = c - 1;
            }
        }
    }
    // combine vList and vnList to verList using triList
    for (int i = 0; i < vList.size(); i += 3)
    {
        // use the vertex index from vList to get the vertex from vList
        verList.push_back(vList[i]);
        verList.push_back(vList[i + 1]);
        verList.push_back(vList[i + 2]);

        // use the vertex index to get the normal index from v_vn_map
        // then use the normal index to get the normal from vnList
        // iterate over vertex index, since verList is ordered by vertex index.
        verList.push_back(vnList[v_vn_map[i]]);
        verList.push_back(vnList[v_vn_map[i + 1]]);
        verList.push_back(vnList[v_vn_map[i + 2]]);
    }

    file.close();
    // print verList and triList into output file
    ofstream outputfile;
    outputfile.open("../data/output.txt");
    for (int i = 0; i < verList.size(); i += 6)
    {
        outputfile << "v, vn " << verList[i] << " " << verList[i + 1] << " " << verList[i + 2] << " " << verList[i + 3] << " " << verList[i + 4] << " " << verList[i + 5] << endl;
    }
    for (int i = 0; i < triList.size(); i += 3)
    {
        outputfile << "f " << triList[i] << " " << triList[i + 1] << " " << triList[i + 2] << endl;
    }
    outputfile.close();
    return 0;
}

// TODO: insert your code in this function for Mesh Coloring
void SetMeshColor(int &colorID)
{
    colorID = (colorID + 1) % 4;
}

// TODO: insert your code in this function for Mesh Transformation (Rotation)
void RotateModel(float angle, glm::vec3 axis)
{
    modelMatrix = glm::rotate(modelMatrix, angle, axis);
}

// TODO: insert your code in this function for Mesh Transformation (Translation)
void TranslateModel(glm::vec3 transVec)
{
    modelMatrix = glm::translate(modelMatrix, transVec);
}

// TODO: insert your code in this function for Mesh Transformation (Scaling)
void ScaleModel(float scale)
{
    modelMatrix = glm::scale(modelMatrix, glm::vec3(scale, scale, scale));
}

/******************************************************************************/
/***************                  Callback Function              **************/
/******************************************************************************/

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.

    glViewport(0, 0, width, height);

    winWidth = width;
    winHeight = height;
}

// glfw: whenever a key is pressed, this callback is called
// ----------------------------------------------------------------------
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_C && action == GLFW_PRESS)
    {
        SetMeshColor(colorID);
    }
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        leftMouseButtonHold = true;
    }
    else
    {
        leftMouseButtonHold = false;
    }
}

// glfw: whenever the cursor moves, this callback is called
// -------------------------------------------------------
void cursor_pos_callback(GLFWwindow *window, double mouseX, double mouseY)
{
    float dx, dy;
    float nx, ny, scale, angle;

    if (leftMouseButtonHold)
    {
        if (isFirstMouse)
        {
            prevMouseX = mouseX;
            prevMouseY = mouseY;
            isFirstMouse = false;
        }

        else
        {
            if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            {
                float dx = _TRANS_FACTOR * (mouseX - prevMouseX);
                float dy = -1.0f * _TRANS_FACTOR * (mouseY - prevMouseY); // reversed since y-coordinates go from bottom to top

                prevMouseX = mouseX;
                prevMouseY = mouseY;

                TranslateModel(glm::vec3(dx, dy, 0));
            }

            else
            {
                float dx = mouseX - prevMouseX;
                float dy = -(mouseY - prevMouseY); // reversed since y-coordinates go from bottom to top

                prevMouseX = mouseX;
                prevMouseY = mouseY;

                // Rotation
                nx = -dy;
                ny = dx;
                scale = sqrt(nx * nx + ny * ny);

                // We use "ArcBall Rotation" to compute the rotation axis and angle based on the mouse motion
                nx = nx / scale;
                ny = ny / scale;
                angle = scale * _ROTATE_FACTOR;

                RotateModel(angle, glm::vec3(nx, ny, 0.0f));
            }
        }
    }

    else
    {
        isFirstMouse = true;
    }
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xOffset, double yOffset)
{
    float scale = 1.0f + _SCALE_FACTOR * yOffset;

    ScaleModel(scale);
}

/******************************************************************************/
/***************                    Main Function                **************/
/******************************************************************************/

int main()
{
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
    GLFWwindow *window = glfwCreateWindow(winWidth, winHeight, "Assignment 1 - Mesh Viewer", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    shader myShader;
    myShader.setUpShader(vertexShaderSource, fragmentShaderSource);

    // Load input mesh data
    vector<float> verList;    // This is the list of vertices and normals for rendering
    vector<unsigned> triList; // This is the list of faces for rendering
    LoadInput(verList, triList);

    // create buffers/arrays
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);

    // load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, verList.size() * sizeof(float), &verList[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, triList.size() * sizeof(unsigned int), &triList[0], GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // vertex Positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // vertex normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), ((void *)(3 * sizeof(float))));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // as we only have a single shader, we could also just activate our shader once beforehand if we want to
    myShader.use();

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.95f, 0.95f, 0.95f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // view/projection transformations
        projection = glm::perspective(glm::radians(camera_fovy), (float)winWidth / (float)winHeight, _Z_NEAR, _Z_FAR);
        glm::mat4 view = glm::lookAt(camera_position, camera_target, camera_up);

        glUniformMatrix4fv(glGetUniformLocation(myShader.ID, "projection"), 1, GL_FALSE, &projection[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(myShader.ID, "view"), 1, GL_FALSE, &view[0][0]);

        // render the loaded model
        // glm::vec3 aColor = glm::vec3 (0.6f, 1.0f, 0.6f);
        glUniformMatrix4fv(glGetUniformLocation(myShader.ID, "model"), 1, GL_FALSE, &modelMatrix[0][0]);
        glUniform3fv(glGetUniformLocation(myShader.ID, "meshColor"), 1, &colorTable[colorID][0]);
        glUniform3fv(glGetUniformLocation(myShader.ID, "viewPos"), 1, &camera_position[0]);

        // render the triangle
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, triList.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(myShader.ID);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}
