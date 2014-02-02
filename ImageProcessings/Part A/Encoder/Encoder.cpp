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
char *source=0,*message=0,*output=0;
const int MESSAGE_VALUE = 1; //value to be assign to carier
const int THRESHOLD = 200;
/// Function header
void hide_image(Mat& src, Mat& dst, Mat& msg);


Mat src; Mat src_gray; Mat src_message;Mat src_message_binary;
int main(int argc, char *argv[]) try
{

	if(argc == 4) //Change default names
	{
		source = argv[1];
		message = argv[2];
		output = argv[3];

	} else //Use default names
	{
		source = "grayscale_carrier.png";
		message = "message.png";
		output = "testing.png";

	}

	// Load source image and convert it to gray
	src = imread( source, 1 );
	src_message = imread( message, 1 );
	// Convert image to gray
	cvtColor( src, src_gray, CV_BGR2GRAY );
	cvtColor( src_message, src_message, CV_BGR2GRAY );
	//Resize message image to carrier size
	resize(src_message, src_message, src.size(), 0, 0, INTER_LINEAR);
	//Create output matrix to store the image
	Mat src_output = Mat::zeros( src_gray.size(), src_gray.type() );
	//treshold 0 white; 1 black
	threshold( src_message, src_message_binary, THRESHOLD, MESSAGE_VALUE, THRESH_BINARY_INV  );
	//Hide image. MESSAGE_VALUE where black, 0 otherwise.	
	hide_image(src_gray, src_output, src_message_binary);
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

	if(src.rows!=msg.rows || src.cols!=msg.cols || src.rows!=dst.rows || src.cols!=dst.cols ) throw std::invalid_argument("images size do not match");
	else
	{
		int channels = src.channels();
		int nRows = src.rows;
		int nCols = src.cols * channels;

		if (src.isContinuous())
		{
			nCols *= nRows;
			nRows = 1;
		}

		int i,j;
		uchar* d;//destination
		uchar* s;//source
		uchar* m;//message
		for( i = 0; i < nRows; ++i)
		{
			s = src.ptr<uchar>(i);
			d= dst.ptr<uchar>(i);
			m = msg.ptr<uchar>(i);
			for ( j = 0; j < nCols; ++j)
			{
				if(s[j]+m[j]<=255)	d[j] = s[j]+m[j];//if overflow occur leave
				else d[j]=s[j];
			}

		}
	}


}

