#include "head.h"

std::string& trim(std::string& s)
{
	if (s.empty())
	{
		return s;
	}

	s.erase(0, s.find_first_not_of(" \r"));
	s.erase(s.find_last_not_of(" \r") + 1);
	return s;
}

std::string UTF8ToGB(const char* str)
{
	std::string result;
	WCHAR* strSrc;
	LPSTR szRes;

	//获得临时变量的大小
	int i = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	strSrc = new WCHAR[i + 1];
	MultiByteToWideChar(CP_UTF8, 0, str, -1, strSrc, i);
	//获得临时变量的大小
	i = WideCharToMultiByte(CP_ACP, 0, strSrc, -1, NULL, 0, NULL, NULL);
	szRes = new CHAR[i + 1];
	WideCharToMultiByte(CP_ACP, 0, strSrc, -1, szRes, i, NULL, NULL);

	result = szRes;
	delete[]strSrc;
	delete[]szRes;

	return result;
}

double distance(Point2d a, Point2d b)
{
	return sqrt((double)((a.x - b.x) * (a.x - b.x)) + (double)((a.y - b.y) * (a.y - b.y)));
}

double distancePointLine(Point2d p0, Point2d p1, Point2d p2)
{
	double dis12 = distance(p1, p2);//线段长度
	double dis01 = distance(p0, p1);//p1与p0的距离
	double dis02 = distance(p0, p2);//p2与p0的距离
	double HalfC = (dis12 + dis01 + dis02) / 2;// 半周长
	double s = sqrt(HalfC * (HalfC - dis12) * (HalfC - dis01) * (HalfC - dis02));//海伦公式求面积
	double xj2DisPL = 2 * s / dis12;// 返回点到线的距离（利用三角形面积公式求高）

	return xj2DisPL;
}

bool onsegment(Point2d pi, Point2d pj, Point2d Q)
{
	if ((Q.x - pi.x) * (pj.y - pi.y) == (pj.x - pi.x) * (Q.y - pi.y) && min(pi.x, pj.x) <= Q.x && Q.x <= max(pi.x, pj.x) && min(pi.y, pj.y) <= Q.y && Q.y <= max(pi.y, pj.y)) 
	{
		return true;
	}
	else 
	{
		return false;
	}
}

void readFile(const char* Filename, vector<string>& strlines)
{
	ifstream fin;
	fin.open(Filename, ios::in | ios::binary);
	string str0;
	string str;
	//int count = 0;
	while (!fin.eof())
	{
		getline(fin, str0);
		str0 = UTF8ToGB(str0.c_str()).c_str();
		str = trim(str0);
		//string temp = "-26039.661223755148";
		//if (str == temp)
		//{
		//	count++;
		//}
		//cout << str << endl;
		strlines.push_back(str);
	}
	fin.close();
}

