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

	// 検出器の読み込み
	if( !face_cascade.load( face_cascade_name ) ){ printf("--(!)Error loading\n"); return -1; };
	if( !profiles_cascade.load( profile_cascade_name ) ){ printf("--(!)Error loading\n"); return -1; };


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

		/* 1回目の呼出し */
		name = strtok(argv[1], ".");

		/* 2回目以降の呼出し */
		extention = strtok(NULL, ".");

		printf("name: %s\n", name);
		printf("extention: %s\n", extention);
		printf("argv[1]: %s\n", argv[1]);
		printf("original Filename: %s\n", org_filename);

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

		// 拡張子が .bmp : 画像読み込み
		if(ext == "bmp") {
			printf("picture success \n");
			frame = imread("obama_1_0008.bmp", CV_LOAD_IMAGE_COLOR);
			// 画像が読み込まれた場合の処理
			// 画像が開けなければエラー表示	
			if( !frame.empty() ) { 
				detectAndDisplay( frame ); 
				imshow("window", frame);
				waitKey(0);
				destroyAllWindows();
			}
			else { 
				printf("Cannot open bmp picture. Please check your file's extention. \n"); 
				exit(-1);
			}
		}
	}

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
	//正面顔検出//
	face_cascade.detectMultiScale( frame_gray, faces, 1.1, 4, 0|CV_HAAR_SCALE_IMAGE, Size(50, 50) );

	//http://docs.opencv.org/modules/objdetect/doc/cascade_classification.html
	//detectMultiScale(const Mat& image, vector<Rect>& objects, double scaleFactor=1.1, int minNeighbors=3, int flags=0, Size minSize=Size(), Size maxSize=Size())
	//scaleFactor –各画像スケールにおける縮小量
	//minNeighbors 最低矩形数

   
	for( size_t i = 0; i < faces.size(); i++ ) {
		Point center( faces[i].x + faces[i].width/2, faces[i].y + faces[i].height/2 );

		Point pt1;
		Point pt2;

		pt1.x = faces[i].x;
		pt1.y = faces[i].y;
		pt2.x = faces[i].x + faces[i].width - 1;
		pt2.y = faces[i].y +faces[i].height - 1;


		//中心座標の算出
		Point cpt;
		cpt.x = (pt1.x + pt2.x)/2;
		cpt.y = (pt1.y + pt2.y)/2;

		//矩形半径の算出
		Point length;
		length.x = (pt2.x - cpt.x);
		length.y = (pt2.y - cpt.y);

		//矩形半径の領域内に

		//検出した領域にあわせて矩形を描く
		rectangle( frame, pt1, pt2, CV_RGB(255, 0, 255), 5, 8, 0 );

		rectangle( frame, pt1, pt2, CV_RGB(255, 255, 255), 3, 8, 0 );

		rectangle( frame, pt1, pt2, CV_RGB(255, 0, 255), 1, 8, 0 );


		//数値から文字列に変換
		//std::to_string
		string num_str = to_string(i+1);
 
		//番号の描画する場所
		Point number( faces[i].x + faces[i].width/2, faces[i].y + faces[i].height/2 - 60 );
 
		//番号の描画
		putText( frame, num_str, number, FONT_HERSHEY_SIMPLEX, 0.8, Scalar(0, 0, 255), 2, 8, 0);

		//矩形描画てすと
		//rectangle( frame, Point(201, 93), Point(276, 168), Scalar(255, 0, 0), 1, 8, 0);
		}

  

		//矩形描写座標の取得
		//if( i = faces.size() - 1) {
		//	printf("i=%d;\n", i);
		//	int pt1_x = pt1.x;	
		//	printf("pt1_x=%d;\n",pt1_x);
		//	int pt1_y = pt1.y;	
		//	printf("pt1_y=%d;\n",pt1_y);
		//	int pt2_x = pt2.x;	
		//	printf("pt2_x=%d;\n",pt2_x);
		//	int pt2_y = pt2.y;	
		//	printf("pt2_y=%d;\n",pt2_y);
			//obama_1_0008.bmp出力結果（矩形数4, 30*30）
			//Point(209, 100) Point(263, 154);d
			//Size(54, 54)
			//obama_1_0022.bmp出力結果(矩形数0に変更, 30*30)
			//Point(201, 93) Point(257, 149);d
			//Size(56, 56)
			//顎の部分が切れているので実際はy軸正方向に長さが足りないと思われたが、正方形に検出するので、
			//Size(75, 75)だと大幅にずれてしまう
			//Point(201, 93), Point(257, 168)
			//Size(56, 75)
			//obama_1_0052.bmp出力結果(矩形数0に変更, 50*50, 縮小量1.2)
			//2番目に検出
			//Point(209, 118), Point(258, 167)
			//Size(49, 49)
			//縮小量を1.200192とか1.2001918に変更すると1番目に検出可能
			//この条件では obama_1_0022.bmp は検出不可、縮小量を1.11に変更することで検出可能
		//}	
	

	

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
