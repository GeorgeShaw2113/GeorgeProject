// ASWJJC.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include"IllegalBuildingsDetector.h"
#include<shlobj.h> 
#include<iostream>
#include<fstream>

#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
using namespace std;
using namespace cv;

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���
#define IDTIMER1 1001




// CwjjcDlg ��Ϣ�������

BOOL CwjjcDlg::OnInitDialog()
{

	char tmpDir[1000];
	_getcwd(tmpDir, 1000);
	g_strWorkDir.Format("%s", tmpDir);
	CString m_AlarmLocalIniPath;
	TCHAR sTempPath[MAX_PATH] = _T("");
	::SHGetSpecialFolderPath(NULL, sTempPath, CSIDL_COMMON_APPDATA, FALSE);
	m_AlarmLocalIniPath.Format(_T("%s\\aswj"), sTempPath);
	if (!::PathFileExists(m_AlarmLocalIniPath)){
		::CreateDirectory(m_AlarmLocalIniPath, NULL);
	}
	m_AlarmLocalIniPath += "\\AlarmRecord.ini";
	m_JudgeTime = GetPrivateProfileInt("JudgeTime", "Day", 0, m_AlarmLocalIniPath);
	if (m_JudgeTime == 0 || m_JudgeTime>30)
	{
		m_JudgeTime = 1;
	}
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}


// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�



inline const float getDistance(Point va){
	return std::sqrt(va.x*va.x + va.y*va.y);
}
inline const float getDistance(Point a, Point b){
	Point target(a.x - b.x, a.y - b.y);
	return getDistance(target);
}
inline const float getDistance(Vec4i line){
	Point a(line[0], line[1]);
	Point b(line[2], line[3]);
	return getDistance(a, b);
}
//�������� ����һ���߶εĽǶȹ�ʽ
//���߶���X��֮��0-180�ĽǶȱ��0-90��
inline float get0To90A(float angle){
	return angle > 90 ? 180 - angle : angle;
}
inline const float getAngle(Point va){
	return get0To90A(std::abs(fastAtan2(va.y, va.x) - 180));
}
inline const float getAngle(Vec4i a){
	Point va(a[2] - a[0], a[3] - a[1]);
	return get0To90A(getAngle(va));
}
inline Point getLineCenter(Vec4i line){
	return Point((line[2] + line[0]) / 2, (line[1] + line[3]) / 2);
}
inline Point getSetCenter(const vector<int> line_set, const vector < Vec4i>&lines_color){
	Point center;
	int num = line_set.size();
	for (int i = 0; i<num; i++){
		Vec4i l = lines_color[line_set[i]];
		center.x += (l[0] + l[2]) / 2;
		center.y += (l[1] + l[3]) / 2;
	}
	center.x /= num;
	center.y /= num;
	return center;
}
//���� �߶�a��b֮��ļн� ��0-90��
inline float findOrthoLine(Vec4i a, Vec4i b){
	float aA = getAngle(a);
	float bA = getAngle(b);
	float abA = std::abs(aA - bA);
	return get0To90A(abA);
}
void CwjjcDlg::PicJudgement()
{

	int startJudge=1;
	int stopJudge=6;

	/*int m_JudgeTime = 6;*/
	vector<string> firstfiles;		//��ȡ����IP�ļ���
	vector<string> secondfiles;		//��ȡ����Ԥ�õ��ļ���
	vector<string> thirdfiles;  //��ȡ���������ļ���
	vector<string> fourthfiles; //��ȡ����ͼƬ·��
	vector<string> fifthfiles; //ĳһԤ�õ�����ͼƬ·��
	string firstpath = "D:\\aswj";
	string sImgPathPast;
	string sImgPathNow;
	int iPicNum = 0;
	int iPicOrder = 0;
	GetAllCatalog(firstpath, firstfiles);
	if (firstfiles.size()<3)
	{
		return;
	}
	for (int i = 2; i <= firstfiles.size() - 1; i++)
	{
		secondfiles.clear();
		GetAllCatalog(firstfiles[i], secondfiles);
		if (secondfiles.size() < 3){ continue; }
		for (int j = 2; j <= secondfiles.size() - 1; j++)
		{
			/*thirdfiles.clear();
			GetAllCatalog(secondfiles[j], thirdfiles);*/
			/*for (int k = 2; k <= thirdfiles.size() - 1; k++)
			{*/
			iPicNum++;
			/*}*/
		}
	}

	/*myProCtrl->SetRange(0, iPicNum);*/
	firstfiles.clear();
	secondfiles.clear();
	thirdfiles.clear();
	fourthfiles.clear();
	GetAllCatalog(firstpath, firstfiles);
	for (int i = 2; i <= firstfiles.size() - 1; i++)
	{
		secondfiles.clear();
		GetAllCatalog(firstfiles[i], secondfiles);
		if (secondfiles.size() < 3){ continue;}
		for (int j = 2; j <= secondfiles.size() - 1; j++)
		{
			thirdfiles.clear();
			GetAllCatalog(secondfiles[j], thirdfiles);
			for (int k = 2; k <= thirdfiles.size() - 1; k++)
			{
				GetAllFiles(thirdfiles[k], fourthfiles);
				for (int l = 0; l < fourthfiles.size(); l++)
				{
					fifthfiles.push_back(fourthfiles[l]);
				}
				fourthfiles.clear();
			}
			sImgPathPast = *(fifthfiles.end() - startJudge);
			sImgPathNow = *(fifthfiles.end() - stopJudge);
			FinalProcess(sImgPathPast, sImgPathNow);
			iPicOrder++;
			double percent = (double)iPicOrder / (double)iPicNum*100.00;
			int iPercent = iPicOrder * 100 / iPicNum;
			for (int i = 0; i <= 100; i++)
			{
				if (i< percent)
				{
					cout << "=";
					continue;
				}
				if (i == (int)percent + 1)
				{
					cout << ">";
					continue;
				}
				if (i < 100)
				{
					cout << "_";
				}
			}
				printf("%2d%%\r", iPercent);

				/*myProCtrl->SetPos(iPicOrder);*/
			
		}
	}


}


void CwjjcDlg::GetAllFiles(string path, vector<string>& files)
{

	long   hFile = 0;
	//�ļ���Ϣ      
	struct _finddata_t fileinfo;//�����洢�ļ���Ϣ�Ľṹ��      
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)  //��һ�β���    
	{
		do
		{
			if ((fileinfo.attrib &  _A_SUBDIR))  //������ҵ������ļ���    
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)  //�����ļ��в���    
				{
					files.push_back(p.assign(path).append("\\").append(fileinfo.name));
					GetAllFiles(p.assign(path).append("\\").append(fileinfo.name), files);
				}
			}
			else //������ҵ��Ĳ����ļ���     
			{
				files.push_back(p.assign(path).append("\\").append(fileinfo.name));  //���ļ�·�����棬Ҳ����ֻ�����ļ���:        
			}

		} while (_findnext(hFile, &fileinfo) == 0);

		_findclose(hFile); //��������    
	}

}

void CwjjcDlg::GetAllCatalog(string path, vector<string>& files)
{

	long   hFile = 0;
	//�ļ���Ϣ      
	struct _finddata_t fileinfo;//�����洢�ļ���Ϣ�Ľṹ��      
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)  //��һ�β���    
	{
		do
		{

			files.push_back(p.assign(path).append("\\").append(fileinfo.name));  //���ļ�·�����棬Ҳ����ֻ�����ļ���:        


		} while (_findnext(hFile, &fileinfo) == 0);


		_findclose(hFile); //��������    
	}

}

