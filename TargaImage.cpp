///////////////////////////////////////////////////////////////////////////////
//
//      TargaImage.cpp                          Author:     Stephen Chenney
//                                              Modified:   Eric McDaniel
//                                              Date:       Fall 2004
//                                              Modified:   Feng Liu
//                                              Date:       Winter 2011
//                                              Why:        Change the library file 
//      Implementation of TargaImage methods.  You must implement the image
//  modification functions.
//
///////////////////////////////////////////////////////////////////////////////

#include "Globals.h"
#include "TargaImage.h"
#include "libtarga.h"
#include <stdlib.h>
#include <assert.h>
#include <memory.h>
#include <math.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>

using namespace std;

// constants
const int           RED             = 0;                // red channel
const int           GREEN           = 1;                // green channel
const int           BLUE            = 2;                // blue channel
const unsigned char BACKGROUND[3]   = { 0, 0, 0 };      // background color


// Computes n choose s, efficiently
double Binomial(int n, int s)
{
    double        res;

    res = 1;
    for (int i = 1 ; i <= s ; i++)
        res = (n - i + 1) * res / i ;

    return res;
}// Binomial


///////////////////////////////////////////////////////////////////////////////
//
//      Constructor.  Initialize member variables.
//
///////////////////////////////////////////////////////////////////////////////
TargaImage::TargaImage() : width(0), height(0), data(NULL)
{}// TargaImage

///////////////////////////////////////////////////////////////////////////////
//
//      Constructor.  Initialize member variables.
//
///////////////////////////////////////////////////////////////////////////////
TargaImage::TargaImage(int w, int h) : width(w), height(h)
{
   data = new unsigned char[width * height * 4];
   ClearToBlack();
}// TargaImage



///////////////////////////////////////////////////////////////////////////////
//
//      Constructor.  Initialize member variables to values given.
//
///////////////////////////////////////////////////////////////////////////////
TargaImage::TargaImage(int w, int h, unsigned char *d)
{
    int i;

    width = w;
    height = h;
    data = new unsigned char[width * height * 4];

    for (i = 0; i < width * height * 4; i++)
	    data[i] = d[i];
}// TargaImage

///////////////////////////////////////////////////////////////////////////////
//
//      Copy Constructor.  Initialize member to that of input
//
///////////////////////////////////////////////////////////////////////////////
TargaImage::TargaImage(const TargaImage& image) 
{
   width = image.width;
   height = image.height;
   data = NULL; 
   if (image.data != NULL) {
      data = new unsigned char[width * height * 4];
      memcpy(data, image.data, sizeof(unsigned char) * width * height * 4);
   }
}


///////////////////////////////////////////////////////////////////////////////
//
//      Destructor.  Free image memory.
//
///////////////////////////////////////////////////////////////////////////////
TargaImage::~TargaImage()
{
    if (data)
        delete[] data;
}// ~TargaImage


///////////////////////////////////////////////////////////////////////////////
//
//      Converts an image to RGB form, and returns the rgb pixel data - 24 
//  bits per pixel. The returned space should be deleted when no longer 
//  required.
//
///////////////////////////////////////////////////////////////////////////////
unsigned char* TargaImage::To_RGB(void)
{
    unsigned char   *rgb = new unsigned char[width * height * 3];
    int		    i, j;

    if (! data)
	    return NULL;

    // Divide out the alpha
    for (i = 0 ; i < height ; i++)
    {
	    int in_offset = i * width * 4;
	    int out_offset = i * width * 3;

	    for (j = 0 ; j < width ; j++)
        {
	        RGBA_To_RGB(data + (in_offset + j*4), rgb + (out_offset + j*3));
	    }
    }

    return rgb;
}// TargaImage


///////////////////////////////////////////////////////////////////////////////
//
//      Save the image to a targa file. Returns 1 on success, 0 on failure.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Save_Image(const char *filename)
{
    TargaImage	*out_image = Reverse_Rows();

    if (! out_image)
	    return false;

    if (!tga_write_raw(filename, width, height, out_image->data, TGA_TRUECOLOR_32))
    {
	    cout << "TGA Save Error: %s\n", tga_error_string(tga_get_last_error());
	    return false;
    }

    delete out_image;

    return true;
}// Save_Image


