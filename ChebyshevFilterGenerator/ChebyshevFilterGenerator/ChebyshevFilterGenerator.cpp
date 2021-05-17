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
#include "cSignalGenerator.h"
#include "cFastFourierTransform.h"
#include "cFilterDesign.h"

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error: %d: %s\n", error, description);
}

// GLOBAL FUNCTION DECLERATIONS
void test_PlotData(std::vector<float>& vsPlotData_top, std::vector<float>& vsPlotData_bottom, ImGuiCond& plotCondition);
static void HelpMarker(const char* desc, bool same_line = true);
std::vector<float> plotFFT(bool bComponent, std::vector<float> &vfTime);

// GLOBAL VARIABLE DECLERATIONS
std::shared_ptr<cSignalGenerator> pLocalSignal = std::make_shared<cSignalGenerator>();
std::shared_ptr<cFastFourierTransform> pLocalFFT = std::make_shared<cFastFourierTransform>();
std::shared_ptr<cFilterDesign> pLocalFilter = std::make_shared<cFilterDesign>();

ImVec4 foreground_color = ImVec4(0.258, 0.529, 0.561, 1);

int main(int argc, char* argv[])
{
    int iOmegaPass_Hz = 3600;			
    int iOmegaStop_Hz = 7000;			
    int iRipplePass = -1;				
    int iRippleStop = -30;				
    std::shared_ptr<cFilterDesign> pLocalFilter = std::make_shared<cFilterDesign>(iOmegaPass_Hz, iOmegaStop_Hz, iRipplePass, iRippleStop);


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

    std::vector<float> vfTime;
    std::vector<float> vfFreq;
    bool bMag = true;

    // RESET INPUT VARIABLES
    int iSampleRate_Hz = 75000;
    int iSmallestFreq_Hz = 10;
    int iLargestFreq_Hz = 30000;
    int iSignalLength_ms = 3000;

    /*int iSampleRate_Hz = 300;
    int iSmallestFreq_Hz = 10;
    int iLargestFreq_Hz = 100;
    int iSignalLength_ms = 3000;*/


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
        ImGui::SetNextWindowSize(ImVec2(380, 977));
        ImGui::SetNextWindowPos(ImVec2(20, 20));
        ImGui::Begin("FILTER INPUT PARAMETERS");
        ImGuiWindowFlags window_flags = 0;
        window_flags |= ImGuiWindowFlags_NoTitleBar;

        ImGui::Text("====================================================");        
        ImGui::Text("              INPUT SIGNAL GENERATION               ");
        ImGui::Text("====================================================");

        // INPUT SAMPLE RATE
        ImGui::Text("Sample Rate");
        HelpMarker("Set the sample rate in hertz (Hz)");
        ImGui::InputInt("Hz##SampleRate", &iSampleRate_Hz, 1, 10);
        ImGui::Text("\n");

        // INPUT SMALLEST FREQUENCY
        ImGui::Text("Smallest Frequency");
        HelpMarker("The smallest frequency in the generated signal (Hz)");
        ImGui::InputInt("Hz##SmallestFreq", &iSmallestFreq_Hz, 1, 10);
        ImGui::Text("\n");

        // INPUT LARGEST FREQUENCY
        ImGui::Text("Largest Frequency");
        HelpMarker("The largest frequency in the generated signal (Hz)");
        ImGui::InputInt("Hz##LargestFreq", &iLargestFreq_Hz, 1, 10);
        ImGui::Text("\n");

        // IF BUTTON PRESSED CALCULATE DATA AND THEN DISPLAY
        std::vector<float> vfTop;
        std::vector<float> vfBottom;
        if (ImGui::Button("LINEAR SWEEP", ImVec2(118, 25))) {
            // PLOT DATA BASED ON INPUT DATA  
            std::shared_ptr<cSignalGenerator> pLocalSignal = std::make_shared<cSignalGenerator>(iSampleRate_Hz, iSignalLength_ms, iSmallestFreq_Hz, iLargestFreq_Hz, 0);
            vfTime = pLocalSignal->getSignal_Time();
        } 
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 29);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 122);
        if (ImGui::Button("LOG SWEEP", ImVec2(118, 25))) {
            // PLOT DATA BASED ON INPUT DATA  
            std::shared_ptr<cSignalGenerator> pLocalSignal = std::make_shared<cSignalGenerator>(iSampleRate_Hz, iSignalLength_ms, iSmallestFreq_Hz, iLargestFreq_Hz, 1);
            vfTime= pLocalSignal->getSignal_Time();
            
        }
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 29);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 122 + 122);
        if (ImGui::Button("SINE SUM", ImVec2(118, 25))) {
            // PLOT DATA BASED ON INPUT DATA  
            std::shared_ptr<cSignalGenerator> pLocalSignal = std::make_shared<cSignalGenerator>(iSampleRate_Hz, iSignalLength_ms, iSmallestFreq_Hz, iLargestFreq_Hz, 2);
            vfTime = pLocalSignal->getSignal_Time();            
        }
        vfTop = vfTime;

        ImGui::Text("\n");
        ImGui::Text("====================================================");
        ImGui::Text("----------- DISPLAY FREQUENCY COMPONENTS -----------");
        ImGui::Text("====================================================");

        if (ImGui::Button("MAGNITUDE", ImVec2(179, 25))) {
            // PLOT DATA BASED ON INPUT DATA  
            vfFreq = plotFFT(true, vfTime);
            /*std::vector<float> vfTest;
            for (int i = 0; i < 8; i++) {
                vfTest.push_back(sin(i + 1));
            }
            vfFreq = plotFFT(true, vfTest);*/
        }
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 29);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 183);
        if (ImGui::Button("PHASE", ImVec2(179, 25))) {
            // PLOT DATA BASED ON INPUT DATA  
            vfFreq = plotFFT(false, vfTime);
            /*std::vector<float> vfTest;
            for (int i = 0; i < 8; i++) {
                vfTest.push_back(sin(i + 1));
            }
            vfFreq = plotFFT(false, vfTest);*/
        }
        vfBottom = vfFreq;
        ImGuiCond cond_Plot = ImGuiCond_Always;
        test_PlotData(vfTop, vfBottom,  cond_Plot);
        vfTop.clear();
        vfBottom.clear();
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

