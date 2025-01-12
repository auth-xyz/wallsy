#include "../include/wallsy.hpp"
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <thread>

const char* wallsy::compositor_ = nullptr;

wallsy::wallsy() {
  std::filesystem::create_directory(defaultTmp);
  std::string framesDir = defaultTmp + "frames/";
  if (std::filesystem::exists(framesDir)) {
    std::filesystem::remove_all(framesDir);
  }
  std::filesystem::create_directory(framesDir);
}

void wallsy::resize(const std::string &path, int width, int height) const {
  ffmpeg::process(path, defaultTmp + "resized.mp4", defaultFPS, width, height);
}

bool wallsy::validateInput(const std::string &path) const {
  return std::filesystem::exists(path);
}

void wallsy::setLoop() const {
  const char* xdgConfigHome = std::getenv("XDG_CONFIG_HOME");
  std::string configHome = xdgConfigHome ? xdgConfigHome : std::string(std::getenv("HOME")) + "/.config";

  std::string localBinPath = std::string(std::getenv("HOME")) + "/.local/bin/";
  std::string wpaperdPath = localBinPath + "wpaperd";
  std::string wpaperCtlPath = localBinPath + "wpaperctl";

  if (!std::filesystem::exists(wpaperdPath) || !std::filesystem::exists(wpaperCtlPath)) {
    std::cerr << "Error: Required binaries (wpaperd/wpaperctl) not found." << std::endl;
    return;
  }

  std::string framesDir = defaultTmp + "frames/";
  if (!std::filesystem::exists(framesDir) || std::filesystem::is_empty(framesDir)) {
    std::cerr << "Error: Frames directory is missing or empty: " << framesDir << std::endl;
    return;
  }

  std::string configPath = configHome + "/wpaperd/config.toml";
  std::filesystem::create_directories(configHome + "/wpaperd/");

  std::ofstream configFile(configPath, std::ios::out | std::ios::trunc);
  if (!configFile) {
    std::cerr << "Error: Could not write config file: " << configPath << std::endl;
    return;
  }

  configFile << "[any]\n"
             << "transition-time = 1\n"
             << "sorting = \"ascending\"\n"
             << "path = \"" << framesDir << "\"\n";
  configFile.close();

  if (std::system((wpaperdPath + " --daemon").c_str()) != 0) {
    std::cerr << "Error: Failed to start wpaperd daemon." << std::endl;
    return;
  }

  int retries = 10;
  while (retries--) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    if (std::system((wpaperCtlPath + " status").c_str()) == 0) {
      break;
    }
    if (retries == 0) {
      std::cerr << "Error: wpaperd did not become ready." << std::endl;
      return;
    }
  }

  int frameDelay = 1000 / defaultFPS;
  while (true) {
    if (std::system((wpaperCtlPath + " next-wallpaper").c_str()) != 0) {
      std::cerr << "Error: Failed to switch wallpaper." << std::endl;
      break;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(frameDelay));
  }
}

void wallsy::startWallpaperLoop(const std::string& video) const {
  try {
    verifyCompositor();
    ffmpeg::convert(video, defaultTmp + "frames/");
    setLoop();
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
}

void wallsy::setResolution(int width, int height) {
  defaultW = width;
  defaultH = height;
}

void wallsy::setFPS(int fps) {
  defaultFPS = fps;
}

void ffmpeg::process(const std::string &path, const std::string &output, int frames, int width, int height) {
  std::string cmd = "ffmpeg -i " + path + " -vf scale=" + std::to_string(width) + ":" + std::to_string(height) + " -r " + std::to_string(frames) + " " + output + " >/dev/null 2>&1";
  if (std::system(cmd.c_str()) != 0) {
    std::cerr << "Error: FFmpeg process failed." << std::endl;
  }
}

void ffmpeg::convert(const std::string &path, const std::string &output) {
  std::filesystem::create_directory(output);
  std::string cmd = "ffmpeg -i " + path + " " + output + "/frame_%04d.png >ffmpeg.log 2>&1";

  if (std::system(cmd.c_str()) != 0) {
    std::cerr << "Error: FFmpeg conversion failed." << std::endl;
  } else {
    std::cout << "Frames generated at: " << output << std::endl;
  }
}

std::string wallsy::verifyCompositor() const {
  const char* session = std::getenv("XDG_SESSION_TYPE");
  if (!session) {
    throw std::runtime_error("Session type not detected.");
  }

  std::string sessionType(session);
  if (sessionType == "wayland" || sessionType == "x11") {
    compositor_ = sessionType.c_str();
    return compositor_;
  }

  throw std::runtime_error("Unsupported compositor: " + sessionType);
}

std::string wallsy::setCommand() const {
  return "";
}

