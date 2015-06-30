#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <stdio.h>
#include <string.h>

#include "Dir.h"

using namespace std;
using namespace cv;

/**-------- Function Headers --------**/
void detectAndDisplay( Mat frame );
void drawFace(cv::Mat image, vector<Rect> *faces, int flag);


/**-------- Global variables --------**/
String face_cascade_name = "haarcascade_frontalface_default.xml";
String profile_cascade_name = "haarcascade_profileface.xml";

CascadeClassifier face_cascade;
CascadeClassifier profiles_cascade;

RNG rng(12345);



/**
 * @function main
 */
int main( int argc, char* argv[] )
{
	VideoCapture cap;
	Mat frame;
	int ImageMax = 48;

	string WindowName = "Window - Face detection";

/**-------- 検出器読み込み --------**/
if( !face_cascade.load( face_cascade_name ) ){ printf("--(!)Error loading\n"); return -1; };
if( !profiles_cascade.load( profile_cascade_name ) ){ printf("--(!)Error loading\n"); return -1; };


/**-------- データ読み込み --------**/
/* 静止画像の読み込み時は フラグ1 */
/* 動画像　の読み込み時は フラグ0 */

#if 1
	// 静止画像読み込み
	string DirectoryName = "img";
	// ディレクトリ名からファイルリストを作成
	vector<string> backfilelist = Dir::read(DirectoryName);
	// 画像Noを指定しない場合（ディレクトリ内全ファイルの出力をする場合）は #if 1 とする
	#if 0
		// ファイル名出力
		for(int i = 0; i < backfilelist.size(); i++) 
	#else
		// 静止画像No範囲指定
		// 開始番号
		int BeginNum = 13;
		// 終了番号
		int EndNum = 13;
		// ファイル名出力
		for(int i = BeginNum -1; i < EndNum; i++)
	#endif
		{
			stringstream FilePath;
			cout << backfilelist[i] << endl;
			FilePath << DirectoryName << "\\" << backfilelist[i];
			// （1）静止画像データをファイルから読み込む
			frame = imread(FilePath.str(), IMREAD_COLOR);
			//  失敗したらエラー表示
			if(frame.empty()) {
				cout << "Cannot open." << endl;
				return -1;
			}
			// （2）静止画像データの処理結果を表示する
			detectAndDisplay(frame);
			imshow(WindowName, frame);
			waitKey(0);
		}

#else
	// 動画像読み込み
	string fileName;
	#if 1
		fileName = "spe_2010_0127_obama_5.avi";
	#else
		fileName = argv[1];
	#endif
		printf("movie success \n");
		cap.open(fileName);
		// 動画像が読み込まれた場合の処理
		// 動画像が開けなければエラー表示
		if(cap.isOpened()) {
			for(;;) {
				// 動画像ファイルから1フレーム分の画像データを取得して、変数frameに格納する
				cap >> frame;

				// 画像データ取得に失敗したらループを抜ける
				if (!frame.empty()) {
					detectAndDisplay( frame );
					imshow(WindowName, frame);
				}
				else
					break;

				// 取得した画像データをウィンドウ表示する
				if(waitKey(100) >= 0) break;
			}
		}

#endif
	return 0;
}


