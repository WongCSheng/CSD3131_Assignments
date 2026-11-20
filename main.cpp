/**
 * @file main.cpp
 * @author Wong Chong Sheng (w.chongsheng@digipen.edu)
 * @brief Main program to choose and compare different question files
 * @date 20-11-2026
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glfw/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <algorithm>
#include <atomic>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <regex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

namespace fs = std::filesystem;

// Texture loading
struct Texture { GLuint id = 0; int w = 0, h = 0; };

/**
 * @brief Loads texture from images
 * 
 * @param path 
 * @return Texture 
 */
static Texture LoadTexture(const std::string& path)
{
    Texture t;
    unsigned char* data = stbi_load(path.c_str(), &t.w, &t.h, nullptr, 4);
    if (!data)
    {
        return t;
    }
    glGenTextures(1, &t.id);
    glBindTexture(GL_TEXTURE_2D, t.id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, t.w, t.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);
    return t;
}

/**
 * @brief Release textures
 * 
 * @param t 
 */
static void FreeTexture(Texture& t)
{
    if (t.id)
    {
         glDeleteTextures(1, &t.id);
    }
    t = {};
}

/**
 * @brief Loads images from the img folder
 * 
 * @param folder 
 * @return std::vector<std::string> 
 */
static std::vector<std::string> LoadImages(const std::string& folder)
{
    std::vector<std::string> out;
    if (!fs::exists(folder))
    {
        return out;
    }
    for (const auto& e : fs::directory_iterator(folder))
    {
        if (!e.is_regular_file())
        {
            continue;
        }
        std::string ext = e.path().extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".bmp")
        {
            out.push_back(e.path().string());
        }
    }
    std::sort(out.begin(), out.end());
    return out;
}

// Type of orientation settings for the different questions
enum class Style { Dir, DirOrient, Q4 };
// Question 1 to 4 algorithm type
struct Algo { const char* label; const char* exe; const char* outDir; Style style; };

// Question types
static const Algo kAlgos[3] = {
    { "Q1 - Dynamic programming", "bin\\q1.exe", "output\\q1", Style::Dir       },
    { "Q3 - Greedy (OpenCV)",     "bin\\q3.exe", "output\\q3", Style::DirOrient },
    { "Q4 - Graph cut",           "bin\\q4.exe", "output\\q4", Style::Q4        },
};

// For parallel processing
struct Job
{
    std::atomic<bool> running{ false };
    std::atomic<bool> finished{ false };
    int    exitCode = 0;
    double ms = -1.0;
};

static std::string Q(const std::string& s) { return "\"" + s + "\""; }

/**
 * @brief Passing in the parameters to the executables
 * 
 * @param a 
 * @param img 
 * @param seams 
 * @param vertical 
 * @return std::string 
 */
static std::string BuildCommand(const Algo& a, const std::string& img, int seams, bool vertical)
{
    std::string od = a.outDir;
    std::string cmd = Q(a.exe) + " " + Q(img) + " " + std::to_string(seams) + " ";
    switch (a.style)
    {
    case Style::Dir:       cmd += Q(od); break;                                   // q1
    case Style::DirOrient: cmd += Q(od) + (vertical ? " 1" : " 0"); break;        // q3
    case Style::Q4:        cmd += Q(od + "\\out.png") + " " + Q(od + "\\gradient.png") + " " +
                                  Q(od + "\\seams.png") + (vertical ? "" : " h"); break;
    }
    return cmd;
}

/**
 * @brief Run the executables
 * 
 * @param a 
 * @param img 
 * @param seams 
 * @param vertical 
 * @param job 
 */
static void RunAlgo(const Algo& a, std::string img, int seams, bool vertical, Job& job)
{
    fs::create_directories(a.outDir);
    std::string log = std::string(a.outDir) + "\\log.txt";

    // outer quotes are required by cmd /c when several parts are quoted
    std::string cmd = "\"" + BuildCommand(a, img, seams, vertical) + " > " + Q(log) + " 2>&1\"";
    job.exitCode = std::system(cmd.c_str());

    std::ifstream f(log);
    std::stringstream ss; ss << f.rdbuf();
    std::string text = ss.str();
    std::smatch m;
    // matches "Total time: 12 msec" (q1/q3) and "Time Elapsed Total: 12 ms" (q4)
    job.ms = std::regex_search(text, m, std::regex(R"(Total(?: time)?:\s*([0-9.]+)\s*ms)"))
                 ? std::stod(m[1]) : -1.0;

    job.running  = false;
    job.finished = true;
}

/**
 * @brief Render the image to the center window
 * 
 * @param t 
 */
static void DrawFitted(const Texture& t)
{
    if (!t.id) { ImGui::TextUnformatted("(nothing to show)"); return; }
    ImVec2 avail = ImGui::GetContentRegionAvail();
    float s = std::min(avail.x / t.w, avail.y / t.h);
    ImGui::Image((ImTextureID)(intptr_t)t.id, ImVec2(t.w * s, t.h * s));
}

