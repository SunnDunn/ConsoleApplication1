# define _USE_MATH_DEFINES

# include <GLFW/glfw3.h>
# include <cmath>
# include <iostream>

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 400, "Atasha Limcuando", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        float angleIncrement = 360.0 / 5;
        angleIncrement *= M_PI / 180.0;
        glBegin(GL_POLYGON);
        float angle = 0.0;
        for (int k = 0; k < 5; ++k) {
            glVertex2f(cos(angle), sin(angle));
            /*std::cout << cos(angle) << " " << sin(angle) << std::endl;*/
            angle += angleIncrement;
        }
        glEnd();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}