/**
* @function detectAndDisplay
*/
void detectAndDisplay(Mat frame)
{
	std::vector<Rect> faces;
	Mat frame_gray;
	cvtColor( frame, frame_gray, COLOR_BGR2GRAY );
	equalizeHist( frame_gray, frame_gray );

	//-- Detect faces
	//正面顔検出//
	face_cascade.detectMultiScale( frame_gray, faces, 1.1, 3, 0|CV_HAAR_SCALE_IMAGE, Size(50, 50) );
	drawFace(frame, &faces, 1);

	if(faces.size() > 0)
	{
		for(size_t i = 0; i < (int)faces.size() -1; i++)
		{
			if(faces[i].width < 0)
				continue;
			//顔があると思われない範囲に検出した場合は太枠表示をしない
			/*if(
				//x軸
				faces[i].x < 190 ||
				faces[i].x > 240 ||
				//y軸
				faces[i].y < 93 ||
				faces[i].y > 150 ||
				(faces[i].y + faces[i].height -1 )  > 195
				)

				faces[i].width = -1;
			*/
			for(size_t j = i+1; j < (int)faces.size() ; j++)
			{
				if(faces[j].width < 0)
					continue;
				/*if(
					//x軸
					faces[j].x < 190  ||
					faces[j].x > 240 ||
					//y軸
					faces[j].y < 93 ||
					faces[j].y > 150 ||
					(faces[j].y + faces[j].height -1 )   > 195
					)
					//faces[j].width = -1;
					*/
				//矩形範囲内に重複して検出した場合は太枠表示をしない
				if(
					//見直しその２
					//条件式見直し(2015/06/25)
					//i基準
					//1のパターン
					//x軸
					faces[i].x <  faces[j].x && faces[i].x < (faces[j].x + faces[j].width -1) && (faces[i].x + faces[i].width - 1) > faces[j].x && (faces[i].x + faces[i].width - 1) > (faces[j].x + faces[j].width - 1) &&
					//y軸
					faces[i].y <  faces[j].y && faces[i].y < (faces[j].y + faces[j].height -1) && (faces[i].y + faces[i].height - 1) > faces[j].y && (faces[i].y + faces[i].height - 1) > (faces[j].y + faces[j].height - 1) ||
					//2のパターン
					//x軸
					faces[i].x <  faces[j].x && faces[i].x > (faces[j].x + faces[j].width -1) && (faces[i].x + faces[i].width - 1) > faces[j].x && (faces[i].x + faces[i].width - 1) > (faces[j].x + faces[j].width - 1) &&
					//y軸
					faces[i].y <  faces[j].y && faces[i].y < (faces[j].y + faces[j].height -1) && (faces[i].y + faces[i].height - 1) > faces[j].y && (faces[i].y + faces[i].height - 1) > (faces[j].y + faces[j].height - 1) ||
					//3のパターン
					//x軸
					faces[i].x <  faces[j].x && faces[i].x < (faces[j].x + faces[j].width -1) && (faces[i].x + faces[i].width - 1) > faces[j].x && (faces[i].x + faces[i].width - 1) < (faces[j].x + faces[j].width - 1) &&
					//y軸
					faces[i].y <  faces[j].y && faces[i].y < (faces[j].y + faces[j].height -1) && (faces[i].y + faces[i].height - 1) > faces[j].y && (faces[i].y + faces[i].height - 1) > (faces[j].y + faces[j].height - 1) ||
					//4のパターン
					//x軸
					faces[i].x <  faces[j].x && faces[i].x < (faces[j].x + faces[j].width -1) && (faces[i].x + faces[i].width - 1) > faces[j].x && (faces[i].x + faces[i].width - 1) > (faces[j].x + faces[j].width - 1) &&
					//y軸
					faces[i].y >  faces[j].y && faces[i].y < (faces[j].y + faces[j].height -1) && (faces[i].y + faces[i].height - 1) > faces[j].y && (faces[i].y + faces[i].height - 1) > (faces[j].y + faces[j].height - 1) ||
					//5のパターン
					//x軸
					faces[i].x <  faces[j].x && faces[i].x < (faces[j].x + faces[j].width -1) && (faces[i].x + faces[i].width - 1) > faces[j].x && (faces[i].x + faces[i].width - 1) > (faces[j].x + faces[j].width - 1) &&
					//y軸
					faces[i].y <  faces[j].y && faces[i].y < (faces[j].y + faces[j].height -1) && (faces[i].y + faces[i].height - 1) > faces[j].y && (faces[i].y + faces[i].height - 1) < (faces[j].y + faces[j].height - 1) ||
					//6のパターン
					//x軸
					faces[i].x <  faces[j].x && faces[i].x > (faces[j].x + faces[j].width -1) && (faces[i].x + faces[i].width - 1) > faces[j].x && (faces[i].x + faces[i].width - 1) > (faces[j].x + faces[j].width - 1) &&
					//y軸
					faces[i].y >  faces[j].y && faces[i].y < (faces[j].y + faces[j].height -1) && (faces[i].y + faces[i].height - 1) > faces[j].y && (faces[i].y + faces[i].height - 1) > (faces[j].y + faces[j].height - 1) ||
					//7のパターン
					//x軸
					faces[i].x <  faces[j].x && faces[i].x < (faces[j].x + faces[j].width -1) && (faces[i].x + faces[i].width - 1) > faces[j].x && (faces[i].x + faces[i].width - 1) < (faces[j].x + faces[j].width - 1) &&
					//y軸
					faces[i].y <  faces[j].y && faces[i].y < (faces[j].y + faces[j].height -1) && (faces[i].y + faces[i].height - 1) > faces[j].y && (faces[i].y + faces[i].height - 1) < (faces[j].y + faces[j].height - 1) ||
					//8のパターン
					//x軸
					faces[i].x <  faces[j].x && faces[i].x > (faces[j].x + faces[j].width -1) && (faces[i].x + faces[i].width - 1) > faces[j].x && (faces[i].x + faces[i].width - 1) > (faces[j].x + faces[j].width - 1) &&
					//y軸
					faces[i].y <  faces[j].y && faces[i].y < (faces[j].y + faces[j].height -1) && (faces[i].y + faces[i].height - 1) > faces[j].y && (faces[i].y + faces[i].height - 1) < (faces[j].y + faces[j].height - 1) ||
					//9のパターン
					//x軸
					faces[i].x <  faces[j].x && faces[i].x < (faces[j].x + faces[j].width -1) && (faces[i].x + faces[i].width - 1) > faces[j].x && (faces[i].x + faces[i].width - 1) < (faces[j].x + faces[j].width - 1) &&
					//y軸
					faces[i].y >  faces[j].y && faces[i].y < (faces[j].y + faces[j].height -1) && (faces[i].y + faces[i].height - 1) > faces[j].y && (faces[i].y + faces[i].height - 1) > (faces[j].y + faces[j].height - 1) ||
					//10のパターン
					//x軸
					faces[i].x <  faces[j].x && faces[i].x > (faces[j].x + faces[j].width -1) && (faces[i].x + faces[i].width - 1) > faces[j].x && (faces[i].x + faces[i].width - 1) > (faces[j].x + faces[j].width - 1) &&
					//y軸
					faces[i].y >  faces[j].y && faces[i].y < (faces[j].y + faces[j].height -1) && (faces[i].y + faces[i].height - 1) > faces[j].y && (faces[i].y + faces[i].height - 1) < (faces[j].y + faces[j].height - 1) ||
					//11のパターン
					//x軸
					faces[i].x <  faces[j].x && faces[i].x < (faces[j].x + faces[j].width -1) && (faces[i].x + faces[i].width - 1) > faces[j].x && (faces[i].x + faces[i].width - 1) < (faces[j].x + faces[j].width - 1) &&
					//y軸
					faces[i].y >  faces[j].y && faces[i].y < (faces[j].y + faces[j].height -1) && (faces[i].y + faces[i].height - 1) > faces[j].y && (faces[i].y + faces[i].height - 1) < (faces[j].y + faces[j].height - 1) ||
					//12のパターン
					//x軸
					faces[i].x >  faces[j].x && faces[i].x < (faces[j].x + faces[j].width -1) && (faces[i].x + faces[i].width - 1) > faces[j].x && (faces[i].x + faces[i].width - 1) < (faces[j].x + faces[j].width - 1) &&
					//y軸
					faces[i].y >  faces[j].y && faces[i].y < (faces[j].y + faces[j].height -1) && (faces[i].y + faces[i].height - 1) > faces[j].y && (faces[i].y + faces[i].height - 1) > (faces[j].y + faces[j].height - 1) ||
					//13
					//x軸
					faces[i].x >  faces[j].x && faces[i].x < (faces[j].x + faces[j].width -1) && (faces[i].x + faces[i].width - 1) > faces[j].x && (faces[i].x + faces[i].width - 1) < (faces[j].x + faces[j].width - 1) &&
					//y軸
					faces[i].y <  faces[j].y && faces[i].y < (faces[j].y + faces[j].height -1) && (faces[i].y + faces[i].height - 1) > faces[j].y && (faces[i].y + faces[i].height - 1) < (faces[j].y + faces[j].height - 1) ||
					//14
					//x軸
					faces[i].x >  faces[j].x && faces[i].x < (faces[j].x + faces[j].width -1) && (faces[i].x + faces[i].width - 1) > faces[j].x && (faces[i].x + faces[i].width - 1) < (faces[j].x + faces[j].width - 1) &&
					//y軸
					faces[i].y >  faces[j].y && faces[i].y < (faces[j].y + faces[j].height -1) && (faces[i].y + faces[i].height - 1) > faces[j].y && (faces[i].y + faces[i].height - 1) < (faces[j].y + faces[j].height - 1) ||

					//j基準
					//1のパターン
					//x軸
					faces[j].x <  faces[i].x && faces[j].x < (faces[i].x + faces[i].width -1) && (faces[j].x + faces[j].width - 1) > faces[i].x && (faces[j].x + faces[j].width - 1) > (faces[i].x + faces[i].width - 1) &&
					//y軸
					faces[j].y <  faces[i].y && faces[j].y < (faces[i].y + faces[i].height -1) && (faces[j].y + faces[j].height - 1) > faces[i].y && (faces[j].y + faces[j].height - 1) > (faces[i].y + faces[i].height - 1) ||
					//2のパターン
					//x軸
					faces[j].x <  faces[i].x && faces[j].x > (faces[i].x + faces[i].width -1) && (faces[j].x + faces[j].width - 1) > faces[i].x && (faces[j].x + faces[j].width - 1) > (faces[i].x + faces[i].width - 1) &&
					//y軸
					faces[j].y <  faces[i].y && faces[j].y < (faces[i].y + faces[i].height -1) && (faces[j].y + faces[j].height - 1) > faces[i].y && (faces[j].y + faces[j].height - 1) > (faces[i].y + faces[i].height - 1) ||
					//3のパターン
					//x軸
					faces[j].x <  faces[i].x && faces[j].x < (faces[i].x + faces[i].width -1) && (faces[j].x + faces[j].width - 1) > faces[i].x && (faces[j].x + faces[j].width - 1) < (faces[i].x + faces[i].width - 1) &&
					//y軸
					faces[j].y <  faces[i].y && faces[j].y < (faces[i].y + faces[i].height -1) && (faces[j].y + faces[j].height - 1) > faces[i].y && (faces[j].y + faces[j].height - 1) > (faces[i].y + faces[i].height - 1) ||
					//4のパターン
					//x軸
					faces[j].x <  faces[i].x && faces[j].x < (faces[i].x + faces[i].width -1) && (faces[j].x + faces[j].width - 1) > faces[i].x && (faces[j].x + faces[j].width - 1) > (faces[i].x + faces[i].width - 1) &&
					//y軸
					faces[j].y >  faces[i].y && faces[j].y < (faces[i].y + faces[i].height -1) && (faces[j].y + faces[j].height - 1) > faces[i].y && (faces[j].y + faces[j].height - 1) > (faces[i].y + faces[i].height - 1) ||
					//5のパターン
					//x軸
					faces[j].x <  faces[i].x && faces[j].x < (faces[i].x + faces[i].width -1) && (faces[j].x + faces[j].width - 1) > faces[i].x && (faces[j].x + faces[j].width - 1) > (faces[i].x + faces[i].width - 1) &&
					//y軸
					faces[j].y <  faces[i].y && faces[j].y < (faces[i].y + faces[i].height -1) && (faces[j].y + faces[j].height - 1) > faces[i].y && (faces[j].y + faces[j].height - 1) < (faces[i].y + faces[i].height - 1) ||
					//6のパターン
					//x軸
					faces[j].x <  faces[i].x && faces[j].x > (faces[i].x + faces[i].width -1) && (faces[j].x + faces[j].width - 1) > faces[i].x && (faces[j].x + faces[j].width - 1) > (faces[i].x + faces[i].width - 1) &&
					//y軸
					faces[j].y >  faces[i].y && faces[j].y < (faces[i].y + faces[i].height -1) && (faces[j].y + faces[j].height - 1) > faces[i].y && (faces[j].y + faces[j].height - 1) > (faces[i].y + faces[i].height - 1) ||
					//7のパターン
					//x軸
					faces[j].x <  faces[i].x && faces[j].x < (faces[i].x + faces[i].width -1) && (faces[j].x + faces[j].width - 1) > faces[i].x && (faces[j].x + faces[j].width - 1) < (faces[i].x + faces[i].width - 1) &&
					//y軸
					faces[j].y <  faces[i].y && faces[j].y < (faces[i].y + faces[i].height -1) && (faces[j].y + faces[j].height - 1) > faces[i].y && (faces[j].y + faces[j].height - 1) < (faces[i].y + faces[i].height - 1) ||
					//8のパターン
					//x軸
					faces[j].x <  faces[i].x && faces[j].x > (faces[i].x + faces[i].width -1) && (faces[j].x + faces[j].width - 1) > faces[i].x && (faces[j].x + faces[j].width - 1) > (faces[i].x + faces[i].width - 1) &&
					//y軸
					faces[j].y <  faces[i].y && faces[j].y < (faces[i].y + faces[i].height -1) && (faces[j].y + faces[j].height - 1) > faces[i].y && (faces[j].y + faces[j].height - 1) < (faces[i].y + faces[i].height - 1) ||
					//9のパターン
					//x軸
					faces[j].x <  faces[i].x && faces[j].x < (faces[i].x + faces[i].width -1) && (faces[j].x + faces[j].width - 1) > faces[i].x && (faces[j].x + faces[j].width - 1) < (faces[i].x + faces[i].width - 1) &&
					//y軸
					faces[j].y >  faces[i].y && faces[j].y < (faces[i].y + faces[i].height -1) && (faces[j].y + faces[j].height - 1) > faces[i].y && (faces[j].y + faces[j].height - 1) > (faces[i].y + faces[i].height - 1) ||
					//10のパターン
					//x軸
					faces[j].x <  faces[i].x && faces[j].x > (faces[i].x + faces[i].width -1) && (faces[j].x + faces[j].width - 1) > faces[i].x && (faces[j].x + faces[j].width - 1) > (faces[i].x + faces[i].width - 1) &&
					//y軸
					faces[j].y >  faces[i].y && faces[j].y < (faces[i].y + faces[i].height -1) && (faces[j].y + faces[j].height - 1) > faces[i].y && (faces[j].y + faces[j].height - 1) < (faces[i].y + faces[i].height - 1) ||
					//11のパターン
					//x軸
					faces[j].x <  faces[i].x && faces[j].x < (faces[i].x + faces[i].width -1) && (faces[j].x + faces[j].width - 1) > faces[i].x && (faces[j].x + faces[j].width - 1) < (faces[i].x + faces[i].width - 1) &&
					//y軸
					faces[j].y >  faces[i].y && faces[j].y < (faces[i].y + faces[i].height -1) && (faces[j].y + faces[j].height - 1) > faces[i].y && (faces[j].y + faces[j].height - 1) < (faces[i].y + faces[i].height - 1) ||
					//12のパターン
					//x軸
					faces[j].x >  faces[i].x && faces[j].x < (faces[i].x + faces[i].width -1) && (faces[j].x + faces[j].width - 1) > faces[i].x && (faces[j].x + faces[j].width - 1) < (faces[i].x + faces[i].width - 1) &&
					//y軸
					faces[j].y >  faces[i].y && faces[j].y < (faces[i].y + faces[i].height -1) && (faces[j].y + faces[j].height - 1) > faces[i].y && (faces[j].y + faces[j].height - 1) > (faces[i].y + faces[i].height - 1) ||
					//13
					//x軸
					faces[j].x >  faces[i].x && faces[j].x < (faces[i].x + faces[i].width -1) && (faces[j].x + faces[j].width - 1) > faces[i].x && (faces[j].x + faces[j].width - 1) < (faces[i].x + faces[i].width - 1) &&
					//y軸
					faces[j].y <  faces[i].y && faces[j].y < (faces[i].y + faces[i].height -1) && (faces[j].y + faces[j].height - 1) > faces[i].y && (faces[j].y + faces[j].height - 1) < (faces[i].y + faces[i].height - 1) ||
					//14
					//x軸
					faces[j].x >  faces[i].x && faces[j].x < (faces[i].x + faces[i].width -1) && (faces[j].x + faces[j].width - 1) > faces[i].x && (faces[j].x + faces[j].width - 1) < (faces[i].x + faces[i].width - 1) &&
					//y軸
					faces[j].y >  faces[i].y && faces[j].y < (faces[i].y + faces[i].height -1) && (faces[j].y + faces[j].height - 1) > faces[i].y && (faces[j].y + faces[j].height - 1) < (faces[i].y + faces[i].height - 1) 
					////faces[i].x ==  faces[j].x || faces[i].x == (faces[j].x + faces[j].width -1) || (faces[i].x + faces[i].width - 1) == faces[j].x || (faces[i].x + faces[i].width - 1) == (faces[j].x + faces[j].width - 1) ||
					//完全に重複した場合
					//x軸
					//faces[i].x <  faces[j].x //|| faces[j].x < faces[i].x + faces[i].width - 1 ||
					//faces[i].x <  faces[j].x + faces[j].width - 1 || faces[j].x + faces[j].width - 1 < faces[i].x + faces[i].width - 1 ||
					//y軸
					//faces[i].y <  faces[j].y || faces[j].y < faces[i].y + faces[i].height - 1 ||
					//faces[i].y <  faces[j].y + faces[j].height - 1 || faces[j].y + faces[j].height - 1 < faces[i].y + faces[i].height - 1
					//半分重複した場合
					//x軸
					//faces[i].x <  faces[j].x || faces[j].x < faces[i].x + (faces[i].width/2) - 1 ||
					//faces[i].x <  faces[j].x + faces[j].width - 1 || faces[j].x + faces[j].width - 1 < faces[i].x + (faces[i].width/2) - 1 
					//y軸
					//faces[i].y <  faces[j].y || faces[j].y < faces[i].y + (faces[i].height/2) - 1 ||
					//faces[i].y <  faces[j].y + faces[j].height - 1 || faces[j].y + faces[j].height - 1 < faces[i].y + (faces[i].height/2) - 1
					//齊藤先生が書いたもの
					//faces[i].x + ((faces[i].width/2) > faces[j].x - (faces[j].width/2) ||
					//faces[i].x - faces[i].width < faces[j].x + faces[j].width ||
					//faces[i].y + faces[i].width > faces[j].y - faces[j].width ||
					//faces[i].y - faces[i].width < faces[j].y + faces[j].width
					)

						faces[j].width = -1;
				
			}	// for(j)
		}	// for(i)
	}

	drawFace(frame, &faces, 2);



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

				//rectangle( image, Point(190, 93), Point(280, 190), Scalar(255, 0, 0), 1, 8, 0);

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