#include <iostream>
#include <opencv.hpp>
#include <iostream> // standard C++ I
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace cv;
using namespace std;
char *source=0,*message=0,*original=0;
/// Function header
void nice_add(Mat& src, Mat& dst, Mat& msg);
void get_hidden_message(Mat& original, Mat& src, Mat& msg);


Mat src;Mat src_original;Mat src_original_gray;
int main(int argc, char *argv[]) try
{

	if(argc == 4) //Change default names
	{
		source = argv[1];
		message = argv[2];
		original = argv[3];

	} else //Use default names
	{
		source = "testing.png";
		message = "recoverd_message.png";
		original = "grayscale_carrier.png";
	}

	// Load source image
	src = imread( source, 1 );
	src_original_gray = imread( original, 1 );
	//Create output matrix to store the image
	Mat src_message = Mat::zeros( src_original_gray.size(), src_original_gray.type() );
	// Recover hidden message
	get_hidden_message(src_original_gray, src, src_message);
	//Save images
	imwrite(message,src_message);
	// Create windows
	namedWindow( source, CV_WINDOW_AUTOSIZE );
	imshow( source, src );
	namedWindow( original, CV_WINDOW_AUTOSIZE );
	imshow( original, src_original_gray );
	namedWindow( message, CV_WINDOW_AUTOSIZE );
	imshow( message, src_message );


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
* Function used to get message from source image. Function subtract original image from source image.
* @param original Matrix containing original image.
* @param src Matrix contains image with message.
* @param msg Matrix to be created which contains recovered message.
*/
void get_hidden_message(Mat& original, Mat& src, Mat& msg)
{
	if(src.rows!=msg.rows || src.cols!=msg.rows || src.rows!=original.rows || src.cols!=original.rows ) throw std::invalid_argument("images size do not match");
	else{
		int channels = src.channels();
		int nRows = src.rows;
		int nCols = src.cols * channels;

		if (src.isContinuous())
		{
			nCols *= nRows;
			nRows = 1;
		}

		int i,j;
		uchar* o;//original
		uchar* s;//source
		uchar* m;//message
		for( i = 0; i < nRows; ++i)
		{
			s = src.ptr<uchar>(i);
			o = original.ptr<uchar>(i);
			m = msg.ptr<uchar>(i);
			for ( j = 0; j < nCols; ++j)
			{
				if(s[j]-o[j]>0)	m[j] = s[j]-o[j];//if overflow occur leave
				else m[j]=255;
			}

		}
	}

}
