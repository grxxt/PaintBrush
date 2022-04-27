#pragma once

#define		DRAW_RECTANGLE 		0		// 사각형 그리기
#define		DRAW_CIRCLE			1		// 원 그리기
#define		DRAW_ECLIPSE		2		// 타원 그리기
#define		DRAW_LINE			3		// 직선 그리기
#define		DRAW_BRUSH			4		// 브러시 그리기
#define		ERASE		 		5		// 지우개
#define		OPEN		 		6		// 열기 명령
#define		SAVE		 		7		// 저장 명령
#define		PLUS	 			8		// 밝게 하기 명령
#define		MINUS	 			9		// 어둡게 하기 명령
#define		CLEAR				10		// 지우기 명령
#define		COLOR	 			11		// 색상 아이콘
#define		PALETTE	 			12		// 색상 팔레트
#define		HUE_IDX	 			13		// 색상 인덱스

int hue;
int mouse_mode = 0, draw_mode = 0;
int thickness = 3;

// 아이콘 배치하는 함수
void placement_icons(Size size)    // size : 아이콘 크기
{
	vector<string> icon_name = {
		"rect", "circle", "ellipse", "line", "brush", "eraser",
		"open", "save", "plus", "minus", "clear", "color"
	};

	int btn_rows = cvCeil(icon_name.size() / 2.0);	// 2열 버튼 행수

	for (int y = 0, k = 0; y < btn_rows; y++) {
		for (int x = 0; x < 2; x++, k++) {
			Point pt(x * size.width, y * size.height);
			icons.push_back(Rect(pt, size));

			Mat icon = imread("./icon/" + icon_name[k] + ".jpg");
			if (icon.empty()) continue;

			resize(icon, icon, size);
			icon.copyTo(image(icons[k]));
		}
	}
}

void create_hueIndex(Rect rect)					// rect - 색상 인덱스 영역
{
	Mat hue_idx = image(icons[13]);				// 색상 인덱스 영역 참조
	float ratio = 140.0f / icons[13].height;    // 색상 인덱스 세로 크기의 hue 스케일 -> 1.8

	for (int i = 0; i < icons[13].height; i++)
	{
		Scalar hue_color(i * ratio, 255, 255);	// HSV 색상 지정
		hue_idx.row(i).setTo(hue_color);		// 한 행의 색상 지정 row(): 부분행렬..
	}

	cvtColor(hue_idx, hue_idx, COLOR_HSV2BGR);  // HSV에서 RGB로 변환
}

// pos_y : 마우스 클릭 y 좌표, rect_pale: 팔레트 영역
void  create_palette(int pos_y, Rect rect_palette)
{
	Mat mat_palette = image(rect_palette);				// 팔레트 영역 참조

	float ratio_hue = 180.0f / rect_palette.height;		// 팔레트 높이에 따른 hue 비율 1.8
	float ratio_sat = 256.0f / rect_palette.width;		// 팔레트 너비에 따른 saturation 비율 2.56
	float ratio_int = 256.0f / rect_palette.height;		// 팔레트 높이에 따른 intensity 비율 2.56

	hue = cvRound((pos_y - rect_palette.y) * ratio_hue);

	cout << mat_palette.rows << endl;	// 100
	cout << mat_palette.cols << endl;	// 100

	for (int i = 0; i < mat_palette.rows; i++) {								// 색상 팔레트 조회
		for (int j = 0; j < mat_palette.cols; j++)
		{
			int saturation = cvRound(j * ratio_sat);							// 채도 계산
			int intensity = cvRound((mat_palette.rows - i - 1) * ratio_int);	// 명도 계산
			mat_palette.at<Vec3b>(i, j) = Vec3b(hue, saturation, intensity);	// HSV 색상 지정
		}
	}

	cvtColor(mat_palette, mat_palette, COLOR_HSV2BGR);		// HSV에서 RGB로 변환
}

