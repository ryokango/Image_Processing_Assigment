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
char *source=0,*message=0,*original=0,*output=0;
char *pass=0;
const int MESSAGE_VALUE = 1; //value to be assign to carier
const int THRESHOLD = 200; //Unused

// Function header
unsigned long hash_fun(unsigned char *str);
void get_hidden_message(Mat& original, Mat& src, Mat& msg);
void de_reorder_color(Mat& source_message ,Mat& reorderd_message,Mat& rand_matrix);
void create_rand_color(Mat& rand_matrix,Mat& original);
//Matrices variables
Mat src; Mat src_message;Mat src_message_binary;Mat src_me;Mat  reorderd_message;Mat src_original;
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
		original = "carrier.png";
		pass = "password";

	}
	//Load source image
	src = imread( source, 1 );
	src_original = imread( original, 1 );
	//Create swap matrix for randomizing the message
	Mat rand_mat(src.rows, src.cols, CV_32FC2, Scalar(0,0) );
	//Intialize random matrix 
	create_rand_color(rand_mat, src_original);
	//Matrix to store randomized hidden mesage
	Mat src_hidden_message = Mat::zeros( src.size(), src.type() );
	//Restoring randomized hidden message
	get_hidden_message(src_original, src, src_hidden_message);
	//Matrix to store hidden mesage
	Mat src_hidden_message_dereorderd = Mat::zeros( src.size(), src.type() );
	// Restore original message
	de_reorder_color(src_hidden_message, src_hidden_message_dereorderd, rand_mat);
	// Convert message from color to gray
	cvtColor( src_hidden_message_dereorderd, src_hidden_message_dereorderd, CV_BGR2GRAY );
	threshold( src_hidden_message_dereorderd, src_hidden_message_dereorderd, 250, 255, 0  );//olny black and white
	threshold( src_hidden_message, src_hidden_message, 1, 255, 1  );//olny RGB or black
	//Save images
	imwrite(message,src_hidden_message_dereorderd);

	// Create windows
	namedWindow( source, CV_WINDOW_AUTOSIZE );
	imshow( source, src );
	namedWindow( original, CV_WINDOW_AUTOSIZE );
	imshow( original, src_original );
	namedWindow( message, CV_WINDOW_AUTOSIZE );
	imshow( message, src_hidden_message_dereorderd );
	namedWindow( "Randomized message", CV_WINDOW_AUTOSIZE );
	imshow( "Randomized message", src_hidden_message );

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
				_II(i,j)[1] = rng.uniform(0, nCols); //Random column and channel
				o = original.ptr<uchar>( (int)_II(i,j)[0] );//ROW
				tmp=o[ (int)_II(i,j)[1] ];//COLUMN + CHANEL
				//	cout<<tmp<<endl;
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

/**       
* Function used to get "randomized" message from source image. Funcgtion subtract original image from source image.
* @param original Matrix containing original image.
* @param src Matrix contains image with randomized message.
* @param msg Matrix to be created which contains randomized message.
*/
void get_hidden_message(Mat& original, Mat& src, Mat& msg)
{
	if(src.rows!=msg.rows || src.cols!=msg.cols || src.rows!=original.rows || src.cols!=original.cols )
	{
		throw std::invalid_argument("images size do not match");
	}
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
* Function used to obtain original message from randomized message for color image.
* @param source_message Matrix containing randomized message.
* @param reorderd_message Matrix containing original message.
* @param rand_matrix Matrix containing random swaps.
*/
void de_reorder_color(Mat& source_message ,Mat& reorderd_message,Mat& rand_matrix)
{
	//We are working on copy of the randomized message
	source_message.copyTo(reorderd_message);

	//Create radomized matrix
	Mat_<Vec2f> _II = rand_matrix;
	//Variables to work
	uchar* sb=0;// swap bufor
	uchar* rm=0;// reorderd_message
	int tmp=0;
	int Nchannels = source_message.channels();

	// De-randomize algorithm; swap position backwards
	for( int i = rand_matrix.rows-1; i >= 0; i--)
	{
		rm = reorderd_message.ptr<uchar>(i);
		for( int j = rand_matrix.cols-1; j >= 0; j--)
		{	
			sb = reorderd_message.ptr<uchar>( (int)_II(i,j)[0] );
			tmp=sb[ (int)_II(i,j)[1] ];
			sb[ (int)_II(i,j)[1] ]=rm[j*Nchannels];//Save data only on one channel B [BGR BGR BGR]
			rm[j*Nchannels]=tmp;
		}
	}
}
