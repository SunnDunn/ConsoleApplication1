# define _USE_MATH_DEFINES

# include <string> 
# include <iostream>
# include <glad/glad.h>
# include <GLFW/glfw3.h>
# include <glm/glm.hpp>
# include <glm/gtc/matrix_transform.hpp>
# include <glm/gtc/type_ptr.hpp>

# define TINYOBJLOADER_IMPLEMENTATION
# include "tiny_obj_loader.h"

float x_mod = 0;
float z_mod = -2.f;

glm::mat4 identity_matrix = glm::mat4(1.0f);

float x = 0.f, y = 0.f, z = -5.f;
float scale_x = 1.0f, scale_y = 1.0f, scale_z = 1.0f;
float theta = 90.0f;
float axis_x = 0.f, axis_y = 1.0f, axis_z = 0.f;

void Key_Callback(GLFWwindow* window, //pointer to the window
                  int key, //Keycode of the press
                  int scancode, //Physical Position of the press
                  int action, //Either Press / Release
                  int mods) //Which modifier keys is held down
{
    //When user presses D
    if (key == GLFW_KEY_D && action == GLFW_PRESS)
    {
        //Move bunny to right
        x_mod += 20.f;
    }

    if (key == GLFW_KEY_A && action == GLFW_PRESS)
    {
        //Move bunny to right
        x_mod -= 20.f;
    }

    if (key == GLFW_KEY_W && action == GLFW_PRESS)
    {
        z_mod -= 0.3f;
    }

    if (key == GLFW_KEY_S && action == GLFW_PRESS)
    {
        z_mod += 0.3f;
    }
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    float window_width = 600.f;
    float window_height = 600.f;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(window_width, window_height, "Atasha Limcuando", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    gladLoadGL();

    //glViewport(0, 0, 300, 600);

    glfwSetKeyCallback(window, Key_Callback);

    //Load the shader file in to a string stream
    std::fstream vertSrc("Shaders/sample.vert");
    std::stringstream vertBuff;
    //Add the file stream to the string stream
    vertBuff << vertSrc.rdbuf();
    //Convert the stream to a character array
    std::string vertS = vertBuff.str();
    const char* v = vertS.c_str();

    //Load the fragment file in to a string stream
    std::fstream fragSrc("Shaders/sample.frag");
    std::stringstream fragBuff;
    //Add the file stream to the string stream
    fragBuff << fragSrc.rdbuf();
    //Convert the stream to a character array
    std::string fragS = fragBuff.str();
    const char* f = fragS.c_str();

    //Create a vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    //Assign the source to the vertex shader
    glShaderSource(vertexShader, 1, &v, NULL);
    //Compile the vertex shader
    glCompileShader(vertexShader);

    //Create a fragment shader
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    //Assign the source to the fragment shader
    glShaderSource(fragShader, 1, &f, NULL);
    //Compile the vertex shader
    glCompileShader(fragShader);

    //Create the shader program
    GLuint shaderProg = glCreateProgram();
    //Attach the compiled vertex shader
    glAttachShader(shaderProg, vertexShader);
    //Attach the compiled fragment shader
    glAttachShader(shaderProg, fragShader);

    glLinkProgram(shaderProg);

    GLint isCompiled = 0;
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(fragShader, maxLength, &maxLength, &errorLog[0]);

        // Provide the infolog in whatever manor you deem best.
        // Exit with failure.
        glDeleteShader(fragShader); // Don't leak the shader.

        std::cout << errorLog[0];

        return 0;
    }

    std::string path = "3D/bunny.obj";
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> material;
    std::string warning, error;

    tinyobj::attrib_t attributes;

    bool success = tinyobj::LoadObj(
        &attributes,
        &shapes,
        &material,
        &warning,
        &error,
        path.c_str()
    );

    std::vector<GLuint> mesh_indices;

    if (success) {
        for (int i = 0; i < shapes[0].mesh.indices.size(); i++)
        {
            mesh_indices.push_back(shapes[0].mesh.indices[i].vertex_index);
        }
    }
    else {
        std::cout << "Failed to load" << std::endl;
    }

    GLfloat vertices[]{
        //x   y    z
        0, 0.5f, 0.f, //0
        -0.5f, 0-.5f, 0.f, //1
        0.5f, -0.5f, 0.f //2
    };

    GLuint indices[]{
        0,1,2
    };

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    //Currently editing VAO = NULL
    glBindVertexArray(VAO);
    //Currently editing VBO = NULL
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
 
    glBufferData(GL_ARRAY_BUFFER, 
        sizeof(GL_FLOAT) * attributes.vertices.size(), //bytes
        attributes.vertices.data(), //array == &attributes.vertices[0]
        GL_STATIC_DRAW);

    glVertexAttribPointer(
        0, //0 pos, 1 tex, 2 norms
        3, //xyz
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(float),
        (void*)0
    );

    //VAO <- VBO

    //Currently editing VBO = VBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * mesh_indices.size(), mesh_indices.data(), GL_STATIC_DRAW);
    //Currently editing VBO = EBO

    //VAO <- VBO
    //    <- EBO

    glEnableVertexAttribArray(0);

    //Currently editing VBO = VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //Currently editing VBO = NULL

    //Currently editing VAO = VAO
    glBindVertexArray(0);
    //Currently editing VAO = NULL

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    //glm::mat4 projectionMatrix = glm::ortho(
    //    -4.f, //left
    //    4.f, //right
    //    -4.f, //bot
    //    4.0f, //top
    //    -1.0f, //znear
    //    1.0f //zfar
    //);

    glm::mat4 projectionMatrix = glm::perspective(
        glm::radians(60.f), //fov zoom in is -FOV, zoom out is +FOV
        window_height / window_width, //aspect ratio
        0.1f, //znear should never be 0
        100.f //zfar
    );

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        z = z_mod;
        theta = x_mod;

        //Start with the translation matrix
        glm::mat4 transformation_matrix = glm::translate(
            identity_matrix,
            glm::vec3(x, y, z)
        );

        //Multiply the resulting matrix with the scale matrix
        transformation_matrix = glm::scale(
            transformation_matrix,
            glm::vec3(scale_x, scale_y, scale_z)
        );

        //Finally multiply it with the roation matrix
        transformation_matrix = glm::rotate(
            transformation_matrix,
            glm::radians(theta),
            glm::normalize(glm::vec3(axis_x, axis_y, axis_z))
        );

        //setting the projection
        unsigned int projectionLoc = glGetUniformLocation(shaderProg, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

        //setting the transformation
        //Get the location of the trnasform variable in the shader
        unsigned int transformLoc = glGetUniformLocation(shaderProg, "transform");
        //Assign the matrix
        glUniformMatrix4fv(transformLoc, //Addess of the transform variable
                           1, //How many matrixes to assign
                           GL_FALSE, //Transpose?
                           glm::value_ptr(transformation_matrix)); //Pointer to the matrix;

        glUseProgram(shaderProg);
        glBindVertexArray(VAO);
        //glDrawArrays(GL_TRIANGLES, 0, 3); //polygon, index, numpoints
        glDrawElements(GL_TRIANGLES, mesh_indices.size(), GL_UNSIGNED_INT, 0);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VBO);
    glDeleteBuffers(1, &VAO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}