#include <glad/gl.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>
 
#include <stdlib.h>
#include <stdio.h>

#include "Constants.h"
#include "App.h"
#include "Shader.h"
 
typedef struct UserData {
    App* app;
    double mouseX, mouseY;
    int ctrlPressed;
    int cursorInWindow;
} UserData;
 
static void errorCallback(int error, const char* description) {
    printf("GLFW error: %s\n", description);
}
 
static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    UserData* userData = (UserData*) glfwGetWindowUserPointer(window);
    App* app = userData->app;
    
    if (action == GLFW_REPEAT) return;
    
    AppKeyName keyName = NUM_APP_KEYNAMES;
    switch (key) {
    case GLFW_KEY_A:
        keyName = APP_KEYNAME_MOVELEFT;
        break;
    case GLFW_KEY_D:
        keyName = APP_KEYNAME_MOVERIGHT;
        break;
    case GLFW_KEY_S:
        keyName = APP_KEYNAME_MOVEBACKWARD;
        break;
    case GLFW_KEY_W:
        keyName = APP_KEYNAME_MOVEFORWARD;
        break;
    case GLFW_KEY_Q:
        keyName = APP_KEYNAME_MOVEDOWN;
        break;
    case GLFW_KEY_E:
        keyName = APP_KEYNAME_MOVEUP;
        break;
    case GLFW_KEY_G:
        keyName = APP_KEYNAME_HIDEGRID;
        break;
    case GLFW_KEY_SPACE:
        keyName = APP_KEYNAME_PAUSESIMULATION;
        break;
    case GLFW_KEY_BACKSPACE:
        if (userData->ctrlPressed)
            keyName = APP_KEYNAME_CLEARDISTANT;
        else
            keyName = APP_KEYNAME_CLEAR;
        break;
    case GLFW_KEY_Z:
        if (userData->ctrlPressed)
            keyName = APP_KEYNAME_UNDO;
        break;
    case GLFW_KEY_X:
        if (userData->ctrlPressed)
            keyName = APP_KEYNAME_DELETE;
        break;
    case GLFW_KEY_LEFT:
        keyName = APP_KEYNAME_PREVIOUSPLANET;
        break;
    case GLFW_KEY_RIGHT:
        keyName = APP_KEYNAME_NEXTPLANET;
        break;
    case GLFW_KEY_1:
        keyName = APP_KEYNAME_SELECTPLANET;
        break;
    case GLFW_KEY_2:
        keyName = APP_KEYNAME_SELECTSTAR;
        break;
    case GLFW_KEY_3:
        keyName = APP_KEYNAME_SELECTBLACKHOLE;
        break;
    case GLFW_KEY_TAB:
        keyName = APP_KEYNAME_SHOWCROSSHAIRS;
        break;
    }
    
    if (key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_RIGHT_CONTROL) {
        userData->ctrlPressed = action == GLFW_PRESS;
    }
    
    if (keyName != NUM_APP_KEYNAMES) {
        AppKey* key = &app->keys[keyName];
        int pressed = action == GLFW_PRESS;
        key->down = pressed;
        if (pressed) {
            key->justPressed = 1;
        }
    }
}

static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    UserData* userData = (UserData*) glfwGetWindowUserPointer(window);
    App* app = userData->app;
    
    app->mouse.x = xpos;
    app->mouse.y = ypos;
    app->mouse.dx += xpos - userData->mouseX;
    app->mouse.dy += ypos - userData->mouseY;
    
    userData->mouseX = xpos;
    userData->mouseY = ypos;
}

static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    UserData* userData = (UserData*) glfwGetWindowUserPointer(window);
    App* app = userData->app;
    
    if (!userData->cursorInWindow)
        return;
    
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            app->mouse.rightDown = 1;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        } else {
            app->mouse.rightDown = 0;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    } else if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            app->mouse.leftJustPressed = 1;
        } else if (action == GLFW_RELEASE) {
            app->mouse.leftJustReleased = 1;
        }
    } else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
        if (action == GLFW_PRESS) {
            app->mouse.middleJustPressed = 1;
        }
    }
    app->mouse.control = userData->ctrlPressed;
}

static void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    UserData* userData = (UserData*) glfwGetWindowUserPointer(window);
    App* app = userData->app;
    
    app->mouse.scroll += yoffset;
    app->mouse.control = userData->ctrlPressed;
}

static void cursorEnterCallback(GLFWwindow* window, int entered) {
    UserData* userData = (UserData*) glfwGetWindowUserPointer(window);
    userData->cursorInWindow = entered;
}

void windowSizeCallback(GLFWwindow* window, int width, int height) {
    UserData* userData = (UserData*) glfwGetWindowUserPointer(window);
    App* app = userData->app;
    App_resize(app, width, height);
}
 
int main(int argc, char* argv[]) {
    GLFWwindow* window;
 
    glfwSetErrorCallback(errorCallback);
 
    if (!glfwInit())
        return 1;
 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
 
    window = glfwCreateWindow(C_START_WIDTH, C_START_HEIGHT, "FuzzyCat - C Gravity Simulator", NULL, NULL);
    if (window == NULL) {
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval(0);
    
    App* app = App_create(C_START_WIDTH, C_START_HEIGHT);
    UserData userData = {
        .app = app,
        .mouseX = 0.0, .mouseY = 0.0,
        .cursorInWindow = 0
    };
    glfwGetCursorPos(window, &userData.mouseX, &userData.mouseY);
    glfwSetWindowUserPointer(window, &userData);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetScrollCallback(window, mouseScrollCallback);
    glfwSetCursorEnterCallback(window, cursorEnterCallback);
    glfwSetWindowSizeCallback(window, windowSizeCallback);
    
    double frameTime = glfwGetTime();
    int fps = 0;
    double startTime = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        double curTime = glfwGetTime();
        double dt = curTime - startTime;
        startTime = curTime;
        
        fps++;
        if (curTime - frameTime > 1.0) {
            printf("FPS: %d\n", fps);
            frameTime = curTime;
            fps = 0;
        }
        
        glfwPollEvents();
        
        App_update(app, dt);
        
        App_draw(app);
        
        glfwSwapBuffers(window);
    }
    
    App_destroy(app);
 
    glfwDestroyWindow(window);
    glfwTerminate();
    
    return 0;
}