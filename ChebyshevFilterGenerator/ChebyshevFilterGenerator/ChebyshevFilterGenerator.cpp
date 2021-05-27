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
//void test_PlotData(std::vector<double>& vsPlotData_top, std::vector<double>& vsPlotData_bottom, ImGuiCond& plotCondition);
void plotTopGraph(std::vector<double> &vsPlotY_top, std::vector<double> &vsPlotX_top, std::string strTitle, std::string strX, std::string strY, ImGuiCond& plotCondition);
void plotBottomGraph(std::vector<double> &vsPlotY_bottom, std::vector<double> &vsPlotX_bottom, std::string strTitle, std::string strX, std::string strY, ImGuiCond& plotCondition);
static void HelpMarker(const char* desc, bool same_line = true);
std::vector<double> plotFFT(bool bComponent, std::vector<double> &vfTime);

// GLOBAL VARIABLE DECLERATIONS
std::shared_ptr<cSignalGenerator> pLocalSignal = std::make_shared<cSignalGenerator>();
std::shared_ptr<cFastFourierTransform> pLocalFFT = std::make_shared<cFastFourierTransform>();

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

    std::vector<double> vfTime_Y;
    std::vector<double> vfTime_X;
    std::vector<double> vfFreq_Y;
    std::vector<double> vfFreq_X;

    std::vector<double> vfTime_Original;
    std::vector<double> vfTime_Realisation;
    std::vector<double> vfMag_Original;
    std::vector<double> vfMag_Realisation;
    std::vector<double> vfPhase_Original;
    std::vector<double> vfPhase_Realisation;

    bool bMag = true;

    // RESET INPUT VARIABLES
    int iSampleRate_Hz = 175000;
    int iSmallestFreq_Hz = 10;
    int iLargestFreq_Hz = 80000;
    int iSignalLength_ms = 1000;

    int iOmegaPass_Hz = 42500;
    int iOmegaStop_Hz = 72250;
    double iRipplePass = -0.25;
    double iRippleStop = -40;

    std::string strTop_Title;
    std::string strTop_X;
    std::string strTop_Y;
    std::string strBottom_Title;
    std::string strBottom_X;
    std::string strBottom_Y;

    int iSweepVariable = 2;

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

        ImGuiCond cond_Plot = ImGuiCond_Once;

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
        std::vector<double> vfTop_Y;
        std::vector<double> vfTop_X;
        std::vector<double> vfBottom_Y;        
        std::vector<double> vfBottom_X;         

        std::shared_ptr<cSignalGenerator> pLocalSignal = std::make_shared<cSignalGenerator>(iSampleRate_Hz, iSignalLength_ms, iSmallestFreq_Hz, iLargestFreq_Hz);
        if (ImGui::Button("LINEAR SWEEP", ImVec2(118, 25))) {
            // PLOT DATA BASED ON INPUT DATA  
            pLocalSignal->generateSignal(0);
            iSweepVariable = 0;

            vfTime_Y = pLocalSignal->getSignal_Time();
            vfTime_X.clear();
            for (int i = 0; i < vfTime_Y.size(); i++) {
                vfTime_X.push_back(((double)i / (double) iSampleRate_Hz) * (double)iSignalLength_ms);
            }

            strTop_Title = "LINEAR SWEEP: TIME DOMAIN";
            strTop_X = "Time (ms)";
            strTop_Y = "Amplitude (V)";

            cond_Plot = ImGuiCond_Always;
        } 
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 29);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 122);
        if (ImGui::Button("LOG SWEEP", ImVec2(118, 25))) {
            // PLOT DATA BASED ON INPUT DATA  
            pLocalSignal->generateSignal(1);
            iSweepVariable = 1;

            vfTime_Y = pLocalSignal->getSignal_Time();
            vfTime_X.clear();
            for (int i = 0; i < vfTime_Y.size(); i++) {
                vfTime_X.push_back(((double)i / (double)iSampleRate_Hz) * (double)iSignalLength_ms);
            }

            strTop_Title = "LOG SWEEP: TIME DOMAIN";
            strTop_X = "Time (ms)";
            strTop_Y = "Amplitude (V)";

            cond_Plot = ImGuiCond_Always;
            
        }
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 29);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 122 + 122);
        if (ImGui::Button("SINE SUM", ImVec2(118, 25))) {
            // PLOT DATA BASED ON INPUT DATA  
            pLocalSignal->generateSignal(2);
            iSweepVariable = 2;

            vfTime_Y = pLocalSignal->getSignal_Time();
            vfTime_X.clear();
            for (int i = 0; i < vfTime_Y.size(); i++) {
                vfTime_X.push_back(((double)i / (double)iSampleRate_Hz) * (double)iSignalLength_ms);
            }

            strTop_Title = "TIME SUMMATION: TIME DOMAIN";
            strTop_X = "Time (ms)";
            strTop_Y = "Amplitude (V)";

            cond_Plot = ImGuiCond_Always;
        }        

        ImGui::Text("\n");
        ImGui::Text("====================================================");
        ImGui::Text("            DISPLAY FREQUENCY COMPONENTS            ");
        ImGui::Text("====================================================");

        if (ImGui::Button("MAGNITUDE", ImVec2(179, 25))) {
            // PLOT DATA BASED ON INPUT DATA  
            vfFreq_Y = plotFFT(true, vfTime_Y);
            vfFreq_X.clear();
            for (int i = 0; i < vfFreq_Y.size(); i++) {
                vfFreq_X.push_back(i);
            }

            strBottom_Title = "FFT - MAGNITUDE SPECTRUM";
            strBottom_X = "Frequency (Hz)";
            strBottom_Y = "Amplitude (V)";

            cond_Plot = ImGuiCond_Always;
        }
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 29);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 183);
        if (ImGui::Button("PHASE", ImVec2(179, 25))) {
            // PLOT DATA BASED ON INPUT DATA  
            vfFreq_Y = plotFFT(false, vfTime_Y);
            vfFreq_X.clear();
            for (int i = 0; i < vfFreq_Y.size(); i++) {
                vfFreq_X.push_back(i);
            }

            strBottom_Title = "FFT - PHASE SPECTRUM";
            strBottom_X = "Frequency (Hz)";
            strBottom_Y = "Angle (rad)";

            cond_Plot = ImGuiCond_Always;
        }
        

        ImGui::Text("\n");
        ImGui::Text("====================================================");
        ImGui::Text("------------------- FILTER DESIGN ------------------");
        ImGui::Text("====================================================");

        // INPUT PASSBAND EDGE FREQUENCY
        ImGui::Text("Passband Edge Frequency");
        HelpMarker("Set the Passband Edge Frequency (Hz)");
        ImGui::InputInt("Hz##PassbandEdge", &iOmegaStop_Hz, 1, 10);
        ImGui::Text("\n");
        
        // INPUT STOPBAND EDGE FREQUENCY
        ImGui::Text("Stopband Edge Frequency");
        HelpMarker("Set the Stopband Edge Frequency (Hz)");
        ImGui::InputInt("Hz##StopbandEdge", &iOmegaPass_Hz, 1, 10);
        ImGui::Text("\n");

        // INPUT PASSBAND RIPPLE
        ImGui::Text("Passband Ripple");
        HelpMarker("Set the Passband Ripple (dB)");
        ImGui::InputDouble("dB##PassbandRipple", &iRipplePass, 1, 10);
        ImGui::Text("\n");

        // INPUT STOPBAND RIPPLE
        ImGui::Text("Stopband Ripple");
        HelpMarker("Set the Stopband Ripple (dB)");
        ImGui::InputDouble("dB##StopbandRipple", &iRippleStop, 1, 10);
        ImGui::Text("\n");

        std::shared_ptr<cFilterDesign> pLocalFilter = std::make_shared<cFilterDesign>(iOmegaPass_Hz, iOmegaStop_Hz, iRipplePass, iRippleStop, iSampleRate_Hz);
        if (ImGui::Button("ANALOG", ImVec2(179, 25))) { 
            pLocalFilter->setAnalogFilterTF();
            vfTime_Y = pLocalFilter->getYAxis(true, true);
            vfFreq_Y = pLocalFilter->getYAxis(true, false);
            vfTime_X.clear();
            vfFreq_X.clear();
            for (int i = 0; i < vfFreq_Y.size(); i++) {
                vfTime_X.push_back(i);
                vfFreq_X.push_back(i);
            }

            strTop_Title = "ANALOG FILTER: MAGNITUDE SPECTRUM";
            strTop_X = "Frequency (Hz)";
            strTop_Y = "Gain (dB)";
            strBottom_Title = "ANALOG FILTER: PHASE SPECTRUM";
            strBottom_X = "Frequency (Hz)";
            strBottom_Y = "Angle (rad)";

            cond_Plot = ImGuiCond_Always;
        }
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 29);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 183);
        if (ImGui::Button("DIGITAL", ImVec2(179, 25))) {    
            pLocalFilter->setAnalogFilterTF();
            vfTime_Y = pLocalFilter->getYAxis(false, true);
            vfFreq_Y = pLocalFilter->getYAxis(false, false);
            vfTime_X.clear();
            vfFreq_X.clear();
            for (int i = 0; i < vfFreq_Y.size(); i++) {
                vfTime_X.push_back(i);
                vfFreq_X.push_back(i);
            }

            strTop_Title = "DIGITAL FILTER: MAGNITUDE SPECTRUM";
            strTop_X = "Frequency (Hz)";
            strTop_Y = "Gain (dB)";
            strBottom_Title = "DIGITAL FILTER: PHASE SPECTRUM";
            strBottom_X = "Frequency (Hz)";
            strBottom_Y = "Angle (rad)";

            cond_Plot = ImGuiCond_Always;
        }

        ImGui::Text("\n");
        ImGui::Text("====================================================");
        ImGui::Text("-------------- REALISE DIGITAL FILTER --------------");
        ImGui::Text("====================================================");

        if (ImGui::Button("REALISE FILTER", ImVec2(362, 25))) {
            // INITIALIZE THE TRANSFER FUNCTION OF H(Z)
            //std::vector<double> num = { 0.971628, -3.886512, 5.829768, -3.886512, 0.971628 };
            //std::vector<double> den = { 487.8392585, 1442.331698, 1733.023415, 983.71881, 221.1878345 };
            std::vector<double> num = { 0.001991697025, -0.007966788101, 0.01195018215, -0.007966788101, 0.001991697025 };
            std::vector<double> den = { 1, 2.956571602, 3.552447625, 2.016481439, 0.4534031049 };
            //std::vector<double> num = { 0, 0.44, 0.36, 0.02 };
            //std::vector<double> den = { 1, 0.4, 0.18, -0.2 };
            pLocalFilter->setGrayMarkel(num, den);

            // GET LATTICE AND FEED_FORWARD GAINS
            std::vector<double> vfFeedForward_a = pLocalFilter->getFeedForward();
            std::vector<double> vfLattice_k = pLocalFilter->getLattice();
            std::cout << "Lattice: ";
            for (int i = 0; i < vfLattice_k.size(); i++) {
                std::cout << vfLattice_k.at(i) << "  ";
            }
            std::cout << std::endl;
            std::cout << "Feed-Forward: ";
            for (int i = 0; i < vfFeedForward_a.size(); i++) {
                std::cout << vfFeedForward_a.at(i) << "  ";
            }
            std::cout << std::endl;

            // GET SIGNAL IN TIME DOMAIN
            pLocalSignal->generateSignal(iSweepVariable);
            std::vector<double> vfX = pLocalSignal->getSignal_Time();
            std::cout << vfX.size() << " " << iSweepVariable << std::endl;

            // CREATE CASCADE PARAMETERS
            std::vector<double> vfY_1;
            std::vector<double> vfY_2;
            std::vector<double> vfY_3;
            std::vector<double> vfY_4;
            std::vector<double> vfY_5;
            std::vector<double> vfY_6;
            std::vector<double> vfY_7;
            std::vector<double> vfY_8;
            std::vector<double> vfY_9;
            std::vector<double> vfY_10;
            std::vector<double> vfY_11;
            std::vector<double> vfY_FINAL;

            // CREATE VECTORS FILLED WITH ZEROS
            for (int i = 0; i < vfX.size(); i++) {
                vfY_1.push_back(0);
                vfY_2.push_back(0);
                vfY_3.push_back(0);
                vfY_4.push_back(0);
                vfY_5.push_back(0);
                vfY_6.push_back(0);
                vfY_7.push_back(0);
                vfY_8.push_back(0);
                vfY_9.push_back(0);
                vfY_10.push_back(0);
                vfY_11.push_back(0);
                vfY_FINAL.push_back(0);
            }
           
            for (int i = 1; i < 2; i++) {
                for (int n = 0; n < vfX.size(); n++) {
                    vfY_1[n] = (vfX[n] - vfLattice_k[3] * vfY_7[n-1]);

                    vfY_2[n] = (vfY_1[n] - vfLattice_k[2] * vfY_6[n-1]);

                    vfY_3[n] = (vfY_2[n] - vfLattice_k[1] * vfY_5[n - 1]);

                    vfY_4[n] = (vfY_3[n] - vfLattice_k[0] * vfY_4[n - 1]);

                    vfY_5[n] = (vfLattice_k[0] * vfY_4[n] + vfY_4[n - 1]);

                    vfY_6[n] = (vfLattice_k[1] * vfY_3[n] + vfY_5[n - 1]);

                    vfY_7[n] = (vfLattice_k[2] * vfY_2[n] + vfY_6[n - 1]);

                    vfY_8[n] = (vfLattice_k[3] * vfY_1[n] + vfY_7[n - 1]);

                    vfY_9[n] = (vfFeedForward_a[0] * vfY_8[n] + vfFeedForward_a[1] * vfY_7[n]);

                    vfY_10[n] = (vfFeedForward_a[2] * vfY_6[n] + vfY_9[n]);

                    vfY_11[n] = (vfFeedForward_a[3] * vfY_5[n] + vfY_10[n]);

                    vfY_FINAL[n] = (vfFeedForward_a[4] * vfY_4[n] + vfY_11[n]);
                }
            }              

            vfTime_Realisation = vfY_FINAL;
            vfTime_Original = vfX;

            vfTime_Y = vfTime_Original;
            vfTime_X.clear();
            for (int i = 0; i < vfTime_Y.size(); i++) {
                vfTime_X.push_back(((double)i / (double)iSampleRate_Hz) * (double)iSignalLength_ms);
            }

            strTop_Title = "UNFILTERED SIGNAL: TIME DOMAIN";
            strTop_X = "Time (ms)";
            strTop_Y = "Amplitude (V)";

            vfFreq_Y = vfTime_Realisation;
            vfFreq_X.clear();
            for (int i = 0; i < vfFreq_Y.size(); i++) {
                vfFreq_X.push_back(((double)i / (double)iSampleRate_Hz) * (double)iSignalLength_ms);
            }

            strBottom_Title = "FILTERED SIGNAL - TIME DOMAIN";
            strBottom_X = "Time (ms)";
            strBottom_Y = "Amplitude (V)";

            cond_Plot = ImGuiCond_Always;
        }
        if (ImGui::Button("MAGNITUDE##Mag_Realisation", ImVec2(179, 25))) {
            // PLOT DATA BASED ON INPUT DATA             

            vfTime_Y = plotFFT(true, vfTime_Original);
            vfTime_X.clear();
            for (int i = 0; i < vfTime_Y.size(); i++) {
                vfTime_X.push_back(i);
            }

            strTop_Title = "UNFILTERED SIGNAL - MAGNITUDE SPECTRUM";
            strTop_X = "Frequency (Hz)";
            strTop_Y = "Amplitude (V)";

            vfFreq_Y = plotFFT(true, vfTime_Realisation);
            vfFreq_X.clear();
            for (int i = 0; i < vfFreq_Y.size(); i++) {
                vfFreq_X.push_back(i);
            }

            strBottom_Title = "FILTERED SIGNAL - MAGNITUDE SPECTRUM";
            strBottom_X = "Frequency (Hz)";
            strBottom_Y = "Amplitude (V)";

            cond_Plot = ImGuiCond_Always;
        }
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 29);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 183);
        if (ImGui::Button("PHASE##Phase_Realisation", ImVec2(179, 25))) {
            // PLOT DATA BASED ON INPUT DATA  
            vfTime_Y = plotFFT(false, vfTime_Original);
            vfTime_X.clear();
            for (int i = 0; i < vfTime_Y.size(); i++) {
                vfTime_X.push_back(i);
            }

            strTop_Title = "UNFILTERED SIGNAL - PHASE SPECTRUM";
            strTop_X = "Phase Angle (Degrees)";
            strTop_Y = "Amplitude (V)";

            vfFreq_Y = plotFFT(false, vfTime_Realisation);
            vfFreq_X.clear();
            for (int i = 0; i < vfFreq_Y.size(); i++) {
                vfFreq_X.push_back(i);
            }

            strBottom_Title = "FILTERED SIGNAL - PHASE SPECTRUM";
            strBottom_X = "Phase Angle (Degrees)";
            strBottom_Y = "Amplitude (V)";

            cond_Plot = ImGuiCond_Always;
        }

        vfTop_Y = vfTime_Y;
        vfTop_X = vfTime_X;
        vfBottom_Y = vfFreq_Y;
        vfBottom_X = vfFreq_X;

        ImGui::SetNextWindowPos(ImVec2(420, 20));
        ImGui::SetNextWindowSize(ImVec2(1480, 977));
        ImGui::Begin("OUTPUT WINDOW");

        plotTopGraph(vfTop_Y, vfTop_X, strTop_Title, strTop_X, strTop_Y, cond_Plot);
        plotBottomGraph(vfBottom_Y, vfBottom_X, strBottom_Title, strBottom_X, strBottom_Y, cond_Plot);

        ImGui::End();

        vfTop_Y.clear();
        vfTop_X.clear();
        vfBottom_Y.clear();
        vfBottom_X.clear();

        ImGui::End();        

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

