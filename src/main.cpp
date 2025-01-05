#include <clicky/clicky.hpp>
#include <cstdlib>
#include <iostream>

#include "../include/wallsy.hpp"

const std::string defaultResolution = "1920x1080";

int main(int argc, char *argv[]) {
  clicky cli("{program} [VIDEO] [OPTIONS]");
  wallsy wallsy;

  cli.set_prefix({":"}, {":"});
  cli.add_argument("input", "i", true, "The GIF to set as wallpaper");

  cli.add_arguments({
      {"resolution", "r", false,
       "Set the resolution of the GIF (WIDTHxHEIGHT)"},
      {"fps", "f", false, "Set the FPS of the GIF"},
  });

  try {
    cli.parse(argc, argv);

    std::string input = cli.argument("input");
    if (!wallsy.validateInput(input)) {
      throw std::runtime_error("Invalid input file: " + input);
    }

    if (cli.has_argument("resolution")) {
      auto resolution = cli.argument("resolution");
      wallsy.setResolution(
          std::stoi(resolution.substr(0, resolution.find("x"))),
          std::stoi(resolution.substr(resolution.find("x") + 1)));

    } else {
      wallsy.setResolution(
          std::stoi(defaultResolution.substr(0, defaultResolution.find("x"))),
          std::stoi(defaultResolution.substr(defaultResolution.find("x") + 1)));
    }

    if (cli.has_argument("fps")) {
      int fps = std::stoi(cli.argument("fps"));
      wallsy.setFPS(fps);
    }

    wallsy.startWallpaperLoop(input);
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << '\n';
    cli.print_help();
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
