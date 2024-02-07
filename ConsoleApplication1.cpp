# include <string> 
# include <iostream>
# include <glad/glad.h>
# include <GLFW/glfw3.h>
# include <glm/glm.hpp>
# include <glm/gtc/matrix_transform.hpp>
# include <glm/gtc/type_ptr.hpp>

# define TINYOBJLOADER_IMPLEMENTATION
# include "tiny_obj_loader.h"

# define STB_IMAGE_IMPLEMENTATION
# include "stb_image.h"

float x_mod = 0;
float z_mod = -2.f;

glm::mat4 identity_matrix = glm::mat4(1.0f);

float x = 0.f, y = 0.f, z = -5.f;
float scale_x = 3.0f, scale_y = 3.0f, scale_z = 3.0f;
float theta = 90.0f;
float axis_x = 0.f, axis_y = 1.0f, axis_z = 1.0f;

void Key_Callback(GLFWwindow* window, //pointer to the window
    int key, //Keycode of the press
    int scancode, //Physical Position of the press
    int action, //Either Press / Release
    int mods) //Which modifier keys is held down
{
    //When user presses D
    if (key == GLFW_KEY_D)
    {
        //Move bunny to right
        x_mod += 30.f;
    }

    if (key == GLFW_KEY_A)
    {
        //Move bunny to right
        x_mod -= 30.f;
    }

    if (key == GLFW_KEY_W)
    {
        z_mod -= 1.f;
    }

    if (key == GLFW_KEY_S)
    {
        z_mod += 1.f;
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

    int img_width, //width of the image
        img_height, //height of the image
        colorChannels; //number of color channels

    stbi_set_flip_vertically_on_load(true);

    unsigned char* tex_bytes =
        stbi_load("3D/ayaya.png", //texture path
            &img_width, //fills out the width
            &img_height, //fills out the height
            &colorChannels, //fills out the color channel
            0);

    //OpenGL reference to the texture
    GLuint texture;
    //Generate a reference
    glGenTextures(1, &texture);
    //Set the current texture we're working on to texture 0
    glActiveTexture(GL_TEXTURE0);
    //Bind our next task to tex0 to our current reference similar to what were doing to VBO
    glBindTexture(GL_TEXTURE_2D, texture);

    //Assign the loaded texture to the openGL reference
    glTexImage2D(GL_TEXTURE_2D,
        0, //texture 0
        GL_RGBA, //target color format of the texture
        img_width, //texture width
        img_height, //texture height
        0,
        GL_RGBA, //color format of the texture
        GL_UNSIGNED_BYTE,
        tex_bytes); //loaded texture in bytes

    //Generate the mipmaps to the current texture
    glGenerateMipmap(GL_TEXTURE_2D);
    //Free up the loaded bytes
    stbi_image_free(tex_bytes);
    //enable depth testing
    glEnable(GL_DEPTH_TEST);

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

    std::string path = "3D/myCube.obj";
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

    GLfloat UV[]{
        0.f, 1.f,
        0.f, 0.f,
        1.f, 1.f,
        1.f, 0.f,
        1.f, 1.f,
        1.f, 0.f,
        0.f, 1.f,
        0.f, 0.f
    };

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
        -0.5f, 0 - .5f, 0.f, //1
        0.5f, -0.5f, 0.f //2
    };

    GLuint indices[]{
        0,1,2
    };

    GLuint VAO, VBO, EBO, VBO_UV;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    //generate our UV buffer
    glGenBuffers(1, &VBO_UV);
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

    //Bind the UV buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO_UV);

    //Add in the buffer data
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(GLfloat) * (sizeof(UV) / sizeof(UV[0])), //float * size of the UV array
        &UV[0], // the UV array from earlier
        GL_DYNAMIC_DRAW);

    //Add in how to interpret the array
    glVertexAttribPointer(
        2, // 2 for UV for tex coords
        2, //UV
        GL_FLOAT, //Type of array
        GL_FALSE,
        2 * sizeof(float), //every 2 index 
        (void*)0
    );

    glEnableVertexAttribArray(2);

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

    glm::vec3 cameraPos = glm::vec3(0, 0, 10.f); //defailt position is 0, 0, 10

    //or just use glm::lookat(eye, center, worldup);

    glm::mat4 cameraPositionMatrix =
        glm::translate(glm::mat4(1.0f), //initialize it as an identity matrix
            cameraPos * -1.0f); //multiply to -1 since we need -P

    glm::vec3 WorldUp = glm::vec3(0, 1.0f, 0);
    glm::vec3 Center = glm::vec3(0, 0.f, 0);

    //Get the forward
    glm::vec3 F = glm::vec3(Center - cameraPos);
    //Normalize the forward
    F = glm::normalize(F);

    //Get the right
    glm::vec3 R = glm::normalize(
        //F X Worldup
        glm::cross(F, WorldUp)
    );

    //Get the up
    glm::vec3 U = glm::normalize(
        //R X F
        glm::cross(R, F)
    );

    glm::lookAt(cameraPos, Center, WorldUp);

    glm::mat4 cameraOrientation = glm::mat4(1.f);

    //Manually assign the matrix
    //Matrix[col][row]

    cameraOrientation[0][0] = R.x;
    cameraOrientation[1][0] = R.y;
    cameraOrientation[2][0] = R.z;

    cameraOrientation[0][1] = U.x;
    cameraOrientation[1][1] = U.y;
    cameraOrientation[2][1] = U.z;

    cameraOrientation[0][2] = -F.x;
    cameraOrientation[1][2] = -F.y;
    cameraOrientation[2][2] = -F.z;

    //Camera view matrix
    glm::mat4 viewMatrix = glm::lookAt(cameraPos, Center, WorldUp);//cameraOrientation * cameraPositionMatrix;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        axis_x = z_mod; //z
        theta = x_mod; //theta

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

        //setting the cameraLoc
        unsigned int viewLoc = glGetUniformLocation(shaderProg, "view");
        glUniformMatrix4fv(viewLoc, //Address of the variable
            1, //how many values are we modifying
            GL_FALSE, //transpose or not
            glm::value_ptr(viewMatrix)); //view matrix

        glUseProgram(shaderProg);
        glBindVertexArray(VAO);
        //glDrawArrays(GL_TRIANGLES, 0, 3); //polygon, index, numpoints

        //Get the location of tex0 in the fragment shader
        GLuint tex0Address = glGetUniformLocation(shaderProg, "tex0");
        //Tell opengl to use the texture
        glBindTexture(GL_TEXTURE_2D, texture);
        //use the texture at 0
        glUniform1i(tex0Address, 0);

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