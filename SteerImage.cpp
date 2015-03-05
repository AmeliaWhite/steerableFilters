#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>

#include <math.h>

#include "Image.h"

#include "FilesBackend.h"

double G2afunc(int x, int y)
{
    double xs, ys;

    xs = (double) (x * x);
    ys = (double) (y * y);
    ys = exp(-(xs + ys));

    return 0.9213 * ((2 * xs) - 1) * ys;

}

double G2bfunc(int x, int y)
{
    double xs, ys;

    xs = (double) (x * x);
    ys = (double) (y * y);

    ys = exp(-(xs + ys));

    return 1.843 * x * y * ys;
}

double H2afunc(int x, int y)
{
    double xs, ys;

    xs = (double) (x * x);
    ys = (double) (y * y);

    ys = exp(-(xs + ys));

    return 0.9780 * (-2.254 * x + x * xs) * ys;
}

double H2bfunc(int x, int y)
{
    double xs, ys;

    xs = (double) (x * x);
    ys = (double) (y * y);

    ys = exp(-(xs + ys));

    return 0.9780 * (-0.7515 + xs) * y * ys;
}

double H2cfunc(int x, int y)
{
    double xs, ys;

    xs = (double) (x * x);
    ys = (double) (y * y);

    ys = exp(-(xs + ys));

    return 0.9780 * (-0.7515 + y * y) * x * ys;
}

double G2cfunc(int x, int y)
{

    double xs, ys;

    xs = (double) (x * x);
    ys = (double) (y * y);

    xs = exp(-(xs + ys));

    return 0.9213 * ((2 * ys) - 1) * xs;
}

double H2dfunc(int x, int y)
{

    double xs, ys;

    xs = (double) pow(x, 2);
    ys = (double) pow(y, 2);

    xs = exp(-(xs + ys));

    return 0.9780 * (-2.254 * y + y * ys) * xs;
}

struct convdata
{
    double G2a;
    double G2b;
    double G2c;
    double H2a;
    double H2b;
    double H2c;
    double H2d;
};

typedef cimg_library::CImg < convdata > ConvImage;

static ConvImage conv;
static DImage C2, C3;

#define NUM_ANGLES	32
double angles[NUM_ANGLES] = { 0,
    31.0 / 16.0 * M_PI, 30.0 / 16.0 * M_PI, 29.0 / 16.0 * M_PI,
    28.0 / 16.0 * M_PI, 27.0 / 16.0 * M_PI,
    26.0 / 16.0 * M_PI, 25.0 / 16.0 * M_PI, 24.0 / 16.0 * M_PI,
    23.0 / 16.0 * M_PI, 22.0 / 16.0 * M_PI,
    21.0 / 16.0 * M_PI, 20.0 / 16.0 * M_PI, 19.0 / 16.0 * M_PI,
    18.0 / 16.0 * M_PI, 17.0 / 16.0 * M_PI,
    16.0 / 16.0 * M_PI, 15.0 / 16.0 * M_PI, 14.0 / 16.0 * M_PI,
    13.0 / 16.0 * M_PI, 12.0 / 16.0 * M_PI,
    11.0 / 16.0 * M_PI, 10.0 / 16.0 * M_PI, 9.0 / 16.0 * M_PI,
    8.0 / 16.0 * M_PI, 7.0 / 16.0 * M_PI,
    6.0 / 16.0 * M_PI, 5.0 / 16.0 * M_PI, 4.0 / 16.0 * M_PI,
    3.0 / 16.0 * M_PI,
    2.0 / 16.0 * M_PI,
    1.0 / 16.0 * M_PI
};

static convdata prefixes[NUM_ANGLES];

void initsteer()
{
    for (int i = 0; i < NUM_ANGLES; i++) {
	double cosa = cos(angles[i]);
	double sina = sin(angles[i]);

	prefixes[i].G2a = cosa * cosa;
	prefixes[i].G2b = -2.0 * cosa * sina;
	prefixes[i].G2c = sina * sina;
	prefixes[i].H2a = cosa * cosa * cosa;
	prefixes[i].H2b = -3.0 * cosa * cosa * sina;
	prefixes[i].H2c = 3.0 * cosa * sina * sina;
	prefixes[i].H2d = -sina * sina * sina;
    }
}