int main()
{
    if (!glfwInit()) return 1;
    GLFWwindow* win = glfwCreateWindow(1680, 960, "Seam Carving Viewer", nullptr, nullptr);
    glfwMakeContextCurrent(win);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::GetIO().IniFilename = nullptr;
    ImGui_ImplGlfw_InitForOpenGL(win, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    std::vector<std::string> images = LoadImages("img");
    int selected = -1;
    int seams = 50;
    bool vertical = true;
    int viewMode = 0; // 0 original, 1 result, 2 energy, 3 seams
    int activeAlgo = -1;
    double lastMs[3] = { -1, -1, -1 };

    Texture original;
    Texture results[3]; // out.png, gradient.png, seams.png
    Job job;
    std::thread worker;

    const ImGuiWindowFlags fixed = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;

    while (!glfwWindowShouldClose(win))
    {
        glfwPollEvents();

        if (job.finished.exchange(false))
        {
            if (worker.joinable())
            {
                worker.join();
            }
            if (activeAlgo >= 0)
            {
                lastMs[activeAlgo] = job.ms;
                const char* names[3] = { "out.png", "gradient.png", "seams.png" };
                for (int i = 0; i < 3; ++i)
                {
                    FreeTexture(results[i]);
                    results[i] = LoadTexture(std::string(kAlgos[activeAlgo].outDir) + "\\" + names[i]);
                }
                viewMode = 1;
            }
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        const ImGuiViewport* vp = ImGui::GetMainViewport();
        const float leftW = 240.f, rightW = 300.f;
        const float H = vp->WorkSize.y;

        // Left window, image list
        ImGui::SetNextWindowPos(vp->WorkPos);
        ImGui::SetNextWindowSize(ImVec2(leftW, H));
        ImGui::Begin("Images", nullptr, fixed);
        for (int i = 0; i < (int)images.size(); ++i)
        {
            std::string name = fs::path(images[i]).filename().string();
            if (ImGui::Selectable(name.c_str(), selected == i) && !job.running)
            {
                selected = i;
                FreeTexture(original);
                original = LoadTexture(images[i]);
                for (auto& r : results)
                {
                    FreeTexture(r);
                }
                viewMode = 0;
            }
        }
        if (images.empty())
        {
            ImGui::TextWrapped("No images found in ./img (run from the project root).");
        }
        ImGui::End();

        // Right window, question executable list
        ImGui::SetNextWindowPos(ImVec2(vp->WorkPos.x + vp->WorkSize.x - rightW, vp->WorkPos.y));
        ImGui::SetNextWindowSize(ImVec2(rightW, H));
        ImGui::Begin("Algorithms", nullptr, fixed);

        ImGui::Checkbox("Vertical seams (Q3 only)", &vertical);
        int maxSeams = 1;
        if (original.id)
        {
            maxSeams = std::max(1, (vertical ? original.w : original.h) * 2 / 3);
        }
        ImGui::InputInt("Seams to remove", &seams);
        seams = std::clamp(seams, 1, maxSeams);
        ImGui::Text("Max for this image: %d", maxSeams);
        ImGui::Separator();

        ImGui::BeginDisabled(selected < 0 || job.running);
        for (int i = 0; i < 3; ++i)
        {
            if (ImGui::Button(kAlgos[i].label, ImVec2(-1, 40)))
            {
                activeAlgo = i;
                job.running = true;
                if (worker.joinable())
                {
                    worker.join();
                }
                worker = std::thread(RunAlgo, std::cref(kAlgos[i]), images[selected], seams, vertical, std::ref(job));
            }
            if (lastMs[i] >= 0) ImGui::Text("   last run: %.1f ms", lastMs[i]);
        }
        ImGui::EndDisabled();

        if (job.running)
        {
            ImGui::TextColored(ImVec4(1, 0.8f, 0.2f, 1), "Running %s ...", kAlgos[activeAlgo].label);
        }
        else if (activeAlgo >= 0 && job.exitCode != 0)
        {
            ImGui::TextColored(ImVec4(1, 0.3f, 0.3f, 1), "Exit code %d - see %s\\log.txt", job.exitCode, kAlgos[activeAlgo].outDir);
        }
        ImGui::End();

        // Image viewer
        ImGui::SetNextWindowPos(ImVec2(vp->WorkPos.x + leftW, vp->WorkPos.y));
        ImGui::SetNextWindowSize(ImVec2(vp->WorkSize.x - leftW - rightW, H));
        ImGui::Begin("Viewer", nullptr, fixed);
        ImGui::RadioButton("Original", &viewMode, 0); ImGui::SameLine();
        ImGui::RadioButton("Result", &viewMode, 1);   ImGui::SameLine();
        ImGui::RadioButton("Energy map", &viewMode, 2); ImGui::SameLine();
        ImGui::RadioButton("Seams", &viewMode, 3);
        ImGui::Separator();
        DrawFitted(viewMode == 0 ? original : results[viewMode - 1]);
        ImGui::End();

        ImGui::Render();
        int fw, fh;
        glfwGetFramebufferSize(win, &fw, &fh);
        glViewport(0, 0, fw, fh);
        glClearColor(0.1f, 0.1f, 0.12f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(win);
    }

    if (worker.joinable())
    {
        worker.join();
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(win);
    glfwTerminate();
    return 0;
}