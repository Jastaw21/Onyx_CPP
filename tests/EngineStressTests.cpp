//
// Created by jacks on 13/02/2026.
//

#include <fstream>
#include <gtest/gtest.h>
#include <sstream>
#include <iostream>
#include <thread>
#include "CLIBot.h"

// Reduce Windows header bloat and macro conflicts
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

class ExternalProcess {
    HANDLE hChildStd_IN_Rd = NULL;
    HANDLE hChildStd_IN_Wr = NULL;
    HANDLE hChildStd_OUT_Rd = NULL;
    HANDLE hChildStd_OUT_Wr = NULL;
    PROCESS_INFORMATION piProcInfo;
    bool started = false;

public:
    ExternalProcess(const std::string& commandLine) {
        SECURITY_ATTRIBUTES saAttr;
        saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
        saAttr.bInheritHandle = TRUE;
        saAttr.lpSecurityDescriptor = NULL;

        if (!CreatePipe(&hChildStd_OUT_Rd, &hChildStd_OUT_Wr, &saAttr, 0)) return;
        if (!SetHandleInformation(hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0)) return;
        if (!CreatePipe(&hChildStd_IN_Rd, &hChildStd_IN_Wr, &saAttr, 0)) return;
        if (!SetHandleInformation(hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0)) return;

        STARTUPINFOA siStartInfo;
        ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
        ZeroMemory(&siStartInfo, sizeof(STARTUPINFOA));
        siStartInfo.cb = sizeof(STARTUPINFOA);
        siStartInfo.hStdError = hChildStd_OUT_Wr;
        siStartInfo.hStdOutput = hChildStd_OUT_Wr;
        siStartInfo.hStdInput = hChildStd_IN_Rd;
        siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

        char* cmd = _strdup(commandLine.c_str());
        started = CreateProcessA(NULL, cmd, NULL, NULL, TRUE, 0, NULL, NULL, &siStartInfo, &piProcInfo);
        free(cmd);

        CloseHandle(hChildStd_OUT_Wr);
        CloseHandle(hChildStd_IN_Rd);
    }

    ~ExternalProcess() {
        if (started) {
            TerminateProcess(piProcInfo.hProcess, 0);
            CloseHandle(piProcInfo.hProcess);
            CloseHandle(piProcInfo.hThread);
        }
        if (hChildStd_IN_Wr) CloseHandle(hChildStd_IN_Wr);
        if (hChildStd_OUT_Rd) CloseHandle(hChildStd_OUT_Rd);
    }

    bool isStarted() const { return started; }

    void write(const std::string& s) {
        DWORD dwWritten;
        WriteFile(hChildStd_IN_Wr, s.c_str(), s.length(), &dwWritten, NULL);
    }

    std::string readAll() {
        DWORD dwRead;
        CHAR chBuf[4096];
        std::string out;
        while (true) {
            DWORD avail = 0;
            if (!PeekNamedPipe(hChildStd_OUT_Rd, NULL, 0, NULL, &avail, NULL) || avail == 0) break;
            if (!ReadFile(hChildStd_OUT_Rd, chBuf, std::min((DWORD)sizeof(chBuf), avail), &dwRead, NULL) || dwRead == 0) break;
            out.append(chBuf, dwRead);
        }
        return out;
    }
};

class RedirectedOutput {
    std::stringstream ss;
    std::streambuf* old_buf;
public:
    RedirectedOutput() : old_buf(std::cout.rdbuf(ss.rdbuf())) {}
    ~RedirectedOutput() { std::cout.rdbuf(old_buf); }
    std::string str() { return ss.str(); }
    void clear() { ss.str(""); ss.clear(); }
};

TEST(StressTests, Stress1){
    auto bot = CliBot();
    UCIParser parser;
    std::string filepath = "C:/Users/jacks/source/repos/Onyx/Onyx/Builds/Games/stresstest.txt";
    std::ifstream file(filepath);
    if (!file.is_open()) {
        GTEST_SKIP() << "Could not open stress test file: " << filepath;
    }

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty())
            lines.push_back(line);
    }

    RedirectedOutput capture;

    for (const auto& line : lines) {
        bool isGo = line.find("go") != std::string::npos;
        auto cmd = parser.parse(line);
        if (!cmd.has_value()) continue;

        bot.HandleCommand(cmd.value());

        if (isGo) {
            // Wait for "bestmove" in the output
            bool found = false;
            while (!found) {
                std::string currentOutput = capture.str();
                if (currentOutput.find("bestmove") != std::string::npos) {
                    std::cerr << currentOutput << std::endl;
                    found = true;
                    EXPECT_EQ(currentOutput.find("a1a1"), std::string::npos);
                } else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            }
        } else if (line.find("isready") != std::string::npos) {
            bool found = false;
            while (!found) {
                std::string currentOutput = capture.str();
                if (currentOutput.find("readyok") != std::string::npos) {
                    found = true;
                } else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            }
        }

        // Print captured output to actual console for visibility during test
        // std::cerr << capture.str(); 
        capture.clear();
    }
}

TEST(StressTests, ExternalExeStress){
    std::string exePath = "C:/Users/jacks/source/repos/OnyxCPP/builds/lt2/OnyxCPP.exe";
    ExternalProcess engine(exePath);
    if (!engine.isStarted()) {
        GTEST_SKIP() << "Could not start external engine: " << exePath;
    }

    std::string filepath = "C:/Users/jacks/source/repos/Onyx/Onyx/Builds/Games/stresstest.txt";
    std::ifstream file(filepath);
    if (!file.is_open()) {
        GTEST_SKIP() << "Could not open stress test file: " << filepath;
    }

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty())
            lines.push_back(line);
    }

    std::string accumulatedOutput;
    for (const auto& line : lines) {
        bool isGo = line.find("go") != std::string::npos;
        engine.write(line + "\n");

        if (isGo) {
            bool found = false;
            while (!found) {
                accumulatedOutput += engine.readAll();
                if (accumulatedOutput.find("bestmove") != std::string::npos) {
                    std::cerr << accumulatedOutput << std::endl;
                    EXPECT_EQ(accumulatedOutput.find("a1a1"), std::string::npos);
                    found = true;
                }
                if (accumulatedOutput.find("PANIC") != std::string::npos) {
                    FAIL() << "Engine panicked during stress test";
                }


                else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            }
        } else if (line.find("isready") != std::string::npos) {
            bool found = false;
            while (!found) {
                bool containsRogueOutput = accumulatedOutput.find("bestmove") != std::string::npos || accumulatedOutput.find("PANIC") != std::string::npos;
                ASSERT_FALSE(containsRogueOutput);
                accumulatedOutput += engine.readAll();
                if (accumulatedOutput.find("readyok") != std::string::npos) {
                    found = true;
                } else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            }
        }
        accumulatedOutput.clear();
    }
}