void splitElements(vector<string> strlines, vector<struct LINE>& lines, vector<struct CIRCLE>& circles, vector<struct ARC>& arcs, vector<struct PART>& parts, vector<struct LABEL>& labels,  int& width, int& height, Mat& mat_)
{
	int flag = 0;
	for (vector<string>::iterator it = strlines.begin(); it != strlines.end(); it++)
	{
		if ((*it) == "$EXTMIN")
		{
			minX_ = atof((*(it + 2)).c_str());
			minY_ = atof((*(it + 4)).c_str());
			//cout << minX_<<endl;
		}
		if ((*it) == "$EXTMAX")
		{
			maxX_ = atof((*(it + 2)).c_str());
			maxY_ = atof((*(it + 4)).c_str());
			//cout << maxX_<<endl;
			flag = 1;
		}
		if (flag == 1)
		{
			break;
		}
	}

	//cout << "minx" << minX_ << " " << minY_ << " " << maxX_ << " " << maxY_ << endl;
	width = abs(static_cast<int>((maxX_ - minX_ + 2 * CoordinatePadding) / step_));
	height = abs(static_cast<int>((maxY_ - minY_ + 2 * CoordinatePadding) / step_));
	mat_ = Mat::zeros(Size(width, height), CV_8UC1);

	for (vector<string>::iterator it = strlines.begin(); it != strlines.end(); it++)
	{
		Scalar color = Scalar(255);
		int lineType = LineTypes::LINE_AA;
		if ((*it) == "LINE")
		{
			vector<string> temp;
			for (int i = 1; i < linenumber && it + i != strlines.end() && *(it + i) != "LINE"; i++)
			{
				vector<string>::iterator itmp = it + i;
				temp.push_back(*itmp);
			}

			for (vector<string>::iterator it0 = temp.begin(); it0 != temp.end(); it0++)
			{
				if ((*it0) == "1F" && *(it0 - 1) == "330")
				{
					for (vector<string>::iterator it1 = it0 + 1; it1 != temp.end(); it1++)
					{
						if ((*it1) == "8" && *(it1 + 1) == linelayer)//(*(it1 + 1) != "0" || *(it1 + 1) != "打印图层" || *(it1 + 1) != "尺寸"))
						{
							for (vector<string>::iterator it2 = it1 + 1; it2 != temp.end(); it2++)
							{
								if (*it2 == "AcDbLine" && *(it2 + 1) == "10")
								{
									double startX = (atof((*(it2 + 2)).c_str()) - minX_ + CoordinatePadding) / step_;
									double startY = mat_.rows - 1 - (atof((*(it2 + 4)).c_str()) - minY_ + CoordinatePadding) / step_;
									double endX = (atof((*(it2 + 8)).c_str()) - minX_ + CoordinatePadding) / step_;
									double endY = mat_.rows - 1 - (atof((*(it2 + 10)).c_str()) - minY_ + CoordinatePadding) / step_;
									cout << startX * step_ + minX_ << endl;
									struct LINE templine;
									templine.l1 = Point2d(startX, startY);
									templine.l2 = Point2d(endX, endY);
									lines.push_back(templine);
									line(mat_, Point2f(startX, startY), Point2f(endX, endY), color, 1, lineType);
									break;
								}
							}
							break;
						}
					}
					break;
				}
			}
			vector<string>().swap(temp);
		}
		else if ((*it) == "CIRCLE")
		{
			vector<string> temp;
			for (int i = 1; i < circlenumber && it + i != strlines.end() && *(it + i) != "CIRCLE"; i++)
			{
				vector<string>::iterator itmp = it + i;
				temp.push_back(*itmp);
			}

			for (vector<string>::iterator it1 = temp.begin(); it1 != temp.end(); it1++)
			{
				if ((*it1) == "8" && *(it1 + 1) == circlelayer)
				{
					for (vector<string>::iterator it2 = it1 + 1; it2 != temp.end(); it2++)
					{
						if (*it2 == "AcDbCircle" && *(it2 + 1) == "10")
						{
							double x = (atof((*(it2 + 2)).c_str()) - minX_ + CoordinatePadding) / step_;
							double y = mat_.rows - 1 - (atof((*(it2 + 4)).c_str()) - minY_ + CoordinatePadding) / step_;
							double radius = atof((*(it2 + 8)).c_str()) / step_;
							cout << "圆" << x << " " << y << " " << radius << endl;

							struct CIRCLE tempcircle;
							tempcircle.center = Point2d(x, y);
							tempcircle.radius = radius;
							circles.push_back(tempcircle);
							cv::circle(mat_, Point(x, y), radius, color, 1, lineType);
							break;
						}
					}
					break;
				}
			}
			vector<string>().swap(temp);
		}
		else if ((*it) == "ARC")
		{
			vector<string> temp;
			for (int i = 1; i < arcnumber && it + i != strlines.end() && *(it + i) != "ARC"; i++)
			{
				vector<string>::iterator itmp = it + i;
				temp.push_back(*itmp);
			}

			for (vector<string>::iterator it0 = temp.begin(); it0 != temp.end(); it0++)
			{
				if ((*it0) == "1F" && *(it0 - 1) == "330")
				{
					for (vector<string>::iterator it1 = it0 + 1; it1 != temp.end(); it1++)
					{
						if ((*it1) == "8" && *(it1 + 1) == arclayer)
						{
							for (vector<string>::iterator it2 = it1 + 1; it2 != temp.end(); it2++)
							{
								if (*it2 == "AcDbCircle" && *(it2 + 1) == "10")
								{
									double x = (atof((*(it2 + 2)).c_str()) - minX_ + CoordinatePadding) / step_;
									double y = mat_.rows - 1 - (atof((*(it2 + 4)).c_str()) - minY_ + CoordinatePadding) / step_;

									double radius = atof((*(it2 + 8)).c_str()) / step_;
									double start = atof((*(it2 + 12)).c_str());
									double end = atof((*(it2 + 14)).c_str());
									start = end > start ? start : start - 2 * 180;
									cout << "弧" << start << " " << end << endl;

									struct ARC temparc;
									temparc.center = Point2d(x, y);
									temparc.radius = radius;
									temparc.start = start;
									temparc.end = end;
									arcs.push_back(temparc);
									cv::ellipse(mat_, Point(x, y), Size(radius, radius), 0.0,
										-end, -start, color, 1);
									break;
								}
							}
							break;
						}
					}
					break;
				}
			}
			vector<string>().swap(temp);
		}
		else if (*it == "LWPOLYLINE")
		{
			vector<string> temp;
			for (int i = 1; i < polynumber && it + i != strlines.end() && *(it + i) != "LWPOLYLINE"; i++)
			{
				vector<string>::iterator itmp = it + i;
				temp.push_back(*itmp);
			}

			for (vector<string>::iterator it1 = temp.begin(); it1 != temp.end(); it1++)
			{
				if (*it1 == "8" && *(it1 + 1) == linelayer)
				{
					for (vector<string>::iterator it2 = it1 + 1; it2 != temp.end(); it2++)
					{
						if (*it2 == "AcDbPolyline" && *(it2 + 7) == "10")
						{
							int vexcount = (int)(atof((*(it2 + 2)).c_str()));
							vector<Point> vertexes;
							vector<Point2d> vertexes1;
							for (int i = 0; i < vexcount; i++)
							{
								double x = (atof((*(it2 + 8 + i * 4)).c_str()) - minX_ + CoordinatePadding) / step_;
								double y = mat_.rows - 1 - (atof((*(it2 + 10 + i * 4)).c_str()) - minY_ + CoordinatePadding) / step_;
								vertexes1.push_back(Point2d(x, y));
								vertexes.push_back(Point(static_cast<int>(x), static_cast<int>(y)));
							}
							cout << "多线段" << endl;

							if (vertexes.size() == 4)//这里只假设折线段画的是矩形
							{
								struct LINE templine;

								templine.l1 = vertexes1[0];
								templine.l2 = vertexes1[1];
								lines.push_back(templine);
								templine.l1 = vertexes1[1];
								templine.l2 = vertexes1[2];
								lines.push_back(templine);
								templine.l1 = vertexes1[2];
								templine.l2 = vertexes1[3];
								lines.push_back(templine);
								templine.l1 = vertexes1[3];
								templine.l2 = vertexes1[0];
								lines.push_back(templine);
								cv::polylines(mat_, vertexes, true, color, 1, lineType, 0);
							}
							break;
						}
					}
					break;
				}
			}
			vector<string>().swap(temp);
		}
		else if (*it == "TEXT")
		{
			vector<string> temp;
			for (int i = 1; i < textnumber && it + i != strlines.end() && *(it + i) != "TEXT"; i++)
			{
				vector<string>::iterator itmp = it + i;
				temp.push_back(*itmp);
			}

			for (vector<string>::iterator it1 = temp.begin(); it1 != temp.end(); it1++)
			{
				if (*it1 == "8" && *(it1 + 1) == textlayer)
				{
					for (vector<string>::iterator it2 = it1 + 1; it2 != temp.end(); it2++)
					{
						if (*it2 == "AcDbText" && *(it2 + 1) == "10")
						{
							string partname = *(it2 + 10);
							double x = (atof((*(it2 + 2)).c_str()) - minX_ + CoordinatePadding) / step_;
							double y = mat_.rows - 1 - (atof((*(it2 + 4)).c_str()) - minY_ + CoordinatePadding) / step_;
							cout << "文字" << partname << endl;
							//putText(mat_, String(partname), Point2d(x, y), FONT_HERSHEY_PLAIN,1,color,1,lineType);

							struct PART tempart;
							tempart.pos = Point2d(x, y);
							tempart.partname = partname;
							parts.push_back(tempart);
							break;
						}
					}
					break;
				}
			}
			vector<string>().swap(temp);
		}
		else if(*it == "CONTEXT_DATA{")
		{
			vector<string> temp;
			struct LABEL labeltemp;
			labeltemp.flag = 1;

			for (int i = 1; i < leadernumber && it + i != strlines.end() && *(it + i) != "CONTEXT_DATA{"; i++)
			{
				vector<string>::iterator itmp = it + i;
				temp.push_back(*itmp);
			}

			for (vector<string>::iterator it1 = temp.begin(); it1 != temp.end(); it1++)
			{
				if (*it1 == "304" && *(it1+1) != "LEADER_LINE{")
				{
					string tempname = *(it1 + 1);
					if (tempname.find("{") == string::npos)
					{
						labeltemp.labelname = tempname;
					}
					else
					{
						int begin = tempname.find_last_of(";");
						int end = tempname.find_last_of("}");
						labeltemp.labelname = tempname.substr(begin + 1, end - begin - 1);
					}
				}
				if (*it1 == "12" && *(it1+2) == "22")
				{
					labeltemp.textpoint.x = (atof((*(it1 + 1)).c_str()) - minX_ + CoordinatePadding) / step_;
					labeltemp.textpoint.y = mat_.rows - 1 - (atof((*(it1 + 3)).c_str()) - minY_ + CoordinatePadding) / step_;
				}
				if (*it1 == "LEADER{")
				{
					for (vector<string>::iterator it2 = it1 + 1; it2 != temp.end(); it2++)
					{
						if (*it2 == "10" && *(it2 + 2) == "20")
						{
							labeltemp.basepoint.x = (atof((*(it2 + 1)).c_str()) - minX_ + CoordinatePadding) / step_;
							labeltemp.basepoint.y = mat_.rows - 1 - (atof((*(it2 + 3)).c_str()) - minY_ + CoordinatePadding) / step_;
							break;
						}
					}
				}
				if (*it1 == "LEADER_LINE{")
				{
					for (vector<string>::iterator it2 = it1 + 1; it2 != temp.end(); it2++)
					{
						if (*it2 == "10" && *(it2 + 2) == "20")
						{
							labeltemp.arrowpoint.x = (atof((*(it2 + 1)).c_str()) - minX_ + CoordinatePadding) / step_;
							labeltemp.arrowpoint.y = mat_.rows - 1 - (atof((*(it2 + 3)).c_str()) - minY_ + CoordinatePadding) / step_;
							break;
						}
					}
					labels.push_back(labeltemp);
					putText(mat_, String(labeltemp.labelname), labeltemp.textpoint, FONT_HERSHEY_PLAIN, 1, color, 1, lineType);
					break;
				}
			}

		}
		else if (*it == "LEADER")
		{
			vector<string> temp;
			struct LABEL labeltemp;
			labeltemp.flag = 0;

			for (int i = 1; i < leadernumber && it + i != strlines.end() && *(it + i) != "LEADER"; i++)
			{
				vector<string>::iterator itmp = it + i;
				temp.push_back(*itmp);
			}

			for (vector<string>::iterator it1 = temp.begin(); it1 != temp.end(); it1++)
			{
				if (*it1 == "76")
				{
					labeltemp.arrowpoint.x = (atof((*(it1 + 3)).c_str()) - minX_ + CoordinatePadding) / step_;
					labeltemp.arrowpoint.y = mat_.rows - 1 - (atof((*(it1 + 5)).c_str()) - minY_ + CoordinatePadding) / step_;
					labeltemp.basepoint.x = (atof((*(it1 + 9)).c_str()) - minX_ + CoordinatePadding) / step_;
					labeltemp.basepoint.y = mat_.rows - 1 - (atof((*(it1 + 11)).c_str()) - minY_ + CoordinatePadding) / step_;
					labels.push_back(labeltemp);
					break;
				}
			}
		}
	}
}

