

So, I'm going to write a proper and prettier readme later, but for now, it'll do.

wallsy is a *very* simplified version of WPE (wallpaper engine), as the description reads,
but it's a bit more than that. Essentially, using ffmpeg, it'll convert any video format
that ffmpeg supports, into a bunch of frames, and using wpaperd, loop through those frames
giving the illusion of an animated background. From my tests on a decade old laptop, it
only used around 6MB of ram. Of course, I will polish the code a bit further, fix some
bugs, and add more features, but it's a start. I hope you like it!

To use wallsy, you need wpaperd and wpaperctl inside your .local/bin/ folder.
And you need ffmpeg installed aswell.
That's it.


here's a url for wpaperd:
https://github.com/danyspin97/wpaperd
and one for ffmpeg:
https://ffmpeg.org/

Usage:
  wallsy [VIDEO] [OPTIONS]

Flags:
  :help, :h         : Display this help message (default: false)

Arguments:
  :fps, :f          : Set the FPS of the GIF (optional)
  :resolution, :r   : Set the resolution of the GIF (WIDTHxHEIGHT) (optional)
  :input, :i        : The GIF to set as wallpaper (required)


ex:
wallsy \
   :input video.mp4 \ 
   :fps 60 
