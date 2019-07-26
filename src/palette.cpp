#include "pixam.h"

void usage(const char * progname)
{
    cerr
            << "Usage: " << endl
            << "\t" << progname << " image.png colorsCount [output.png]" << endl
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
            << "\tAn image containing the palette's colors will be saved." << endl;
    exit(1);
}

int main(int argc, char* argv[])
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
    if (argc < 3)
    {
        // Display an error and exit
        usage(argv[0]);
    }

    // Retrieve the file path argument
    file = argv[1];
    // Retrieve the number of colors argument
    K = atoi(argv[2]);
    // If was given the output file name
    if(argc == 4)
    {
        outputFile = string(argv[3]);
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

    return 0;
}