void splitPackage(vector<struct LINE>& lines, vector<struct CIRCLE>& circles, vector<struct ARC>& arcs, vector<struct PART>& parts, vector<struct LABEL>& labels, vector<struct package>& pkgs)
{
	for (vector<struct PART>::iterator itpart = parts.begin(); itpart != parts.end(); itpart++)
	{
		string::size_type idx1, idx2, idx3;
		//idx1 = (*itpart).partname.find("零件");
		idx2 = (*itpart).partname.find("正");
		idx3 = (*itpart).partname.find("反");
		vector<struct LABEL>::iterator tempitlabel = labels.begin();
		//vector<struct package>::iterator tempit = pkgs.begin();
		if (idx2 != string::npos || idx3 != string::npos)
		{
			double dis = distance((*tempitlabel).basepoint, (*itpart).pos);
			for (vector<struct LABEL>::iterator itlabel = labels.begin(); itlabel != labels.end(); itlabel++)
			{
				if ((*itlabel).flag == 0)
				{
					if (distance((*itlabel).basepoint, (*itpart).pos) < dis)
					{
						tempitlabel = itlabel;
						dis = distance((*itlabel).basepoint, (*itpart).pos);
					}
				}
			}
			(*tempitlabel).labelname = (*itpart).partname;
			(*tempitlabel).textpoint = (*itpart).pos;
		}
		//if (idx1 != string::npos)
		//{
		//	double dis = distance((*itpart).pos, Point2d(pkgs[0].midx, pkgs[0].midy));
		//	for (vector<struct package>::iterator itpkg = pkgs.begin(); itpkg != pkgs.end(); itpkg++)
		//	{
		//		if ((*itpkg).midy > (*itpart).pos.y)
		//		{
		//			if (distance((*itpart).pos, Point2d((*itpkg).midx, (*itpkg).midy)) < dis)
		//			{
		//				tempit = itpkg;
		//				dis = distance((*itpart).pos, Point2d((*itpkg).midx, (*itpkg).midy));
		//			}
		//		}
		//	}
		//	(*tempit).parts.push_back(*itpart);
		//}
	}

	for (vector<struct LABEL>::iterator itlabel = labels.begin(); itlabel != labels.end(); itlabel++)
	{
		double dis = distancePointLine((*itlabel).arrowpoint, lines[0].l1, lines[0].l2);
		for (vector<struct LINE>::iterator itline = lines.begin(); itline != lines.end(); itline++)
		{
			double ac = distance((*itlabel).arrowpoint, (*itline).l1);
			double bc = distance((*itlabel).arrowpoint, (*itline).l2);
			double d = distancePointLine((*itlabel).arrowpoint, (*itline).l1, (*itline).l2);
			double ab = distance((*itline).l1, (*itline).l2);
			if (max(ac, bc) * max(ac, bc) - d * d <= ab * ab)
			{
				if (d < dis)
				{
					dis = d;
					(*itlabel).labelline = *itline;
				}
			}
		}
	}

	for (vector<struct LINE>::iterator itline = lines.begin(); itline != lines.end(); )
	{
		struct package temppkg;
		temppkg.lines.push_back(*itline);
		while (1)
		{
			int count = 0;
			for (vector<struct LINE>::iterator itline1 = itline + 1; itline1 != lines.end();)
			{
				int flag = 0;
				for (vector<struct LINE>::iterator itline2 = temppkg.lines.begin(); itline2 != temppkg.lines.end(); itline2++)
				{
					if (distance((*itline2).l1, (*itline1).l1) < DIFline || distance((*itline2).l2, (*itline1).l2) < DIFline || distance((*itline2).l1, (*itline1).l2) < DIFline || distance((*itline2).l2, (*itline1).l1) < DIFline)
					{
						temppkg.lines.push_back(*itline1);
						itline1 = lines.erase(itline1);
						flag = 1;
						count++;
						break;
					}

				}
				if (flag == 0)
				{
					itline1++;
				}
			}

			for (vector<struct ARC>::iterator itarc = arcs.begin(); itarc != arcs.end();)
			{
				int flag = 0;
				for (vector<struct LINE>::iterator itline2 = temppkg.lines.begin(); itline2 != temppkg.lines.end(); itline2++)
				{
					if (fabs(distance((*itline2).l1, (*itarc).center) - (*itarc).radius) < DIFarc || fabs(distance((*itline2).l2, (*itarc).center) - (*itarc).radius) < DIFarc)
					{
						cout << "线弧距离：" << fabs(distance((*itline2).l1, (*itarc).center) - (*itarc).radius) << " "<< fabs(distance((*itline2).l2, (*itarc).center) - (*itarc).radius)<<endl;
						temppkg.arcs.push_back(*itarc);
						itarc = arcs.erase(itarc);
						flag = 1;
						count++;
						break;
					}
				}
				if (flag == 0)
				{
					itarc++;
				}
			}

			for (vector<struct LINE>::iterator itline1 = itline + 1; itline1 != lines.end();)
			{
				int flag = 0;
				for (vector<struct ARC>::iterator itarc = temppkg.arcs.begin(); itarc != temppkg.arcs.end(); itarc++)
				{
					if (fabs(distance((*itline1).l1, (*itarc).center) - (*itarc).radius) < DIFarc || fabs(distance((*itline1).l2, (*itarc).center) - (*itarc).radius) < DIFarc)
					{
						cout << "线弧距离：" << fabs(distance((*itline1).l1, (*itarc).center) - (*itarc).radius) << " "<< fabs(distance((*itline1).l2, (*itarc).center) - (*itarc).radius)<<endl;
						temppkg.lines.push_back(*itline1);
						itline1 = lines.erase(itline1);
						flag = 1;
						count++;
						break;
					}
				}
				if (flag == 0)
				{
					itline1++;
				}
			}

			if (count == 0)
			{
				break;
			}
		}

		double minx, miny, maxx, maxy;
		minx = temppkg.lines[0].l1.x;
		miny = temppkg.lines[0].l1.y;
		maxx = temppkg.lines[0].l1.x;
		maxy = temppkg.lines[0].l1.y;
		for (vector<struct LINE>::iterator itline2 = temppkg.lines.begin(); itline2 != temppkg.lines.end(); itline2++)
		{
			if ((*itline2).l1.x < minx)
				minx = (*itline2).l1.x;
			if ((*itline2).l2.x < minx)
				minx = (*itline2).l2.x;
			if ((*itline2).l1.y < miny)
				miny = (*itline2).l1.y;
			if ((*itline2).l2.y < miny)
				miny = (*itline2).l2.y;
			if ((*itline2).l1.x > maxx)
				maxx = (*itline2).l1.x;
			if ((*itline2).l2.x > maxx)
				maxx = (*itline2).l2.x;
			if ((*itline2).l1.y > maxy)
				maxy = (*itline2).l1.y;
			if ((*itline2).l2.y > maxy)
				maxy = (*itline2).l2.y;
		}

		temppkg.midx = (minx + maxx) / 2;
		temppkg.midy = (miny + maxy) / 2;
		for (vector<struct CIRCLE>::iterator itcircle = circles.begin(); itcircle != circles.end();)
		{
			if ((*itcircle).center.x > minx && (*itcircle).center.y > miny && (*itcircle).center.x < maxx && (*itcircle).center.y < maxy)
			{
				temppkg.circles.push_back(*itcircle);
				itcircle = circles.erase(itcircle);
			}
			else
			{
				itcircle++;
			}
		}

		for (vector<struct LINE>::iterator itline1 = itline + 1; itline1 != lines.end();)
		{
			if ((*itline1).l1.x + (*itline1).l2.x > 2 * minx && (*itline1).l1.y + (*itline1).l2.y > 2 * miny && (*itline1).l1.x + (*itline1).l2.x < 2 * maxx && (*itline1).l1.y + (*itline1).l2.y < 2 * maxy)
			{
				temppkg.lines.push_back(*itline1);
				itline1 = lines.erase(itline1);
			}
			else
			{
				itline1++;
			}
		}

		itline = lines.erase(itline);
		pkgs.push_back(temppkg);
	}

	for (vector<struct package>::iterator itpkg = pkgs.begin(); itpkg != pkgs.end(); itpkg++)
	{
		for (vector<struct LABEL>::iterator itlabel = labels.begin(); itlabel != labels.end(); itlabel++)
		{
			for (vector<struct LINE>::iterator itline = (*itpkg).lines.begin(); itline != (*itpkg).lines.end(); itline++)
			{
				if (((*itlabel).labelline.l1 == (*itline).l1 && (*itlabel).labelline.l2 == (*itline).l2) || ((*itlabel).labelline.l1 == (*itline).l2 && (*itlabel).labelline.l2 == (*itline).l1))
				{
					(*itpkg).labels.push_back(*itlabel);
					break;
				}
			}
		}

	}

	for (vector<struct PART>::iterator itpart = parts.begin(); itpart != parts.end(); itpart++)
	{
		string::size_type idx1, idx2, idx3;
		idx1 = (*itpart).partname.find("零件");
		vector<struct package>::iterator tempit = pkgs.begin();
		if (idx1 != string::npos)
		{
			double dis = distance((*itpart).pos, Point2d(pkgs[0].midx, pkgs[0].midy));
			for (vector<struct package>::iterator itpkg = pkgs.begin(); itpkg != pkgs.end(); itpkg++)
			{
				if ((*itpkg).midy > (*itpart).pos.y)
				{
					if (distance((*itpart).pos, Point2d((*itpkg).midx, (*itpkg).midy)) < dis)
					{
						tempit = itpkg;
						dis = distance((*itpart).pos, Point2d((*itpkg).midx, (*itpkg).midy));
					}
				}
			}
			(*tempit).parts.push_back(*itpart);
		}
	}
}