///////////////////////////////////////////////////////////////////////////////
//
//      Load a targa image from a file.  Return a new TargaImage object which 
//  must be deleted by caller.  Return NULL on failure.
//
///////////////////////////////////////////////////////////////////////////////
TargaImage* TargaImage::Load_Image(char *filename)
{
    unsigned char   *temp_data;
    TargaImage	    *temp_image;
    TargaImage	    *result;
    int		        width, height;

    if (!filename)
    {
        cout << "No filename given." << endl;
        return NULL;
    }// if

    temp_data = (unsigned char*)tga_load(filename, &width, &height, TGA_TRUECOLOR_32);
    if (!temp_data)
    {
        cout << "TGA Error: %s\n", tga_error_string(tga_get_last_error());
	    width = height = 0;
	    return NULL;
    }
    temp_image = new TargaImage(width, height, temp_data);
    free(temp_data);

    result = temp_image->Reverse_Rows();

    delete temp_image;

    return result;
}// Load_Image


///////////////////////////////////////////////////////////////////////////////
//
//      Convert image to grayscale.  Red, green, and blue channels should all 
//  contain grayscale value.  Alpha channel shoould be left unchanged.  Return
//  success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::To_Grayscale()
{
//    ClearToBlack();

    for (int i = 0; i < (height * width * 4); i += 4) {
        int gray = 0.299 * data[i + RED]
            + 0.587 * data[i + GREEN]
            + 0.114 * data[i + BLUE];
        data[i + RED] = gray;
        data[i + GREEN] = gray;
        data[i + BLUE] = gray;
    }
    return true;
 //   return false;
}// To_Grayscale


///////////////////////////////////////////////////////////////////////////////
//
//  Convert the image to an 8 bit image using uniform quantization.  Return 
//  success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Quant_Uniform()
{
//    ClearToBlack();
    cout << "YEP, WE'RE IN THE QUANT-UNIF METHOD ALRIGHT... THAT'S FOR SURE."
        << endl;

    cout << "old red: " << data[RED] << " new red is: " << data[RED] / 32 << endl;
    for (int i = 0; i < (height * width * 4); i += 4) {
        int newRed = data[i + RED] / 32 + 0.5;
        int newGreen = data[i + GREEN] / 32 + 0.5;
        int newBlue = data[i + BLUE] / 64 + 0.5;
        data[i + RED] = newRed * 32;
        data[i + GREEN] = newGreen * 32;
        data[i + BLUE] = newBlue * 64;
    }
//    return false;
    return true;
}// Quant_Uniform


///////////////////////////////////////////////////////////////////////////////
//
//      Convert the image to an 8 bit image using populosity quantization.  
//  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////

// this I got from
// https://www.tutorialspoint.com/c_standard_library/c_function_qsort.htm
// but it appears to be other places.
int cmpfunc(const void* a, const void* b) {
    return (*(int*)b - *(int*)a);
}

