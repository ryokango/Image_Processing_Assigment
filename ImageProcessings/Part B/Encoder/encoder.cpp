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
char *source=0,*message=0,*output=0;
char *pass=0;
const int MESSAGE_VALUE = 1; //value to be assign to carier
const int THRESHOLD = 200;
unsigned long password;
//vector<Point2f> points;
//Mat pointsMat = Mat(points)

/// Function header
unsigned long hash_fun(unsigned char *str);
void reorder(Mat& rand_matrix,Mat& source_message ,Mat& reorderd_message);
void hide_image (Mat& src, Mat& dst, Mat& msg);
void get_hidden_message(Mat& original, Mat& src, Mat& msg);
void de_reorder(Mat& rand_matrix,Mat& source_message ,Mat& reorderd_message);

Mat src; Mat src_gray; Mat src_message;Mat src_message_binary;Mat src_hidden_message;Mat src_me;
int main(int argc, char *argv[]) try
{
	if(argc == 2) // only password change
	{
		pass = argv[1];

	} else if(argc == 5) //Change default names
	{
		source = argv[1];
		message = argv[2];
		output = argv[3];
		pass = argv[4];


	} else //Use default names
	{
		source = "grayscale_carrier.png";
		message = "message.png";
		output = "testing.png";
		pass = "password";

	}
	//Load source image and convert to gray
	src = imread( source, 1 );
	cvtColor( src, src_gray, CV_BGR2GRAY );
	src_me = imread( message, 1 );
	cvtColor( src_me, src_message, CV_BGR2GRAY );
	//Resize message image to carrier size
	resize(src_message, src_message, src_gray.size(), 0, 0, INTER_LINEAR);
	//Create random swap matrix
	Mat rand_mat(src_gray.rows, src_gray.cols, CV_32FC2, Scalar(0,0) );
	//Create output matrix to store the image
	Mat src_output = Mat::zeros( src_gray.size(), src_gray.type() );
	//treshold 0 white; 1 black
	threshold( src_message, src_message_binary, THRESHOLD, MESSAGE_VALUE, THRESH_BINARY_INV  );
	//Create output matrix to store the image
	Mat reorderd_message = Mat::zeros( src_gray.size(), src_gray.type() );
	//Randomize message
	reorder(rand_mat, src_message_binary ,reorderd_message);
	Mat src_hidden_message_dereordere_binary = Mat::zeros( src_gray.size(), src_gray.type() );
	//Hide image. MESSAGE_VALUE where black, 0 otherwise.		
	hide_image(src_gray, src_output, reorderd_message);
	//Save images
	imwrite(output,src_output);

	// Create windows
	namedWindow( source, CV_WINDOW_AUTOSIZE );
	imshow( source, src_gray );
	namedWindow( message, CV_WINDOW_AUTOSIZE );
	imshow( message, src_message );
	namedWindow( output, CV_WINDOW_AUTOSIZE );
	imshow( output, src_output );

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

/**       
* Function used to hide message into gary image. Function adds elements from source matrix and message matrix to output matrix.    
* @param src Matrix containing source image.
* @param dst Matrix containing output image with hidden message.
* @param msg Matrix containing message.
*/
void hide_image (Mat& src, Mat& dst, Mat& msg)
{
	//Matrix size chceck
	if(src.rows!=msg.rows || src.cols!=msg.rows || src.rows!=dst.rows || src.cols!=dst.rows ) throw std::invalid_argument("images size do not match");
	else
	{
		int channels = src.channels();
		int nRows = src.rows;
		int nCols = src.cols * channels;

		int i,j;
		uchar* d;//destination
		uchar* s;//source
		uchar* m;//message
		for( i = 0; i < nRows; i++)
		{
			s = src.ptr<uchar>(i);
			d= dst.ptr<uchar>(i);
			m = msg.ptr<uchar>(i);
			for ( j = 0; j < nCols; j++)
			{
				if(s[j]+m[j]<=255)	d[j] = s[j]+m[j];//if overflow occur leave
				else d[j]=s[j];
			}

		}
	}


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
* Function used to randomize message. Message is randomized according to "random" swaps generated for every point.
* No overflow is checked for message, possible data loss can occur.
* @param rand_matrix Output matrix containing random swaps.
* @param source_message Matrix containing original message.
* @param reorderd_message Matrix containing output randomized message.
*/
void reorder(Mat& rand_matrix,Mat& source_message ,Mat& reorderd_message)
{
	//We are working on copy of the original message
	source_message.copyTo(reorderd_message);

	//Variables to work
	Mat_<Vec2f> _II = rand_matrix;
	uchar* sb=0;// swap bufor
	uchar* rm=0;// reorderd_message
	int tmp=0; // temp used to swap

	// Seeding random number generator with given passowrd
	rng(hash_fun((unsigned char*)pass));

	//Hiding function
	for( int i = 0; i < rand_matrix.rows; i++)
	{
		rm = reorderd_message.ptr<uchar>(i);
		for( int j = 0; j < rand_matrix.cols; j++ )

		{	//Select random positions
			_II(i,j)[0] = rng.uniform(0, rand_matrix.rows);
			_II(i,j)[1] = rng.uniform(0, rand_matrix.cols);
			//Swap points
			sb = reorderd_message.ptr<uchar>( (int)_II(i,j)[0] );
			tmp=sb[ (int)_II(i,j)[1] ];
			sb[ (int)_II(i,j)[1] ]=rm[j];
			rm[j]=tmp;
		}


	}
	// Allocate the drawed random points to the matrix
	rand_matrix=_II;

}