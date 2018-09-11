#include <vector>
#include <string>
#include <io.h>
#include "opencv2/core/core.hpp"
#include "opencv2/core/utility.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2\opencv.hpp>  
#include <opencv2\xfeatures2d.hpp>
#include "direct.h"
#include <afxwin.h>
#include <Windows.h>

//#pragma once
using namespace std;
using namespace cv;
typedef struct _UserInfo
{
	char devicename[48];
	char ip[64];
	DWORD port;
	char username[40];
	char userpsw[40];
	char cameranumber[20];
	BOOL DevType;
}UserInfo;
typedef struct
{
	char Location[20];
	char CameraName[20]; //登录用户名
	char AlarmTime[24];   //设备名称
	char IP[64];     //设备IP地址字符
	//char Date[20];
	int AlarmType;
	int Pset;
	double Lng;
	double Lat;
}AlarmRecord;
// CwjjcDlg 对话框
class CwjjcDlg
{
	// 构造
public:
	string  trueorfalse;
	string falsesampl = "D:\\false";
	CString g_strWorkDir;
	int m_JudgeTime = 1;
	void PicJudgement();
	void GetAllCatalog(string path, vector<string>& files);
	void GetAllFiles(string path, vector<string>& files);
	void FinalProcess(string ImgPathPast, string ImgPathNow);
	bool VarianceCheckWu(Mat src);
	int OpenCVSurfMatch(const Mat& colorImg1, const Mat& colorImg2, Mat& tran);
	int CwjjcDlg::getSelectionDescriptors(const vector<KeyPoint>& key1, const Mat& descriptors, vector<KeyPoint>& keygood, Mat& descriptorstmp, const Mat& img);
	void getPointNormal(Point2f& a, int cols, int rows);
	string CwjjcDlg::CString2string(CString &str);
	void CwjjcDlg::GetMatches(const cv::Mat& queryDescriptors, const cv::Mat&trainDescriptors, std::vector<cv::DMatch>& matches,float rate);
	vector<Mat> CwjjcDlg::Noiseeliminate(Mat img1, Mat img2, vector<Rect>&rects, string ImgPathPast);
	bool CwjjcDlg::MatchingMethod1(Mat templ, Mat img);
	/*bool CwjjcDlg::colorSelection(Mat src, double Variance, int BGRLimit = 200);*/
	bool CwjjcDlg::colorSelection(Mat src);
	Mat CwjjcDlg::CutImage(Mat templ1, int i, int j, int k, int l, int border);
	bool CwjjcDlg::IsGreen(const Mat inputImage);
	bool CwjjcDlg::BuildingDetectFlag2(Mat src);
	vector<Rect> CwjjcDlg::HaarVehicle3(Mat imgnow, double scale = 1);
	vector<Rect> detection2(IplImage *img1, CascadeClassifier& cascade, double scale);
	bool CwjjcDlg::CarDetection(Rect rect, vector<Rect> rects);
	BOOL CwjjcDlg::OnInitDialog();
	bool CwjjcDlg::MatchingMethod2(Mat src1, Mat src2);
	bool HaarVehicleD(Mat &image, Rect &rect, double scale );
	bool  CwjjcDlg::getTheSimilarityBySSIM(const Mat& src1, const Mat& src2);
	Scalar getSSIM(const Mat&  inputimage1, const Mat& inputimage2);
	bool CwjjcDlg::BuildingDetectFlag1(const Mat& img1, const Mat& img2);
	int  CwjjcDlg::finalProcessGetLineNumbers(const Mat& img, float& length);
	bool CwjjcDlg::IsSamePic(Mat &image1, Mat &image2);
	void CwjjcDlg::PaintBlack(Mat test);
	bool CwjjcDlg::BuildingDetectFlag3(const Mat& img);
	void  CwjjcDlg::finalProcessGetLineAngles(const Mat& img, vector<int>& angles);
};