bool TargaImage::Quant_Populosity()
{
//    ClearToBlack();
    cout << "yes, we're in the quant pop method... cool." << endl;

    // this downgrades all the colors and rounds them.
    // so should be between 0 and 32
    for (int i = 0; i < (height * width * 4); i += 4) {
        data[i + RED] = data[i + RED] / 8;// +0.5; // took off rounding
        data[i + GREEN] = data[i + GREEN] / 8;// +0.5;//see what happens
        data[i + BLUE] = data[i + BLUE] / 8;// +0.5;
    }

    // sets up a histogram and one to be ordered.
    int hist[32*32*32] = { 0 };
    int ordHist[32 * 32 * 32] = { 0 };
    
    for (int i = 0; i < (height * width * 4); i += 4) {
        // we add a num to the color position
        hist[data[i + RED] * 1024 + 
            data[i + GREEN] * 32 + 
            data[i + BLUE]] += 1;
        ordHist[data[i + RED] * 1024 + 
            data[i + GREEN] * 32 + 
            data[i + BLUE]] += 1;
    }

    //copy(hist, hist + size(hist), ordHist);
    qsort(ordHist, (32 * 32 * 32), sizeof(int), cmpfunc);
    // couldn't get this to work so instead am using ^^^
    //sort(arrToOrd, arrToOrd + (sizeP * sizeof(arrToOrd[0])));
    //sort(ordHist, ordHist + (32 * 32 * 32));

    int least_common = ordHist[255];
    int j = 0;
    int colors[256][3] = { 0 };

    // this adds the colors that are more popular than the 256th
    for (int i = 0; i < (32 * 32 * 32); i++) {
        if (hist[i] > least_common) {
            colors[j][RED] = i / 1024;
            int greenDiv = i % 1024;
            colors[j][GREEN] = greenDiv / 32;
            colors[j][BLUE] = greenDiv % 32;
            ++j;
        }
    }

    // this evens everything out and gets rid of the stragglers.
    int i = 0;
    while (j < 256) {
        if (hist[i] == least_common) {
            colors[j][RED] = i / 1024;
            int greenDiv = i % 1042;
            colors[j][GREEN] = greenDiv / 32;
            colors[j][BLUE] = greenDiv % 32;
            ++j;
        }
        ++i;
    } // now we should have the total 256 colors.

    for (int i = 0; i < (height * width * 4); i += 4) {
        // bigest distance with our 32 colors is actually 56ish
        double closest = 1000.0; 
        int newColor[3];

        for (int j = 0; j < 256; ++j) {
            double euclidDist = sqrt(
                pow(data[i + RED] - colors[j][RED], 2) +
                pow(data[i + GREEN] - colors[j][GREEN], 2) +
                pow(data[i + BLUE] - colors[j][BLUE], 2) 
            );

            if (euclidDist < closest) {
                closest = euclidDist;
                newColor[RED] = colors[j][RED];
                newColor[GREEN] = colors[j][GREEN];
                newColor[BLUE] = colors[j][BLUE];
            }
        }

        // finally sets the new color to the closest
        data[i + RED] = newColor[RED];
        data[i + GREEN] = newColor[GREEN];
        data[i + BLUE] = newColor[BLUE];
    }

    // shifts the colors back to their 256 slotted color scheme
    // instead of the 1-32.
    for (int i = 0; i < (height * width * 4); i += 4) {
        data[i + RED] = data[i + RED] * 8;
        data[i + GREEN] = data[i + GREEN] * 8;
        data[i + BLUE] = data[i + BLUE] * 8;
    }

    return true;
}// Quant_Populosity


///////////////////////////////////////////////////////////////////////////////
//
//      Dither the image using a threshold of 1/2.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Dither_Threshold()
{
//    ClearToBlack();
    To_Grayscale();

    // i am not going to be changing it and comparing to 0.5, instead could
    // just compare to 128 as ints i think...
    for (int i = 0; i < (height * width * 4); i += 4) {
        if (data[i] < 128) {
            data[i + RED] = 0;
            data[i + GREEN] = 0;
            data[i + BLUE] = 0;
        }
        else {
            data[i + RED] = 255;
            data[i + GREEN] = 255;
            data[i + BLUE] = 255;
        }
    }
    return true;
}// Dither_Threshold


///////////////////////////////////////////////////////////////////////////////
//
//      Dither image using random dithering.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Dither_Random()
{
    for (int i = 0; i < (height * width * 4); i += 4) {
        // this gives the [0-1) grayscale
        float gray = (0.299 * data[i + RED]
            + 0.587 * data[i + GREEN]
            + 0.114 * data[i + BLUE]) / 256.0;
        gray += ((static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 0.4))) - .2);

        int newGray = (int)floor(gray * 256);

        if (newGray < 128) {
            data[i + RED] = 0;
            data[i + GREEN] = 0;
            data[i + BLUE] = 0;
        }
        else {
            data[i + RED] = 255;
            data[i + GREEN] = 255;
            data[i + BLUE] = 255;
        }

    }