double steer(int ai, int x, int y)
{
    double H2, G2, G2a, G2b, G2c, H2a, H2b, H2c, H2d;
    
    G2a = prefixes[ai].G2a * conv(x, y).G2a;
    G2b = prefixes[ai].G2b * conv(x, y).G2b;
    G2c = prefixes[ai].G2c * conv(x, y).G2c;
    G2 = G2a + G2b + G2c;
    H2a = prefixes[ai].H2a * conv(x, y).H2a;
    H2b = prefixes[ai].H2b * conv(x, y).H2b;
    H2c = prefixes[ai].H2c * conv(x, y).H2c;
    H2d = prefixes[ai].H2d * conv(x, y).H2d;
    H2 = H2a + H2b + H2c + H2d;

    return G2 * G2 + H2 * H2;

}

int main(int argc, char **argv)
{
    DImage logdata, steered, Ostrength;
    Image image;
    Backend *backend = 0;
    char outname[50];

    //read in image
    printf("start\n");
    backend = new FilesBackend();
    backend->LoadImage(image, argv[1]);
    image = image.get_channel(0);
    printf("read in image\n");

   
    
    int y, x, j, k;
    double
	G2akernel[11][11],
	G2bkernel[11][11],
	H2akernel[11][11],
	H2bkernel[11][11],
	G2ckernel[11][11],
	H2ckernel[11][11],
	H2dkernel[11][11], max, sum1, sum2, sum3, sum4, sum5, sum6, sum7;

    /*allocate memory */

    conv = ConvImage(image.dimx(), image.dimy());
    C2 = DImage(image.dimx(), image.dimy());
    C3 = DImage(image.dimx(), image.dimy());
    logdata = DImage(image.dimx(), image.dimy());
    steered = DImage(image.dimx(), image.dimy(), 32);
    Ostrength = DImage(image.dimx(), image.dimy());

   printf("point 1\n");

    /* put in gray form */
    for (y = 0; y < image.dimy(); y++) {
	for (x = 0; x < image.dimx(); x++) {
	    //if (image(x, y, 0) == 0)
		//logdata(x, y) = log(0.1);
	    //else
		//logdata(x, y) = log((double) image(x, y, 0));
	    logdata(x,y) = (double) image(x,y,0);
	}
    }

   printf("point 2\n");

    /*make the kernels */
    for (y = -5; y <= 5; y++) {
	for (x = -5; x <= 5; x++) {
	    G2akernel[y + 5][x + 5] = G2afunc(x, y);
	    H2dkernel[y + 5][x + 5] = H2dfunc(x, y);
	    G2bkernel[y + 5][x + 5] = G2bfunc(x, y);
	    H2akernel[y + 5][x + 5] = H2afunc(x, y);
	    H2bkernel[y + 5][x + 5] = H2bfunc(x, y);
	    G2ckernel[y + 5][x + 5] = G2cfunc(x, y);
	    H2ckernel[y + 5][x + 5] = H2cfunc(x, y);
	}
    }

   printf("point 4\n");

    /*convolve image with kernels */
    for (y = 6; y <= image.dimy() - 6; y++) {
	for (x = 6; x <= image.dimx() - 6; x++) {
	    sum1 = 0.0;
	    sum2 = 0.0;
	    sum3 = 0.0;
	    sum4 = 0.0;
	    sum5 = 0.0;
	    sum6 = 0.0;
	    sum7 = 0.0;
	    for (k = -5; k <= 5; k++) {
		for (j = -5; j <= 5; j++) {
		    sum1 =
			sum1 + G2akernel[k + 5][j + 5] * logdata(x + j,
								 y + k);
		    sum2 =
			sum2 + G2bkernel[k + 5][j + 5] * logdata(x + j,
								 y + k);
		    sum3 =
			sum3 + H2akernel[k + 5][j + 5] * logdata(x + j,
								 y + k);
		    sum4 =
			sum4 + H2bkernel[k + 5][j + 5] * logdata(x + j,
								 y + k);
		    sum5 =
			sum5 + G2ckernel[k + 5][j + 5] * logdata(x + j,
								 y + k);
		    sum6 =
			sum6 + H2ckernel[k + 5][j + 5] * logdata(x + j,
								 y + k);
		    sum7 =
			sum7 + H2dkernel[k + 5][j + 5] * logdata(x + j,
								 y + k);
		}
	    }
	    conv(x, y).G2a = sum1;
	    conv(x, y).G2b = sum2;
	    conv(x, y).G2c = sum5;
	    conv(x, y).H2a = sum3;
	    conv(x, y).H2b = sum4;
	    conv(x, y).H2c = sum6;
	    conv(x, y).H2d = sum7;
	}
    }

  printf("point 5\n");

    /*remember to take into acount image read upside down */

    initsteer();
   
    /*make arrays of responses steered to each angle */
    for (y = 6; y <= image.dimy() - 7; y++) {
	for (x = 6; x <= image.dimx() - 7; x++) {

	    for (int i = 0; i < 32; i++) {
		steered(x, y, i) = steer(i, x, y);
	    }
	}
    }

   printf("point 5.5\n");

    max = 0;
    for (y = 6; y <= steered.dimy() - 7; y++) {
	for (x = 6; x <= steered.dimx() - 7; x++) {
	    for (j = 0; j < steered.dimz(); j++) {
		if (steered(x, y, j) > max)
		    max = steered(x, y, j);
	    }
	}
    }
    
   printf("point 6\n");

    /*to find areas with high contrast */
    /*compute C2 and C3 */
    for (y = 0; y < image.dimy(); y++) {
	for (x = 0; x < image.dimx(); x++) {
	    C2(x, y) =
		0.5 * (pow(conv(x, y).G2a, 2) - pow(conv(x, y).G2c, 2)) +
		0.46875 * (pow(conv(x, y).H2a, 2) -
			   pow(conv(x, y).H2d,
			       2)) + 0.28125 * (pow(conv(x, y).H2b,
						    2) - pow(conv(x,
								  y).H2c,
							     2)) +
		0.1875 * (conv(x, y).H2a * conv(x, y).H2c -
			  conv(x, y).H2b * conv(x, y).H2d);

	    C3(x, y) = -(conv(x, y).G2a * conv(x, y).G2b) -
		(conv(x, y).G2b * conv(x, y).G2c) -
		0.9375 * (conv(x, y).H2c * conv(x, y).H2d -
			  conv(x, y).H2a * conv(x, y).H2b) -
		1.6875 * conv(x, y).H2b * conv(x, y).H2c -
		0.1875 * conv(x, y).H2a * conv(x, y).H2d;

	}
    }

   printf("point 7\n");

    /*Compute strength of dominant orientation at each pixel */
    max = 0;
    for (y = 0; y < image.dimy(); y++) {
	for (x = 0; x < image.dimx(); x++) {
	    Ostrength(x, y) = sqrt(pow(C2(x, y), 2) + pow(C3(x, y), 2));
	    if (Ostrength(x,y) > max)
		max = Ostrength(x, y);
	}
    }

   //make into grayscale
   for (y = 0; y < image.dimy(); y++) {
	for (x = 0; x < image.dimx(); x++) {
		image(x, y) = (unsigned char)((Ostrength(x, y)/max)*255);
	}
    }
   //image of high contrast areas
   backend->SaveInterImage(image, argv[1], "Ostrength");

   //write data from steered responses
   for (j = 0; j < steered.dimz(); j++) {
       for (y = 0; y < steered.dimy() ; y++) {
	   for (x = 0; x < steered.dimx() ; x++) {
	       Ostrength(x, y) = steered(x, y, j);
	    }
        }
	sprintf(outname,"steer%d",j);
	backend->SaveInterDImage(Ostrength, argv[1], outname);
    }


return 0;
}
