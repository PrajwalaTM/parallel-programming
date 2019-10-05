#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

int main()
{
	Mat img, grayImage, face_img;
	CascadeClassifier face_detect;
	vector<Rect> faces;

	if(!face_detect.load("/usr/local/share/OpenCV/haarcascades/haarcascade_frontalface_alt.xml"))
		cout << "error";

	img = imread("/home/ashritha/Desktop/flower.jpg");
	namedWindow("Input_Image",WINDOW_NORMAL);
	imshow("Input_Image",img);
	waitKey(2000);
	//cout << img;
	cvtColor(img,grayImage, CV_BGR2GRAY);
	face_detect.detectMultiScale(grayImage, faces, 1.3, 5);
	for(int i=0; i<(int)faces.size(); i++)
	{
		Point pt1(faces[i].x+faces[i].width,faces[i].y+faces[i].height);
		Point pt2(faces[i].x,faces[i].y);
		//rectangle(img, pt1, pt2, cvScalar(0,0,255), 2,8,0);
		face_img = img(Rect(pt2.x, pt2.y, faces[i].width, faces[i].height));	
	}
	cout << "Number of faces detected = " << faces.size() <<endl;
	if(faces.size()==0)
	{
		cout << "No faces have been detected in the image" << endl;
		return 0;
	}
	namedWindow("face_Image",WINDOW_NORMAL);
	imshow("face_Image",face_img);
	namedWindow("detected",WINDOW_NORMAL);
	imshow("detected",img);
	waitKey(10000);
	return 0;
}