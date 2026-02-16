#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../SuperSpriteMaker/src/application/App.h"

int main()
{
    App app;
    if (!app.init(1280, 720, "SuperSpriteMaker"))
        return 1;
    return app.run();
}