void CwjjcDlg::FinalProcess(string ImgPathPast, string ImgPathNow)
{
	//if (ImgPathPast.size() != 42 || ImgPathNow.size() != 42)
	//{
	//	return;
	//}
	//his = 1;
	bool flag1, flag2, flag3, flag4, flag5, flag6, flag7;
	cv::Mat inputImg, imgGRAY;
	cv::Mat outputImg;
	//cv::Mat result;
	/*int mode = MODE_NIETO;*/
	int numVps = 3;//vp������
	bool playMode = false;
	bool stillImage = false;
	bool verbose = false;//�Ƿ���ʾ��ʧ��������Ϣ

	int procWidth = -1;
	int procHeight = -1;//ǰ��������
	Mat Img5 = imread(ImgPathPast, CV_LOAD_IMAGE_COLOR);
	Mat Img6 = imread(ImgPathNow, CV_LOAD_IMAGE_COLOR);
	Mat Img7 = Img6.clone();
	PaintBlack(Img5);
	PaintBlack(Img6);
	if (VarianceCheckWu(Img5)){
		/*MessageBox("��������������仯");*/
	/*	cout << ImgPathPast << "Wu1"<<endl;*/
		return;
	}
	if (VarianceCheckWu(Img6)){
		/*MessageBox("��������������仯");*/
	/*	cout << ImgPathNow << "Wu2"<<endl;*/
		return;
	}
	//int IsMatch = SurfPictureMatch(ImgPathNow, ImgPathPast);
	Mat transform;
	int IsMatch = OpenCVSurfMatch(Img6, Img5, transform);
	//int IsMatch = PictureMatch(ImgPathNow.c_str(), ImgPathPast.c_str());//ͼ����׼
	if (IsMatch == 1){
		/*MessageBox("����������Ԥ�õ��쳣");*/
	/*	cout << ImgPathNow << "not match1" << endl;*/
		return;
	}
	//if (IsMatch == 2){
	//	/*������������������ȥ����ٽ���ͼƬ��׼*/
	//	IplImage* w1 = cvLoadImage(ImgPathNow.c_str(), 3);//��ͨ��ȥ��Ľ�
	//	IplImage* w2 = cvLoadImage(ImgPathPast.c_str(), 3);
	//	w1 = dcp(1, 1, 0, 0, w1, TRUE);
	//	w2 = dcp(1, 1, 0, 0, w2, TRUE);
	//	cvSaveImage(ImgPathNow.c_str(), w1);
	//	cvSaveImage(ImgPathPast.c_str(), w2);
	//	cvReleaseImage(&w1);
	//	cvReleaseImage(&w2);

	//	IsMatch = PictureMatch(ImgPathNow.c_str(), ImgPathPast.c_str());//ͼ����׼
	//	if (IsMatch != 0)
	//		return;
	//}
	if (IsMatch == 3){	//�����ͼ�񼸺�һ�����Է�ֹ��׼��ѭ��
		/*MessageBox("����ͼƬû�б仯");*/
	/*	cout << ImgPathNow << "not match2" << endl;*/
		return;
	}
	//PictureMatch(CString2string(Img1Name).c_str(), CString2string(Img2Name).c_str());//ͼ����׼
	CString cMatchPath = g_strWorkDir + "\\MatchPic\\tempPic.jpg";
	char* MatchPath;
	MatchPath = (char*)cMatchPath.GetBuffer(cMatchPath.GetLength());
	Img6 = imread(MatchPath, CV_LOAD_IMAGE_COLOR);//��ȡ��׼���ͼ��
	cMatchPath.ReleaseBuffer();
	/*delete MatchPath;*/
	/*ImgRepair(Img6);*/
	//Img5 = imread(ImgPathPast, CV_LOAD_IMAGE_COLOR);
	//int border = 13;//ȥ���߽�
	//Mat Img3 = Img5(Range(Img5.rows * 1 / border, Img5.rows*(border - 1) / border), Range(0, Img5.cols));
	//Mat Img4 = Img6(Range(Img6.rows * 1 / border, Img6.rows*(border - 1) / border), Range(0, Img6.cols));

	vector<Rect>rects, nowcars;
	vector<Mat>pictures;
	pictures = Noiseeliminate(Img5, Img6, rects, ImgPathPast);//�������б仯�����rect
	vector<Point2f> ponits, points_trans;
	for (int i = 0; i < rects.size(); i++)
	{
		//rects[i] = rects[i] + Point(0, Img5.rows * 1 / border); //ƽ��
		ponits.push_back(Point2f(rects[i].x, rects[i].y));
	}
	if (!ponits.empty())
	{
		perspectiveTransform(ponits, points_trans, transform);
	}

	for (int t = 0; t < points_trans.size(); t++)
	{
		Rect tempi(points_trans[t].x, points_trans[t].y, rects[t].width, rects[t].height);
		nowcars.push_back(tempi);
	}//�õ�ԭͼ�仯�������
	/*************������Ŀ¼�ļ���������***************************************************************/
	/*vector<Rect>RectVehicleNow = HaarVehicle3(Img6);
	vector<Rect>RectVehiclePast = HaarVehicle3(Img5);*/
	

	if (pictures.size() == 0)
	{
		/*cout << ImgPathNow <<"size=0"<< endl;*/
	}
	int is = 0;
	for (int i = 0; i < pictures.size(); i++)        //	for (; it != itend; it++)�����ļ�
	{

		//����ͼ��ǿ��ת��ΪGray
		inputImg = pictures.at(i);

		cv::Size procSize;
		// Check input
		int width = 0, height = 0, fps = 0, fourcc = 0;
		if (inputImg.empty())
		{
			/*return -1;*/

		}
		else
		{
			width = inputImg.cols;
			height = inputImg.rows;
			/*printf("Input image: (%d x %d)\n", width, height);*/
			playMode = false;
		}
		// Resize	
		if (procWidth != -1)
		{

			procHeight = height*((double)procWidth / width);
			procSize = cv::Size(procWidth, procHeight);
			/*	printf("Resize to: (%d x %d)\n", procWidth, procHeight);*/
		}
		else
		{
			procSize = cv::Size(width, height);

		}
		// Create and init MSAC
		/*	MSAC msac;
		msac.init(mode, procSize, verbose);*/

		int frameNum = 0;

		if (inputImg.empty())
			break;

		// Resize to processing size
		cv::resize(inputImg, inputImg, procSize);

		// Color Conversion
		if (inputImg.channels() == 3)
		{
			cv::cvtColor(inputImg, imgGRAY, CV_BGR2GRAY);
			inputImg.copyTo(outputImg);
		}
		else
		{
			inputImg.copyTo(imgGRAY);
			cv::cvtColor(inputImg, outputImg, CV_GRAY2BGR);
		}

		/*	flag2 = processImage(msac, numVps, imgGRAY, outputImg);*/
	/*	flag2 = true;
		flag3 = BuildingDetectFlag2(outputImg);
		flag4 = IsGreen(inputImg);
		flag5 = VarianceCheckWu(inputImg);*/

		flag2 = true;
		flag3 = true;
		flag4 = IsGreen(inputImg);
		flag5 = false;

	/*	flag5 = false;*/
		//rects[i] = rects[i] + Point(0, Img6.rows * 1 / border); //ƽ�Ƶ�ԭͼ������ߴ�
		/*flag6 = HaarVehicle(inputImg);*/
		/*flag6 = HaarVehicle2(Img6, rects[i]);*/

		//flag6 = CarDetection(rects[i], RectVehicleNow);

		//flag7 = CarDetection(rects[i], RectVehiclePast);
	/*	flag6 = HaarVehicleD(Img7, nowcars[i],5);
		flag7 = HaarVehicleD(Img5, rects[i],5);*/
		flag6 = false;
		flag7 = false;

		/*if (flag2 == true && flag3 == true && flag4 == false && flag5 == false && flag6 == false && flag7 == false)*/
		if (flag2 == true && flag3 == true && flag4 == false && flag5 == false && flag6 == false && flag7 == false)
		{

			is = 1;
			//rects[i] = rects[i] + Point(0, Img6.rows * 1 / border); //ƽ�ƣ�Ҳ�������϶����x����-100��y����+100
			rectangle(Img7, nowcars[i], Scalar(0, 0, 255), 3, 4, 0);//�����ߣ����̺�
		}
		if (i == pictures.size() - 1){
			if (is == 1){
				string pathname1, pathname2;
				int nLen = ImgPathNow.size();
				int IPLen = nLen - 29;
				int DreaseLen = IPLen + 19;
				string tempPathDecrease1 = ImgPathNow;
				string tempPathDecrease2 = ImgPathPast;
				tempPathDecrease1.erase(0, DreaseLen);
				tempPathDecrease2.erase(0, DreaseLen);
				tempPathDecrease1.erase(6, 4);
				tempPathDecrease2.erase(6, 4);
				string tempPath1 = ImgPathNow;
				string tempPath2 = ImgPathPast;
				tempPath1.erase(0, 7);
				tempPath2.erase(0, 7);
				tempPath1.erase(IPLen + 12, 10);
				tempPath2.erase(IPLen + 12, 10);

				pathname1 = "D:\\AlarmPictures" + tempPath1 + tempPathDecrease1 + "\\" + tempPathDecrease1 + ".jpg";
				pathname2 = "D:\\AlarmPictures" + tempPath1 + tempPathDecrease1 + "\\" + tempPathDecrease2 + ".jpg";
				int i = 0;
				int iRet;
				int iLen = 0;
				char* TempDir;
				/*	strcpy(TempDir, pathname1.c_str());*/
				TempDir = _strdup(pathname1.c_str());
				iLen = strlen(TempDir);
				for (i = 0; i < iLen; i++)
				{
					if (TempDir[i] == '\\' || TempDir[i] == '/')
					{
						TempDir[i] = '\0';
						iRet = _access(TempDir, 0);
						if (iRet != 0)
						{
							iRet = _mkdir(TempDir);
							if (iRet != 0)
							{
								break;
							}
						}
						TempDir[i] = '\\';
					}
				}
				free(TempDir);

				imwrite(pathname1.c_str(), Img7);//���洦�����outputͼ��
				Mat Img8 = imread(ImgPathPast);
				imwrite(pathname2.c_str(), Img8);
				/*��ini��д�뱨����Ϣ*/
				CString m_AlarmLocalIniPath;
				TCHAR sTempPath[MAX_PATH] = _T("");
				::SHGetSpecialFolderPath(NULL, sTempPath, CSIDL_COMMON_APPDATA, FALSE);
				m_AlarmLocalIniPath.Format(_T("%s\\aswj"), sTempPath);

				//�����Ŀ¼�Ƿ���ڣ���������ڣ��򴴽���
				if (!::PathFileExists(m_AlarmLocalIniPath)){
					::CreateDirectory(m_AlarmLocalIniPath, NULL);
				}
				nLen = ImgPathNow.size();
				IPLen = nLen - 29;
				string tmpIP = ImgPathNow.substr(8, IPLen);
				string tmpPset = ImgPathNow.substr(9 + IPLen, 2);
				string tmpAlarmtime = ImgPathNow.substr(12 + IPLen, 4) + "/" + ImgPathNow.substr(16 + IPLen, 2) + "/" + ImgPathNow.substr(19 + IPLen, 2) + "  " + ImgPathNow.substr(21 + IPLen, 2) + ":" + ImgPathNow.substr(23 + IPLen, 2);
				m_AlarmLocalIniPath += "\\AlarmRecord.ini";
				char tmpdeviceName[24];
				char tmpcameraname[20];
				vector<UserInfo> vUserInfo;
				UserInfo tmpInfo;
				CString strPath = g_strWorkDir + "\\userinfo.ini";
				vUserInfo.clear();
				for (int i = 0; i < 1000; i++)
				{

					CString strKey;
					strKey.Format("%d", i);
					strPath = g_strWorkDir + "\\userinfo.ini";
					if (GetPrivateProfileStruct("UserInfo", strKey, (LPVOID)&tmpInfo, sizeof(UserInfo), strPath))
					{
						vUserInfo.push_back(tmpInfo);
					}
					else break;
				}
				for (int i = 0; i < vUserInfo.size(); i++)
				{
					if (!strcmp(vUserInfo[i].ip, tmpIP.c_str()))
					{
						strcpy_s(tmpdeviceName, vUserInfo[i].devicename);
						strcpy_s(tmpcameraname, vUserInfo[i].cameranumber);
					}
				}
				AlarmRecord test;
				memset(&test, 0, sizeof(AlarmRecord));
				strcpy_s(test.Location, tmpdeviceName);
				strcpy_s(test.AlarmTime, tmpAlarmtime.c_str());
				strcpy_s(test.IP, tmpIP.c_str());
				strcpy_s(test.CameraName, tmpcameraname);
				test.Pset = atoi(tmpPset.c_str());
				int Count = GetPrivateProfileInt("AlarmInfo", "Count", 0, m_AlarmLocalIniPath);
				char count1[4];
				char count2[4];
				sprintf_s(count1, "%d", Count);
				Count++;
				sprintf_s(count2, "%d", Count);
				WritePrivateProfileString("AlarmInfo", "Count", count2, m_AlarmLocalIniPath);
				WritePrivateProfileStruct("AlarmInfo", count1, (LPVOID)&test, sizeof(AlarmRecord), m_AlarmLocalIniPath);

	/*			pathname1 = "D:\\false" + tempPath1 + tempPathDecrease1 + "origin.jpg";
				pathname2 = "D:\\false" + tempPath1 + tempPathDecrease2 + "origin.jpg";*/


				//int iRet1;
				//int iLen1 = 0;
				//char* TempDir1;
				///*	strcpy(TempDir, pathname1.c_str());*/
				//TempDir1 = _strdup(pathname1.c_str());
				//iLen1 = strlen(TempDir1);
				//for (int i = 0; i < iLen1; i++)
				//{
				//	if (TempDir1[i] == '\\' || TempDir1[i] == '/')
				//	{
				//		TempDir1[i] = '\0';
				//		iRet1 = _access(TempDir1, 0);
				//		if (iRet1 != 0)
				//		{
				//			iRet1 = _mkdir(TempDir1);
				//			if (iRet1 != 0)
				//			{
				//				break;
				//			}
				//		}
				//		TempDir1[i] = '\\';
				//	}
				//}
				//free(TempDir1);
				//Mat tmp21 = imread(ImgPathPast);
				//Mat tmp22 = imread(ImgPathNow);
				//imwrite(pathname1, tmp22);
				//imwrite(pathname2, tmp21);
				/*delete[]count;*/
			}
			else{
				/*	MessageBox("������Υ�½���");*/
			}
		}
	}
}

