// ---------------------------------------------------------------------------
// NAME & SURNAME       : RJ VAN STADEN
// STUDENTNUMBER        : 30026792
// ---------------------------------------------------------------------------
// DATE                 : 2021/05/05
// REVISION             : rev01
// ---------------------------------------------------------------------------
// PROJECT NAME         : ChebyshevFilterGenerator           
// ---------------------------------------------------------------------------

#define _USE_MATH_DEFINES

// ---------------------------------------------------------------------------
// SYSTEM INCLUDE FILES
#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <iterator>
#include <tchar.h>
#include <cmath>
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

// GLOBAL FUNCTION DECLERATIONS
void test_PlotData(std::vector<float>& vsPlotData, ImGuiCond& plotCondition);

// GLOBAL VARIABLE DECLERATIONS

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

    std::vector<float> vfTest;

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

        // DISPLAY INPUT PANEL
        ImGui::SetNextWindowSize(ImVec2(1024, 60));
        ImGui::SetNextWindowPos(ImVec2(20, 20));
        ImGui::Begin(" ");
        ImGuiWindowFlags window_flags = 0;
        window_flags |= ImGuiWindowFlags_NoTitleBar;

        // IF BUTTON PRESSED CALCULATE DATA AND THEN DISPLAY
        std::vector<float> vfData;
        if (ImGui::Button("PRESS FOR CALCULATION", ImVec2(150, 25))) {
            // PLOT DATA BASED ON INPUT DATA            
            for (int i = 0; i <= 360; i++) {
                vfTest.push_back(sin(i * (M_PI / 180)));
                std::cout << vfTest.size() << std::endl;
            }             
        }  
        vfData = vfTest;
        ImGuiCond cond_Plot = ImGuiCond_Always;
        test_PlotData(vfData, cond_Plot);
        ImGui::End();

        

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

void test_PlotData(std::vector<float> &vsPlotData, ImGuiCond &plotCondition)
{
    //ImGuiCond condPlot = plotCondition;
    ImGui::SetNextWindowPos(ImVec2(20, 100));
    ImGui::SetNextWindowSize(ImVec2(1024, 780));
    ImGui::Begin("TEST PLOT APP");

    // ONLY IF VECTOR IS POPULATED - PLOT DATA
    if (vsPlotData.size() != 0) {
        // START PLOT INSTANCE
        ImGui::SetCursorPosY(50);
        ImPlot::SetNextPlotLimits(0, vsPlotData.size() + 10, *std::min_element(vsPlotData.begin(), vsPlotData.end()) - 1, *std::max_element(vsPlotData.begin(), vsPlotData.end()) + 1, plotCondition);
        ImPlot::BeginPlot("PLOT OF SINE WAVE", "Y AXIS", "X AXIS", ImVec2(ImGui::GetWindowWidth() - 18, ImGui::GetWindowHeight() - 158));

        float* dat_y = new float[vsPlotData.size()];
        float* dat_x = new float[vsPlotData.size()];
        for (int i = 0; i < vsPlotData.size(); i++) {
            dat_y[i] = vsPlotData[i];
            dat_x[i] = i;
        }
        ImPlot::PlotLine("Test Plot", dat_x, dat_y, vsPlotData.size());
        ImPlot::EndPlot();
    }    
    ImGui::End();
}
