# PixelArtMaker

PixelArtMaker is a two-features graphics tool designed around the
style of [Pixel Art](http://en.wikipedia.org/wiki/Pixel_art). These
two functionalities are :

* Generating an optimized limited color palette for an image.
* [Dithering](http://en.wikipedia.org/wiki/Dither) an image with an
  arbitrary color palette, using a user-defined pattern.

The palette generation is made using a
[k-means clustering](http://en.wikipedia.org/wiki/K-means_clustering)
algorithm that try to find the set of K colors that average best the
colors of the input image.

The dithering is a positional ordered dithering algorithm, similar to
the
[Bayer ordered dithering algorithm](http://en.wikipedia.org/wiki/Ordered_dithering).
However, instead of using the optimised Bayer matrix as a pattern, any
grayscale image can be used as pattern source. This is destined to
artists wanting to try out some artistic patterns in their
creation. Draw a pattern image, define your palette and launch the
dithering. If you like the result, use it (possibly tweaking it) or
try out something else !