bool CwjjcDlg::VarianceCheckWu(Mat src){
	int SIZE = 5;
	/*char *p = const_cast<char*>(pic1);
	IplImage* img1 = cvLoadImage(pic1, 1);*/
	//if (!img1){

	//	return true;
	//}
	/*cv::Mat src = cv::cvarrToMat(img1);*/
	Mat dst = Mat::zeros(Size(src.cols, src.rows), src.type());
	/*
	���Ƚ���һ���²�������Сͼ��ĳߴ磬��Ҫ��ԭͼ����
	*/
	pyrDown(src, dst);
	//imshow("x1", dst);
	/*
	���㷽��ķ��������Աȶȵ����
	*/
	// ѡȡ100������� ���޶����ȡֵ��Χ,��ķ�Χ��ȷ��Ŀǰ������ȷ
	vector<Point2d> points(100);
	int count = 0;
	int high_limitx = dst.cols - SIZE * 4;
	int high_limity = dst.rows - SIZE * 4;
	int low_limitx = SIZE * 2;
	int low_limity = SIZE * 2;
	//if (low_limitx + SIZE>dst.cols | low_limity + SIZE>dst.rows | high_limitx - SIZE<0 | high_limity - SIZE<0 | low_limitx>high_limitx | low_limity>high_limity){//��ֹԽ��
	//	return true;
	//}
	/*
	��ɸѡ���ĵ���ӵ�points��������
	*/
	while (count++ < 100){
		int x = randu<unsigned int>() % (high_limitx)+low_limitx;
		int y = randu<unsigned int>() % (high_limity)+low_limity;
		Point point = Point(x, y);
		points[count - 1] = point;
	}
	/*
	���� ROI�ķ���۵㣺���ڹ۲�ͼƬ��֪��һ����������ͼƬ�ĶԱȶȺܲ��ͨ�����㷽��Ĵ�С�������
	*/
	vector<Mat> standards(100);
	vector<Mat> means(100);
	int k = 0;
	/*
	����points������ÿ�����ص㣬5X5�ķ�Χ�ķ���ֵ
	*/
	while (k < 100){
		Mat ROI = dst(Rect(points[k].x, points[k].y, SIZE, SIZE));
		meanStdDev(ROI, means[k], standards[k]);
		k++;
	}
	int satifyPoints = 0;//�ñ���ָ�������㷽��С���޶�ֵ�ĵ�
	for (Mat s : standards){
		double choose = cvRound(s.at<double>(0, 0));
		//�趨����ֵС��5.0�ĵ��趨Ϊ�����
		if (choose < 4.0){
			satifyPoints++;
		}
	}

	/*
	�����ϸ��ͼƬ���浽txt��,������ֵ����Ϊ70��������satify���������>70���жϸ�ͼƬ���ϸ�
	*/
	/*cvReleaseImage(&img1);*/
	if (satifyPoints>70){
		return true;
	}
	else{ return false; }
}

int CwjjcDlg::OpenCVSurfMatch(const Mat& colorImg1, const Mat& colorImg2, Mat& tran)
{
	int m, n1, n2;
	Mat img1, img2;


	//colorImg1.convertTo(img1, CV_32FC1);
	//colorImg2.convertTo(img2, CV_32FC1);
	/*img1 /= 255.0;
	img2 /= 255.0;*/
	Ptr<cv::xfeatures2d::SURF> surf;      //������ʽ��2�еĲ�һ��  
	surf = cv::xfeatures2d::SURF::create(400);
	Mat descriptors1, descriptors2;
	vector<KeyPoint>key1, key2;
	vector< DMatch > good_matches;
	surf->detectAndCompute(colorImg1, Mat(), key1, descriptors1);
	surf->detectAndCompute(colorImg2, Mat(), key2, descriptors2);

	vector<KeyPoint> keygood1, keygood2;
	Mat descriptors1t = Mat::zeros(descriptors1.size(), descriptors1.type());
	Mat descriptors2t = Mat::zeros(descriptors2.size(), descriptors2.type());

	int count1 = getSelectionDescriptors(key1, descriptors1, keygood1, descriptors1t, colorImg1);
	int count2 = getSelectionDescriptors(key2, descriptors2, keygood2, descriptors2t, colorImg2);

	Mat descriptorsgood1 = descriptors1t(Rect(0, 0, 64, count1)).clone();
	Mat descriptorsgood2 = descriptors2t(Rect(0, 0, 64, count2)).clone();
	GetMatches(descriptorsgood1, descriptorsgood2, good_matches,0.55);
	m = good_matches.size(); //�ж���ͼ�����ƶ�
	n1 = keygood1.size();
	n2 = keygood2.size();

	//GetMatches(descriptors1, descriptors2, good_matches);
	//m = good_matches.size(); //�ж���ͼ�����ƶ�
	//n1 = key1.size();
	//n2 = key2.size();
	if (((double)m / ((n1 > n2) ? n1 : n2))<0.01){
		if (m<30){
			return 1;				//ƥ���ϵĵ�̫����Ϊ��ͬԤ�õ��ͼƬ
		}
		//else{
		//	return 2;				//��Ƶ�������죬���Ƚ�
		//}

	}
	if ((double)m / ((n1 > n2) ? n1 : n2)>0.96){		//����һ����ͼƬ��ֹH��ѭ��
		return 3;
	}

	std::vector<Point2f> obj;
	std::vector<Point2f> scene;

	for (size_t i = 0; i < good_matches.size(); i++)
	{
		obj.push_back(keygood1[good_matches[i].queryIdx].pt);
		scene.push_back(keygood2[good_matches[i].trainIdx].pt);
	}
	Mat imageTransform,BlackEdgeTransform;
	Mat H = findHomography(obj, scene, RANSAC);      //Ѱ��ƥ���ͼ�� 
	Mat tranH = findHomography(scene, obj, RANSAC);
	tran = tranH;//���淴�任����
	Mat BlackEdge(colorImg1.rows, colorImg1.cols, CV_8UC3, Scalar(255, 255, 255));
	/*perspectiveTransform(obj_corners, scene_corners, H);*/
	warpPerspective(colorImg1, imageTransform, H, Size(colorImg2.cols, colorImg2.rows));
	warpPerspective(BlackEdge, BlackEdgeTransform, H, Size(colorImg2.cols, colorImg2.rows));
	CString strPath;
	CString cBlackEdge;
	strPath = g_strWorkDir + "\\MatchPic\\";
	int i = 0;
	int iRet;
	int iLen = 0;
	char* TempDir;
	TempDir = _strdup(strPath);
	iLen = strlen(TempDir);
	for (i = 0; i < iLen; i++)
	{
		if (TempDir[i] == '\\' || TempDir[i] == '/')
		{
			TempDir[i] = '\0';
			iRet = _access(TempDir, 0);
			if (iRet != 0)
			{
				iRet = _mkdir(TempDir);
				if (iRet != 0)
				{
					break;
				}
			}
			TempDir[i] = '\\';
		}
	}
	free(TempDir);
	cBlackEdge = strPath + "BlackEdge.jpg";
	strPath = strPath + "tempPic.jpg";
	imwrite(CString2string(strPath), imageTransform);
	imwrite(CString2string(cBlackEdge), BlackEdgeTransform);
	return 0;
}

//int CwjjcDlg::getSelectionDescriptors(const vector<KeyPoint>& key1, const Mat& descriptors, vector<KeyPoint>& keygood, Mat& descriptorstmp, const Mat& img){
//	int count = 0;
//	for (int i = 0; i < key1.size(); i++){
//		const float* inDate = descriptors.ptr<float>(i);
//
//		Point2f center = key1[i].pt;
//
//		Point2f start = Point(center.x - 5, center.y - 5);
//		Point2f end = Point(center.x + 5, center.y + 5);
//		getPointNormal(start, img.cols, img.rows);
//		getPointNormal(end, img.cols, img.rows);
//		Mat ROI = img(Rect(start, end));
//		double maxVal = 0;
//		minMaxLoc(ROI, 0, &maxVal);
//		if (maxVal > 225) continue;
//		keygood.push_back(key1[i]);
//		float* outDate = descriptorstmp.ptr<float>(count++);
//		for (int j = 0; j < 64; j++){
//
//			*outDate++ = *inDate++;
//		}
//	}
//	return count;
//}


int CwjjcDlg::getSelectionDescriptors(const vector<KeyPoint>& key1, const Mat& descriptors, vector<KeyPoint>& keygood, Mat& descriptorstmp, const Mat& img){
	int count = 0;
	for (int i = 0; i < key1.size(); i++){
		const float* inDate = descriptors.ptr<float>(i);

		Point2f center = key1[i].pt;

		int cx = center.x;
		int cy = center.y;

		/*if (cy >= 900 && cy <= 1000 && cx >= 1550 && cx <= 1880)
			continue;*/
		if (cy >= 48 && cy <= 95 && cx >= 1058 && cx <= 1845)
			continue;
		/*if (cy >= 990 && cy <= 1050 && cx >= 1400 && cx <= 1880)*/
			/*continue;*/
		if (cy >= 1000 && cy <= 1055 && cx >= 55 && cx <= 485)
			continue;
		Vec3b cPoint = img.at<Vec3b>(center);
		keygood.push_back(key1[i]);
		float* outDate = descriptorstmp.ptr<float>(count++);
		for (int j = 0; j < 64; j++){

			*outDate++ = *inDate++;
		}
	}
	return count;
}



void CwjjcDlg::getPointNormal(Point2f& a, int cols, int rows){
	if (a.x < 0) a.x = 0;
	if (a.x>cols - 1) a.x = cols - 1;
	if (a.y < 0) a.y = 0;
	if (a.y>rows - 1) a.y = rows - 1;
}

