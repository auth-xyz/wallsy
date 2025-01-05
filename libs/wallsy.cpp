#include "../include/wallsy.hpp"
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <thread>
#include <fstream>

// Constructor
const char* wallsy::compositor_ = nullptr;
wallsy::wallsy() {
  std::filesystem::create_directory(defaultTmp);
  std::string framesDir = defaultTmp + "frames/";
  if (std::filesystem::exists(framesDir)) {
    std::filesystem::remove_all(framesDir);
  }

  std::filesystem::create_directory(framesDir);
}
// Resize a video or image
void wallsy::resize(const std::string &path, int width, int height) const {
  ffmpeg::process(path, defaultTmp + "resized.mp4", defaultFPS, width, height);
}

// Validate the input file
bool wallsy::validateInput(const std::string &path) const {
  return std::filesystem::exists(path);
}

void wallsy::setLoop() const {
  const char* xdgConfigHome = std::getenv("XDG_CONFIG_HOME");
  std::string configHome = xdgConfigHome ? xdgConfigHome : std::string(std::getenv("HOME")) + "/.config";

  std::string localBinPath = std::string(std::getenv("HOME")) + "/.local/bin/";
  std::string wpaperdPath = localBinPath + "wpaperd";
  std::string wpaperCtlPath = localBinPath + "wpaperctl";

  if (!std::filesystem::exists(wpaperdPath)) {
    std::cerr << "Error: wpaperd not found at " << wpaperdPath << std::endl;
    return;
  }

  if (!std::filesystem::exists(wpaperCtlPath)) {
    std::cerr << "Error: wpaperctl not found at " << wpaperCtlPath << std::endl;
    return;
  }

  std::string framesDir = defaultTmp + "frames/";
  if (!std::filesystem::exists(framesDir) || std::filesystem::is_empty(framesDir)) {
    std::cerr << "Error: Frames directory is missing or empty: " << framesDir << std::endl;
    return;
  }

  std::string wpaperdDir = configHome + "/wpaperd/";
  std::filesystem::create_directories(wpaperdDir);
  std::string configPath = wpaperdDir + "config.toml";

  std::ofstream configFile(configPath);
  if (!configFile.is_open()) {
    std::cerr << "Error: Could not open config file for writing: " << configPath << std::endl;
    return;
  }
  configFile << "[any]\n";
  configFile << "transition-time = 1\n";
  configFile << "sorting = \"ascending\"\n";
  configFile << "path = \"" + framesDir + "\"\n";
  configFile.close();

  // Start wpaperd daemon
  std::string startDaemonCmd = wpaperdPath + " --daemon";
  if (std::system(startDaemonCmd.c_str()) != 0) {
    std::cerr << "Error: Failed to start wpaperd daemon." << std::endl;
    return;
  }

  // Wait for daemon readiness
  int retries = 10;
  bool ready = false;
  while (retries-- > 0) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::string checkStatusCmd = wpaperCtlPath + " status";
    if (std::system(checkStatusCmd.c_str()) == 0) {
      ready = true;
      break;
    }
    std::cout << "Waiting for wpaperd to be ready..." << std::endl;
  }

  if (!ready) {
    std::cerr << "Error: wpaperd did not become ready in time." << std::endl;
    return;
  }

  // Start wallpaper loop
  while (true) {
    std::string nextCmd = wpaperCtlPath + " next-wallpaper";
    int result = std::system(nextCmd.c_str());
    if (result != 0) {
      std::cerr << "Error: Command failed: " << nextCmd << std::endl;
      break;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10000 / defaultFPS));
  }
}



// Start wallpaper loop
void wallsy::startWallpaperLoop(const std::string& video) const {
  try {
    verifyCompositor(); // Verify which compositor to use
    ffmpeg::convert(video, defaultTmp + "frames/");
    setLoop();          // Start the loop
  } catch (const std::exception &e) {
    std::cerr << "Error starting wallpaper loop: " << e.what() << std::endl;
  }
}

// Set resolution
void wallsy::setResolution(int width, int height) {
  defaultW = width;
  defaultH = height;
}

// Set wallpaper method
void wallsy::setMethod(const std::string &method) { defaultMethod = method; }
void wallsy::setFPS(int fps) { defaultFPS = fps; }


// FFmpeg namespace process implementation
void ffmpeg::process(const std::string &path, const std::string &output,
                     int frames, int width, int height) {
  std::string cmd =
      "ffmpeg -i " + path + " -vf scale=" + std::to_string(width) + ":" +
      std::to_string(height) + " -r " + std::to_string(frames) + " " + output;
  int result = std::system(cmd.c_str());
  if (result != 0) {
    std::cerr << "Error: FFmpeg process failed." << std::endl;
  }
}

void ffmpeg::convert(const std::string &path, const std::string &output) {
  // Extract frames from video and save them as individual images
  std::filesystem::create_directory(output);
  std::string cmd =
      "ffmpeg -i " + path + " " + output + "/frame_%04d.png";
  int result = std::system(cmd.c_str());
  if (result != 0) {
    std::cerr << "Error: FFmpeg conversion failed." << std::endl;
  } else {
    std::cout << "Conversion to frames complete: " << output << std::endl;
  }
}

std::string wallsy::verifyCompositor() const {

// Define static member

  std::cout << "Verifying compositor..." << std::endl;
    const char* session = std::getenv("XDG_SESSION_TYPE");
    if (session) {
        std::string sessionType(session);
        if (sessionType == "wayland") {
            return this->compositor_ = "wayland";
        } else if (sessionType == "x11") {
            return this->compositor_ = "x11";
        }
    }
    throw std::runtime_error("Unsupported compositor or session type could not be detected.");
}

std::string wallsy::setCommand() const  {

  return "";
}