// Plot Function to plot the Top Graph
void plotTopGraph(std::vector<double>& vsPlotY_top, std::vector<double>& vsPlotX_top, std::string strTitle, std::string strX, std::string strY, ImGuiCond& plotCondition)
{
    ImGuiCond condPlot = plotCondition;

    // PLOT FOR TOP GRAPH
    if (vsPlotY_top.size() != 0) {
        // START PLOT INSTANCE
        //ImGui::SetCursorPosY(50);
        double max = -999999;
        double min = 999999;
        for (int i = 1; i < vsPlotX_top.size(); i++) {
            if (vsPlotX_top.at(i) > max)
                max = vsPlotX_top.at(i);
        }
        ImPlot::SetNextPlotLimitsX(0, max + 10, plotCondition);
        
        max = -999999;
        for (int i = 1; i < vsPlotY_top.size(); i++) {
            if (vsPlotY_top.at(i) > max)
                max = vsPlotY_top.at(i);
            if (vsPlotY_top.at(i) < min && vsPlotY_top.at(i) > -50)
                min = vsPlotY_top.at(i);
        }
        ImPlot::SetNextPlotLimitsY(min - 0.2, max + 0.2, plotCondition);
        ImPlot::BeginPlot(strTitle.c_str(), strX.c_str(), strY.c_str(), ImVec2(ImGui::GetWindowWidth() - 18, ImGui::GetWindowHeight() / 2 - 20));

        double* dat_y = new double[vsPlotY_top.size()];
        double* dat_x = new double[vsPlotY_top.size()];
        for (int i = 0; i < vsPlotY_top.size(); i++) {
            dat_y[i] = vsPlotY_top[i];
            dat_x[i] = vsPlotX_top[i];

        }
        ImPlot::PlotLine(strTitle.c_str(), dat_x, dat_y, vsPlotY_top.size());
        ImPlot::EndPlot();

        delete dat_y;
        delete dat_x;
    }
}

