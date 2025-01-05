#include <clicky/clicky.hpp>
#include <cstdlib>


#include "../libs/image.hpp"

int main(int argc, char* argv[]) 
{
  clicky cli("{program} [GIF] [OPTIONS]");
  cli.set_prefix({":"}, {":"});
  cli.add_argument("input", "i", false, "the gif to set as wallpaper");

  cli.add_arguments({
    {"downsize", "d", false, "Downsizes the resolution of the gif"},
  });

  cli.add_flags({
    {"loop", "l", false, "Loops the gif"},
    {"startup", "s", false, "Sets the wallpaper on startup"},
  });

  cli.parse(argc,argv);

  return EXIT_SUCCESS;
}
