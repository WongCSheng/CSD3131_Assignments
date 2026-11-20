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

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <algorithm>
#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <regex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

namespace fs = std::filesystem;

struct ProjectPaths
{
    fs::path root;
    fs::path img;
    fs::path bin;
    fs::path output;
};

static ProjectPaths ResolveProjectPaths()
{
    fs::path root = fs::current_path();

    // 1. Launched from batch file / project root
    if (!fs::exists(root / "img") || !fs::exists(root / "bin"))
    {
        // 2. Launched directly from bin\launcher.exe (or nested folder)
        char exeBuf[MAX_PATH] = {};
        if (GetModuleFileNameA(nullptr, exeBuf, MAX_PATH) > 0)
        {
            fs::path dir = fs::path(exeBuf).parent_path();
            bool found = false;
            for (int i = 0; i < 4 && !dir.empty(); ++i)
            {
                if (fs::exists(dir / "img") && fs::exists(dir / "bin"))
                {
                    root = dir;
                    found = true;
                    break;
                }
                dir = dir.parent_path();
            }
            if (!found)
            {
                fs::path exeDir = fs::path(exeBuf).parent_path();
                if (exeDir.filename() == "bin")
                    root = exeDir.parent_path();
            }
        }
    }

    return { root, root / "img", root / "bin", root / "output" };
}

// Texture loading
struct Texture { GLuint id = 0; int w = 0, h = 0; };

/**
 * @brief Loads texture from images
 * 
 * @param path 
 * @return Texture 
 */
static Texture LoadTexture(const fs::path& path)
{
    Texture t;
    unsigned char* data = stbi_load(path.string().c_str(), &t.w, &t.h, nullptr, 4);
    if (!data) return t;
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
static std::vector<fs::path> LoadImages(const fs::path& folder)
{
    std::vector<fs::path> out;
    if (!fs::exists(folder)) return out;
    for (const auto& e : fs::directory_iterator(folder))
    {
        if (!e.is_regular_file()) continue;
        std::string ext = e.path().extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".bmp")
            out.push_back(e.path());
    }
    std::sort(out.begin(), out.end());
    return out;
}

// Type of orientation settings for the different questions
enum class Style { Dir, DirOrient, Q4 };
// Question 1 to 4 algorithm type
struct Algo
{
    const char* label;
    const char* shortLabel;
    const char* exeName;
    const char* outSubDir;
    Style style;
};

// Question types
static const Algo kAlgos[3] = {
    { "Q1 - Dynamic programming", "Q1 (DP)",        "q1.exe", "q1", Style::Dir       },
    { "Q3 - Greedy (OpenCV)",     "Q3 (Greedy)",    "q3.exe", "q3", Style::DirOrient },
    { "Q4 - Graph cut",           "Q4 (Graph Cut)", "q4.exe", "q4", Style::Q4        },
};

// For parallel processing
struct Job
{
    std::atomic<bool> running{ false };
    std::atomic<bool> finished{ false };
    std::atomic<bool> cancelRequested{ false };
    std::atomic<int>  currentAlgo{ -1 };
    std::atomic<bool> wasRunAll{ false };

    std::mutex procMutex;
    HANDLE     hProcess = nullptr;

    int    lastCompletedAlgo = -1;
    int    exitCode = 0;
    double runTimes[3] = { -1.0, -1.0, -1.0 };
    bool   updatedMask[3] = { false, false, false };
};

static std::string Q(const std::string& s) { return "\"" + s + "\""; }

/**
 * @brief Passing in the parameters to the executables
 * 
 * @param paths 
 * @param a 
 * @param img 
 * @param seams 
 * @param vertical 
 * @return std::string 
 */