// Plot Function to plot the Bottom Graph
void plotBottomGraph(std::vector<double>& vsPlotY_bottom, std::vector<double>& vsPlotX_bottom, std::string strTitle, std::string strX, std::string strY, ImGuiCond& plotCondition)
{
    ImGuiCond condPlot = plotCondition;

    // PLOT FOR TOP GRAPH
    if (vsPlotY_bottom.size() != 0) {
        // START PLOT INSTANCE
        //ImGui::SetCursorPosY(50);
        double max = -999999;
        double min = 999999;
        for (int i = 1; i < vsPlotX_bottom.size(); i++) {
            if (vsPlotX_bottom.at(i) > max)
                max = vsPlotX_bottom.at(i);
        }
        ImPlot::SetNextPlotLimitsX(0, max + 10, plotCondition);

        max = -999999;
        for (int i = 1; i < vsPlotY_bottom.size(); i++) {
            if (vsPlotY_bottom.at(i) > max)
                max = vsPlotY_bottom.at(i);
            if (vsPlotY_bottom.at(i) < min && vsPlotY_bottom.at(i) > -50)
                min = vsPlotY_bottom.at(i);
        }
        ImPlot::SetNextPlotLimitsY(min - 0.2, max + 0.2, plotCondition);
        ImPlot::BeginPlot(strTitle.c_str(), strX.c_str(), strY.c_str(), ImVec2(ImGui::GetWindowWidth() - 18, ImGui::GetWindowHeight() / 2 - 20));

        double* dat_y = new double[vsPlotY_bottom.size()];
        double* dat_x = new double[vsPlotY_bottom.size()];
        for (int i = 0; i < vsPlotY_bottom.size(); i++) {
            dat_y[i] = vsPlotY_bottom[i];
            dat_x[i] = vsPlotX_bottom[i];

        }
        ImPlot::PlotLine(strTitle.c_str(), dat_x, dat_y, vsPlotY_bottom.size());
        ImPlot::EndPlot();

        delete dat_y;
        delete dat_x;
    }
}


