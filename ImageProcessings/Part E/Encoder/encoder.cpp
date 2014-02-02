#include <iostream>
#include <opencv.hpp>
#include <iostream> // standard C++ I/O
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include <bitset>
#include <iostream>
#include <fstream>


using namespace cv;
using namespace std;
RNG rng;
char *source=0,*message=0,*output=0;
char *pass=0;
const int MESSAGE_VALUE = 1; //value to be assign to carier
const int THRESHOLD = 200;

// Function header
unsigned long hash_fun(unsigned char *str);
void reorder_color(Mat& rand_matrix,Mat& source_message ,Mat& reorderd_message);
void hide_message_color (Mat& src, Mat& dst, Mat& msg);
void get_hidden_message(Mat& original, Mat& src, Mat& msg);
void de_reorder_color(Mat& source_message ,Mat& reorderd_message,Mat& rand_matrix);
void create_rand_color(Mat& rand_matrix,Mat& original);
void bin2mat(char* name, Mat& dest);
void mat2bin(char* name, Mat& dest);
bitset<8> ToBits(unsigned char byte);

Mat src;Mat src_message_binary;Mat src_me;Mat  reorderd_message;Mat src_message;

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
		message = "message.dat";
		output = "testing.png";
		pass = "password";

	}
	Mat src = imread( source, 1 ); //Read source image
	Mat src_hidden_message = Mat::zeros( src.size(), src.type() ); //Matrix to Store image and hidden message
	Mat src_output = Mat::zeros( src.size(), src.type() ); // Matrix to store output image with information
	Mat src_message = Mat::zeros( src.size(), src.type() ); // Matrix to store message
	Mat reordered_message = Mat::zeros( src.size(), src.type() ); // Matrix to store randomized message
	Mat rand_mat(src.rows, src.cols * src.channels() , CV_8UC2, Scalar(0,0) ); // Matrix for "random" changes

	bin2mat(message,src_message); // Write message into matrix
	create_rand_color(rand_mat,src); // Create random matrix according to selected image
	reorder_color(rand_mat, src_message, reordered_message); // Randomize message according to random matrix
	//Hide image. MESSAGE_VALUE where black, 0 otherwise.	
	hide_message_color (src, src_output, reordered_message);

	imwrite(output,src_output); // save image with hidden message


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
* Function used to  hide message into image. Function adds elements from source matrix and message matrix to output matrix.    
* @param src Matrix containing source image.
* @param dst Matrix containing output image with hidden message.
* @param msg Matrix containing message.
*/
void hide_image (Mat& src, Mat& dst, Mat& msg)
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
* Function used to convert byte into bits.
* @param byte Byte value to be converted into bits.
*/
bitset<8> ToBits(unsigned char byte)
{
	return bitset<8>(byte);
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
* Function used to hide message into color image. Function adds elements from source matrix and message matrix to output matrix.    
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
{
	source_message.copyTo(reorderd_message);

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
			sb[ (int)_II(i,j)[1] ]=rm[j]; //Save data only on one channel B [BGR BGR BGR]
			rm[j]=tmp;
		}


	}

}
/**       
* Function used to create random matrix on the basis of original image. Function genrates random swap for each pixel,
* if overflow occurs another random swap is generated for this pixel.
* @param original Matrix containing original image.
* @param rand_matrix Matrix to be created which contains random swaps.
*/
void create_rand_color(Mat& rand_matrix,Mat& original)
{
	Mat copy;
	original.copyTo(copy);
	//Variables to work
	Mat_<Vec2f> _II = rand_matrix;
	int channels = original.channels();
	int nRows = original.rows;
	int nCols = original.cols * channels;
	uchar* co=0;// copy of the original image
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
				co = copy.ptr<uchar>( (int)_II(i,j)[0] );//ROW
				tmp=co[ (int)_II(i,j)[1] ];//COLUMN + CHANEL
				if(tmp<255)//NO OVERFLOW
				{
					co[ (int)_II(i,j)[1] ] += 1;//secure if the same point will be selected several times
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
				else m[j]=0;
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

	// De-randomize algorithm; swap position backwards
	for( int i = rand_matrix.rows-1; i >= 0; i--)
	{
		rm = reorderd_message.ptr<uchar>(i);
		for( int j = rand_matrix.cols-1; j >= 0; j--)
		{	
			sb = reorderd_message.ptr<uchar>( (int)_II(i,j)[0] );
			tmp=sb[ (int)_II(i,j)[1] ];
			sb[ (int)_II(i,j)[1] ]=rm[j];//Save data only on one channel B [BGR BGR BGR]
			rm[j]=tmp;
		}
	}
}
/**       
* Function used to convert matrix into file. Function gather 8  bits from sequence and create one byte. Functions reads 
* header which conains message size and then read message and save it in the given file.
* @param name Name of the file to be read from.
* @param dest Matrix containing data from file.
*/
void mat2bin(char* name, Mat& dest)
{
	char  data;
	bitset<8> my_set;
	ofstream file (name, ios::out|ios::binary);
	int max_message_size = dest.rows * dest.cols * dest.channels() ; 
	std::string s_max = std::to_string(max_message_size);
	//Variables to read header
	std::string header;
	int header_size=s_max.size();
	//Read bytes
	const int channels = dest.channels();
	switch(channels)
	{
	case 1:
		{
			MatIterator_<uchar> it = dest.begin<unsigned char>();
			for(int j=0; j < header_size; j++)
			{
				for(int i=0; i<8 ;i++)
				{

					my_set[i] = ( int)*( it++ );
				}
				unsigned long cos = my_set.to_ulong();
				header.push_back( (char) cos );
			}
			int message_size = atoi( header.c_str() );
			//Read bytes
			for(int j=0; j<message_size; j++)
			{

				//read bits
				for(int i=0; i<8 ;i++)
				{

					my_set[i] = ( int)*( it++ );
				}
				unsigned long cos = my_set.to_ulong();
				data = (char) cos;
				file.write( &data, sizeof(unsigned char) );

			}

			break;
		}
	case 3:
		{
			//Variables for case 3
			int j=0;
			int i=0;
			int rgb=0;
			uchar* rm = dest.ptr<uchar>(0);
			MatIterator_<Vec3b> it = dest.begin<Vec3b>();
			for(; j < header_size;)
			{

				rgb = (rgb)%3;
				my_set[i] = (int)(*it)[rgb];
				if(rgb==2) it++;
				if (i == 7)
				{

					unsigned long cos = my_set.to_ulong();
					header.push_back( (char) cos );
					j++;
				}
				i=(i+1)%8;
				rgb++;


			}
			int message_size = atoi( header.c_str() );
			//continue to read message and write to file
			for(j=0; j<message_size;)
			{
				rgb = rgb%3;
				my_set[i] = (int)(*it)[rgb];
				if(rgb==2) it++;
				if (i == 7)
				{

					unsigned long cos = my_set.to_ulong();
					data = (char) cos;
					file.write( &data, sizeof(unsigned char) );
					j++;
				}
				i=(i+1)%8;
				rgb++;

			}

		}
	}
	file.close();
	//delete[] memblock;

}
/**       
* Function used to convert file into matrix. Function copies data from file bit by bit and insters it into matrix.
* Message contains header(message size) and message as binary sequence. Sequence is hidden row by row.
* @param name Name of the file to be read from.
* @param dest Matrix containing data from file.
*/
void bin2mat(char* name, Mat& dest)
{
	streampos size,beg;
	char * file_message;
	bitset<8> my_set;
	ifstream file (name, ios::in|ios::binary|ios::ate);
	if (!file.is_open())  throw std::invalid_argument("can not open the file");
	else
	{
		//maximum message size
		//we left number such number of bits for header which contain message size
		// for 512x512 1CH it is 262144; we left 6 bytes as header for message size 100 bytes 000100
		int max_message_size = dest.rows * dest.cols * dest.channels() ; 
		size = file.tellg();
		file_message = new char [size];
		file.seekg (0, ios::beg);
		beg = file.tellg();
		file.read (file_message, size);
		char bit_size=8;
		int counter= (int) size;//bytes
		//calculate header
		std::string s_max = std::to_string(max_message_size);
		std::string s_cout = std::to_string(counter);
		std::string header;
		for(int i=0 ; i < s_max.size() - s_cout.size() ; i++) header.push_back('0');
		header+=s_cout;

		int header_size=header.size();
		counter+=header_size; //message size + header size
		max_message_size-=header_size; //max message size - header size
		std::string message = header + file_message;
		const char* memblock = message.c_str();
		int counter_bit = counter*bit_size;//bits
		if( counter_bit > max_message_size) throw std::invalid_argument("size of the file is to big");
		int repitition = ceil((double)counter_bit/(double)( dest.cols*dest.channels() )); // rows
		//stored in one column. Size is (n+1) * row size * channels. extra row for zeros if not fully filled with bits
		Mat Bits(1, (repitition) * dest.cols * dest.channels()  , CV_8UC1, Scalar(0) );//Vector to assign data
		uchar* p = Bits.data;
		for(int j=0 ; j < counter ; j++)
		{
			my_set=ToBits(*memblock);
			for(int i=0 ; i < bit_size ; i++)
			{ 
				p[j*bit_size+i] = (unsigned char)my_set[i];
			}
			memblock++;
		}

		for(int z=0; z< repitition; z++)//i next row
		{
			uchar* dp = dest.ptr<uchar>(z);
			uchar *tp = Bits.ptr<uchar>(0);
			for(int j=0; j<dest.cols * dest.channels(); j++)
			{
				dp[j] = tp[z * dest.cols * dest.channels() + j];
			}
		}

		file.close();



	}
}


