#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <stdio.h>
#include <ctime>

using namespace std;
using namespace cv;


int main()
{
	String path("/home/ashritha/Desktop/BurstImages/*.jpg");
	vector<String> file_names;
	vector<Mat> burst;
	clock_t start;
	glob(path,file_names,true);
	int i,k;

	start = clock();

	for(k=0; k<file_names.size(); ++k)
	{
		Mat image = imread(file_names[k]);
		if(image.empty())
			continue;
		burst.push_back(image);
	}	
	
	Mat img, grayImage, face_img;
	CascadeClassifier face_detect;
	//vector< vector<Rect> > faces;

	if(!face_detect.load("/usr/local/share/OpenCV/haarcascades/haarcascade_frontalface_alt.xml"))
		cout << "error" << endl;

	
	for(i=0;i<burst.size();i++)
	{
		vector<Rect> faces;
		img = burst[i];
		cvtColor(img,grayImage, CV_BGR2GRAY);
		cout << "done" <<endl;
		cout << typeid(faces[i]).name()<<endl;
		face_detect.detectMultiScale(grayImage, faces, 1.3, 5);
		cout << faces[i].size() << endl;
		for(int j=0; j<(int)faces.size(); j++)
		{
			Point pt1(faces[j].x+faces[j].width,faces[j].y+faces[j].height);
			Point pt2(faces[j].x,faces[j].y);
			rectangle(img, pt1, pt2, cvScalar(0,0,255), 2,8,0);
			face_img = img(Rect(pt2.x, pt2.y, faces[j].width, faces[j].height));	
		}
		cout << faces.size() <<endl;
		
		namedWindow("face_Image",WINDOW_NORMAL);
		imshow("face_Image",face_img);
		namedWindow("detected",WINDOW_NORMAL);
		imshow("detected",img);
		waitKey(10000);
		
	}

	cout << "Total time = " << clock()-start << endl;
	return 0;
}