void test_PlotData(std::vector<double> &vsPlotData_top, std::vector<double> &vsPlotData_bottom, ImGuiCond &plotCondition)
{
    ImGuiCond condPlot = plotCondition;
    ImGui::SetNextWindowPos(ImVec2(420, 20));
    ImGui::SetNextWindowSize(ImVec2(1480, 977));
    ImGui::Begin("##TOP GRAPH");

    // PLOT FOR TOP GRAPH
    if (vsPlotData_top.size() != 0) {
        // START PLOT INSTANCE
        //ImGui::SetCursorPosY(50);
        ImPlot::SetNextPlotLimitsX(0, vsPlotData_top.size() + 10, plotCondition);
        double min = 999999;
        double max = -999999;
        for (int i = 1; i < vsPlotData_top.size(); i++) {
            if (vsPlotData_top.at(i) > max)
                max = vsPlotData_top.at(i);
            if (vsPlotData_top.at(i) < min && vsPlotData_top.at(i) > -50)
                min = vsPlotData_top.at(i);
        }
        ImPlot::SetNextPlotLimitsY(min - 0.5, max + 0.5, plotCondition);
        ImPlot::BeginPlot("PLOT OF SINE WAVE", "Y AXIS", "X AXIS", ImVec2(ImGui::GetWindowWidth() - 18, ImGui::GetWindowHeight()/2 - 10));

        double* dat_y = new double[vsPlotData_top.size()];
        double* dat_x = new double[vsPlotData_top.size()];
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
        ImPlot::SetNextPlotLimitsX(0, vsPlotData_bottom.size() + 10, plotCondition);
        double min = 999999;
        double max = -999999;        
        for (int i = 1; i < vsPlotData_bottom.size(); i++) {
            if (vsPlotData_bottom.at(i) > max)
                max = vsPlotData_bottom.at(i);
            if (vsPlotData_bottom.at(i) < min)
                min = vsPlotData_bottom.at(i);
        }        
        ImPlot::SetNextPlotLimitsY(min - 0.5, max + 0.5, plotCondition);
        ImPlot::BeginPlot("PLOT OF FREQUENCY COMPONENTS", "Y AXIS", "X AXIS", ImVec2(ImGui::GetWindowWidth() - 18, ImGui::GetWindowHeight() / 2 - 10));

        double* dat_y = new double[vsPlotData_bottom.size()];
        double* dat_x = new double[vsPlotData_bottom.size()];
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
std::vector<double> plotFFT(bool bComponent, std::vector<double> &vfTime)
{
    pLocalFFT->getFFT(vfTime);
    if (bComponent == true) {
        return pLocalFFT->getMagnitude();
    }
    else {
        return pLocalFFT->getPhase();
    }
}
