#include <iostream>
#include <opencv.hpp>
#include <iostream> // standard C++ I/O
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include <conio.h>

using namespace cv;
using namespace std;
RNG rng;
char *source=0,*message=0,*original=0,*rand_message="randomized message";
char *pass=0;
const int MESSAGE_VALUE = 1; //value to be assign to carier
const int THRESHOLD = 200;

/// Function header
unsigned long hash_fun(unsigned char *str);
void get_hidden_message(Mat& original, Mat& src, Mat& msg);
void de_reorder(Mat& source_message ,Mat& reorderd_message);

Mat src; Mat src_gray; Mat src_message;Mat src_message_binary;Mat src_hidden_message;Mat src_original;
int main(int argc, char *argv[]) try
{

	if(argc == 2) // only password change
	{
		pass = argv[1];

	} else if(argc == 5) //Change default names
	{
		source = argv[1];
		message = argv[2];
		original = argv[3];
		pass = argv[4];

	} else //Use default names
	{
		source = "testing.png";
		message = "recoverd_message.png";
		original = "grayscale_carrier.png";
		pass = "password";

	}
	//Load source images
	src_gray = imread( source, 1 );
	cvtColor( src_gray, src_gray, CV_BGR2GRAY );
	src_original = imread( original, 1 );
	cvtColor( src_original, src_original, CV_BGR2GRAY );
	Mat src_hidden_message = Mat::zeros( src_gray.size(), src_gray.type() );
	//Get hidden randomized message
	get_hidden_message(src_original, src_gray, src_hidden_message);
	Mat src_hidden_message_dereordere = Mat::zeros( src_gray.size(), src_gray.type() );
	//De-randomize message, get original message
	de_reorder(src_hidden_message , src_hidden_message_dereordere);

	//Save images
	imwrite(message, src_hidden_message_dereordere);

	// Create windows
	namedWindow( source, CV_WINDOW_AUTOSIZE );
	imshow( source, src_gray );
	namedWindow(rand_message , CV_WINDOW_AUTOSIZE );
	imshow( rand_message, src_hidden_message );
	namedWindow( message, CV_WINDOW_AUTOSIZE );
	imshow( message, src_hidden_message_dereordere );




	waitKey(0);
	return(0);
}

/** @function thresh_callback */

catch (cv::Exception &ex)
{
	std::cerr << ex.code << std::endl;
}
catch (std::exception &ex)
{
	std::cerr << "Exception of type " << typeid(ex).name() << " with message " << ex.what() << std::endl;
}
catch (...)
{
	std::cerr << "Unexpected error!" << std::endl;
} 

//function from http://www.cse.yorku.ca/~oz/hash.html
unsigned long hash_fun(unsigned char *str)
{
	unsigned long hash = 5381;
	int c;

	while (c = *str++)
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

	return hash;
}
/**       
* Function used to get message from source image. Funcgtion subtract original image from source image.
* @param original Matrix containing original image.
* @param src Matrix contains image with randomized message.
* @param msg Matrix to be created which contains randomized message.
*/
void get_hidden_message(Mat& original, Mat& src, Mat& msg)
{
	if(src.rows!=msg.rows || src.cols!=msg.cols || src.rows!=original.rows || src.cols!=original.cols ) throw std::invalid_argument("images size do not match");
	else{
		int channels = src.channels();
		int nRows = src.rows;
		int nCols = src.cols * channels;

		int i,j;
		uchar* o;//original
		uchar* s;//source
		uchar* m;//message
		for( i = 0; i < nRows; i++)
		{
			s = src.ptr<uchar>(i);
			o = original.ptr<uchar>(i);
			m = msg.ptr<uchar>(i);
			for ( j = 0; j < nCols; j++)
			{
				if(s[j]-o[j]>0)	m[j] = s[j]-o[j];//if overflow occur leave
				else m[j]=255;
			}

		}
	}

}
/**
* Function used to obtain original message from randomized message for gray image.
* @param source_message Matrix containing randomized message.
* @param reorderd_message Matrix containing original message.
*/
void de_reorder(Mat& source_message ,Mat& reorderd_message)
{
	//We are working on copy of the randomized message
	source_message.copyTo(reorderd_message);

	//Create radomized matrix
	Mat rand_mat(source_message.rows, source_message.cols, CV_32FC2, Scalar(0,0) );
	Mat_<Vec2f> _II = rand_mat;
	//Variables to work
	uchar* sb=0;// swap bufor
	uchar* rm=0;// reorderd_message
	int tmp=0;

	// Seeding random number generator with given passowrd
	rng(hash_fun( ( unsigned char* ) pass ) );
	//Fill the random matrix; need seperate loops
	for( int i = 0; i < rand_mat.rows; i++)
	{
		for( int j = 0; j < rand_mat.cols; j++ )
		{
			//Select random positions
			_II(i,j)[0] = rng.uniform(0, rand_mat.rows);
			_II(i,j)[1] = rng.uniform(0, rand_mat.cols);
		}
	}
	// De-randomize algorithm; swap position backwards
	for( int i = rand_mat.rows-1; i >= 0; i--)
	{
		rm = reorderd_message.ptr<uchar>(i);
		for( int j = rand_mat.cols-1; j >= 0; j--)
		{	
			sb = reorderd_message.ptr<uchar>( (int)_II(i,j)[0] );
			tmp=sb[ (int)_II(i,j)[1] ];
			sb[ (int)_II(i,j)[1] ]=rm[j];
			rm[j]=tmp;
		}
	}
}