static std::string BuildCommand(const ProjectPaths& paths, const Algo& a, const fs::path& img, int seams, bool vertical)
{
    fs::path exePath = paths.bin / a.exeName;
    fs::path outDir  = paths.output / a.outSubDir;
    std::string cmd  = Q(exePath.string()) + " " + Q(img.string()) + " " + std::to_string(seams) + " ";

    switch (a.style)
    {
    case Style::Dir:       cmd += Q(outDir.string()); break;
    case Style::DirOrient: cmd += Q(outDir.string()) + (vertical ? " 1" : " 0"); break;
    case Style::Q4:        cmd += Q((outDir / "out.png").string()) + " " +
                                  Q((outDir / "gradient.png").string()) + " " +
                                  Q((outDir / "seams.png").string()) + (vertical ? "" : " h"); break;
    }
    return cmd;
}

static bool ExecuteAlgoProcess(const ProjectPaths& paths, const Algo& a, const fs::path& img,
                               int seams, bool vertical, Job& job, int& outExitCode, double& outMs)
{
    fs::path outDir = paths.output / a.outSubDir;
    fs::create_directories(outDir);
    fs::path logPath = outDir / "log.txt";

    std::string cmdLine = BuildCommand(paths, a, img, seams, vertical);
    std::vector<char> cmdBuf(cmdLine.begin(), cmdLine.end());
    cmdBuf.push_back('\0');

    SECURITY_ATTRIBUTES sa{};
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;

    HANDLE hLog = CreateFileA(logPath.string().c_str(), GENERIC_WRITE, FILE_SHARE_READ,
                              &sa, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

    STARTUPINFOA si{};
    si.cb = sizeof(si);
    if (hLog != INVALID_HANDLE_VALUE)
    {
        si.dwFlags = STARTF_USESTDHANDLES;
        si.hStdOutput = hLog;
        si.hStdError  = hLog;
        si.hStdInput  = nullptr;
    }

    PROCESS_INFORMATION pi{};
    BOOL created = CreateProcessA(
        nullptr, cmdBuf.data(), nullptr, nullptr, TRUE,
        CREATE_NO_WINDOW, nullptr, paths.root.string().c_str(), &si, &pi);

    if (hLog != INVALID_HANDLE_VALUE) CloseHandle(hLog);

    if (!created)
    {
        outExitCode = -1;
        outMs = -1.0;
        return false;
    }

    {
        std::lock_guard<std::mutex> lk(job.procMutex);
        if (job.cancelRequested)
            TerminateProcess(pi.hProcess, 1);
        else
            job.hProcess = pi.hProcess;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);

    {
        std::lock_guard<std::mutex> lk(job.procMutex);
        job.hProcess = nullptr;
    }

    DWORD code = 1;
    GetExitCodeProcess(pi.hProcess, &code);
    outExitCode = static_cast<int>(code);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    if (job.cancelRequested)
    {
        outMs = -1.0;
        return false;
    }

    std::ifstream f(logPath);
    std::stringstream ss;
    ss << f.rdbuf();
    std::string text = ss.str();
    std::smatch m;
    outMs = std::regex_search(text, m, std::regex(R"(Total(?: time)?:\s*([0-9.]+)\s*ms)"))
                ? std::stod(m[1]) : -1.0;
    return true;
}

static void SaveRuntimesToFile(const fs::path& filePath, const fs::path& img, int seams, bool vertical,
                               const double times[3], const bool ran[3], bool cancelled)
{
    fs::create_directories(filePath.parent_path());
    std::ofstream out(filePath, std::ios::app);
    if (!out) return;

    std::time_t now = std::time(nullptr);
    char timeBuf[64] = {};
    std::strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

    out << "========================================\n";
    out << "Timestamp   : " << timeBuf << "\n";
    out << "Image       : " << img.filename().string() << "\n";
    out << "Seams       : " << seams << "\n";
    out << "Orientation : " << (vertical ? "Vertical" : "Horizontal") << "\n";
    if (cancelled) out << "Status      : Stopped by user\n";
    for (int i = 0; i < 3; ++i)
    {
        out << "  " << kAlgos[i].label << ": ";
        if (!ran[i])          out << "Not completed\n";
        else if (times[i] < 0) out << "Failed / No timing found\n";
        else                  out << times[i] << " ms\n";
    }
}

static void RunJobs(ProjectPaths paths, fs::path img, int seams, bool vertical, int singleIdx, Job& job)
{
    job.wasRunAll = (singleIdx < 0);
    job.lastCompletedAlgo = -1;
    job.exitCode = 0;
    for (int i = 0; i < 3; ++i)
    {
        job.runTimes[i] = -1.0;
        job.updatedMask[i] = false;
    }

    int startIdx = (singleIdx >= 0) ? singleIdx : 0;
    int endIdx   = (singleIdx >= 0) ? singleIdx : 2;

    for (int i = startIdx; i <= endIdx; ++i)
    {
        if (job.cancelRequested) break;
        job.currentAlgo = i;

        int code = 0;
        double ms = -1.0;
        bool completed = ExecuteAlgoProcess(paths, kAlgos[i], img, seams, vertical, job, code, ms);
        job.exitCode = code;

        if (!completed) break;

        job.runTimes[i] = ms;
        job.updatedMask[i] = true;
        job.lastCompletedAlgo = i;
    }

    if (singleIdx < 0)
    {
        SaveRuntimesToFile(paths.output / "runtimes.txt", img, seams, vertical,
                           job.runTimes, job.updatedMask, job.cancelRequested.load());
    }

    job.currentAlgo = -1;
    job.running = false;
    job.finished = true;
}

static void StopRunningJob(Job& job)
{
    job.cancelRequested = true;
    std::lock_guard<std::mutex> lk(job.procMutex);
    if (job.hProcess != nullptr)
    {
        TerminateProcess(job.hProcess, 1);
    }
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

static void DrawRuntimeGraphWindow(bool* pOpen, const double lastMs[3], const fs::path& reportPath)
{
    if (!*pOpen) return;
    ImGui::SetNextWindowSize(ImVec2(540, 380), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Runtime Comparison", pOpen))
    {
        double maxMs = 1.0;
        for (int i = 0; i < 3; ++i)
            if (lastMs[i] > maxMs) maxMs = lastMs[i];

        ImVec2 canvasPos  = ImGui::GetCursorScreenPos();
        ImVec2 canvasSize = ImGui::GetContentRegionAvail();
        canvasSize.y = std::max(180.0f, canvasSize.y - 60.0f);

        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImU32 bgCol   = IM_COL32(28, 30, 36, 255);
        ImU32 gridCol = IM_COL32(65, 70, 82, 255);
        ImU32 textCol = IM_COL32(230, 230, 230, 255);
        ImU32 barCols[3] = {
            IM_COL32(80, 160, 245, 255),
            IM_COL32(85, 200, 120, 255),
            IM_COL32(240, 145, 70, 255)
        };

        dl->AddRectFilled(canvasPos, ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y), bgCol, 6.0f);

        const float padLeft = 58.0f, padRight = 20.0f, padTop = 26.0f, padBottom = 34.0f;
        float plotW = std::max(10.0f, canvasSize.x - padLeft - padRight);
        float plotH = std::max(10.0f, canvasSize.y - padTop - padBottom);
        ImVec2 plotMin(canvasPos.x + padLeft, canvasPos.y + padTop);
        ImVec2 plotMax(plotMin.x + plotW, plotMin.y + plotH);

        // Horizontal grid lines (0%, 50%, 100%)
        for (int g = 0; g <= 2; ++g)
        {
            float frac = g * 0.5f;
            float y = plotMax.y - frac * plotH;
            dl->AddLine(ImVec2(plotMin.x, y), ImVec2(plotMax.x, y), gridCol);
            char tick[32];
            std::snprintf(tick, sizeof(tick), "%.0f ms", maxMs * frac);
            dl->AddText(ImVec2(canvasPos.x + 6.0f, y - 7.0f), textCol, tick);
        }

        // Bars
        float slotW = plotW / 3.0f;
        float barW  = std::min(80.0f, slotW * 0.55f);
        for (int i = 0; i < 3; ++i)
        {
            float cx = plotMin.x + slotW * (i + 0.5f);
            if (lastMs[i] >= 0.0)
            {
                float h = static_cast<float>(lastMs[i] / maxMs) * plotH;
                ImVec2 bMin(cx - barW * 0.5f, plotMax.y - h);
                ImVec2 bMax(cx + barW * 0.5f, plotMax.y);
                dl->AddRectFilled(bMin, bMax, barCols[i], 3.0f);

                char valBuf[32];
                std::snprintf(valBuf, sizeof(valBuf), "%.1f ms", lastMs[i]);
                ImVec2 ts = ImGui::CalcTextSize(valBuf);
                dl->AddText(ImVec2(cx - ts.x * 0.5f, bMin.y - ts.y - 3.0f), textCol, valBuf);
            }
            else
            {
                const char* na = "N/A";
                ImVec2 ts = ImGui::CalcTextSize(na);
                dl->AddText(ImVec2(cx - ts.x * 0.5f, plotMax.y - ts.y - 4.0f), IM_COL32(140, 140, 140, 255), na);
            }

            ImVec2 ls = ImGui::CalcTextSize(kAlgos[i].shortLabel);
            dl->AddText(ImVec2(cx - ls.x * 0.5f, plotMax.y + 8.0f), textCol, kAlgos[i].shortLabel);
        }

        ImGui::Dummy(canvasSize);
        ImGui::Separator();
        ImGui::TextWrapped("Saved benchmark log: %s", reportPath.string().c_str());
    }
    ImGui::End();
}

int main()
{
    ProjectPaths paths = ResolveProjectPaths();

    if (!glfwInit()) return 1;
    GLFWwindow* win = glfwCreateWindow(1680, 960, "Seam Carving Viewer", nullptr, nullptr);
    glfwMakeContextCurrent(win);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::GetIO().IniFilename = nullptr;
    ImGui_ImplGlfw_InitForOpenGL(win, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    std::vector<fs::path> images = LoadImages(paths.img);
    int selected = -1;
    int seams = 50;
    bool vertical = true;
    int viewMode = 0;
    int activeAlgo = -1;
    double lastMs[3] = { -1, -1, -1 };
    bool showGraph = false;
    bool wasCancelled = false;

    Texture original;
    Texture results[3];
    Job job;
    std::thread worker;

    const ImGuiWindowFlags fixed = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;

    while (!glfwWindowShouldClose(win))
    {
        glfwPollEvents();

        if (job.finished.exchange(false))
        {
            if (worker.joinable()) worker.join();

            for (int i = 0; i < 3; ++i)
                if (job.updatedMask[i]) lastMs[i] = job.runTimes[i];

            wasCancelled = job.cancelRequested.load();
            if (job.lastCompletedAlgo >= 0)
            {
                activeAlgo = job.lastCompletedAlgo;
                const char* names[3] = { "out.png", "gradient.png", "seams.png" };
                for (int i = 0; i < 3; ++i)
                {
                    FreeTexture(results[i]);
                    results[i] = LoadTexture(paths.output / kAlgos[activeAlgo].outSubDir / names[i]);
                }
                viewMode = 1;
            }
            if (job.wasRunAll && !wasCancelled) showGraph = true;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        const ImGuiViewport* vp = ImGui::GetMainViewport();
        const float leftW = 240.f, rightW = 300.f;
        const float H = vp->WorkSize.y;

        // Left window: image list
        ImGui::SetNextWindowPos(vp->WorkPos);
        ImGui::SetNextWindowSize(ImVec2(leftW, H));
        ImGui::Begin("Images", nullptr, fixed);
        for (int i = 0; i < (int)images.size(); ++i)
        {
            std::string name = images[i].filename().string();
            if (ImGui::Selectable(name.c_str(), selected == i) && !job.running)
            {
                selected = i;
                FreeTexture(original);
                original = LoadTexture(images[i]);
                for (auto& r : results) FreeTexture(r);
                viewMode = 0;
            }
        }
        if (images.empty())
            ImGui::TextWrapped("No images found in:\n%s", paths.img.string().c_str());
        ImGui::End();

        // Right window: algorithms & controls
        ImGui::SetNextWindowPos(ImVec2(vp->WorkPos.x + vp->WorkSize.x - rightW, vp->WorkPos.y));
        ImGui::SetNextWindowSize(ImVec2(rightW, H));
        ImGui::Begin("Algorithms", nullptr, fixed);

        ImGui::Checkbox("Vertical seams (Q3/Q4)", &vertical);
        int maxSeams = 1;
        if (original.id)
            maxSeams = std::max(1, (vertical ? original.w : original.h) * 2 / 3);
        ImGui::InputInt("Seams to remove", &seams);
        seams = std::clamp(seams, 1, maxSeams);
        ImGui::Text("Max for this image: %d", maxSeams);
        ImGui::Separator();

        ImGui::BeginDisabled(selected < 0 || job.running);
        for (int i = 0; i < 3; ++i)
        {
            if (ImGui::Button(kAlgos[i].label, ImVec2(-1, 38)))
            {
                activeAlgo = i;
                wasCancelled = false;
                job.cancelRequested = false;
                job.running = true;
                if (worker.joinable()) worker.join();
                worker = std::thread(RunJobs, paths, images[selected], seams, vertical, i, std::ref(job));
            }
            if (lastMs[i] >= 0) ImGui::Text("   last run: %.1f ms", lastMs[i]);
        }

        ImGui::Spacing();
        if (ImGui::Button("Run All & Save Runtimes", ImVec2(-1, 42)))
        {
            wasCancelled = false;
            job.cancelRequested = false;
            job.running = true;
            if (worker.joinable()) worker.join();
            worker = std::thread(RunJobs, paths, images[selected], seams, vertical, -1, std::ref(job));
        }
        ImGui::EndDisabled();

        // Stop Job button (enabled ONLY while a job is running)
        ImGui::BeginDisabled(!job.running);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.75f, 0.2f, 0.2f, 1.0f));
        if (ImGui::Button("Stop Running Job", ImVec2(-1, 36)))
            StopRunningJob(job);
        ImGui::PopStyleColor();
        ImGui::EndDisabled();

        ImGui::Spacing();
        if (ImGui::Button(showGraph ? "Hide Runtime Graph" : "Show Runtime Graph", ImVec2(-1, 32)))
            showGraph = !showGraph;

        ImGui::Separator();
        if (job.running)
        {
            int cur = job.currentAlgo.load();
            const char* label = (cur >= 0 && cur < 3) ? kAlgos[cur].label : "job";
            ImGui::TextColored(ImVec4(1, 0.8f, 0.2f, 1), "Running %s ...", label);
        }
        else if (wasCancelled)
        {
            ImGui::TextColored(ImVec4(1, 0.6f, 0.2f, 1), "Job stopped by user.");
        }
        else if (activeAlgo >= 0 && job.exitCode != 0)
        {
            ImGui::TextColored(ImVec4(1, 0.3f, 0.3f, 1), "Exit code %d - see %s\\log.txt",
                               job.exitCode, (paths.output / kAlgos[activeAlgo].outSubDir).string().c_str());
        }
        ImGui::End();

        // Middle window: image viewer
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

        DrawRuntimeGraphWindow(&showGraph, lastMs, paths.output / "runtimes.txt");

        ImGui::Render();
        int fw, fh;
        glfwGetFramebufferSize(win, &fw, &fh);
        glViewport(0, 0, fw, fh);
        glClearColor(0.1f, 0.1f, 0.12f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(win);
    }

    if (job.running) StopRunningJob(job);
    if (worker.joinable()) worker.join();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(win);
    glfwTerminate();
    return 0;
}