#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <stdio.h>
#include <string.h>

using namespace std;
using namespace cv;

/** Function Headers */
void detectAndDisplay( Mat frame );

void drawFace(cv::Mat image, vector<Rect> *faces, int flag);

/** Global variables */
//String face_cascade_name = "haarcascade_frontalface_alt.xml";
String face_cascade_name = "haarcascade_frontalface_default.xml";
String profile_cascade_name = "haarcascade_profileface.xml";


//CascadeClassifier 物体検出のためのカスケード分類器クラス
CascadeClassifier face_cascade;
CascadeClassifier profiles_cascade;
string window_name = "Capture - Face detection";
RNG rng(12345);

//Mat frame;


/**
 * @function main
 */
int main( int argc, char* argv[] )
{
	VideoCapture cap;
	Mat frame;

	string fileName;

#if 1
	fileName = "spe_2010_0127_obama_5.avi";
#else
	fileName = argv[1];
#endif

	// 検出器の読み込み
	if( !face_cascade.load( face_cascade_name ) ){ printf("--(!)Error loading\n"); return -1; };
	if( !profiles_cascade.load( profile_cascade_name ) ){ printf("--(!)Error loading\n"); return -1; };

	printf("movie success \n");
	cap.open(fileName);
	// 動画が読み込まれた場合の処理
	// 動画が開けなければエラー表示
	if(cap.isOpened()) {
		for(;;) {
			// 動画ファイルから1フレーム分の画像データを取得して、変数frameに格納する
			cap >> frame;

			// 画像データ取得に失敗したらループを抜ける
			if (!frame.empty()) {
				detectAndDisplay( frame );
				imshow("window", frame);
			}
			else
				break;

			// 取得した画像データをウィンドウ表示する
			if(waitKey(30) >= 0) break;
		}
	}

	/*
	// 入力動画の読み込み　引数がなければサンプル動画を読み込む
	if (argc < 2) {
		cap.open("spe_2010_0127_obama_5.avi");
	}
	else {
		// 動画か画像かの判別
		// ファイル名から拡張子を strtok で分割して取り出す
		string org_filename;
		org_filename = argv[1];
		char *name, *extention;

		// 1回目の呼出し
		name = strtok(argv[1], ".");

		// 2回目以降の呼出し
		extention = strtok(NULL, ".");

		printf("name: %s\n", name);
		printf("extention: %s\n", extention);
		printf("argv[1]: %s\n", argv[1]);
		//printf("original Filename: %s\n", org_filename);

		string ext = extention;

		// 拡張子が .avi : 動画読み込み
		if(ext == "avi") {
			printf("movie success \n");
			cap.open(org_filename);
			// 動画が読み込まれた場合の処理
			// 動画が開けなければエラー表示
			if(cap.isOpened()) {
				for(;;) {
					// 動画ファイルから1フレーム分の画像データを取得して、変数frameに格納する
					cap >> frame;

					// 画像データ取得に失敗したらループを抜ける
					if (!frame.empty()) {
						detectAndDisplay( frame );
						imshow("window", frame);
					}
					else
						break;

					// 取得した画像データをウィンドウ表示する
					if(waitKey(30) >= 0) break;
				}
			}
			else {
				printf("Cannot open avi movie. Please check your file's extention. \n");
				exit (-1);
			}
		}

		// 拡張子が .jpg : 画像読み込み
		if(ext == "jpg") {
			printf("picture success \n");
			frame = imread(org_filename, CV_LOAD_IMAGE_COLOR);
			// 画像が読み込まれた場合の処理
			// 画像が開けなければエラー表示	
			if( !frame.empty() ) { 
				detectAndDisplay( frame ); 
				imshow("window", frame);
				waitKey(0);
				destroyAllWindows();
			}
			else { 
				printf("Cannot open picture. Please check your file's extention. \n"); 
				exit(-1);
			}
		}
		//else {
		//	printf("Cannot open picture. Please check your file's extention. \n"); 
		//	exit(-1);
		//}
	}
	*/

	//何も動画像が開けなければエラー表示
	//if(!cap.isOpened()) {
	//	printf("Cannot open anything. \n");
	//	exit (-1);
	//}

	return 0;
}


