﻿#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

/** Function Headers */
void detectAndDisplay( Mat frame );

/** Global variables */
//-- Note, either copy these two files from opencv/data/haarscascades to your current folder, or change these locations
//String face_cascade_name = "haarcascade_frontalface_alt.xml";
String face_cascade_name = "haarcascade_frontalface_default.xml";
String profile_cascade_name = "haarcascade_profileface.xml";


//CascadeClassifier 物体検出のためのカスケード分類器クラス
CascadeClassifier face_cascade;
CascadeClassifier profiles_cascade;
string window_name = "Capture - Face detection";
RNG rng(12345);

Mat frame;

/**
 * @function main
 */
int main( int argc, char* argv[] )
{
	//入力画像の読み込み　引数がなければサンプル画像を読み込む
	if (argc < 2) {
		frame = imread("P1120221.JPG", CV_LOAD_IMAGE_COLOR);
	}
	else {
		frame = imread(argv[1], CV_LOAD_IMAGE_COLOR);
	}

//画像が開けなければエラー表示
	if (frame.empty()) {
		printf("Cannot open image\n");
		exit (-1);
	}


  //-- 1. Load the cascades
  if( !face_cascade.load( face_cascade_name ) ){ printf("--(!)Error loading\n"); return -1; };
  if( !profiles_cascade.load( profile_cascade_name ) ){ printf("--(!)Error loading\n"); return -1; };

 
  //-- 3. Apply the classifier to the frame
  if( !frame.empty() )
	 { detectAndDisplay( frame ); 
		//imshow("window", frame);
		destroyAllWindows();
	}
   else
	 { printf(" --(!) No captured frame -- Break!"); }

	  return 0;

}


/**
 * @function detectAndDisplay
 */
void detectAndDisplay( Mat frame )
{
   std::vector<Rect> faces;
   //std::vector<型> オブジェクト名

   Mat frame_gray;

	

   cvtColor( frame, frame_gray, COLOR_BGR2GRAY );
   //画像の色空間の変換
   //カラー画像からグレースケール画像に変換している
   //void cvtColor(const Mat, Mat, int code(色空間の変換コード), int dstCn=0(出力画像のチャネル数、0の場合は自動的に求められる) )

   equalizeHist( frame_gray, frame_gray );
   //ヒストグラムの均一化
   //void equalizeHist(const Mat, Mat)
   //均一化を行うことでより鮮明な画像になる


   //-- Detect faces
   //正面顔検出
   face_cascade.detectMultiScale( frame_gray, faces, 1.11, 1, 0|CV_HAAR_SCALE_IMAGE, Size(20, 20) );

   //http://docs.opencv.org/modules/objdetect/doc/cascade_classification.html
   //detectMultiScale(const Mat& image, vector<Rect>& objects, double scaleFactor=1.1, int minNeighbors=3, int flags=0, Size minSize=Size(), Size maxSize=Size())
   //scaleFactor –各画像スケールにおける縮小量
   //minNeighbors 最低矩形数

   

   for( size_t i = 0; i < faces.size(); i++ )
    {
	  //Point  x と y によって指定される 2 次元の点を表現するクラス
      Point center( faces[i].x + faces[i].width/2, faces[i].y + faces[i].height/2 );
	  //楕円の描画
      ellipse( frame, center, Size( faces[i].width/2, faces[i].height/2), 0, 0, 360, Scalar( 255, 0, 255 ), 1, 4, 0 );
	  
	  //数値から文字列に変換
	  //std::to_string
	  string num_str = to_string(i+1);

	  //番号の描画する場所
	  Point number( faces[i].x + faces[i].width/2, faces[i].y + faces[i].height/2 - 30 );

	  //番号の描画
	  putText( frame, num_str, number, FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 1, 4, false);
   }

	

   //横顔検出
   Mat faceROI = frame_gray;

   std::vector<Rect> profiles;

   //-- In each face, detect profiles
   profiles_cascade.detectMultiScale( faceROI, profiles, 1.11, 0, 0 |CV_HAAR_SCALE_IMAGE, Size(20, 20) );

   for( size_t j = 0; j < profiles.size(); j++ )
    {
      Point profile_center( profiles[j].x + profiles[j].width/2, profiles[j].y + profiles[j].height/2 );
	  ellipse( frame, profile_center, Size( profiles[j].width/2, profiles[j].height/2), 0, 0, 360, Scalar( 255, 0, 0 ), 1, 4, 0 );

	   //数値から文字列に変換
	  //std::to_string
	  string num_str = to_string(j+1);

	  //番号の描画する場所
	 Point profile_number( profiles[j].x + profiles[j].width/2 + 20, profiles[j].y + profiles[j].height/2 - 30);

	  //番号の描画
	  putText( frame, num_str, profile_number, FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 0), 1, 4, false);
       }
 
   //-- Show what you got
   imshow( window_name, frame );
	waitKey(0);
}