string CwjjcDlg::CString2string(CString &str)
{
	if (str.IsEmpty())
		return 0;

	CStringA strA;
	std::string strs;

	strA = str.GetBuffer(0);
	str.ReleaseBuffer();
	strs = strA.GetBuffer(0);
	strA.ReleaseBuffer();

	return strs;
}

void CwjjcDlg::GetMatches(const cv::Mat& queryDescriptors, const cv::Mat&trainDescriptors, std::vector<cv::DMatch>& matches,float rate)
{
	matches.clear();

	// To avoid NaNs when best match has   
	// zero distance we will use inverse ratio.   
	const float minRatio = 0.9f;
	// KNN match will return 2 nearest   
	// matches for each query descriptor 
	BFMatcher m_matcher;
	vector<vector<DMatch> > m_knnMatches;
	m_matcher.knnMatch(queryDescriptors, trainDescriptors, m_knnMatches, 2);
	for (size_t i = 0; i<m_knnMatches.size(); i++)
	{
		const cv::DMatch& bestMatch = m_knnMatches[i][0];
		const cv::DMatch& betterMatch = m_knnMatches[i][1];
		float distanceRatio = bestMatch.distance /
			(betterMatch.distance);
		// Pass only matches where distance ratio between   
		// nearest matches is greater than 1.5   
		// (distinct criteria)  
		if (distanceRatio < minRatio)
		{
			matches.push_back(bestMatch);
		}
	}
}
int CalThreshold(Mat &input)
{
	int w = input.cols;
	int h = input.rows;
	//int s=input.channels();
	int maxthresh = 0, minthresh = 255;  //���ڴ��������С�ĻҶ�ֵ
	uchar * data = NULL;
	int gray[256] = { 0 };     //�����ص�ֱ��ͼ��ͳ��
	int totalgray = 0;       //ͼ��ĻҶ��ܺ�
	int totalnum = 0;        //ͼ�����������
	int flag = 0;
	for (int i = 0; i<h; i++)
	{
		data = input.ptr<uchar>(i);
		for (int j = 0; j<w; j++)
		{
			flag = data[j];
			if (flag>maxthresh)
			{
				maxthresh = flag;
			}
			if (flag<minthresh)
			{
				minthresh = flag;
			}
			gray[flag]++;
		}
	}
	int avgvalue = (maxthresh + minthresh) / 2;  //��������ֵΪ���ص�ƽ�������е���
	int newvalue = 0;
	double value1 = 0.0;
	double value2 = 0.0;//��������ֵ�ָ�������ֵĻҶȵ�ƽ����
	while (abs(avgvalue - newvalue)>0.1)
	{
		newvalue = avgvalue;
		for (int i = 0; i<newvalue; i++)
		{
			totalgray += i*gray[i];
			totalnum += gray[i];
		}
		value1 = ((double)totalgray) / totalnum;
		totalgray = 0;
		totalnum = 0;
		for (int j = newvalue; j<maxthresh + 1; j++)
		{
			totalgray += j*gray[j];
			totalnum += gray[j];
		}
		value2 = ((double)totalgray) / totalnum;
		avgvalue = (value1 + value2) / 2;
	}
	return avgvalue;
}
vector<Mat> CwjjcDlg::Noiseeliminate(Mat img1, Mat img2, vector<Rect>&rects, string ImagePath)
{
	
	Mat img_result1, img_result2, img;
	Mat img11, img22;
	
	
	CString cMatchPath = g_strWorkDir + "\\MatchPic\\BlackEdge.jpg";
	Mat tmpx = imread(CString2string(cMatchPath));
	cvtColor(tmpx, tmpx, COLOR_BGR2GRAY);
	//int border = 13;//ȥ���߽�
	//tmpx = tmpx(Range(tmpx.rows * 1 / border, tmpx.rows*(border - 1) / border), Range(0, tmpx.cols));
	//Mat largestContour(img2.rows, img2.cols, CV_8UC1, Scalar::all(0));
	//uchar table[256];          //ӳ����涨�˱任ǰ��Ҷ�ֵ�Ķ�Ӧ��ϵ table[gray_value_before]=gray_value_after  
	//for (int i = 0; i<256; i++)
	//{
	//	table[i] = i / 3;   //table���㷨  
	//}
	//img1 = ScanImageAndReduceIterator(img1, table);    //�任����  
	//namedWindow("�任��", 0);
	//imshow("�任��", img1);
	//img2 = ScanImageAndReduceIterator(img2, table);    //�任����  
	//imshow("�任��", img2);


	//���⻯�󣬻Ҷȴ���
	cvtColor(img1, img11, COLOR_BGR2GRAY);
	// ��3������ֱ��ͼ���⻯

	equalizeHist(img11, img11);
	//namedWindow("�任1", 0);
	//imshow("�任1", img11);
	//imwrite("bianhuan1.jpg", img11);
	cvtColor(img2, img22, COLOR_BGR2GRAY);
	// ��3������ֱ��ͼ���⻯

	equalizeHist(img22, img22);
	/*namedWindow("�任2", 0);
	imshow("�任2", img22);*/
	//imwrite("bianhuan2.jpg", img22);
	//waitKey();
	resize(img11, img11, img22.size(), 0, 0, 1);
	subtract(img11, img22, img_result1);
	subtract(img22, img11, img_result2);
	//imwrite("img_result1.jpg", img_result1);
	//imwrite("img_result2.jpg", img_result2);
	add(img_result1, img_result2, img);//���ߵĲ�ͬ
	
	/*imwrite("img1.jpg", img);*/
	img = img&tmpx;
	/*imwrite("img2.jpg", img);*/
	//absdiff(img11, img22, img);//���ߵĲ�ͬ
	/*subtract(img11, img22, img_result1);//ͼ1��ͼ2�ߵĵط�
	img = img_result1;*/
	/*namedWindow("result", 0);
	imshow("result", img);*/


	//δ�Ӿ��⻯
	//subtract(img1, img2, img_result1);
	//subtract(img2, img1, img_result2);
	///*subtract(img1,img2, img_result1);
	//subtract(img2,img1, img_result2);*/
	//add(img_result1, img_result2, img);
	//cvtColor(img, img, COLOR_BGR2GRAY);


	if (img.empty())
	{
		cout << "Error: Could not load image" << endl;
	}

	Mat gray;
	//	cvtColor(img, gray, CV_BGR2GRAY);

	Mat dst1, dst2, dst;
	/*threshold(img, dst, 0, 255, CV_THRESH_OTSU);*/
	//threshold(img, dst, 80, 255, CV_THRESH_BINARY);
	/*int thres = CalThreshold(img);
	cout << "��������ֵ" << thres << endl;*/
	//threshold(img, dst, thres, 255, CV_THRESH_TOZERO);//����������
	//threshold(img, dst, 80, 255, CV_THRESH_BINARY);//�仯��ֵ�趨
	int thres = CalThreshold(img);
	//Mat structure = getStructuringElement(MORPH_RECT, Size(3, 3));
	//morphologyEx(img, img, MORPH_CLOSE, structure);
	
	threshold(img, dst, thres, 255, CV_THRESH_OTSU & CV_THRESH_BINARY_INV);//��ֵ��ΪOTSU�ͱ仯С��100����
	
	//threshold(img_result1, dst1, thres, 255, CV_THRESH_OTSU & CV_THRESH_BINARY_INV);
	//threshold(img_result2, dst2, thres, 255, CV_THRESH_OTSU & CV_THRESH_BINARY_INV);
	Mat element1 = getStructuringElement(MORPH_RECT, Size(2,2));
	Mat element2 = getStructuringElement(MORPH_RECT, Size(2,2));
	
	dilate(dst,dst,element2);
	erode(dst, dst, element1);
	//erode(dst1, dst1, element1);
	//dilate(dst1, dst1, element2);
	//erode(dst2, dst2, element1);
	//dilate(dst2, dst2, element2);
	
	
	//imwrite("dst1.jpg", dst1);
	//imwrite("dst2.jpg", dst2);

	//int blockSize = 7;
	//int constValue = 11;
	//adaptiveThreshold(img, dst, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, blockSize, constValue);//����Ӧ��ֵ
	//namedWindow("src", 0);
	//namedWindow("dst", 0);

	//imshow("src", img);
	//imshow("dst", dst);
	//waitKey(500);
	//imwrite("02.png", dst);	



	vector<vector<Point>> contours, contours1, contours2, contourstmp;
	vector<Vec4i>Hierarchy, Hierarchy1, Hierarchy2;
	//CV_CHAIN_APPROX_NONE  ��ȡÿ������ÿ�����ص� 
	
	findContours(dst, contours, Hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0));
	/*findContours(dst1, contours1, Hierarchy1, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0));
	findContours(dst2, contours2, Hierarchy2, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0));*/
	int num;
	int flag = img2.rows*img2.cols / 500;		//�������õ���ֵ



	num = 0;
	Mat drawing;
	Rect r0, r1;//�������������������mat������
	vector<Mat>pictures,pictures2;
	Mat B, C,D;
	//for (int i = 0; i< contours1.size(); i++)
	//{



	for (int i = 0; i< contours.size(); i++)
	{


		if (contourArea(contours[i])>flag&&boundingRect(Mat(contours[i])).area()<(img2.rows*img2.cols / 6))
		{
			num++;


			Scalar color = Scalar(255, 255, 255);
			drawing = Mat::zeros(img2.size(), CV_8UC1);
			drawContours(drawing, contours, i, color, 2, 8, Hierarchy, 0, Point());
			r0 = boundingRect(Mat(contours[i]));
			

			
			double areaRate = (double)contourArea(contours[i]) / r0.area();
		
			B = img1(r0).clone();//��img1�ж�Ӧ������
			C = img2(r0).clone();//��img2�ж�Ӧ������
			float rows = B.rows;
			float cols = B.cols;
			if (rows / cols < 0.125 || rows / cols>8) {continue;}
			Mat hole(img1.size(), CV_8U, Scalar(0));
			contourstmp.clear();
			contourstmp.push_back(contours[i]);
			drawContours(hole, contourstmp, -1, Scalar(255), CV_FILLED);
			D = hole(r0).clone();
			Mat crop1(B.rows, B.cols, CV_8UC3);
			Mat crop2(C.rows, C.cols, CV_8UC3);
			B.copyTo(crop1, D);
			C.copyTo(crop2, D);

		/*	imwrite("B.jpg", B);
			imwrite("C.jpg", C);*/
			/*	imshow("B", B);
			waitKey();*/
			if (areaRate > 0.2){
				if (colorSelection(B) && colorSelection(C)){
					if (getTheSimilarityBySSIM(B, C)){
						if (true){//MatchingMethod2(B, C)){
							/*if (PictureIsDifferent(B, C)){*/
							if (BuildingDetectFlag1(B, C)){
								if (BuildingDetectFlag3(B) || BuildingDetectFlag3(C)){
									if (IsSamePic(crop1, crop2)){
										pictures.push_back(C);
										rects.push_back(r0);
									}
								}
							}
						}
					}
				}
			}
			else{
				
			}
		}
	}
	
	/*coutIsSamePic.close();*/
	return pictures;
}