void drawMats(vector<struct package> pkgs, int width, int height, string file)
{
	int count1 = 0;
	for (vector<struct package>::iterator itpkg = pkgs.begin(); itpkg != pkgs.end(); itpkg++)
	{
		Mat mat = Mat::zeros(Size(width, height), CV_8UC1);;
		for (vector<struct LINE>::iterator itline = (*itpkg).lines.begin(); itline != (*itpkg).lines.end(); itline++)
		{
			double startX = (*itline).l1.x;
			double startY = (*itline).l1.y;
			double endX = (*itline).l2.x;
			double endY = (*itline).l2.y;
			line(mat, Point(startX, startY), Point(endX, endY), Scalar(255), 1, LineTypes::LINE_AA);
		}
		for (vector<struct CIRCLE>::iterator itcircle = (*itpkg).circles.begin(); itcircle != (*itpkg).circles.end(); itcircle++)
		{
			double x = (*itcircle).center.x;
			double y = (*itcircle).center.y;
			double radius = (*itcircle).radius;
			cv::circle(mat, Point2d(x, y), radius, Scalar(255), 1, LineTypes::LINE_8);
		}
		for (vector<struct ARC>::iterator itarc = (*itpkg).arcs.begin(); itarc != (*itpkg).arcs.end(); itarc++)
		{
			double x = (*itarc).center.x;
			double y = (*itarc).center.y;
			double radius = (*itarc).radius;
			double start = (*itarc).start;
			double end = (*itarc).end;
			start = end > start ? start : start - 2 * 180;
			cv::ellipse(mat, Point2d(x, y), Size(radius, radius), 0.0,
				-end, -start, Scalar(255), 1);
		}
		for (vector<struct PART>::iterator itpart = (*itpkg).parts.begin(); itpart != (*itpkg).parts.end(); itpart++)
		{
			string partname;
			partname = (*itpart).partname;
			cv::putText(mat, String(partname), (*itpart).pos, FONT_HERSHEY_PLAIN, 1, Scalar(255), 1, LINE_AA);
		}
		for (vector<struct LABEL>::iterator itlabel = (*itpkg).labels.begin(); itlabel != (*itpkg).labels.end(); itlabel++)
		{
			string partname;
			partname = (*itlabel).labelname;
			cv::putText(mat, String(partname), (*itlabel).textpoint, FONT_HERSHEY_PLAIN, 1, Scalar(255), 1, LINE_AA);
		}

		double minx, miny, maxx, maxy;
		minx = (*itpkg).lines[0].l1.x;
		miny = (*itpkg).lines[0].l1.y;
		maxx = (*itpkg).lines[0].l1.x;
		maxy = (*itpkg).lines[0].l1.y;
		for (vector<struct LINE>::iterator itline2 = (*itpkg).lines.begin(); itline2 != (*itpkg).lines.end(); itline2++)
		{
			if ((*itline2).l1.x < minx)
				minx = (*itline2).l1.x;
			if ((*itline2).l2.x < minx)
				minx = (*itline2).l2.x;
			if ((*itline2).l1.y < miny)
				miny = (*itline2).l1.y;
			if ((*itline2).l2.y < miny)
				miny = (*itline2).l2.y;
			if ((*itline2).l1.x > maxx)
				maxx = (*itline2).l1.x;
			if ((*itline2).l2.x > maxx)
				maxx = (*itline2).l2.x;
			if ((*itline2).l1.y > maxy)
				maxy = (*itline2).l1.y;
			if ((*itline2).l2.y > maxy)
				maxy = (*itline2).l2.y;
		}
		string file1;
		file1 = file + to_string(count1) + ".bmp";
		Mat mat0;
		mat0 = mat(Rect((int)(minx-500>0?minx-500:0), (int)(miny-500>0?miny-500:0), (int)(maxx - minx + 1000<width-minx+500?maxx-minx+1000:width-minx+500), (int)(maxy - miny + 1000 < height - miny+500 ? maxy - miny + 1000 : height - miny+500)));
		cv::imwrite(file1, mat0);
		count1++;
	}
}

int main()
{
	vector<string> strlines;
	vector<struct LINE> lines;
	vector<struct CIRCLE> circles;
	vector<struct ARC> arcs;
	vector<struct PART> parts;
	vector<struct LABEL> labels;
	vector<struct package> pkgs;
	int width, height;
	Mat mat_;

	readFile("D:/codeProjects/Projects/垂直支撑（五）002.dxf", strlines);

	splitElements(strlines, lines, circles, arcs, parts, labels, width, height, mat_);

	//for (vector<struct LABEL>::iterator it = labels.begin(); it != labels.end(); it++)
	//{
	//	cout << (*it).arrowpoint.x << endl;
	//}

	splitPackage(lines, circles, arcs, parts, labels, pkgs);

	string file = "D:/codeProjects/Projects/Project3/picture/chuizhi/";
	drawMats(pkgs, width, height, file);

	string outFileName = "D:/codeProjects/Projects/Project3/chuizhi.bmp";
	cv::imwrite(outFileName, mat_);
	cv::imshow("mat", mat_);
	cv::waitKey(0);
	return 0;
}