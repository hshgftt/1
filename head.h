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
struct LINE {		//直线
	Point2d l1;
	Point2d l2;
};
struct CIRCLE {		//圆
	Point2d center;
	double radius;
};
struct ARC {		//弧
	Point2d center;
	double radius;
	double start;
	double end;
};
struct package {		//零件内的包含元素集合
	vector<struct LINE> lines;
	vector<struct CIRCLE> circles;
	vector<struct ARC> arcs;
	vector<struct PART> parts;
	vector<struct LABEL> labels;
	double midx;
	double midy;
};
struct PART {		//零件名称
	string partname;
	Point2d pos;
};
struct LABEL {
	string labelname;
	Point2d textpoint;
	Point2d basepoint;
	Point2d arrowpoint;
	struct LINE labelline;
	int flag;//flag=1标注文字属于引线，flag=0标注文字属于文字
};
const int CoordinatePadding = 0;
const double step_ = 5;
const double DIFline = 3;		//分隔距离参数
const double DIFarc = 8;	
string linelayer="1零件";		//元素所属的图层名
string circlelayer="螺栓";
string arclayer="1零件";
string textlayer = "2文字";
int linenumber=39;		//每个元素包含的信息行数目
int arcnumber = 41;
int polynumber = 47;
int circlenumber = 41;
int textnumber = 35;
int leadernumber = 490;
double minX_;
double minY_;
double maxX_;
double maxY_;
double distance(Point2d a, Point2d b);		//两点距离
std::string& trim(std::string& s);		//去除字符串首位空白符
std::string UTF8ToGB(const char* str);		//utf8转gb，处理中文字符

//读取dxf文件
void readFile(const char* Filename, vector<string>& strlines);

//提取直线，弧，圆，文字等信息
void splitElements(vector<string> strlines, vector<struct LINE>& lines, vector<struct CIRCLE>& circles, vector<struct ARC>& arcs, vector<struct PART>& parts, vector<struct LABEL>& labels, int& width, int& height, Mat& mat_);

//分割出单个零件
void splitPackage(vector<struct LINE>& lines, vector<struct CIRCLE>& circles, vector<struct ARC>& arcs, vector<struct PART>& parts, vector<struct LABEL>& labels, vector<struct package>& pkgs);

//为每个零件绘制图形
void drawMats(vector<struct package> pkgs, int width, int height, string file);

//点到线段距离，三角形面积求解
double distancePointLine(Point2d p0, Point2d p1, Point2d p2);

//判断点在线段上
bool onsegment(Point2d pi, Point2d pj, Point2d Q);
#endif // !HEAD_H
