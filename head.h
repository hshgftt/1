#ifndef HEAD_H
#define HEAD_H
#define _CRT_SECURE_NO_WARNINGS
#include <WTypesbase.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cmath>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;
struct LINE {		//ֱ��
	Point2d l1;
	Point2d l2;
};
struct CIRCLE {		//Բ
	Point2d center;
	double radius;
};
struct ARC {		//��
	Point2d center;
	double radius;
	double start;
	double end;
};
struct package {		//����ڵİ���Ԫ�ؼ���
	vector<struct LINE> lines;
	vector<struct CIRCLE> circles;
	vector<struct ARC> arcs;
	vector<struct PART> parts;
	vector<struct LABEL> labels;
	double midx;
	double midy;
};
struct PART {		//�������
	string partname;
	Point2d pos;
};
struct LABEL {
	string labelname;
	Point2d textpoint;
	Point2d basepoint;
	Point2d arrowpoint;
	struct LINE labelline;
	int flag;//flag=1��ע�����������ߣ�flag=0��ע������������
};
const int CoordinatePadding = 0;
const double step_ = 5;
const double DIFline = 3;		//�ָ��������
const double DIFarc = 8;	
string linelayer="1���";		//Ԫ��������ͼ����
string circlelayer="��˨";
string arclayer="1���";
string textlayer = "2����";
int linenumber=39;		//ÿ��Ԫ�ذ�������Ϣ����Ŀ
int arcnumber = 41;
int polynumber = 47;
int circlenumber = 41;
int textnumber = 35;
int leadernumber = 490;
double minX_;
double minY_;
double maxX_;
double maxY_;
double distance(Point2d a, Point2d b);		//�������
std::string& trim(std::string& s);		//ȥ���ַ�����λ�հ׷�
std::string UTF8ToGB(const char* str);		//utf8תgb�����������ַ�

//��ȡdxf�ļ�
void readFile(const char* Filename, vector<string>& strlines);

//��ȡֱ�ߣ�����Բ�����ֵ���Ϣ
void splitElements(vector<string> strlines, vector<struct LINE>& lines, vector<struct CIRCLE>& circles, vector<struct ARC>& arcs, vector<struct PART>& parts, vector<struct LABEL>& labels, int& width, int& height, Mat& mat_);

//�ָ���������
void splitPackage(vector<struct LINE>& lines, vector<struct CIRCLE>& circles, vector<struct ARC>& arcs, vector<struct PART>& parts, vector<struct LABEL>& labels, vector<struct package>& pkgs);

//Ϊÿ���������ͼ��
void drawMats(vector<struct package> pkgs, int width, int height, string file);

//�㵽�߶ξ��룬������������
double distancePointLine(Point2d p0, Point2d p1, Point2d p2);

//�жϵ����߶���
bool onsegment(Point2d pi, Point2d pj, Point2d Q);
#endif // !HEAD_H
