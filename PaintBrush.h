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

int mouse_mode = 0, draw_mode = 0;

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
