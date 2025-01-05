#pragma once

#include <string>
#include <vector>

class wallsy {
public:
  // Constructors
  wallsy();

  // Core Methods
  void resize(const std::string& path, int width, int height) const;
  void setStartup() const;  
  void setLoop() const;     
  void startWallpaperLoop(const std::string& video) const; 

  // Validation
  bool validateInput(const std::string& path) const;

  // Resolution/Scaling things
  void setResolution(int width, int height);
  void setMethod(const std::string& method);
  void setFPS(int fps);

private:
  // funny little shits. 
  static const char* compositor_;
  static const char* command_; 

  // Default Settings
  int defaultW = 1920;
  int defaultH = 1080;
  int defaultFPS = 30;
  std::string defaultMethod = "fill";
  std::string defaultTmp = "/tmp/wallsy/";
  std::vector<std::string> framePaths;

  // Helper Methods
  void createSystemUnit(const std::string& command) const;
  std::string verifyCompositor() const;
  std::string setCommand() const;
  

  // Wallpaper Method Enum
  enum class WallpaperMethod { Fill, Stretch, Tile, Center };
  WallpaperMethod method = WallpaperMethod::Fill;
};

// FFmpeg Namespace
namespace ffmpeg {
  void process(const std::string& path, const std::string& output, 
               int frames, int width, int height);
  void convert(const std::string& path, const std::string& output);
}