//    float r2 = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / X));

    return true;
}// Dither_Random


///////////////////////////////////////////////////////////////////////////////
//
//      Perform Floyd-Steinberg dithering on the image.  Return success of 
//  operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Dither_FS()
{
    ClearToBlack();
    return false;
}// Dither_FS


///////////////////////////////////////////////////////////////////////////////
//
//      Dither the image while conserving the average brightness.  Return 
//  success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Dither_Bright()
{
    To_Grayscale();

    int sum = 0;

    unsigned char* arrToOrd = new unsigned char[height * width];

    for (int i = 0; i < (height * width); ++i) {
        sum += data[i*4];
        arrToOrd[i] = data[i*4];
    }

    int sizeP = height * width;
    double avg = (sum / double(sizeP)) / 256.0;
    int spot = (1-avg) * (sizeP);

    sort(arrToOrd, arrToOrd + (sizeP * sizeof(arrToOrd[0])));

    int theSpot = arrToOrd[spot];

    // NEED TO FIX THIS TO TAKE IN THE AVERAGE VALUE...
    for (int i = 0; i < (height * width * 4); i += 4) {
        if (data[i] < theSpot) {
            data[i + RED] = 0;
            data[i + GREEN] = 0;
            data[i + BLUE] = 0;
        } else {
            data[i + RED] = 255;
            data[i + GREEN] = 255;
            data[i + BLUE] = 255;
        }
    }
    return true;
}// Dither_Bright


///////////////////////////////////////////////////////////////////////////////
//
//      Perform clustered differing of the image.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Dither_Cluster()
{
    ClearToBlack();
    return false;
}// Dither_Cluster


///////////////////////////////////////////////////////////////////////////////
//
//  Convert the image to an 8 bit image using Floyd-Steinberg dithering over
//  a uniform quantization - the same quantization as in Quant_Uniform.
//  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Dither_Color()
{
    ClearToBlack();
    return false;
}// Dither_Color


///////////////////////////////////////////////////////////////////////////////
//
//      Composite the current image over the given image.  Return success of 
//  operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Comp_Over(TargaImage* pImage)
{
    if (width != pImage->width || height != pImage->height)
    {
        cout <<  "Comp_Over: Images not the same size\n";
        return false;
    }

    for (int i = 0; i < (height * width * 4); i += 4) {
        float alpha = (((int) data[i + 3]) / 255.0);

        data[i + RED] = ((data[i + RED] / 255.0) + \
            ((1.0 - alpha) * (pImage->data[i + RED] / 255.0))) * 255;
        data[i + GREEN] = ((data[i + GREEN] / 255.0) + \
            ((1.0 - alpha) * (pImage->data[i + GREEN] / 255.0))) * 255;
        data[i + BLUE] = ((data[i + BLUE] / 255.0) + \
            ((1.0 - alpha) * (pImage->data[i + BLUE] / 255.0))) * 255;
        data[i + 3] = (alpha + \
            ((1.0 - alpha) * (pImage->data[i + 3] / 255.0))) * 255;
    }
    return true;
}// Comp_Over


///////////////////////////////////////////////////////////////////////////////
//
//      Composite this image "in" the given image.  See lecture notes for 
//  details.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Comp_In(TargaImage* pImage)
{
    if (width != pImage->width || height != pImage->height)
    {
        cout << "Comp_In: Images not the same size\n";
        return false;
    }

    for (int i = 0; i < (height * width * 4); i += 4) {

        float alpha = (((int) pImage->data[i + 3]) / 255.0);

        data[i + RED] = ((data[i + RED] / 255.0) * (alpha)) * 255;

        data[i + GREEN] = ((data[i + GREEN] / 255.0) * (alpha)) * 255;

        data[i + BLUE] = ((data[i + BLUE] / 255.0) * (alpha)) * 255;
        data[i + 3] = (alpha * (data[i + 3] / 255.0)) * 255;
    }

//    ClearToBlack();
    return true;
}// Comp_In