/**
* @function detectAndDisplay
*/
void detectAndDisplay(Mat frame)
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
	//正面顔検出//
	face_cascade.detectMultiScale( frame_gray, faces, 1.1, 0, 0|CV_HAAR_SCALE_IMAGE, Size(50, 50) );

	//http://docs.opencv.org/modules/objdetect/doc/cascade_classification.html
	//detectMultiScale(const Mat& image, vector<Rect>& objects, double scaleFactor=1.1, int minNeighbors=3, int flags=0, Size minSize=Size(), Size maxSize=Size())
	//scaleFactor –各画像スケールにおける縮小量
	//minNeighbors 最低矩形数

	drawFace(frame, &faces, 1);

	if(faces.size() > 0)
	{
		for(size_t i = 0; i < (int)faces.size() -1; i++)
		{
			if(faces[i].width < 0)
				continue;

			for(size_t j = i+1; j < (int)faces.size() ; j++)
			{
				if(faces[j].width < 0)
					continue;
				if(
					faces[i].x + faces[i].width > faces[j].x - faces[j].width ||
					faces[i].x - faces[i].width < faces[j].x + faces[j].width ||
					faces[i].y + faces[i].width > faces[j].y - faces[j].width ||
					faces[i].y - faces[i].width < faces[j].y + faces[j].width
					)

					faces[j].width = -1;
			}	// for(j)
		}	// for(i)
	}

	drawFace(frame, &faces, 2);

	/*
	//ソートをする
	int temp;
	for(size_t i = 0; i < faces.size() -1; i++){
		for( size_t j = faces.size() - 1; j > i; j-- ) {
			if (faces[j - 1].x > faces[j].x) {  // 前の要素の方が大きかったら
				temp = faces[j].x;        // 交換する
				faces[j].x = faces[j - 1].x;
				faces[j - 1].x = temp;
				}
			}
	}
	*/

	//座標取得
	for( size_t i = 0; i < faces.size(); i++ )
	{
		printf("i=%d;\n", i);
		printf("faces[i].x=%d\t\n", faces[i].x);
		Point pt1;
		Point pt2;

		pt1.x = faces[i].x;
		pt1.y = faces[i].y;
		pt2.x = faces[i].x + faces[i].width - 1;
		pt2.y = faces[i].y + faces[i].height - 1;

		//中心座標の算出
		Point cpt1;
		//coordinates[i].x = (pt1.x + pt2.x)/2;
		//cpt1.y = (pt1.y + pt2.y)/2;

		//矩形半径の算出
		int rad1;
		rad1 = (pt2.x - cpt1.x);

		//if( i == 2 ) {
		//	faces[i].height = -1;
		//}

		//中心座標の描画
		//circle( frame, cpt, 4, CV_RGB(200, 255, 255), -1);

		//矩形半径の描画
		//line( frame, pt2, cpt, CV_RGB(200, 255, 255), 3, 8, 0);
	}


	//横顔検出//
	Mat faceROI = frame_gray;

	std::vector<Rect> profiles;

	//-- In each face, detect profiles
	profiles_cascade.detectMultiScale( faceROI, profiles, 1.1, 3, 0 |CV_HAAR_SCALE_IMAGE, Size(40, 40) );

	for( size_t j = 0; j < profiles.size(); j++ ) {
		Point profile_center( profiles[j].x + profiles[j].width/2, profiles[j].y + profiles[j].height/2 );

		//楕円の描画
		ellipse( frame, profile_center, Size( profiles[j].width/2, profiles[j].height/2), 0, 0, 360, Scalar( 255, 0, 0 ), 5, 8, 0 );
		ellipse( frame, profile_center, Size( profiles[j].width/2, profiles[j].height/2), 0, 0, 360, Scalar( 255, 255, 255 ), 3, 8, 0 );
		ellipse( frame, profile_center, Size( profiles[j].width/2, profiles[j].height/2), 0, 0, 360, Scalar( 255, 0, 0 ), 1, 8, 0 );

		//数値から文字列に変換
		//std::to_string
		string num_str = to_string(j+1);

		//番号の描画する場所
		Point profile_number( profiles[j].x + profiles[j].width/2 + 20, profiles[j].y + profiles[j].height/2 - 60);

		//番号の描画
		putText( frame, num_str, profile_number, FONT_HERSHEY_SIMPLEX, 0.8, Scalar(255, 255, 0), 2, 8, 0);
	}
 
	//-- Show what you got
	//imshow( window_name, frame );
	//waitKey(0);
}


// 矩形描画関数
void drawFace(cv::Mat image, vector<Rect> *faces, int flag)
{
	for(size_t i=0; i<(int)faces->size(); i++ )
	{
		if(faces->at(i).width != -1)
		{
			Point pt1;
			Point pt2;

			pt1.x = faces->at(i).x;
			pt1.y = faces->at(i).y;
			pt2.x = faces->at(i).x + faces->at(i).width - 1;
			pt2.y = faces->at(i).y + faces->at(i).height - 1;

			// 数値から文字列に変換
			char c_str[256];

			sprintf(c_str, "%d", i+1);
 
			//番号の描画する場所
			Point number(faces->at(i).x + faces->at(i).width/2, faces->at(i).y + faces->at(i).height/2 - 60 );

			switch (flag)
			{
			case 1:
				rectangle(image, pt1, pt2, CV_RGB(255, 0, 255), 1, 8, 0 );

				break;

			case 2:

				rectangle(image, pt1, pt2, CV_RGB(255, 0, 255), 5, 8, 0 );

				rectangle(image, pt1, pt2, CV_RGB(255, 255, 255), 3, 8, 0 );

				rectangle(image, pt1, pt2, CV_RGB(255, 0, 255), 1, 8, 0 );

				// 番号の描画
				putText(image, c_str, number, FONT_HERSHEY_SIMPLEX, 0.8, Scalar(0, 0, 255), 2, 8, 0);
				break;
			}	// swtich(flag)
		}
	}	// for(i)
}