#include <iostream>
#include <opencv.hpp>
#include <iostream> // standard C++ I/O
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace cv;
using namespace std;
RNG rng;
char *source=0,*message=0,*output=0;
char *pass=0;
const int MESSAGE_VALUE = 1; //value to be assign to carier
const int THRESHOLD = 200; //unused


// Function header
unsigned long hash_fun(unsigned char *str);
void reorder_color(Mat& rand_matrix,Mat& source_message ,Mat& reorderd_message);
void hide_message_color (Mat& src, Mat& dst, Mat& msg);
void create_rand_color(Mat& rand_matrix,Mat& original);
// Matrices variables
Mat src; Mat src_message;Mat src_message_binary;Mat src_me;Mat reordered_message;
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
		source = "carrier.png";
		message = "message.png";
		output = "testing.png";
		pass = "password";

	}
	//Load images
	src = imread( source, 1 );
	src_message = imread( message, 1 );
	//Convert message to gray
	cvtColor( src_message, src_message, CV_BGR2GRAY );
	//Resize message image to carrier size
	resize(src_message, src_message, src.size(), 0, 0, INTER_LINEAR);
	//treshold the message 0 white; 1 black 
	threshold( src_message, src_message_binary, THRESHOLD, MESSAGE_VALUE, THRESH_BINARY_INV  );
	//Create output matrix to store the image
	Mat src_output = Mat::zeros( src.size(), src.type() );
	//Create swap matrix for randomizing the message
	Mat rand_mat(src.rows, src.cols, CV_32FC2, Scalar(0,0) );
	//Intialize random matrix 
	create_rand_color(rand_mat,src);
	//Randomize message
	reorder_color(rand_mat, src_message_binary, reordered_message);

	//Hide imageon  the last bit. +1 where black, +0 otherwise.	
	hide_message_color (src, src_output, reordered_message);
	//Save images
	imwrite(output,src_output);

	// Create windows
	namedWindow( source, CV_WINDOW_AUTOSIZE );
	imshow( source, src );
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
* Function used to  hide message into image. Function adds elements from source matrix and message matrix to output matrix.    
* @param src Matrix containing source image.
* @param dst Matrix containing output image with hidden message.
* @param msg Matrix containing message.
*/
void hide_message_color (Mat& src, Mat& dst, Mat& msg)
{
	//Matrix size chceck
	if(src.rows!=msg.rows || src.cols!=msg.cols || src.rows!=dst.rows || src.cols!=dst.cols ) throw std::invalid_argument("images size do not match");
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

/**       
* Function used to randomize message. Message is randomized according to "random" swaps contained in random matrix.
* @param rand_matrix Matrix containing random swaps.
* @param source_message Matrix containing original message.
* @param reorderd_message Matrix containing output randomized message.
*/
void reorder_color(Mat& rand_matrix,Mat& source_message ,Mat& reorderd_message)
{//Swaping gray scale reorderd_message due to the rand_matrix into 3 chanel(color) reorderd_message

	//Copy one channel to the reorderd_message
	// Copy gray image to B channel
	vector<Mat> channels;
	channels.push_back(source_message);
	channels.push_back(Mat::zeros(source_message.size(), CV_8UC1));
	channels.push_back(Mat::zeros(source_message.size(), CV_8UC1));
	merge(channels, reorderd_message);

	//Variables to work
	Mat_<Vec2f> _II = rand_matrix;
	uchar* sb=0;// swap bufor
	uchar* rm=0;// reorderd_message
	int tmp=0; // temp used to swap
	int Nchannels = reorderd_message.channels();


	//Hiding function
	for( int i = 0; i < rand_matrix.rows; i++)
	{
		rm = reorderd_message.ptr<uchar>(i);

		for( int j = 0; j < rand_matrix.cols; j++ )
		{	
			//Swap points
			sb = reorderd_message.ptr<uchar>( (int)_II(i,j)[0] );//ROW
			tmp=sb[ (int)_II(i,j)[1] ];//COLUMN + CHANEL
			//Swap points
			sb[ (int)_II(i,j)[1] ]=rm[j*Nchannels]; //Save data only on one channel B [BGR BGR BGR]
			rm[j*Nchannels]=tmp;
		}


	}
	Mat binary_message;
	threshold( reorderd_message, binary_message, 0, 255, 0  );
	namedWindow( "Randomized message", CV_WINDOW_AUTOSIZE );
	imshow( "Randomized message", binary_message );

}

/**       
* Function used to create random matrix on the basis of original image. Function genrates random swap for each pixel,
* if overflow occurs another random swap is generated for this pixel.
* @param original Matrix containing original image.
* @param rand_matrix Matrix to be created which contains random swaps.
*/
void create_rand_color(Mat& rand_matrix,Mat& original)
{
	//Variables to work
	Mat_<Vec2f> _II = rand_matrix;
	int channels = original.channels();
	int nRows = original.rows;
	int nCols = original.cols * channels;
	uchar* o=0;// copy of the original image
	int tmp=0; // temp used to swap

	// Seeding random number generator with given passowrd
	rng(hash_fun( ( unsigned char*)pass ) );

	//Selecting possible random swaps
	for( int i = 0; i < rand_matrix.rows; i++)
	{
		for( int j = 0; j < rand_matrix.cols; j++ )

		{	
			bool flag = false; //no overflow 
			//Do while no overflow occur
			while(!flag)
			{
				//Select random positions
				_II(i,j)[0] = rng.uniform(0, nRows); //Random row
				_II(i,j)[1] = rng.uniform(0, nCols); //Random column and chan
				//Check if no overflow for a seleceted piont
				o = original.ptr<uchar>( (int)_II(i,j)[0] );//ROW
				tmp=o[ (int)_II(i,j)[1] ];//COLUMN + CHANEL
				if(tmp<255)//NO OVERFLOW
				{
					flag = true;//Allowed to change and go to next point
				}
			}
		}
	}
	// Allocate the drawed random points to the matrix
	rand_matrix=_II;

}