///////////////////////////////////////////////////////////////////////////////
//
//      Composite this image "out" the given image.  See lecture notes for 
//  details.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Comp_Out(TargaImage* pImage)
{
    if (width != pImage->width || height != pImage->height)
    {
        cout << "Comp_Out: Images not the same size\n";
        return false;
    }

    for (int i = 0; i < (height * width * 4); i += 4) {

        float alpha = (((int) pImage->data[i + 3]) / 255.0);

        data[i + RED] = ((data[i + RED] / 255.0) * (1 - alpha)) * 255;

        data[i + GREEN] = ((data[i + GREEN] / 255.0) * (1 - alpha)) * 255;

        data[i + BLUE] = ((data[i + BLUE] / 255.0) * (1 - alpha)) * 255;
        data[i + 3] = ((1 - alpha) * (data[i + 3] / 255.0)) * 255;
    }

//    ClearToBlack();
    return true;
}// Comp_Out


///////////////////////////////////////////////////////////////////////////////
//
//      Composite current image "atop" given image.  Return success of 
//  operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Comp_Atop(TargaImage* pImage)
{
    if (width != pImage->width || height != pImage->height)
    {
        cout << "Comp_Atop: Images not the same size\n";
        return false;
    }
    
    for (int i = 0; i < (height * width * 4); i += 4) {
        float alpha = (((int) data[i + 3]) / 255.0);
        float pAlpha = (((int) pImage->data[i + 3]) / 255.0);

        data[i + RED] = (((data[i + RED] / 255.0) * pAlpha) + \
            ((1.0 - alpha) * (pImage->data[i + RED] / 255.0))) * 255;
        data[i + GREEN] = (((data[i + GREEN] / 255.0) * pAlpha) + \
            ((1.0 - alpha) * (pImage->data[i + GREEN] / 255.0))) * 255;
        data[i + BLUE] = (((data[i + BLUE] / 255.0) * pAlpha) + \
            ((1.0 - alpha) * (pImage->data[i + BLUE] / 255.0))) * 255;
        data[i + 3] = ((pAlpha * alpha) + \
            ((1.0 - alpha) * pAlpha)) * 255;
    }

//    ClearToBlack();
    return true;
}// Comp_Atop


///////////////////////////////////////////////////////////////////////////////
//
//      Composite this image with given image using exclusive or (XOR).  Return
//  success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Comp_Xor(TargaImage* pImage)
{
    if (width != pImage->width || height != pImage->height)
    {
        cout << "Comp_Xor: Images not the same size\n";
        return false;
    }

    for (int i = 0; i < (height * width * 4); i += 4) {
        float alpha = (((int) data[i + 3]) / 255.0);
        float pAlpha = (((int) pImage->data[i + 3]) / 255.0);

        data[i + RED] = (((data[i + RED] / 255.0) * (1-pAlpha)) + \
            ((1.0 - alpha) * (pImage->data[i + RED] / 255.0))) * 255;
        data[i + GREEN] = (((data[i + GREEN] / 255.0) * (1-pAlpha)) + \
            ((1.0 - alpha) * (pImage->data[i + GREEN] / 255.0))) * 255;
        data[i + BLUE] = (((data[i + BLUE] / 255.0) * (1-pAlpha)) + \
            ((1.0 - alpha) * (pImage->data[i + BLUE] / 255.0))) * 255;
        data[i + 3] = (((1-pAlpha) * alpha) + \
            ((1.0 - alpha) * pAlpha)) * 255;
    }

//    ClearToBlack();
    return true;
}// Comp_Xor


