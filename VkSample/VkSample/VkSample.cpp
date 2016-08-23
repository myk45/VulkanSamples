#include "vkutils.h"

extern void initVk();
extern void drawStuff();
extern glfwWindowHelper    glfwHelper;

int main() 
{
    initVk();

    while (!glfwWindowShouldClose(glfwHelper.getWindowHandle()))
    {
        glfwPollEvents();
        drawStuff();
    }

    return 0;
}