// 일반 명령 수행 함수
void command(int mode)
{
	if (mode == PALETTE)	// 팔레트 영역 클릭
	{
		float  ratio1 = 256.0f / icons[PALETTE].width;		// 너비로 채도 비율 지정
		float  ratio2 = 256.0f / icons[PALETTE].height;		// 높이로 명도 비율 지정

		Point pt = pt2 - icons[PALETTE].tl();						// 팔레트 내 클릭 좌표
		int saturation = cvRound(pt.x * ratio1);
		int value = cvRound((icons[PALETTE].height - pt.y - 1) * ratio2);
		Scalar  hsv(hue, saturation, value);					// HSV 색상 지정

		Mat    m_color = image(icons[COLOR]);			// 색상 아이콘 참조
		m_color.setTo(hsv);
		cvtColor(m_color, m_color, COLOR_HSV2BGR);		// HSV → BGR 변환
		rectangle(image, icons[COLOR], Scalar(0, 0, 0), 1); // 색상 아이콘 테두리 색

		color = Scalar(m_color.at < Vec3b >(10, 10));
	}
	else if (mode == HUE_IDX)							// 색상 인덱스 클릭시
	{
		create_palette(pt2.y, icons[PALETTE]);				// 팔레트 다시 그리기
	}
	else if (mode == CLEAR)
	{
		canvas.setTo(Scalar(255, 255, 255));
		mouse_mode = 0;
	}
	else if (mode == OPEN)
	{
		//        Mat tmp = imread("./image/my_picture.jpg", 1);
		Mat tmp = imread("./image/my_save.jpg", 1);

		CV_Assert(!tmp.empty());

		resize(tmp, tmp, canvas.size());
		tmp.copyTo(canvas);
	}
	else if (mode == SAVE)
	{
		imwrite("./image/my_save.jpg", canvas);
	}
	else if (mode == PLUS)
	{
		canvas += Scalar(10, 10, 10);
	}
	else if (mode == MINUS)
	{
		canvas -= Scalar(10, 10, 10);
	}

	imshow("그림판", image);
}

void onMouse(int event, int x, int y, int flags, void*)
{
	cout << x << ", " << y << endl;
	Point pt(x, y);

	if (event == EVENT_LBUTTONUP)		// 마우스 왼쪽 버튼을 떼는 경우
	{
		for (int i = 0; i < icons.size(); i++)
		{
			if (icons[i].contains(pt))  // 메뉴 클릭 여부 검사
			{
				if (i < 6)				// 그리기 모드 ("rect", "circle", "ellipse", "line", "brush", "eraser")
				{
					mouse_mode = 0;		// 마우스 상태 초기화
					draw_mode = i;		// 그리기 모드
				}
				else
				{
					command(i);		// 일반 명령어 ("open", "save", "plus", "minus", "clear", "color")
				}
				return;
			}
		}
		pt2 = pt;						// 종료 좌표 저장
		mouse_mode = 1;					// 버튼 떼기 상태 지정
		cout << "EVENT_LBUTTONUP:" << pt2 << endl;
	} 
	else if (event == EVENT_LBUTTONDOWN) // 마우스 왼쪽 버튼을 누른 경우
	{
		pt1 = pt;						// 시작 좌표 저장 
		mouse_mode = 2;					// 버튼 누름 상태 지정
		cout << "EVENT_LBUTTONDOWN:" << pt1 << endl;
	}

	if (mouse_mode >= 2)	// 왼쪽 버튼 누르기 or 드래그
	{
		Rect rect(0, 0, 125, image.rows);
		mouse_mode = (rect.contains(pt)) ? 0 : 3; // 마우스 상태 지정
		pt2 = pt;
	}
}

void draw(Mat image, Scalar color=Scalar(200, 200, 200))
{
	if (draw_mode == DRAW_RECTANGLE)
	{
		rectangle(image, pt1, pt2, color, thickness);
	}
	else if (draw_mode == DRAW_LINE)
	{
		line(image, pt1, pt2, color, thickness);
	}
	else if (draw_mode == DRAW_BRUSH)
	{
		line(image, pt1, pt2, color, thickness * 3);
		pt1 = pt2;
	}
	else if (draw_mode == ERASE)
	{
		line(image, pt1, pt2, Scalar(255, 255, 255), thickness * 5);
		pt1 = pt2;
	}
	else if (draw_mode == DRAW_CIRCLE)
	{
		Point2d pt3 = pt1 - pt2;
		int  radius = (int)sqrt(pt3.x * pt3.x + pt3.y * pt3.y);	// 두 좌표간 거리
		circle(image, pt1, radius, color, thickness);
	}
	else if (draw_mode == DRAW_ECLIPSE)
	{
		Point2d center = (pt1 + pt2) / 2.0;
		Size2d  size = (Size2d)(pt1 - pt2) / 2.0;
		// Size2d  size = (pt1 - pt2) / 2.0;
		size.width = abs(size.width);
		size.height = abs(size.height);

		ellipse(image, center, size, 0, 0, 360, color, thickness);
	}
	imshow("그림판", image);
}