///////////////////////////////////////////////////////////////////////////////
//
//      Calculate the difference bewteen this imag and the given one.  Image 
//  dimensions must be equal.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Difference(TargaImage* pImage)
{
    if (!pImage)
        return false;

    if (width != pImage->width || height != pImage->height)
    {
        cout << "Difference: Images not the same size\n";
        return false;
    }// if

    for (int i = 0 ; i < width * height * 4 ; i += 4)
    {
        unsigned char        rgb1[3];
        unsigned char        rgb2[3];

        RGBA_To_RGB(data + i, rgb1);
        RGBA_To_RGB(pImage->data + i, rgb2);

        data[i] = abs(rgb1[0] - rgb2[0]);
        data[i+1] = abs(rgb1[1] - rgb2[1]);
        data[i+2] = abs(rgb1[2] - rgb2[2]);
        data[i+3] = 255;
    }

    return true;
}// Difference


///////////////////////////////////////////////////////////////////////////////
//
//      Perform 5x5 box filter on this image.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Filter_Box()
{
    ClearToBlack();
    return false;
}// Filter_Box


///////////////////////////////////////////////////////////////////////////////
//
//      Perform 5x5 Bartlett filter on this image.  Return success of 
//  operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Filter_Bartlett()
{
    ClearToBlack();
    return false;
}// Filter_Bartlett


///////////////////////////////////////////////////////////////////////////////
//
//      Perform 5x5 Gaussian filter on this image.  Return success of 
//  operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Filter_Gaussian()
{
    ClearToBlack();
    return false;
}// Filter_Gaussian

///////////////////////////////////////////////////////////////////////////////
//
//      Perform NxN Gaussian filter on this image.  Return success of 
//  operation.
//
///////////////////////////////////////////////////////////////////////////////

bool TargaImage::Filter_Gaussian_N( unsigned int N )
{
    ClearToBlack();
   return false;
}// Filter_Gaussian_N


///////////////////////////////////////////////////////////////////////////////
//
//      Perform 5x5 edge detect (high pass) filter on this image.  Return 
//  success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Filter_Edge()
{
    ClearToBlack();
    return false;
}// Filter_Edge


///////////////////////////////////////////////////////////////////////////////
//
//      Perform a 5x5 enhancement filter to this image.  Return success of 
//  operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Filter_Enhance()
{
    ClearToBlack();
    return false;
}// Filter_Enhance


///////////////////////////////////////////////////////////////////////////////
//
//      Run simplified version of Hertzmann's painterly image filter.
//      You probably will want to use the Draw_Stroke funciton and the
//      Stroke class to help.
// Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::NPR_Paint()
{
    ClearToBlack();
    return false;
}



///////////////////////////////////////////////////////////////////////////////
//
//      Halve the dimensions of this image.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Half_Size()
{
    ClearToBlack();
    return false;
}// Half_Size


///////////////////////////////////////////////////////////////////////////////
//
//      Double the dimensions of this image.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Double_Size()
{
    ClearToBlack();
    return false;
}// Double_Size


///////////////////////////////////////////////////////////////////////////////
//
//      Scale the image dimensions by the given factor.  The given factor is 
//  assumed to be greater than one.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Resize(float scale)
{
    ClearToBlack();
    return false;
}// Resize


//////////////////////////////////////////////////////////////////////////////
//
//      Rotate the image clockwise by the given angle.  Do not resize the 
//  image.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Rotate(float angleDegrees)
{
    ClearToBlack();
    return false;
}// Rotate


//////////////////////////////////////////////////////////////////////////////
//
//      Given a single RGBA pixel return, via the second argument, the RGB
//      equivalent composited with a black background.
//
///////////////////////////////////////////////////////////////////////////////
void TargaImage::RGBA_To_RGB(unsigned char *rgba, unsigned char *rgb)
{
    const unsigned char	BACKGROUND[3] = { 0, 0, 0 };

    unsigned char  alpha = rgba[3];

    if (alpha == 0)
    {
        rgb[0] = BACKGROUND[0];
        rgb[1] = BACKGROUND[1];
        rgb[2] = BACKGROUND[2];
    }
    else
    {
	    float	alpha_scale = (float)255 / (float)alpha;
	    int	val;
	    int	i;

	    for (i = 0 ; i < 3 ; i++)
	    {
	        val = (int)floor(rgba[i] * alpha_scale);
	        if (val < 0)
		    rgb[i] = 0;
	        else if (val > 255)
		    rgb[i] = 255;
	        else
		    rgb[i] = val;
	    }
    }
}// RGA_To_RGB


