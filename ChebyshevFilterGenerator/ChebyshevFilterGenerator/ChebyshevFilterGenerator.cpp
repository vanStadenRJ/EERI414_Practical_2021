// ---------------------------------------------------------------------------
// NAME & SURNAME       : RJ VAN STADEN
// STUDENTNUMBER        : 30026792
// ---------------------------------------------------------------------------
// DATE                 : 2021/05/05
// REVISION             : rev01
// ---------------------------------------------------------------------------
// PROJECT NAME         : ChebyshevFilterGenerator           
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// SYSTEM INCLUDE FILES
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <iterator>
#include <tchar.h>
#include <windows.h>
#include <string>
#include <sstream>

// ---------------------------------------------------------------------------
// LIBRARY INCLUDE FILES
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

// ---------------------------------------------------------------------------
// LOCAL INCLUDE FILES
#include "imgui.h"
#include "implot.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error: %d: %s\n", error, description);
}

ImVec4 foreground_color = ImVec4(0.258, 0.529, 0.561, 1);

int main(int argc, char* argv[])
{
    // ---------- GLFW ERROR STATE ----------
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // ---------- GLSL FOR WINDOWS ----------
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    GLFWwindow* window = glfwCreateWindow(1920, 1080, "Chebyshev Filter Generator", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    gl3wInit();

    // ---------- SETUP IMGUI ----------
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // ---------- MAIN PROGRAM LOOP ----------
    while (!glfwWindowShouldClose(window))
    {
        // Poll Events one-by-one
        glfwPollEvents();

        // Start the new Frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        // =======================
        // Enter Program Code Here
        // =======================

        // Final GUI Rendering 
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(foreground_color.x, foreground_color.y, foreground_color.z, foreground_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        
    }

    // ---------- IMGUI & OPENGL CLEANUP ----------
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    ImPlot::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
