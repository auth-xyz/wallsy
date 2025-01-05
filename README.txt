

So, I'm going to write a proper and prettier readme later, but for now, it'll do.

wallsy is, essentially, a wallpaper engine for linux systems, as the description reads
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
