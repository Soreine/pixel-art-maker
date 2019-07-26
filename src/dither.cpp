#include "pixam.h"


void usage(const char * progname)
{
    cerr
            << "Usage: " << endl
            << "\t" << progname << " image.png palette.png pattern.png [output.png]" << endl
            << endl
            << "This program loads a palette image and an image and then try to" << endl
            << "display this image using the computed palette and a given" << endl
            << "threshold matrix." << endl
            << endl
            << "Parameters:" << endl
            << "\timage.png: path to the image file to dither (support CImg, recommends PNG)" << endl
            << "\tpalette.png: path to the palette file." << endl
            << "\tpattern.png: path to the image to use as a dithering pattern" << endl
            << "\toutput.png: result image will be saved under this name" << endl
            << "\t\t(default to '<image>-dithered-by-<pattern>-with-<palette>.png')" << endl
            << endl
            << "Result:" << endl
            << "\tA dithered image will be saved." << endl;
    exit(1);
}

int main(int argc, char* argv[])
{

    // The image file path
    char * fileImage;
    // The palette image file path
    char * filePalette;
    // The threshold pattern image file path
    char * fileThreshold;

    // If not enough arguments were given when called
    if (argc != 4 && argc != 5)
    {
        // Display an error and exit
        usage(argv[0]);
    }

    fileImage = argv[1];
    filePalette = argv[2];
    fileThreshold = argv[3];

    // The image
    CImg<unsigned char>  image(fileImage);
    // The palette image
    CImg<unsigned char>  paletteImage(filePalette);
    // The threshold image
    CImg<unsigned char>  thresholdImage(fileThreshold);

    // The dithered result image
    CImg<unsigned char>  result(fileThreshold);

    // Save dithered image
    string outputFilename;
    if(argc >= 5)
    {
        outputFilename = string(argv[4]);
    }
    else
    {
        // Create a generic file name
        fs::path pathname(fileImage);
        string dirname  = pathname.parent_path().string();
        string basename = pathname.stem().string();

        fs::path palettepathname(filePalette);
        string palettebasename = palettepathname.stem().string();

        fs::path patternpathname(fileThreshold);
        string patternbasename = patternpathname.stem().string();
        if (dirname.length() == 0)
        {
            outputFilename = basename + "-dithered-by-" + palettebasename + "-with-" + patternbasename + ".png";
        }
        else
        {
            outputFilename = dirname + "/" + basename + "-dithered-by-" + palettebasename + "-with-" + patternbasename + ".png";
        }
    }

    result = ditherNearest(image, paletteImage, thresholdImage);

    /* Display the result
    CImgDisplay result_disp(result, "Result");
    cout << "Enter to finish" << endl;
    cin.ignore();
    */

    result.save(outputFilename.c_str());
    cout << "Saved to " << outputFilename << endl;

    return 0;
}
