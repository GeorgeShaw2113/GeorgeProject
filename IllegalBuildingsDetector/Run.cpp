#include"stdafx.h"
#include"IllegalBuildingsDetector.h"

void main()
{
	CwjjcDlg dection;
	cout << "正在进行违建判别，请勿关闭窗口！" << endl;
	dection.OnInitDialog();
	dection.PicJudgement();
	cout << "end";
} 