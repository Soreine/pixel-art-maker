# PixelArtMaker


## What is it ?

PixelArtMaker is a two-features graphics tool designed around the
style of [Pixel Art](http://en.wikipedia.org/wiki/Pixel_art). These
two functionalities are :

* Generating an optimized limited color palette for an image.
* [Dithering](http://en.wikipedia.org/wiki/Dither) an image with an
  arbitrary color palette, using a user-defined pattern.

The palette generation is made using a
[k-means clustering](http://en.wikipedia.org/wiki/K-means_clustering)
algorithm that tries to find the set of K colors that average best the
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

## Usage

### Example

Original picture:

![Original Picture](/readme/paraglider.png)

Assuming you are located at the root of the repo, you can generate a
16 colors palette for the example picture `paraglider.png` and dither
it with an 8x8 Bayer pattern like this:

	$ ./palette picture.png 16 palette.png

Now you have an optimized 16 colors palette:

![16 color palette](/readme/paraglider-palette.png)

You can now dither this same picture, using for example the `bayer8.png` pattern:

	$ ./dither picture.png palette.png examples/bayer8.png dithered-picture.png

The 8x8 Bayer threshold matrix looks like this (zoom 8:1) :

![8x8 Bayer Matrix](/readme/zoomBayer8.png)


Result:

![Dithered picture](/readme/bayer-dithered-paraglider.png)

Although the output from the `palette` command can be used as the
palette argument for the `dither` command, this is not mandatory.

### Palette generation

Syntax: `./palette imageFile colorsCount [outputFile]`

Parameters:

* `imageFile`: path to the image file that needs an optimized palette
* `colorsCount`: desired number of colors in the palette
* (optional) `outputFile`: name of the output palette image file
  (default to `palette-<colorsCount>-<imageFile>.png`)

### Dithering

Syntax: `./dither  imageFile paletteFile patternFile [outputFile]`

Parameters:

* `imageFile`: path to the image file to dither
* `paletteFile`: path to the palette to use
* `patternFile`: path to the image used as a dithering pattern
* (optional) `outputFile`: result image will be saved under this name
  (default to `<imageFile>-dithered-by-<patternFile>-with-<paletteFile>.png`)