bool CwjjcDlg::BuildingDetectFlag1(const Mat& img1, const Mat& img2){
	Mat src1 = img1.clone();
	Mat src2 = img2.clone();
	resize(src1, src1, Size(300, 150));
	resize(src2, src2, Size(300, 150));

	float length1, length2;

	int linenumber1 = finalProcessGetLineNumbers(src1, length1);
	int linenumber2 = finalProcessGetLineNumbers(src2, length2);

	if (length2 > 14 || length1 > 14 && (length1 + length2)> 24) return true;
	//if ((length1 + length2) / 2 > 12 && (abs(length1 - length2) > 5 || (abs(length1 - length2) > 1&&(linenumber1 + linenumber2) / 2>70))) return true;
	return false;
}

int  CwjjcDlg::finalProcessGetLineNumbers(const Mat& img, float& length){
	Mat src = img.clone();
	Mat gray_img;
	cvtColor(src, gray_img, CV_BGR2GRAY);
	//imshow("xx", gray_img);
	Ptr<LineSegmentDetector> ls = createLineSegmentDetector(LSD_REFINE_ADV);
	vector<Vec4f> lines_std;
	//��� �߶β�������lines_std��
	ls->detect(gray_img, lines_std);
	Mat drawSrc = src.clone();
	vector<Vec4f> candidatelines;
	length = 0.0;

	int length_average = 0.0;
	for (int i = 0; i < lines_std.size(); i++){
		Vec4f first = lines_std[i];
		float firstD = getDistance(first);
		for (int j = 0; j < lines_std.size(); j++){
			if (i == j) continue;
			Vec4f second = lines_std[j];
			float secondD = getDistance(second);
			//��� ���߶�first�е㣬second�е�Ϊ�˵���߶�
			Vec4f disTwo = Vec4f((first[2] + first[0]) / 2, (first[3] + first[1]) / 2
				, (second[2] + second[0]) / 2, (second[3] + second[1]) / 2);
			float rate = secondD / firstD;
			//��disTwo�߶γ���С�ڴ�����߶�-��first��1.2��ʱ����������   ��Ϊ�жϵ�Ŀ����Ϊ�˼�⿿��first��Χ���߶�
			if (getDistance(disTwo) <firstD || getDistance(disTwo) < secondD){
				float crossA = findOrthoLine(first, second);//����first��second֮��ļн�
				if (crossA>30 && (rate > 0.4&rate < 2.5)){ //����Ƕȴ���67.5�ȣ�����  ��Ϊ�������������ɴ�ֱ��ˮƽ�߶ι��ɣ����һ������ֻ��ˮƽ�ģ����п���������
					candidatelines.push_back(first);
					length_average += firstD;
					break;
				}
			}
		}
	}
	if (candidatelines.size() == 0) return 0;
	length_average /= candidatelines.size();
	int final_number = 0;
	for (int i = 0; i < candidatelines.size(); i++){
		Vec4f vline = candidatelines[i];
		float vlength = getDistance(vline);
		if (vlength < 0.8*length_average) continue;
		length += vlength;
		final_number++;
	}
	length /= final_number == 0 ? 1 : final_number;
	return final_number;
}
bool  CwjjcDlg::getTheSimilarityBySSIM(const Mat& src1, const Mat& src2){
	Scalar SSIM1 = getSSIM(src1, src2);
	int scale = (SSIM1.val[2] + SSIM1.val[1] + SSIM1.val[0]) / 3 * 100;
	if (scale >45) return false;
	return true;
}
Scalar CwjjcDlg::getSSIM(const Mat&  inputimage1, const Mat& inputimage2)
{
	Mat i1 = inputimage1.clone();
	Mat i2 = inputimage2.clone();
	const double C1 = 6.5025, C2 = 58.5225;
	int d = CV_32F;
	Mat I1, I2;
	i1.convertTo(I1, d);
	i2.convertTo(I2, d);
	Mat I2_2 = I2.mul(I2);
	Mat I1_2 = I1.mul(I1);
	Mat I1_I2 = I1.mul(I2);
	Mat mu1, mu2;
	GaussianBlur(I1, mu1, Size(11, 11), 1.5);
	GaussianBlur(I2, mu2, Size(11, 11), 1.5);
	Mat mu1_2 = mu1.mul(mu1);
	Mat mu2_2 = mu2.mul(mu2);
	Mat mu1_mu2 = mu1.mul(mu2);
	Mat sigma1_2, sigma2_2, sigma12;
	GaussianBlur(I1_2, sigma1_2, Size(11, 11), 1.5);
	sigma1_2 -= mu1_2;
	GaussianBlur(I2_2, sigma2_2, Size(11, 11), 1.5);
	sigma2_2 -= mu2_2;
	GaussianBlur(I1_I2, sigma12, Size(11, 11), 1.5);
	sigma12 -= mu1_mu2;
	Mat t1, t2, t3;
	t1 = 2 * mu1_mu2 + C1;
	t2 = 2 * sigma12 + C2;
	t3 = t1.mul(t2);
	t1 = mu1_2 + mu2_2 + C1;
	t2 = sigma1_2 + sigma2_2 + C2;
	t1 = t1.mul(t2);
	Mat ssim_map;
	divide(t3, t1, ssim_map);
	Scalar mssim = mean(ssim_map);
	return mssim;
}
bool CwjjcDlg::MatchingMethod1(Mat templ, Mat img)
{

	vector<Mat>vimg, vtemp;
	int border;
	border = pow((int)sqrt(templ.cols*templ.rows) / 60, 2);

	if (border < 1)
		border = 1;

	for (int i = 0; i < sqrt(border); i++){
		for (int j = 0; j < sqrt(border); j++){
			vimg.push_back(CutImage(img, i, i + 1, j, j + 1, sqrt(border)));
			vtemp.push_back(CutImage(templ, i, i + 1, j, j + 1, sqrt(border)));
		}
	}
	Mat result;
	/// Դͼ����ʾ  
	Mat img_display;
	img.copyTo(img_display);
	int flag = 0;
	for (int i = 0; i < border; i++){
		/// �����������  
		int result_cols = vimg[i].cols - vtemp[i].cols + 1;
		int result_rows = vimg[i].rows - vtemp[i].rows + 1;

		result.create(result_rows, result_cols, CV_32FC1);

		/// ƥ��������  
		matchTemplate(vimg[i], vtemp[i], result, CV_TM_SQDIFF_NORMED);
		//normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());

		/// ���ػ�minMaxLoc�����ƥ��  
		double minVal; double maxVal; Point minLoc; Point maxLoc;
		Point matchLoc;

		minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());

		/// For SQDIFF and SQDIFF_NORMED, the best matches are lower values. For all the other methods, the higher the better  
		matchLoc = minLoc;

		/// ��������õ���ʲô 
		if (minVal < 0.10)
		{
			//rectangle(img_display, matchLoc, Point(matchLoc.x + vtemp[i].cols, matchLoc.y + vtemp[i].rows), Scalar::all(0), 3, 4, 0);//����
			//rectangle(result, matchLoc, Point(matchLoc.x + vtemp[i].cols, matchLoc.y + vtemp[i].rows), Scalar::all(0), 3, 4, 0);
			flag++;
		}
		/*	imshow("Source Image", img_display);
		imshow("Result window", result);
		waitKey(500);*/
	}

	if ((double)flag / border >= 0.13){
		return false;
	}
	else{ return true; }
}
bool CwjjcDlg::MatchingMethod2(Mat src1, Mat src2)
{
	int length_x = src1.cols / 4;
	int length_y = src2.rows / 4;
	float x2 = 0.0;
	for (int i = 0; i < 3; i++){
		for (int j = 0; j < 3; j++){
			Mat tmp = src1(Rect(j*length_x, i*length_y, length_x * 2, length_y * 2));
			Mat result;
			/// ƥ��������  
			matchTemplate(tmp, src2, result, CV_TM_CCORR_NORMED);
			//normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());

			/// ���ػ�minMaxLoc�����ƥ��  
			double minVal; double maxVal; Point minLoc; Point maxLoc;
			Point matchLoc;

			minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());
			//float x1 = sqrt(pow(minLoc.x - length_x, 2) + pow(minLoc.y - length_y, 2));
			x2 += sqrt(pow(maxLoc.x - j*length_x, 2) + pow(maxLoc.y - i*length_y, 2));
		}
	}
	if (x2 / 9 <= 1) return false;
	return true;
}


Mat CwjjcDlg::CutImage(Mat templ1, int i, int j, int k, int l, int border)
{
	Mat templ2 = templ1(Range(templ1.rows * i / border, templ1.rows * j / border), Range(templ1.cols * k / border, templ1.cols * l / border));//����
	//rectangle(templ1, Point(templ1.cols * k / border, templ1.rows * i / border), Point(templ1.cols * l / border, templ1.rows * j / border), Scalar::all(0), 3, 4, 0);//����
	return templ2;
}

//bool CwjjcDlg::colorSelection(Mat src, double Variance, int BGRLimit){
//
//	Mat dst = Mat::zeros(src.size(), src.type());
//	for (int i = 0; i < dst.rows; i++){
//		const uchar* inData = src.ptr<uchar>(i);
//		uchar* outData = dst.ptr<uchar>(i);
//		for (int j = 0; j < dst.cols; j++){
//			int B = *inData++;
//			int G = *inData++;
//			int R = *inData++;
//			int average = (B + G + R) / 3;
//			double variance = 1.0 / 3 * (pow(average - B, 2) + pow(average - G, 2) + pow(average - R, 2));
//			if (variance<Variance && B>BGRLimit && G>BGRLimit && R>BGRLimit){
//				*outData++ = B;
//				*outData++ = G;
//				*outData++ = R;
//			}
//			else{
//				*outData++ = 0;
//				*outData++ = 0;
//				*outData++ = 0;
//			}
//		}
//	}
//	cvtColor(dst, dst, CV_BGR2GRAY);
//	threshold(dst, dst, BGRLimit, 255, THRESH_BINARY);
//	double limit = mean(dst)[0] * dst.cols*dst.rows / 255;//����ͼ�������ص�����
//	limit = limit / (dst.cols*dst.rows);
//
//	if (limit>0.05){
//		return false;
//	}
//	else{
//		return true;
//	}
//}

