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
	CascadeClassifier eye_cascade;
	//vector< vector<Rect> > faces;

	if(!face_detect.load("/usr/local/share/OpenCV/haarcascades/haarcascade_frontalface_alt.xml"))
		cout << "error" << endl;

	if(!eye_cascade.load("/usr/local/share/OpenCV/haarcascades/haarcascade_eye_tree_eyeglasses.xml"))
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
			Mat faceROI = grayImage(faces[j]);
			vector<Rect> eyes;
			Point pt1(faces[j].x+faces[j].width,faces[j].y+faces[j].height);
			Point pt2(faces[j].x,faces[j].y);
			rectangle(img, pt1, pt2, cvScalar(0,0,255), 2,8,0);
			face_img = img(Rect(pt2.x, pt2.y, faces[j].width, faces[j].height));	
			//img = img(Rect(pt2.x, pt2.y, faces[j].width, faces[j].height));
			//Detect eyes	
			eye_cascade.detectMultiScale(faceROI, eyes, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(20,20));
			printf("%d Eye size\n",eyes.size());	
			for (int k=0;k<eyes.size();k++)
			{
				printf("%d x %d y\n",eyes[k].x,eyes[k].y);
				
				
				Point center(faces[j].x+eyes[k].x+eyes[k].width*0.5,faces[j].y+eyes[k].y+eyes[k].height*0.5);
				int radius = cvRound((eyes[k].width+eyes[k].height)*0.25);
				circle(img,center,radius,cvScalar(0,0,255),4,8,0);
				
			}					
		}
		cout << faces.size() <<endl;
		
		namedWindow("face_Image",WINDOW_NORMAL);
		imshow("face_eye_Image",face_img);
		namedWindow("detected",WINDOW_NORMAL);
		imshow("detected",img);
		waitKey(1000);	
		
	}
	
	cout << "Total time = " << clock()-start << endl;
	return 0;
}