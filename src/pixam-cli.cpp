#include "pixam.h"


void usage(const char * progname)
{
    cerr
            << "Usage: " << endl
            << "\t" << progname << " command parameters" << endl
            << endl
            << "Command: palette | dither | undither" << endl
            << endl
            << "Command palette:" << endl
            << endl
            << "\t" << progname << " palette image.png colorsCount [output.png]" << endl
            << endl
            << "This program determines the best color palette to use to display an" << endl
            << "image, using the K-mean algorithm." << endl
            << endl
            << "Parameters:" << endl
            << "\timage.png: path to the image file (support CImg, recommends PNG)." << endl
            << "\tcolorsCount: desired number of colors in the palette used" << endl
            << "\toutput.png: name for the output palette image file" << endl
            << "\t\t(default to 'palette-<colorCount>-<image>.png')" << endl
            << endl
            << "Result:" << endl
            << "\tAn image containing the palette's colors will be saved." << endl
            << endl
            << "Command dither:" << endl
            << endl
            << "\t" << progname << " dither image.png palette.png pattern.png [output.png]" << endl
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
            << "\tA dithered image will be saved." << endl
            << endl
            << "Command undither:" << endl
            << endl
            << "\t" << progname << " undither ditheredimage.png pattern.png [output.png]" << endl
            << endl
            << "This program loads a dithered image and tries to isolate a pattern" << endl
            << "using linear OLS." << endl
            << endl
            << "Parameters:" << endl
            << "\tditheredimage.png: path to the image file from dither (support CImg, recommends PNG)" << endl
            << "\tpattern.png: path to the image to use as a dithering pattern" << endl
            << "\toutput.png: result image will be saved under this name" << endl
            << "\t\t(default to '<image>-undithered-by-<pattern>.png')" << endl
            << endl
            << "Result:" << endl
            << "\tA undithered image will be saved." << endl;
    exit(1);
}

int main(int argc, char* argv[])
{

    if(argc < 2)
    {
        usage(argv[0]);
    }
    char * tcommand = argv[1];
    if (strcmp("dither", tcommand) == 0)
    {
        // The image file path
        char * fileImage;
        // The palette image file path
        char * filePalette;
        // The threshold pattern image file path
        char * fileThreshold;

        // If not enough arguments were given when called
        if (argc < 5 || argc > 6)
        {
            // Display an error and exit
            usage(argv[0]);
        }

        fileImage = argv[2];
        filePalette = argv[3];
        fileThreshold = argv[4];

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
        if(argc > 5)
        {
            outputFilename = string(argv[5]);
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
    }
    else if (strcmp("palette", tcommand) == 0)
    {
        // The image file path
        char * file;
        // The output destination file path
        string outputFile;
        // The corresponding image
        CImg<unsigned char> image;
        // The generated palette image
        CImg<unsigned char> palette;
        // The number of colors in the palette. This correspond to the K of
        // the K-mean algorithm.
        int K;

        // If not enough arguments were given when called
        if (argc < 4)
        {
            // Display an error and exit
            usage(argv[0]);
        }

        // Retrieve the file path argument
        file = argv[2];
        // Retrieve the number of colors argument
        K = atoi(argv[3]);
        // If was given the output file name
        if(argc > 4)
        {
            outputFile = string(argv[4]);
        }
        else
        {
            // or create a generic file name
            stringstream ss;
            ss << K;
            fs::path pathname(file);
            string dirname  = pathname.parent_path().string();
            string basename = pathname.stem().string();
            if (dirname.length() == 0)
            {
                outputFile = "palette-" + ss.str() + "-" + basename + ".png";
            }
            else
            {
                outputFile = dirname + "/" + "palette-" + ss.str() + "-" + basename + ".png";
            }
        }
        // Check that K is between 2 and 65536
        if(K < 2 || K > 65536)
        {
            // Error and exit
            cerr << "*** Color Count must be between 2 and 65536!" << endl << endl;
            usage(argv[0]);
        }

        // Create the image
        image = CImg<unsigned char>(file);

        palette = generatePalette(image, K);

        // Save the palette image
        palette.save(outputFile.c_str());
        cout << "Saved to " << outputFile << endl;
    } else if (strcmp("undither", tcommand) == 0) {
        // The image file path
        char * fileImage;
        // The threshold pattern image file path
        char * fileThreshold;

        // If not enough arguments were given when called
        if (argc < 4 || argc > 5)
        {
            // Display an error and exit
            usage(argv[0]);
        }

        fileImage = argv[2];
        fileThreshold = argv[3];

        // The image
        CImg<unsigned char>  image(fileImage);
        // The threshold image
        CImg<unsigned char>  thresholdImage(fileThreshold);

        // The dithered result image
        CImg<unsigned char>  result(fileThreshold);

        // Save dithered image
        string outputFilename;
        if(argc > 4)
        {
            outputFilename = string(argv[4]);
        }
        else
        {
            // Create a generic file name
            fs::path pathname(fileImage);
            string dirname  = pathname.parent_path().string();
            string basename = pathname.stem().string();

            fs::path patternpathname(fileThreshold);
            string patternbasename = patternpathname.stem().string();
            if (dirname.length() == 0)
            {
                outputFilename = basename + "-undithered-by-" + patternbasename + ".png";
            }
            else
            {
                outputFilename = dirname + "/" + basename + "-undithered-by-" + patternbasename + ".png";
            }
        }

        result = undither(image, thresholdImage);

        result.save(outputFilename.c_str());
        cout << "Saved to " << outputFilename << endl;
    } else {
        usage(argv[0]);
    }

    return 0;
}