bool CwjjcDlg::colorSelection(Mat src){

	/*imshow("change",src);
	waitKey(0);*/
	Mat dst = Mat::zeros(src.size(), src.type());
	int black = 0;
	for (int i = 0; i < dst.rows; i++){
		const uchar* inData = src.ptr<uchar>(i);
		uchar* outData = dst.ptr<uchar>(i);
		for (int j = 0; j < dst.cols; j++){
			int B = *inData++;
			int G = *inData++;
			int R = *inData++;
			int average = (B + G + R) / 3;
			if (average == 0)
			{
				black++;
			}
		}
	}
	double limit = 0;
	limit = 1.0*black / (dst.cols*dst.rows);

	if (limit > 0.1){
		return false;
	}
	else{
		return true;
	}
}

//bool CwjjcDlg::processImage(MSAC &msac, int numVps, cv::Mat &imgGRAY, cv::Mat &outputImg)
//{
//	bool flag;
//	int nStructElementSize = 4; //�ṹԪ��(�ں˾���)�ĳߴ�
//
//	Mat midImage, cannyImage;
//	Mat element = getStructuringElement(MORPH_RECT, Size(2 * nStructElementSize + 1, 2 * nStructElementSize + 1), Point(nStructElementSize, nStructElementSize));
//
//	//������̬ѧ����
//	morphologyEx(imgGRAY, midImage, MORPH_OPEN, element);
//	/*Canny(midImage, cannyImage, 200, 80, 3);*/
//
//	//LSD���
//	Mat linesImage;
//	Ptr<LineSegmentDetector> ls1 = createLineSegmentDetector(LSD_REFINE_STD);
//
//	vector<Vec4i> lines_std1;
//	// Detect the lines
//	ls1->detect(imgGRAY, lines_std1);
//	//lines cut
//	vector<Vec4i> lines_lsd1;
//
//	for (size_t i = 0; i < lines_std1.size(); i++)
//	{
//		double length = sqrt((lines_std1[i][0] - lines_std1[i][2])*(lines_std1[i][0] - lines_std1[i][2])
//			+ (lines_std1[i][1] - lines_std1[i][3])*(lines_std1[i][1] - lines_std1[i][3]));
//		if (length >= 10)//���ж�С�߶Σ���ֵһ��Ϊ����4��ƽ��
//		{
//			lines_lsd1.push_back(lines_std1[i]);
//		}
//	}
//
//	// Show found lines
//	Mat drawnLines1(imgGRAY);
//	ls1->drawSegments(drawnLines1, lines_lsd1);
//	/*imshow("δ����", drawnLines1);*/
//
//
//	Ptr<LineSegmentDetector> ls2 = createLineSegmentDetector(LSD_REFINE_STD);
//	vector<Vec4i> lines_std2;
//	vector<Vec4i> lines_lsd;//������ֱ��
//	// Detect the lines
//	ls2->detect(midImage, lines_std2);
//	//��lines_std2���д�����
//
//	vector<vector<cv::Point> > lineSegments;
//	vector<cv::Point> aux;
//
//	for (size_t i = 0; i < lines_std2.size(); i++)
//	{
//		double length = sqrt((lines_std2[i][0] - lines_std2[i][2])*(lines_std2[i][0] - lines_std2[i][2])
//			+ (lines_std2[i][1] - lines_std2[i][3])*(lines_std2[i][1] - lines_std2[i][3]));
//		if (/*length >= 25&&*/length > imgGRAY.rows / 10)//���ж�С�߶Σ���ֵһ��Ϊ����4��ƽ��
//		{
//
//			lines_lsd.push_back(lines_std2[i]);
//
//			//Ϊ�����ʧ����в�������
//			Point pt1, pt2;
//			pt1.x = lines_std2[i][0];
//			pt1.y = lines_std2[i][1];
//			pt2.x = lines_std2[i][2];
//			pt2.y = lines_std2[i][3];
//			line(outputImg, pt1, pt2, CV_RGB(0, 0, 0), 2);//��ʾ���м�⵽����
//
//			// Store into vector of pairs of Points for msac
//			aux.clear();
//			aux.push_back(pt1);
//			aux.push_back(pt2);
//			lineSegments.push_back(aux);
//
//		}
//	}
//	//��ʾlines_lsdֱ��
//	Mat drawnLines2(imgGRAY);
//	ls2->drawSegments(drawnLines2, lines_lsd);
//	//imshow("������+LSD", drawnLines2);
//
//
//
//	// 3.Multiple vanishing points
//	std::vector<cv::Mat> vps;			// vector of vps: vps[vpNum], with vpNum=0...numDetectedVps
//	std::vector<std::vector<int> > CS;	// index of Consensus Set for all vps: CS[vpNum] is a vector containing indexes of lineSegments belonging to Consensus Set of vp numVp
//	std::vector<int> numInliers;
//
//	std::vector<std::vector<std::vector<cv::Point> > > lineSegmentsClusters;
//
//	// Call msac function for multiple vanishing point estimation
//	msac.multipleVPEstimation(lineSegments, lineSegmentsClusters, numInliers, vps, numVps);
//	//��ӡ��ʧ����Ϣ

//
//	if (vps.size() >= 3 && lineSegments.size()>10)//ͨ����ʧ��������߶γ��Ƚ��н������⣡����
//	{
//
//		//std::cout << "�����Ϊ��  " << "���ڽ�����" << endl;
//		flag = true;
//	}
//	else
//	{
//
//		//	std::cout << "�����Ϊ��  " << "�����ڽ�����" << endl;
//		flag = false;
//	}
//
//	for (int v = 0; v<vps.size(); v++)
//	{
//		printf("VP %d (%.3f, %.3f, %.3f)", v, vps[v].at<float>(0, 0), vps[v].at<float>(1, 0), vps[v].at<float>(2, 0));
//		fflush(stdout);
//		double vpNorm = cv::norm(vps[v]);
//		if (fabs(vpNorm - 1) < 0.001)
//		{
//			printf("(INFINITE)");
//			fflush(stdout);
//		}
//		printf("\n");
//	}
//
//	// 4.Draw line segments according to their cluster
//	msac.drawCS(outputImg, lineSegmentsClusters, vps);
//	return flag;
//}

bool CwjjcDlg::BuildingDetectFlag2(Mat src){
	if (!src.data) {
		cout << "����ͼ�����" << endl;
		return 1;
	}
	if (src.cols >= 1200)
		pyrDown(src, src);
	//pyrDown(src, src);
	//imshow("xx", src);
	Mat gray_img;
	cvtColor(src, gray_img, CV_BGR2GRAY);
	//Canny(gray_img, gray_img, 50, 200);
	//ʹ��LSD�㷨
	Ptr<LineSegmentDetector> ls = createLineSegmentDetector(LSD_REFINE_ADV);
	vector<Vec4i> lines_std;
	//��� �߶β�������lines_std��
	ls->detect(gray_img, lines_std);
	//Mat drawSrc = src.clone();
	//cout << lines_std.size() << endl;
	//for (int i = 0; i < lines_std.size(); i++){	//��ʾLSDֱ�߼��
	//	Vec4i vline = lines_std[i];
	//	line(drawSrc, Point(vline[0], vline[1]), Point(vline[2], vline[3]), Scalar(0, 255, 0), 2, 8);
	//}
	//namedWindow("xx", 0);
	//imshow("xx", drawSrc);
	vector<Vec4i> candidatelines;
	for (int i = 0; i < lines_std.size(); i++){
		Vec4i first = lines_std[i];
		float firstD = getDistance(first);
		if (firstD <15) continue;//���first�߶εĳ��ȣ�����С��5pix���ų�

		//float firstvA = getAngle(first);

		//float first_sA = get0To90A(firstvA);//���first�߶εĵĽǶȣ�0-90)
		//if (first_sA>75)
		//{
		//	//�Ƕȴ���45�ȵı��Ϊ��ɫ�����Ҳ���Ҫ����������жϣ�ֱ�ӱ�������Ϊ���Է���ͼ���߶μ��ʱ����ֱ�߶��������Ŀ����Ը�С
		//	//���߶���ӵ�lines_color������
		//	candidatelines.push_back(first);
		//	continue;
		//}
		bool match = false;
		int count = 0;
		for (int j = 0; j < lines_std.size(); j++){
			if (i == j) continue;
			Vec4f second = lines_std[j];
			float SecondD = getDistance(second);
			//��� ���߶�first���˵㣬second���˵���߶�
			Vec4f disOne = Vec4f(first[0], first[1], second[0], second[1]);
			Vec4f disTwo = Vec4f(first[0], first[1], second[2], second[3]);
			Vec4f disThree = Vec4f(first[2], first[3], second[0], second[1]);
			Vec4f disFour = Vec4f(first[2], first[3], second[2], second[3]);
			//��disTwo�߶γ���С�ڴ�����߶�-��first��1.2��ʱ����������   ��Ϊ�жϵ�Ŀ����Ϊ�˼�⿿��first��Χ���߶�
			float length = firstD<SecondD ? firstD : SecondD;
			length = 0.5 * length;
			if (getDistance(disOne) < length | getDistance(disTwo) < length | getDistance(disThree) < length | getDistance(disFour) < length){

				double fsA = findOrthoLine(first, second);//����first��second֮��ļн�
				if (fsA> 45)  //����Ƕȴ���67.5�ȣ�����  ��Ϊ�������������ɴ�ֱ��ˮƽ�߶ι��ɣ����һ������ֻ��ˮƽ�ģ����п���������
					match = true;
				count++;//count��Ŀ����Ϊ�˼��� first��Χ�м�������Ƕȴ���67.5�ȵ�
			}
		}
		if (count < 2)// ���first��Χֻ��2���������� ������жϻ���Ҫ��֤
			match = false;
		if (match == true)
			//���ս��߶���ӵ�lines_color������
			candidatelines.push_back(first);
	}

	int vlengths = candidatelines.size();
	for (int i = 0; i < vlengths; i++){
		Vec4i vline = candidatelines[i];
		line(src, Point(vline[0], vline[1]), Point(vline[2], vline[3]), Scalar(0, 255, 0), 2, 8);
	}
	/*namedWindow("src", 0);
	imshow("src", src);*/
	if (candidatelines.size()>0)return true; else{
		return false;
	}
}



bool CwjjcDlg::IsGreen(const Mat inputImage){
	Mat hsvImage;
	int count = 0;
	cvtColor(inputImage, hsvImage, CV_BGR2HSV);
	double H = 0.0, S = 0.0, V = 0.0;
	for (int i = 0; i<hsvImage.rows; i++)
	{
		for (int j = 0; j<hsvImage.cols; j++)
		{
			H = hsvImage.at<Vec3b>(i, j)[0];
			S = hsvImage.at<Vec3b>(i, j)[1];
			V = hsvImage.at<Vec3b>(i, j)[2];
			if ((H >= 25 && H < 85) && (S>40) && (V>0))		//&&(H >= 80 && H < 100)
			{
				count++;
			}
		}
	}

	if ((double)count / (hsvImage.rows*hsvImage.cols) > 0.20){ return true; }
	else{ return false; }
}

