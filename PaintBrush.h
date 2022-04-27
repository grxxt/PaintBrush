#pragma once

#define		DRAW_RECTANGLE 		0		// �簢�� �׸���
#define		DRAW_CIRCLE			1		// �� �׸���
#define		DRAW_ECLIPSE		2		// Ÿ�� �׸���
#define		DRAW_LINE			3		// ���� �׸���
#define		DRAW_BRUSH			4		// �귯�� �׸���
#define		ERASE		 		5		// ���찳
#define		OPEN		 		6		// ���� ���
#define		SAVE		 		7		// ���� ���
#define		PLUS	 			8		// ��� �ϱ� ���
#define		MINUS	 			9		// ��Ӱ� �ϱ� ���
#define		CLEAR				10		// ����� ���
#define		COLOR	 			11		// ���� ������
#define		PALETTE	 			12		// ���� �ȷ�Ʈ
#define		HUE_IDX	 			13		// ���� �ε���

int hue;
int mouse_mode = 0, draw_mode = 0;
int thickness = 3;

// ������ ��ġ�ϴ� �Լ�
void placement_icons(Size size)    // size : ������ ũ��
{
	vector<string> icon_name = {
		"rect", "circle", "ellipse", "line", "brush", "eraser",
		"open", "save", "plus", "minus", "clear", "color"
	};

	int btn_rows = cvCeil(icon_name.size() / 2.0);	// 2�� ��ư ���

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

void create_hueIndex(Rect rect)					// rect - ���� �ε��� ����
{
	Mat hue_idx = image(icons[13]);				// ���� �ε��� ���� ����
	float ratio = 140.0f / icons[13].height;    // ���� �ε��� ���� ũ���� hue ������ -> 1.8

	for (int i = 0; i < icons[13].height; i++)
	{
		Scalar hue_color(i * ratio, 255, 255);	// HSV ���� ����
		hue_idx.row(i).setTo(hue_color);		// �� ���� ���� ���� row(): �κ����..
	}

	cvtColor(hue_idx, hue_idx, COLOR_HSV2BGR);  // HSV���� RGB�� ��ȯ
}

// pos_y : ���콺 Ŭ�� y ��ǥ, rect_pale: �ȷ�Ʈ ����
void  create_palette(int pos_y, Rect rect_palette)
{
	Mat mat_palette = image(rect_palette);				// �ȷ�Ʈ ���� ����

	float ratio_hue = 180.0f / rect_palette.height;		// �ȷ�Ʈ ���̿� ���� hue ���� 1.8
	float ratio_sat = 256.0f / rect_palette.width;		// �ȷ�Ʈ �ʺ� ���� saturation ���� 2.56
	float ratio_int = 256.0f / rect_palette.height;		// �ȷ�Ʈ ���̿� ���� intensity ���� 2.56

	hue = cvRound((pos_y - rect_palette.y) * ratio_hue);

	cout << mat_palette.rows << endl;	// 100
	cout << mat_palette.cols << endl;	// 100

	for (int i = 0; i < mat_palette.rows; i++) {								// ���� �ȷ�Ʈ ��ȸ
		for (int j = 0; j < mat_palette.cols; j++)
		{
			int saturation = cvRound(j * ratio_sat);							// ä�� ���
			int intensity = cvRound((mat_palette.rows - i - 1) * ratio_int);	// �� ���
			mat_palette.at<Vec3b>(i, j) = Vec3b(hue, saturation, intensity);	// HSV ���� ����
		}
	}

	cvtColor(mat_palette, mat_palette, COLOR_HSV2BGR);		// HSV���� RGB�� ��ȯ
}

// �Ϲ� ��� ���� �Լ�
void command(int mode)
{
	if (mode == PALETTE)	// �ȷ�Ʈ ���� Ŭ��
	{
		float  ratio1 = 256.0f / icons[PALETTE].width;		// �ʺ�� ä�� ���� ����
		float  ratio2 = 256.0f / icons[PALETTE].height;		// ���̷� �� ���� ����

		Point pt = pt2 - icons[PALETTE].tl();						// �ȷ�Ʈ �� Ŭ�� ��ǥ
		int saturation = cvRound(pt.x * ratio1);
		int value = cvRound((icons[PALETTE].height - pt.y - 1) * ratio2);
		Scalar  hsv(hue, saturation, value);					// HSV ���� ����

		Mat    m_color = image(icons[COLOR]);			// ���� ������ ����
		m_color.setTo(hsv);
		cvtColor(m_color, m_color, COLOR_HSV2BGR);		// HSV �� BGR ��ȯ
		rectangle(image, icons[COLOR], Scalar(0, 0, 0), 1); // ���� ������ �׵θ� ��

		color = Scalar(m_color.at < Vec3b >(10, 10));
	}
	else if (mode == HUE_IDX)							// ���� �ε��� Ŭ����
	{
		create_palette(pt2.y, icons[PALETTE]);				// �ȷ�Ʈ �ٽ� �׸���
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

	imshow("�׸���", image);
}

void onMouse(int event, int x, int y, int flags, void*)
{
	cout << x << ", " << y << endl;
	Point pt(x, y);

	if (event == EVENT_LBUTTONUP)		// ���콺 ���� ��ư�� ���� ���
	{
		for (int i = 0; i < icons.size(); i++)
		{
			if (icons[i].contains(pt))  // �޴� Ŭ�� ���� �˻�
			{
				if (i < 6)				// �׸��� ��� ("rect", "circle", "ellipse", "line", "brush", "eraser")
				{
					mouse_mode = 0;		// ���콺 ���� �ʱ�ȭ
					draw_mode = i;		// �׸��� ���
				}
				else
				{
					command(i);		// �Ϲ� ��ɾ� ("open", "save", "plus", "minus", "clear", "color")
				}
				return;
			}
		}
		pt2 = pt;						// ���� ��ǥ ����
		mouse_mode = 1;					// ��ư ���� ���� ����
		cout << "EVENT_LBUTTONUP:" << pt2 << endl;
	} 
	else if (event == EVENT_LBUTTONDOWN) // ���콺 ���� ��ư�� ���� ���
	{
		pt1 = pt;						// ���� ��ǥ ���� 
		mouse_mode = 2;					// ��ư ���� ���� ����
		cout << "EVENT_LBUTTONDOWN:" << pt1 << endl;
	}

	if (mouse_mode >= 2)	// ���� ��ư ������ or �巡��
	{
		Rect rect(0, 0, 125, image.rows);
		mouse_mode = (rect.contains(pt)) ? 0 : 3; // ���콺 ���� ����
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
		int  radius = (int)sqrt(pt3.x * pt3.x + pt3.y * pt3.y);	// �� ��ǥ�� �Ÿ�
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
	imshow("�׸���", image);
}