void test_PlotData(std::vector<float> &vsPlotData_top, std::vector<float> &vsPlotData_bottom, ImGuiCond &plotCondition)
{
    //ImGuiCond condPlot = plotCondition;
    ImGui::SetNextWindowPos(ImVec2(420, 20));
    ImGui::SetNextWindowSize(ImVec2(1480, 977));
    ImGui::Begin("SIGNAL OUTPUT");

    // PLOT FOR TOP GRAPH
    if (vsPlotData_top.size() != 0) {
        // START PLOT INSTANCE
        //ImGui::SetCursorPosY(50);
        ImPlot::SetNextPlotLimitsX(0, vsPlotData_top.size() + 10);
        ImPlot::SetNextPlotLimitsY(*std::min_element(vsPlotData_top.begin(), vsPlotData_top.end()) - 0.5, *std::max_element(vsPlotData_top.begin(), vsPlotData_top.end()) + 0.5);
        ImPlot::BeginPlot("PLOT OF SINE WAVE", "Y AXIS", "X AXIS", ImVec2(ImGui::GetWindowWidth() - 18, ImGui::GetWindowHeight()/2 - 10));

        float* dat_y = new float[vsPlotData_top.size()];
        float* dat_x = new float[vsPlotData_top.size()];
        for (int i = 0; i < vsPlotData_top.size(); i++) {
            dat_y[i] = vsPlotData_top[i];
            dat_x[i] = i;
        }
        ImPlot::PlotLine("Test Plot", dat_x, dat_y, vsPlotData_top.size());
        ImPlot::EndPlot();

        delete dat_y;
        delete dat_x;
    }  

    // PLOT FOR BOTTOM GRAPH
    if (vsPlotData_bottom.size() != 0) {
        // START PLOT INSTANCE
        //ImGui::SetCursorPosY(50);
        ImPlot::SetNextPlotLimitsX(0, vsPlotData_bottom.size() + 10);
        ImPlot::SetNextPlotLimitsY(*std::min_element(vsPlotData_bottom.begin(), vsPlotData_bottom.end()), *std::max_element(vsPlotData_bottom.begin(), vsPlotData_bottom.end()));
        ImPlot::BeginPlot("PLOT OF FREQUENCY COMPONENTS", "Y AXIS", "X AXIS", ImVec2(ImGui::GetWindowWidth() - 18, ImGui::GetWindowHeight() / 2 - 10));

        float* dat_y = new float[vsPlotData_bottom.size()];
        float* dat_x = new float[vsPlotData_bottom.size()];
        for (int i = 0; i < vsPlotData_bottom.size(); i++) {
            dat_y[i] = vsPlotData_bottom[i];
            dat_x[i] = i;
        }
        ImPlot::PlotLine("Test Plot", dat_x, dat_y, vsPlotData_bottom.size());
        ImPlot::EndPlot();

        delete dat_y;
        delete dat_x;
    }

    ImGui::End();
}

// Helper to display a little (?) mark which shows a tooltip when hovered.
static void HelpMarker(const char* desc, bool same_line)
{
    if (same_line)
        ImGui::SameLine();

    ImGui::TextDisabled("(+)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

// 0 - MAGNITUDE      1 - PHASE
std::vector<float> plotFFT(bool bComponent, std::vector<float> &vfTime)
{
    pLocalFFT->getFFT(vfTime);
    if (bComponent == true) {
        return pLocalFFT->getMagnitude();
    }
    else {
        return pLocalFFT->getPhase();
    }
}