vector<Rect> CwjjcDlg::HaarVehicle3(Mat imgnow, double scale){
	vector<Rect> RectVechicle;
	RectVechicle.clear();
	IplImage frame = imgnow;   //mat��IplImage
	static CvMemStorage* storage = 0;
	String car_detection = "haarcascade_frontalface_alt.xml";
	IplImage *frame_copy = 0;
	Mat image;
	CascadeClassifier cascade;
	if (!cascade.load(car_detection)){
		/*cerr << "ERROR: Could not load classifier cascade" << endl;*/
		return RectVechicle;
	}
	storage = cvCreateMemStorage(0);
	int i = 0, num = 0;
	if (!&frame)	{
		return RectVechicle;
	}
	if (!frame_copy)
		frame_copy = cvCreateImage(cvSize((&frame)->width, (&frame)->height), IPL_DEPTH_8U, (&frame)->nChannels);
	if ((&frame)->origin == IPL_ORIGIN_TL)
		cvCopy(&frame, frame_copy, 0);
	else
		cvFlip(&frame, frame_copy, 0);
	vector<Rect> carrects = detection2(frame_copy, cascade, scale);
	cvReleaseImage(&frame_copy);
	//cvReleaseImage(&frame);
	bool flag = false;
	/*int nMaxLeft = 0;
	int nMaxTop = 0;
	int nMinRight = 0;
	int nMinBottom = 0;*/
	for (int i = 0; i < carrects.size(); i++)
	{
		RectVechicle.push_back(carrects[i]);
		////���������ο��ܵ��ཻ���εı߽�  
		//nMaxLeft = rect.x >= carrects[i].x ? rect.x : carrects[i].x;
		//nMaxTop = rect.y >= carrects[i].y ? rect.y : carrects[i].y;
		//nMinRight = (rect.x + rect.width) <= (carrects[i].x + carrects[i].width) ? (rect.x + rect.width) : (carrects[i].x + carrects[i].width);
		//nMinBottom = (rect.y + carrects[i].height) <= (carrects[i].y + carrects[i].height) ? (rect.y + rect.height) : (carrects[i].y + carrects[i].height);
		//// �ж��Ƿ��ཻ  
		//if (!(nMaxLeft > nMinRight || nMaxTop > nMinBottom))
		//{
		//	flag=true;
		//}

		//if (rect.contains(CvPoint(carrects[i].x + carrects[i].width / 2, carrects[i].y + carrects[i].height / 2)))
		//{
		//	flag = true;
		//}
	}
	return RectVechicle;
}

vector<Rect> CwjjcDlg::detection2(IplImage *img1, CascadeClassifier& cascade, double scale){
	bool num = false;
	Mat img = cvarrToMat(img1);
	/*pyrDown(img, img);*/
	int i = 0;
	double t = 0;
	vector<Rect> cars;
	Mat gray, smallImg(cvRound(img.rows / scale), cvRound(img.cols / scale), CV_8UC1);//��СͼƬ���ӿ����ٶ�
	const static Scalar colors[] = { CV_RGB(0, 0, 255),
		CV_RGB(0, 128, 255),
		CV_RGB(0, 255, 255),
		CV_RGB(0, 255, 0),
		CV_RGB(255, 128, 0),
		CV_RGB(255, 255, 0),
		CV_RGB(255, 0, 0),
		CV_RGB(255, 0, 255) };//��ͬ����ɫ���ʾ��ͬ�ĳ���

	cvtColor(img, gray, CV_BGR2GRAY);//����ʹ�õ���haar���������Զ��ǻ��ڻҶ�ͼ��ģ�ת���ɻҶ�ͼ��
	resize(gray, smallImg, smallImg.size(), 0, 0, INTER_LINEAR);//���ߴ���С��1/scale�������Բ�ֵ
	equalizeHist(smallImg, smallImg);//ֱ��ͼ����

	t = (double)cvGetTickCount();//�����㷨ִ��ʱ��
	//��⳵��
	//detectMultiScale������samllImg��ʾ����Ҫ��������ͼ��ΪsmallImg��cars��ʾ��⵽�ĳ���Ŀ������
	//1.1��ʾÿ��ͼ��ߴ���ٵı���Ϊ1.1�� 2��ʾûһ��Ŀ������Ҫ����⵽3�β��������Ŀ�꣨��Ϊ��Χ�����غͲ�ͬ�Ĵ��ڴ�С�����Լ�⵽������
	//CV_HAAR_SCALE_IMAGE��ʾ�������ŷ���������⣬�������ŵ�ͼ��Size��20,20��ΪĿ�����С�ߴ�
	cascade.detectMultiScale(smallImg, cars,
		1.1, 1, 0
		| CV_HAAR_DO_CANNY_PRUNING
		,
		Size(20, 20));
	t = (double)cvGetTickCount() - t;//������㷨ִ��ʱ��
	/*printf("detection time = %g ms\n", t / ((double)cvGetTickFrequency()*1000.));*/
	for (vector<Rect>::const_iterator r = cars.begin(); r != cars.end(); r++, i++){
		Mat smallImgROI;
		Point center;
		Scalar color = colors[i % 8];
		/*int radius;
		center.x = cvRound((r->x + r->width*0.5)*scale);//��ԭ��ԭ���Ĵ�С
		center.y = cvRound((r->y + r->height*0.5)*scale);
		radius = cvRound((r->width + r->height)*0.25*scale);
		circle(img, center, radius, color, 3, 8, 0); */

		rectangle(img, cvPoint(cvRound(r->x*scale),
			cvRound(r->y*scale)),
			cvPoint(cvRound((r->x + r->width - 1)*scale),
			cvRound((r->y + r->height - 1)*scale)),
			color, 3, 8, 0);
		num = true;
	}
	/*cv::imshow("result", img);*/
	return cars;
}