///////////////////////////////////////////////////////////////////////////////
//
//      Copy this into a new image, reversing the rows as it goes. A pointer
//  to the new image is returned.
//
///////////////////////////////////////////////////////////////////////////////
TargaImage* TargaImage::Reverse_Rows(void)
{
    unsigned char   *dest = new unsigned char[width * height * 4];
    TargaImage	    *result;
    int 	        i, j;

    if (! data)
    	return NULL;

    for (i = 0 ; i < height ; i++)
    {
	    int in_offset = (height - i - 1) * width * 4;
	    int out_offset = i * width * 4;

	    for (j = 0 ; j < width ; j++)
        {
	        dest[out_offset + j * 4] = data[in_offset + j * 4];
	        dest[out_offset + j * 4 + 1] = data[in_offset + j * 4 + 1];
	        dest[out_offset + j * 4 + 2] = data[in_offset + j * 4 + 2];
	        dest[out_offset + j * 4 + 3] = data[in_offset + j * 4 + 3];
        }
    }

    result = new TargaImage(width, height, dest);
    delete[] dest;
    return result;
}// Reverse_Rows


///////////////////////////////////////////////////////////////////////////////
//
//      Clear the image to all black.
//
///////////////////////////////////////////////////////////////////////////////
void TargaImage::ClearToBlack()
{
    memset(data, 0, width * height * 4);
}// ClearToBlack


///////////////////////////////////////////////////////////////////////////////
//
//      Helper function for the painterly filter; paint a stroke at
// the given location
//
///////////////////////////////////////////////////////////////////////////////
void TargaImage::Paint_Stroke(const Stroke& s) {
   int radius_squared = (int)s.radius * (int)s.radius;
   for (int x_off = -((int)s.radius); x_off <= (int)s.radius; x_off++) {
      for (int y_off = -((int)s.radius); y_off <= (int)s.radius; y_off++) {
         int x_loc = (int)s.x + x_off;
         int y_loc = (int)s.y + y_off;
         // are we inside the circle, and inside the image?
         if ((x_loc >= 0 && x_loc < width && y_loc >= 0 && y_loc < height)) {
            int dist_squared = x_off * x_off + y_off * y_off;
            if (dist_squared <= radius_squared) {
               data[(y_loc * width + x_loc) * 4 + 0] = s.r;
               data[(y_loc * width + x_loc) * 4 + 1] = s.g;
               data[(y_loc * width + x_loc) * 4 + 2] = s.b;
               data[(y_loc * width + x_loc) * 4 + 3] = s.a;
            } else if (dist_squared == radius_squared + 1) {
               data[(y_loc * width + x_loc) * 4 + 0] = 
                  (data[(y_loc * width + x_loc) * 4 + 0] + s.r) / 2;
               data[(y_loc * width + x_loc) * 4 + 1] = 
                  (data[(y_loc * width + x_loc) * 4 + 1] + s.g) / 2;
               data[(y_loc * width + x_loc) * 4 + 2] = 
                  (data[(y_loc * width + x_loc) * 4 + 2] + s.b) / 2;
               data[(y_loc * width + x_loc) * 4 + 3] = 
                  (data[(y_loc * width + x_loc) * 4 + 3] + s.a) / 2;
            }
         }
      }
   }
}


///////////////////////////////////////////////////////////////////////////////
//
//      Build a Stroke
//
///////////////////////////////////////////////////////////////////////////////
Stroke::Stroke() {}

///////////////////////////////////////////////////////////////////////////////
//
//      Build a Stroke
//
///////////////////////////////////////////////////////////////////////////////
Stroke::Stroke(unsigned int iradius, unsigned int ix, unsigned int iy,
               unsigned char ir, unsigned char ig, unsigned char ib, unsigned char ia) :
   radius(iradius),x(ix),y(iy),r(ir),g(ig),b(ib),a(ia)
{
}