bool CwjjcDlg::CarDetection(Rect rect, vector<Rect> rects)
{
	for (int i = 0; i < rects.size(); i++)
	{
		if (rect.contains(CvPoint(rects[i].x + rects[i].width / 2, rects[i].y + rects[i].height / 2)))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}


bool CwjjcDlg::HaarVehicleD(Mat &image, Rect &rect, double scale)
{
	bool flag = false;
	/*for (int i = 0; i < cars.size(); i++)
	{
	float overlap = bbOverlap(cars[i], rect);
	if (overlap>0.3)
	{
	flag = true;
	}
	}*/
	int a, b, c, d;
	/*if ((rect.y - rect.height*scale)>0)
	{
	a = rect.y - rect.height*scale;
	}
	else
	{
	a = 0;
	}*/
	if ((rect.y - rect.height / scale)>image.rows || (rect.y + rect.height / scale * (scale + 1))<0 || (rect.x - rect.width / scale)>image.cols || (rect.x + rect.width / scale * (scale + 1))<0)
	{
		return  false;
	}
	if ((rect.y - rect.height / scale)>0)
	{
		a = rect.y - rect.height / scale;
	}
	else
	{
		a = 0;
	}

	if ((rect.y + rect.height / scale * (scale + 1))<image.rows)
	{
		b = rect.y + rect.height / scale * (scale + 1);
	}
	else
	{
		b = image.rows;
	}
	/*if ((rect.x - rect.width*scale)>0)
	{
	c = rect.x - rect.width*scale;
	}
	else
	{
	c = 0;
	}*/
	if ((rect.x - rect.width / scale)>0)
	{
		c = rect.x - rect.width / scale;
	}
	else
	{
		c = 0;
	}
	if ((rect.x + rect.width / scale * (scale + 1))<image.cols)
	{
		d = rect.x + rect.width / scale * (scale + 1);
	}
	else
	{
		d = image.cols;
	}
	Mat imgcut = image(Range(a, b), Range(c, d)).clone();
	/*imshow("ԭͼ�Ŵ�����", imgcut);
	waitKey(0);*/
	/*Mat imgcut = image(rect);*/
	IplImage *frame = (IplImage *)&IplImage(imgcut);
	IplImage *frame_copy = 0;
	CvHaarClassifierCascade *cascade = (CvHaarClassifierCascade*)cvLoad("cars.xml");
	CvMemStorage *storage = cvCreateMemStorage(0);
	if (!frame_copy)
		frame_copy = cvCreateImage(cvSize(frame->width, frame->height), IPL_DEPTH_8U, 1);
	cvCvtColor(frame, frame_copy, CV_BGR2GRAY); // ת���ɻҶ�ͼ����߼���ٶ�
	CvSize img_size = cvGetSize(frame);
	CvSeq *object = cvHaarDetectObjects(
		frame_copy,
		cascade,
		storage,
		1.1,
		1,
		0,
		cvSize(0, 0),
		img_size
		);

	/*std::cout << "Total: " << object->total << " cars detected." << std::endl;*/
	for (int i = 0; i < (object ? object->total : 0); i++)
	{
		/*CvRect *r = (CvRect*)cvGetSeqElem(object, i);*/
		/*cvRectangle(frame,
		cvPoint(r->x, r->y),
		cvPoint(r->x + r->width, r->y + r->height),
		CV_RGB(255, 0, 0), 2, 8, 0);*/
		flag = true;
	}
	cvReleaseImage(&frame_copy);
	cvReleaseHaarClassifierCascade(&cascade);
	cvReleaseMemStorage(&storage);

	//cvReleaseImage(&frame_copy);
	//IplImage frame = imgcut;   //mat��IplImage
	//static CvMemStorage* storage = 0;
	//String car_detection = "cars.xml";
	//IplImage *frame_copy = 0;
	//Mat image;
	//CascadeClassifier cascade;
	//if (!cascade.load(car_detection)){
	//	cerr << "ERROR: Could not load classifier cascade" << endl;
	//	return 0;
	//}
	//storage = cvCreateMemStorage(0);
	//int i = 0, num = 0;
	//if (!&frame)	{
	//	return true;   //�ǳ����߸���
	//}
	//if (!frame_copy)
	//	frame_copy = cvCreateImage(cvSize((&frame)->width, (&frame)->height), IPL_DEPTH_8U, (&frame)->nChannels);
	//if ((&frame)->origin == IPL_ORIGIN_TL)
	//	cvCopy(&frame, frame_copy, 0);
	//else
	//	cvFlip(&frame, frame_copy, 0);
	//bool flag = detection(frame_copy, cascade, scale);
	//cvReleaseImage(&frame_copy);
	/*bool flag = false;*/
	/*int nMaxLeft = 0;
	int nMaxTop = 0;
	int nMinRight = 0;
	int nMinBottom = 0;*/
	//for (int i = 0; i < carrects.size(); i++)
	//{
	//	////���������ο��ܵ��ཻ���εı߽�  
	//	//nMaxLeft = rect.x >= carrects[i].x ? rect.x : carrects[i].x;
	//	//nMaxTop = rect.y >= carrects[i].y ? rect.y : carrects[i].y;
	//	//nMinRight = (rect.x + rect.width) <= (carrects[i].x + carrects[i].width) ? (rect.x + rect.width) : (carrects[i].x + carrects[i].width);
	//	//nMinBottom = (rect.y + carrects[i].height) <= (carrects[i].y + carrects[i].height) ? (rect.y + rect.height) : (carrects[i].y + carrects[i].height);
	//	//// �ж��Ƿ��ཻ  
	//	//if (!(nMaxLeft > nMinRight || nMaxTop > nMinBottom))
	//	//{
	//	//	flag = true;
	//	//}
	//	//�����������ص���

	//	if (rect.contains(CvPoint(carrects[i].x + carrects[i].width/2, carrects[i].y + carrects[i].height/2)))
	//	{
	//	flag = true;
	//	}
	//}
	return flag;
}

bool CwjjcDlg::IsSamePic(Mat &image1, Mat &image2)
{
	
	int m, n1, n2;
	Mat img, img1, img2;
	imwrite("image1.jpg", image1);
	imwrite("image2.jpg", image2);

	//colorImg1.convertTo(img1, CV_32FC1);
	//colorImg2.convertTo(img2, CV_32FC1);
	/*img1 /= 255.0;
	img2 /= 255.0;*/
	Ptr<cv::xfeatures2d::SURF> surf;      //������ʽ��2�еĲ�һ��  
	surf = cv::xfeatures2d::SURF::create(400);
	Mat descriptors1, descriptors2;
	vector<KeyPoint>key1, key2;
	vector< DMatch > good_matches;
	surf->detectAndCompute(image1, Mat(), key1, descriptors1);
	surf->detectAndCompute(image2, Mat(), key2, descriptors2);
	n1 = key1.size();
	n2 = key2.size();
	if (n1 == 0 || n2 == 0)
	{
		return FALSE;
	}
	vector<KeyPoint> keygood1, keygood2;
	/*Mat descriptors1t = Mat::zeros(descriptors1.size(), descriptors1.type());
	Mat descriptors2t = Mat::zeros(descriptors2.size(), descriptors2.type());*/

	/*int count1 = getSelectionDescriptors(key1, descriptors1, keygood1, descriptors1t, image1);
	int count2 = getSelectionDescriptors(key2, descriptors2, keygood2, descriptors2t, image2);*/
	/*if (count1 == 0 || count2 == 0)
	{
		return true;
	}*/
	
	/*Mat descriptorsgood1 = descriptors1t(Rect(0, 0, 64, count1)).clone();
	Mat descriptorsgood2 = descriptors2t(Rect(0, 0, 64, count2)).clone();*/

	/*GetMatches(descriptorsgood1, descriptorsgood2, good_matches);*/
	GetMatches(descriptors1, descriptors2, good_matches,0.9);


	//cout << "good_matches" << good_matches.size() << endl;

	m = good_matches.size(); //�ж���ͼ�����ƶ�
	if (m == 0)
	{
		return false;
	}
	
	//cout << "n1:"<<n1 << "-" <<"n2:"<< n2 << endl;
	//cout <<"rate1"<< ((double)m / ((n1 > n2) ? n1 : n2)) << endl;
	
	
	double cha = (double)((n1 < n2) ? n1 : n2) / ((n1 > n2) ? n1 : n2);
	
	if (cha<0.4)
	{
		
		return FALSE;
	
	}
	Mat matchImage;
	/*drawMatches(image1, key1, image2, key2, good_matches, matchImage);*/

	vector<KeyPoint> R_keypoint01, R_keypoint02;
	for (size_t i = 0; i<good_matches.size(); i++)
	{
		R_keypoint01.push_back(key1[good_matches[i].queryIdx]);
		R_keypoint02.push_back(key2[good_matches[i].trainIdx]);
		//�����仰����⣺R_keypoint1��Ҫ�洢img01������img02ƥ��������㣬
		//matches�д洢����Щƥ���Ե�img01��img02������ֵ
	}

	vector<Point2f>p01, p02;
	for (size_t i = 0; i<good_matches.size(); i++)
	{
		p01.push_back(R_keypoint01[i].pt);
		p02.push_back(R_keypoint02[i].pt);
	}
	vector<uchar> RansacStatus;
	Mat Fundamental = findFundamentalMat(p01, p02, RansacStatus, FM_RANSAC);

	vector<KeyPoint> RR_keypoint01, RR_keypoint02;
	vector<DMatch> RR_matches;            //���¶���RR_keypoint ��RR_matches���洢�µĹؼ����ƥ�����
	int index = 0;
	for (size_t i = 0; i<good_matches.size(); i++)
	{
		if (good_matches.size() != RansacStatus.size())
		{
			return true;
		}
		if (RansacStatus[i] != 0)
		{
			RR_keypoint01.push_back(R_keypoint01[i]);
			RR_keypoint02.push_back(R_keypoint02[i]);
			good_matches[i].queryIdx = index;
			good_matches[i].trainIdx = index;
			RR_matches.push_back(good_matches[i]);
			index++;
		}
	}
	Mat img_RR_matches;
	
	drawMatches(image1, RR_keypoint01, image2, RR_keypoint02, RR_matches, img_RR_matches);
	imwrite("img_RR_matches.jpg", img_RR_matches);
	//Mat MatchImageR;
	//drawMatches(image1, key1, image2, key2, RR_matches, MatchImageR);
	//imwrite("MatchImageR.jpg", MatchImageR);
	int k = RR_matches.size();
	
	double rate2 = (double)k / ((n1 > n2) ? n1 : n2);
	
	
	if (rate2 < 0.2)
	{
		return true;
	}
	else
	{
		return false;
	}
	//cout << "m:" << m << endl;
	//cout << "k:"<<k << endl;
	//cout << "rate2:" << ((double)k/ ((n1 > n2) ? n1 : n2)) << endl;
	//imwrite("������ƥ����.jpg", img_RR_matches);

	//imwrite("matchImage.jpg", matchImage);
	//imwrite("image1.jpg", image1);
	//imwrite("image2.jpg", image2);


	//if ((double)m / ((n1 > n2) ? n1 : n2)>0.96){		//����һ����ͼƬ��ֹH��ѭ��
	//	return 3;
	//}

	//std::vector<Point2f> obj;
	//std::vector<Point2f> scene;
	//if (good_matches.size() == 0)
	//{
	//	return true;
	//}
	//for (size_t i = 0; i < good_matches.size(); i++)
	//{
	//	obj.push_back(key1[good_matches[i].queryIdx].pt);
	//	scene.push_back(key2[good_matches[i].trainIdx].pt);
	//}
	//Mat imageTransform, BlackEdgeTransform;
	//Mat H = findHomography(obj, scene, RANSAC);      //Ѱ��ƥ���ͼ�� 
	//cout << H << endl;
	//if (H.cols != 3 || H.rows != 3)
	//{
	//	return true;
	//}
	//cv::Mat me = cv::Mat::eye(cv::Size(3, 3), CV_32FC1);
	//cv::Mat mz = cv::Mat::zeros(cv::Size(image2.rows, image2.cols), CV_8UC1);
	////for (int i = 0; i < image2.rows; i+=10)
	////{
	////	for (int j = 0; j < image2.cols; j+=10)
	////	{
	////		mz.at<uchar>(i, j) = 255;
	////	}
	////}
	//warpPerspective(image1, imageTransform, H, Size(image2.cols, image2.rows));
	//imwrite("trans.jpg",image2);
	//imwrite("tranpast.jpg", image1);
	//imwrite("trannow.jpg", imageTransform);
	//imwrite("trannow.jpg", imageTransform);
	
}

void CwjjcDlg::PaintBlack(Mat test)
{
	//for (int row = 900; row < 1000; row++)
	//{
	//	for (int col = 1550; col < 1880; col++)
	//	{
	//		test.at<Vec3b>(row, col) = Vec3b(0, 0, 0);
	//	}
	//}

	for (int row = 48; row < 95; row++)
	{
		for (int col = 1058; col < 1845; col++)
		{
			test.at<Vec3b>(row, col) = Vec3b(0, 0, 0);
		}
	}

	//for (int row = 990; row < 1050; row++)
	//{
	//	for (int col = 1400; col < 1880; col++)
	//	{
	//		test.at<Vec3b>(row, col) = Vec3b(0, 0, 0);
	//	}
	//}

	for (int row = 1000; row <1055; row++)
	{
		for (int col = 65; col < 470; col++)
		{
			test.at<Vec3b>(row, col) = Vec3b(0, 0, 0);
		}
	}
}

void  CwjjcDlg::finalProcessGetLineAngles(const Mat& img, vector<int>& angles){
	Mat src = img.clone();
	Mat gray_img;
	cvtColor(src, gray_img, CV_BGR2GRAY);
	Ptr<LineSegmentDetector> ls = createLineSegmentDetector(LSD_REFINE_ADV);
	vector<Vec4f> lines_std;
	//��� �߶β�������lines_std��
	ls->detect(gray_img, lines_std);

	int length_average = 0.0;
	for (int i = 0; i < lines_std.size(); i++){
		Vec4f first = lines_std[i];
		float firstD = getDistance(first);
		length_average += firstD;
	}
	length_average /= lines_std.size();
	int final_number = 0;
	for (int i = 0; i < lines_std.size(); i++){
		Vec4f vline = lines_std[i];
		float vlength = getDistance(vline);
		if (vlength < 0.8*length_average) continue;
		int xangle = cvRound(getAngle(vline));
		xangle = xangle == 360 ? 359 : xangle;
		angles[xangle / 18]++;
	}
}
bool CwjjcDlg::BuildingDetectFlag3(const Mat& img){
	Mat src = img.clone();

	vector<int> angles(20);
	finalProcessGetLineAngles(img, angles);
	int sum = 0;
	for (int i = 0; i < 20; i++){
		sum += angles[i];
	}
	int sum_avage = sum / 20;
	for (int i = 0; i < 20; i++){
		sum += angles[i];
	}
	float stdA = 0.f;
	for (int i = 0; i < 20; i++){
		stdA += pow(sum_avage - angles[i], 2);
	}
	float xStd = sqrt(stdA / 20);
	return xStd>3.5f;
}
