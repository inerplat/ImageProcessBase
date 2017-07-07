// ImageProcessBaseDlg.cpp : 구현 파일
// 웹캠 BGR24

#include "stdafx.h"
#include "ImageProcessBase.h"
#include "ImageProcessBaseDlg.h"
#include "afxdialogex.h"

#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>

using namespace std;

#define RED  0
#define GREEN 1
#define BLUE 2
#define CLIP(x) (((x) <0)?0:(((x)>255)?255:(x)))

#define PI 3.141592
#define Labeling_Threshold 50
#define Min_Size 1500

#define HEIGHT 480
#define HEIGHT_2 240
#define WIDTH 640
#define WIDTH_2 320

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BITMAPINFO BmInfo;
LPBYTE pImgBuffer;

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

														// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CImageProcessBaseDlg 대화 상자



CImageProcessBaseDlg::CImageProcessBaseDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_IMAGEPROCESSBASE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CImageProcessBaseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CImageProcessBaseDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CImageProcessBaseDlg 메시지 처리기

BOOL CImageProcessBaseDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	RECT m_Rect = { 0, 0, 640, 480 };     // 생성하고 싶은 사이즈를 RECT 변수에 초기화. 

	AdjustWindowRect(&m_Rect, WS_OVERLAPPEDWINDOW, FALSE);
	int width = m_Rect.right - m_Rect.left;
	int height = m_Rect.bottom - m_Rect.top;
	this->SetWindowPos(NULL, 0, 0, width, height, SWP_NOSIZE);

	// 윈도우 생성
	m_Cap = capCreateCaptureWindow(TEXT("Image Test"), WS_CHILD
		| WS_VISIBLE, 0, 0, 640, 480, this->m_hWnd, NULL);

	// 콜백함수 지정
	if (capSetCallbackOnFrame(m_Cap, CallbackOnFrame) == FALSE) {
		return FALSE;
	}

	// 카메라 드라이버와 연결
	if (capDriverConnect(m_Cap, 0) == FALSE) {
		return FALSE;
	}

	capPreviewRate(m_Cap, 33);    // 초당 프레임 지정
	capOverlay(m_Cap, false);
	capPreview(m_Cap, true);        // 미리보기 기능 설정

	if (BmInfo.bmiHeader.biBitCount != 24) {

		BmInfo.bmiHeader.biBitCount = 24;
		BmInfo.bmiHeader.biCompression = 0;
		BmInfo.bmiHeader.biSizeImage = BmInfo.bmiHeader.biWidth * BmInfo.bmiHeader.biHeight * 3;


		capGetVideoFormat(m_Cap, &BmInfo, sizeof(BITMAPINFO));
	}

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CImageProcessBaseDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CImageProcessBaseDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CImageProcessBaseDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CImageProcessBaseDlg::OnDestroy()
{
	CDialog::OnDestroy();

	capDriverDisconnect(m_Cap);

	if (pImgBuffer != NULL) {
		delete[] pImgBuffer;
	}
}

struct point
{
	int x;
	int y;
};

struct vertex
{
	struct point point;
};

struct Label
{
	unsigned char R, G, B;
	unsigned int cnt;
	struct point top;
};

struct rectangle
{
	bool check = false;
	int cnt = 0;
	double size = 0;
	struct point center;
	unsigned char R, G, B;
}pre_rectangle;

int Labeling_data[HEIGHT_2][WIDTH_2];
int Edge_data[HEIGHT_2][WIDTH_2];
int Labeling_cnt;
unsigned char RGB[HEIGHT_2][WIDTH_2][3] = { 0, };
vector <struct Label> Labeling_color;
vector <struct point>Edge_Line;

void Labeling(int y, int x)
{
	if (y < Labeling_color[Labeling_data[y][x]].top.y)
	{
		Labeling_color[Labeling_data[y][x]].top.x = x;
		Labeling_color[Labeling_data[y][x]].top.y = y;
	}

	Labeling_color[Labeling_data[y][x]].cnt++;
	if (y > 0 &&
		(RGB[y][x][RED] - RGB[y - 1][x][RED])*(RGB[y][x][RED] - RGB[y - 1][x][RED]) + 
		(RGB[y][x][GREEN] - RGB[y - 1][x][GREEN])*(RGB[y][x][GREEN] - RGB[y - 1][x][GREEN]) +
		(RGB[y][x][BLUE] - RGB[y - 1][x][BLUE])*(RGB[y][x][BLUE] - RGB[y - 1][x][BLUE]) < Labeling_Threshold)
	{
		if (Labeling_data[y - 1][x] == -1)
		{
			Labeling_data[y - 1][x] = Labeling_data[y][x];
			Labeling(y - 1, x);
		}
	}

	if (y < HEIGHT_2 - 1 &&
		(RGB[y][x][RED] - RGB[y + 1][x][RED])*(RGB[y][x][RED] - RGB[y + 1][x][RED]) +
		(RGB[y][x][GREEN] - RGB[y + 1][x][GREEN])*(RGB[y][x][GREEN] - RGB[y + 1][x][GREEN]) +
		(RGB[y][x][BLUE] - RGB[y + 1][x][BLUE])*(RGB[y][x][BLUE] - RGB[y + 1][x][BLUE]) < Labeling_Threshold)
	{
		if (Labeling_data[y + 1][x] == -1)
		{
			Labeling_data[y + 1][x] = Labeling_data[y][x];
			Labeling(y + 1, x);
		}
	}

	if (x > 0 &&
		(RGB[y][x - 1][RED] - RGB[y][x][RED])*(RGB[y][x - 1][RED] - RGB[y][x][RED]) +
		(RGB[y][x - 1][GREEN] - RGB[y][x][GREEN])*(RGB[y][x - 1][GREEN] - RGB[y][x][GREEN]) +
		(RGB[y][x - 1][BLUE] - RGB[y][x][BLUE])*(RGB[y][x - 1][BLUE] - RGB[y][x][BLUE]) < Labeling_Threshold)
	{
		if (Labeling_data[y][x - 1] == -1)
		{
			Labeling_data[y][x - 1] = Labeling_data[y][x];
			Labeling(y, x - 1);
		}
	}

	if (x < WIDTH_2 - 1 &&
		(RGB[y][x + 1][RED] - RGB[y][x][RED])*(RGB[y][x + 1][RED] - RGB[y][x][RED]) +
		(RGB[y][x + 1][GREEN] - RGB[y][x][GREEN])*(RGB[y][x + 1][GREEN] - RGB[y][x][GREEN]) +
		(RGB[y][x + 1][BLUE] - RGB[y][x][BLUE])*(RGB[y][x + 1][BLUE] - RGB[y][x][BLUE]) < Labeling_Threshold)
	{
		if (Labeling_data[y][x + 1] == -1)
		{
			Labeling_data[y][x + 1] = Labeling_data[y][x];
			Labeling(y, x + 1);
		}
	}
}

void Edge(int y, int x, int Labeling_num, int pre, int cnt)//12시부터 반시계방향으로 0->7
{
	cnt++;
	struct point tmp;
	tmp.x = x;
	tmp.y = y;
	Edge_Line.push_back(tmp);
	if (cnt > 10 && y == Labeling_color[Labeling_num].top.y && x == Labeling_color[Labeling_num].top.x)return;

	switch (pre)
	{
	case 0:
		if (y > 0 && x > 0 && Labeling_data[y - 1][x - 1] == Labeling_data[y][x])
		{
			Edge_data[y - 1][x - 1] = Edge_data[y][x];
			Edge(y - 1, x - 1, Labeling_num, 5, cnt);
			break;
		}
	case 1:
		if (x > 0 && Labeling_data[y][x - 1] == Labeling_data[y][x])
		{
			Edge_data[y][x - 1] = Edge_data[y][x];
			Edge(y, x - 1, Labeling_num, 6, cnt);
			break;
		}
	case 2:
		if (y < HEIGHT_2 - 1 && x > 0 && Labeling_data[y + 1][x - 1] == Labeling_data[y][x])
		{
			Edge_data[y + 1][x - 1] = Edge_data[y][x];
			Edge(y + 1, x - 1, Labeling_num, 7, cnt);
			break;
		}
	case 3:
		if (y < HEIGHT_2 - 1 && Labeling_data[y + 1][x] == Labeling_data[y][x])
		{
			Edge_data[y + 1][x] = Edge_data[y][x];
			Edge(y + 1, x, Labeling_num, 0, cnt);
			break;
		}
	case 4:
		if (y < HEIGHT_2 - 1 && x < WIDTH_2 - 1 && Labeling_data[y + 1][x + 1] == Labeling_data[y][x])
		{
			Edge_data[y + 1][x + 1] = Edge_data[y][x];
			Edge(y + 1, x + 1, Labeling_num, 1, cnt);
			break;
		}
	case 5:
		if (x < WIDTH_2 - 1 && Labeling_data[y][x + 1] == Labeling_data[y][x])
		{
			Edge_data[y][x + 1] = Edge_data[y][x];
			Edge(y, x + 1, Labeling_num, 2, cnt);
			break;
		}
	case 6:
		if (y > 0 && x < WIDTH_2 - 1 && Labeling_data[y - 1][x + 1] == Labeling_data[y][x])
		{
			Edge_data[y - 1][x + 1] = Edge_data[y][x];
			Edge(y - 1, x + 1, Labeling_num, 3, cnt);
			break;
		}
	case 7:
		if (y > 0 && Labeling_data[y - 1][x] == Labeling_data[y][x])
		{
			Edge_data[y - 1][x] = Edge_data[y][x];
			Edge(y - 1, x, Labeling_num, 4, cnt);
			break;
		}

		if (y > 0 && x > 0 && Labeling_data[y - 1][x - 1] == Labeling_data[y][x])
		{
			Edge_data[y - 1][x - 1] = Edge_data[y][x];
			Edge(y - 1, x - 1, Labeling_num, 5, cnt);
			break;
		}
		if (x > 0 && Labeling_data[y][x - 1] == Labeling_data[y][x])
		{
			Edge_data[y][x - 1] = Edge_data[y][x];
			Edge(y, x - 1, Labeling_num, 6, cnt);
			break;
		}
		if (y < HEIGHT_2 - 1 && x > 0 && Labeling_data[y + 1][x - 1] == Labeling_data[y][x])
		{
			Edge_data[y + 1][x - 1] = Edge_data[y][x];
			Edge(y + 1, x - 1, Labeling_num, 7, cnt);
			break;
		}
		if (y < HEIGHT_2 - 1 && Labeling_data[y + 1][x] == Labeling_data[y][x])
		{
			Edge_data[y + 1][x] = Edge_data[y][x];
			Edge(y + 1, x, Labeling_num, 0, cnt);
			break;
		}
		if (y < HEIGHT_2 - 1 && x < WIDTH_2 - 1 && Labeling_data[y + 1][x + 1] == Labeling_data[y][x])
		{
			Edge_data[y + 1][x + 1] = Edge_data[y][x];
			Edge(y + 1, x + 1, Labeling_num, 1, cnt);
			break;
		}
		if (x < WIDTH_2 - 1 && Labeling_data[y][x + 1] == Labeling_data[y][x])
		{
			Edge_data[y][x + 1] = Edge_data[y][x];
			Edge(y, x + 1, Labeling_num, 2, cnt);
			break;
		}
		if (y > 0 && x < WIDTH_2 - 1 && Labeling_data[y - 1][x + 1] == Labeling_data[y][x])
		{
			Edge_data[y - 1][x + 1] = Edge_data[y][x];
			Edge(y - 1, x + 1, Labeling_num, 3, cnt);
			break;
		}
	}
}

void Edge_expansion(int y, int x, int cnt)
{
	if (cnt < 0)return;
	cnt--;
	if (y > 0 && Edge_data[y - 1][x] == -1)
	{
		Edge_data[y - 1][x] = Edge_data[y][x];
		Edge_expansion(y - 1, x, cnt);
	}
	if (y > 0 && x > 0 && Edge_data[y - 1][x - 1] == -1)
	{
		Edge_data[y - 1][x - 1] = Edge_data[y][x];
		Edge_expansion(y - 1, x - 1, cnt);
	}
	if (x > 0 && Edge_data[y][x - 1] == -1)
	{
		Edge_data[y][x - 1] = Edge_data[y][x];
		Edge_expansion(y, x - 1, cnt);
	}
	if (y < HEIGHT_2 - 1 && x > 0 && Edge_data[y + 1][x - 1] == -1)
	{
		Edge_data[y + 1][x - 1] = Edge_data[y][x];
		Edge_expansion(y + 1, x - 1, cnt);
	}
	if (y < HEIGHT_2 - 1 && Edge_data[y + 1][x] == -1)
	{
		Edge_data[y + 1][x] = Edge_data[y][x];
		Edge_expansion(y + 1, x, cnt);
	}
	if (y < HEIGHT_2 - 1 && x < WIDTH_2 - 1 && Edge_data[y + 1][x + 1] == -1)
	{
		Edge_data[y + 1][x + 1] = Edge_data[y][x];
		Edge_expansion(y + 1, x + 1, cnt);
	}
	if (x < WIDTH_2 - 1 && Edge_data[y][x + 1] == -1)
	{
		Edge_data[y][x + 1] = Edge_data[y][x];
		Edge_expansion(y, x + 1, cnt);
	}
	if (y > 0 && x < WIDTH_2 - 1 && Edge_data[y][x + 1] == -1)
	{
		Edge_data[y - 1][x + 1] = Edge_data[y][x];
		Edge_expansion(y - 1, x + 1, cnt);
	}
}

void Draw_Line(struct point A, struct point B, int color)
{
	double angle = atan2(A.y - B.y, A.x - B.x);
	int r = (int)sqrt((A.x - B.x)*(A.x - B.x) + (A.y - B.y)*(A.y - B.y));

	for (int i = 0; i <= r; i++)
	{
		int Y = (int)(A.y - i*sin(angle));
		int X = (int)(A.x - i*cos(angle));
		if (X >= 0 && X < WIDTH_2 && Y >= 0 && Y < HEIGHT_2)
			Edge_data[Y][X] = color;
	}
}

double SignArea_rect(struct point A, struct point B, struct point C, struct point D)
{
	return (A.x*B.y - A.y*B.x + B.x*C.y - C.x*B.y + C.x*D.y - D.x*C.y + D.x*A.y - A.x*D.y);
}

bool Convex_Check(struct point A, struct point B, struct point C, struct point D)
{
	if ((A.x*C.y - A.y*C.x + C.x*B.y - B.x*C.y + B.x*A.y - A.x*B.y)*(A.x*C.y - A.y*C.x + C.x*D.y - D.x*C.y + D.x*A.y - A.x*D.y) > -10)return false;
	if ((B.x*D.y - B.y*D.x + D.x*A.y - A.x*D.y + A.x*B.y - B.x*A.y)*(B.x*D.y - B.y*D.x + D.x*C.y - C.x*D.y + C.x*B.y - B.x*C.y) > -10)return false;
	return true;
}

float GaussianMask[5][5] =
{ { 1,4,6,4,1 },
{ 4,16,24,16,4 },
{ 6,24,36,24,6 },
{ 4,16,24,16,4 },
{ 1,4,6,4,1 } };


LRESULT CALLBACK CallbackOnFrame(HWND hWnd, LPVIDEOHDR lpVHdr)
{
	unsigned int uiBuflen = lpVHdr->dwBufferLength;
	unsigned int nWidth, nHeight;
	unsigned int i, j;

	nWidth = WIDTH;
	nHeight = HEIGHT;

	//Gaussian && 절반 축소
	for (j = 0; j < nHeight; j += 2) { // height
		for (i = 0; i < nWidth; i += 2) { //width
			int cnt = 0;
			int R = 0, G = 0, B = 0;
			for (int y = -2; y <= 2; y++)
			{
				for (int x = -2; x <= 2; x++)
				{
					if (x + i < 0 || x + i >= nWidth || y + j < 0 || y + j >= nHeight);
					else
					{
						cnt += GaussianMask[y + 2][x + 2];
						R += GaussianMask[y + 2][x + 2] * lpVHdr->lpData[(nWidth*(y + j) + x + i) * 3 + 2];
						G += GaussianMask[y + 2][x + 2] * lpVHdr->lpData[(nWidth*(y + j) + x + i) * 3 + 1];
						B += GaussianMask[y + 2][x + 2] * lpVHdr->lpData[(nWidth*(y + j) + x + i) * 3];
					}
				}
			}
			RGB[j / 2][i / 2][RED] = R / cnt;
			RGB[j / 2][i / 2][GREEN] = G / cnt;
			RGB[j / 2][i / 2][BLUE] = B / cnt;
		}
	}

	/////////////////////////////////////////////////////////////////////////////////
	// RGB영상처리부분
	for (j = 0; j < HEIGHT_2; j++) { // height
		for (i = 0; i < WIDTH_2; i++) { //width
			Labeling_data[j][i] = -1;
			Edge_data[j][i] = -1;
		}
	}

	int max_size = 0;
	double max_similarity = 0;
	struct point max_vertex[4];
	vector <struct vertex> vertex;
	Labeling_color.clear();

	if (pre_rectangle.check)//이전에 사각형이 있었을때 그 위치만 확인하는거 아래도 똑같은 코드니까 자세한건 아래를 참고
	{
		pre_rectangle.check = false;
		if ((RGB[pre_rectangle.center.y][pre_rectangle.center.x][RED] - pre_rectangle.R)*(RGB[pre_rectangle.center.y][pre_rectangle.center.x][RED] - pre_rectangle.R) +
			(RGB[pre_rectangle.center.y][pre_rectangle.center.x][GREEN] - pre_rectangle.G)*(RGB[pre_rectangle.center.y][pre_rectangle.center.x][GREEN] - pre_rectangle.G) +
			(RGB[pre_rectangle.center.y][pre_rectangle.center.x][BLUE] - pre_rectangle.B)*(RGB[pre_rectangle.center.y][pre_rectangle.center.x][BLUE] - pre_rectangle.B) < Labeling_Threshold * 2)
		{
			struct Label tmp;
			tmp.R = RGB[pre_rectangle.center.y][pre_rectangle.center.x][RED];
			tmp.G = RGB[pre_rectangle.center.y][pre_rectangle.center.x][GREEN];
			tmp.B = RGB[pre_rectangle.center.y][pre_rectangle.center.x][BLUE];
			tmp.cnt = 0;
			tmp.top.x = pre_rectangle.center.x;
			tmp.top.y = pre_rectangle.center.y;

			Labeling_color.push_back(tmp);
			Labeling_data[pre_rectangle.center.y][pre_rectangle.center.x] = Labeling_color.size() - 1;
			Labeling(pre_rectangle.center.y, pre_rectangle.center.x);

			vertex.clear();
			Edge_Line.clear();
			if (Labeling_color[0].cnt > Min_Size)
			{
				Edge_data[Labeling_color[0].top.y][Labeling_color[0].top.x] = 0;
				Edge(Labeling_color[0].top.y, Labeling_color[0].top.x, 0, 0, 0);

				for (int d = 0; d < Edge_Line.size(); d++)
					Edge_expansion(Edge_Line[d].y, Edge_Line[d].x, 1);

				double *len = new double[Edge_Line.size()];
				int *sum_x = new int[Edge_Line.size()];
				int *sum_y = new int[Edge_Line.size()];

				int n = 15;
				sum_x[0] = 0;
				sum_y[0] = 0;
				for (int l = -n; l < 0; l++)
				{
					sum_x[0] += Edge_Line[Edge_Line.size() + l].x;
					sum_y[0] += Edge_Line[Edge_Line.size() + l].y;
				}
				for (int l = 0; l <= n; l++)
				{
					sum_x[0] += Edge_Line[l].x;
					sum_y[0] += Edge_Line[l].y;
				}

				len[0] = (Edge_Line[0].y - sum_y[0] / (2 * n + 1))*(Edge_Line[0].y - sum_y[0] / (2 * n + 1)) + (Edge_Line[0].x - sum_x[0] / (2 * n + 1))*(Edge_Line[0].x - sum_x[0] / (2 * n + 1));

				for (int l = 1; l < Edge_Line.size(); l++)
				{
					sum_x[l] = sum_x[l - 1] + Edge_Line[(l + n) % Edge_Line.size()].x - Edge_Line[(l - n - 1 + Edge_Line.size()) % Edge_Line.size()].x;
					sum_y[l] = sum_y[l - 1] + Edge_Line[(l + n) % Edge_Line.size()].y - Edge_Line[(l - n - 1 + Edge_Line.size()) % Edge_Line.size()].y;

					len[l] = (Edge_Line[l].y - sum_y[l] / (2 * n + 1))*(Edge_Line[l].y - sum_y[l] / (2 * n + 1)) + (Edge_Line[l].x - sum_x[l] / (2 * n + 1))*(Edge_Line[l].x - sum_x[l] / (2 * n + 1));
				}

				for (int l = 0; l < Edge_Line.size(); l++)
				{
					if (len[l] >= 8)
					{
						bool check = true;
						for (int x = -5; x <= 5; x++)
						{
							if (len[(l + x + Edge_Line.size()) % Edge_Line.size()] == -1 || len[l] < len[(l + x + Edge_Line.size()) % Edge_Line.size()])
							{
								check = false;
								break;
							}
						}

						if (check)
						{
							struct vertex tmp;
							tmp.point.x = Edge_Line[l].x;
							tmp.point.y = Edge_Line[l].y;
							vertex.push_back(tmp);
							len[l] = -1;
						}
					}
				}

				if (vertex.size() >= 4 && vertex.size() < 10)//모서리 4개씩 골라서 사각형인지 확인
				{
					int Ver[5];
					pair <double, int> V[11];
					struct point center;//이거 정렬 처음에 한 번만 해도 될듯?
					center.x = 0;
					center.y = 0;

					for (int i = 0; i < vertex.size(); i++)
					{
						center.x += vertex[i].point.x;
						center.y += vertex[i].point.y;
					}
					center.x /= vertex.size();
					center.y /= vertex.size();

					for (int i = 0; i < vertex.size(); i++)
					{
						V[i].first = atan2(vertex[i].point.y - center.y, vertex[i].point.x - center.x);
						V[i].second = i;
					}

					sort(&V[0], &V[vertex.size()]);

					for (Ver[0] = 0; Ver[0] < vertex.size() - 3; Ver[0]++)
					{
						for (Ver[1] = Ver[0] + 1; Ver[1] < vertex.size() - 2; Ver[1]++)
						{
							for (Ver[2] = Ver[1] + 1; Ver[2] < vertex.size() - 1; Ver[2]++)
							{
								for (Ver[3] = Ver[2] + 1; Ver[3] < vertex.size(); Ver[3]++)
								{
									if (!Convex_Check(vertex[V[Ver[0]].second].point, vertex[V[Ver[1]].second].point, vertex[V[Ver[2]].second].point, vertex[V[Ver[3]].second].point))break;

									//라벨링한 점이랑 크기가 너무 차이나면 무시, 기준을 기존에 저장된 데이터로 바꿀가 생각중
									Ver[4] = Ver[0];

									double size = SignArea_rect(vertex[V[Ver[0]].second].point, vertex[V[Ver[1]].second].point, vertex[V[Ver[2]].second].point, vertex[V[Ver[3]].second].point);

									double similarity = 1;

									for (int n = 0; n < 4; n++)//점끼리 이으면서 그 위치의 배열이 외각선인지 확인
									{
										int sum = 0;

										double angle = atan2(vertex[V[Ver[n]].second].point.y - vertex[V[Ver[n + 1]].second].point.y, vertex[V[Ver[n]].second].point.x - vertex[V[Ver[n + 1]].second].point.x);
										int r = (int)sqrt((vertex[V[Ver[n]].second].point.x - vertex[V[Ver[n + 1]].second].point.x)*(vertex[V[Ver[n]].second].point.x - vertex[V[Ver[n + 1]].second].point.x) + (vertex[V[Ver[n]].second].point.y - vertex[V[Ver[n + 1]].second].point.y)*(vertex[V[Ver[n]].second].point.y - vertex[V[Ver[n + 1]].second].point.y));
									
										for (int d = 1; d <= r; d++)
										{
											int Y = (int)(vertex[V[Ver[n]].second].point.y - d*sin(angle));
											int X = (int)(vertex[V[Ver[n]].second].point.x - d*cos(angle));
											if (X >= 0 && X < WIDTH_2 && Y >= 0 && Y < HEIGHT_2)
												if (Edge_data[Y][X] == i)
													sum++;
										}
										similarity *= (double)sum / r;
									}

									double size_similarity = size - pre_rectangle.size;
									if (size_similarity > 0)size_similarity /= (double)size;
									else size_similarity /= -(double)pre_rectangle.size;
									similarity *= (1 - size_similarity);

									if (max_similarity < similarity)
									{
										max_similarity = similarity;
										pre_rectangle.check = true;
										max_vertex[0].x = vertex[V[Ver[0]].second].point.x;
										max_vertex[0].y = vertex[V[Ver[0]].second].point.y;
										max_vertex[1].x = vertex[V[Ver[1]].second].point.x;
										max_vertex[1].y = vertex[V[Ver[1]].second].point.y;
										max_vertex[2].x = vertex[V[Ver[2]].second].point.x;
										max_vertex[2].y = vertex[V[Ver[2]].second].point.y;
										max_vertex[3].x = vertex[V[Ver[3]].second].point.x;
										max_vertex[3].y = vertex[V[Ver[3]].second].point.y;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	
	if (!pre_rectangle.check && pre_rectangle.cnt > 0)//사각형 찾기 시작 && 이전에 있음
	{
		//색기준 라벨링
		for (j = 0; j < HEIGHT_2; j++) { // height
			for (i = 0; i < WIDTH_2; i++) { //width
				if (Labeling_data[j][i] == -1 && 
					((RGB[j][i][RED] - pre_rectangle.R)*(RGB[j][i][RED] - pre_rectangle.R) +
					(RGB[j][i][GREEN] - pre_rectangle.G)*(RGB[j][i][GREEN] - pre_rectangle.G) +
					(RGB[j][i][BLUE] - pre_rectangle.B)*(RGB[j][i][BLUE] - pre_rectangle.B) < Labeling_Threshold * 2))
				{
					struct Label tmp;
					tmp.R = RGB[j][i][RED];
					tmp.G = RGB[j][i][GREEN];
					tmp.B = RGB[j][i][BLUE];
					tmp.cnt = 0;
					tmp.top.x = i;
					tmp.top.y = j;

					Labeling_color.push_back(tmp);
					Labeling_data[j][i] = Labeling_color.size() - 1;
					Labeling(j, i);
				}
			}
		}

		//라벨링 기준 외각선 검출
		for (int i = 0; i < Labeling_color.size(); i++)
		{
			vertex.clear();
			Edge_Line.clear();
			if (Labeling_color[i].cnt > pre_rectangle.size/4 && Labeling_color[i].cnt > Min_Size)//작은 건 무시
			{
				Edge_data[Labeling_color[i].top.y][Labeling_color[i].top.x] = i;
				Edge(Labeling_color[i].top.y, Labeling_color[i].top.x, i, 0, 0);//외각선 검출 시작

				for (int d = 0; d < Edge_Line.size(); d++)
					Edge_expansion(Edge_Line[d].y, Edge_Line[d].x, 1);//외각선 팽창

				double *len = new double[Edge_Line.size()];
				int *sum_x = new int[Edge_Line.size()];
				int *sum_y = new int[Edge_Line.size()];

				int n = 15;
				sum_x[0] = 0;
				sum_y[0] = 0;
				for (int l = -n; l < 0; l++)//연속된 점들을 평균내서 새로운 외각선을 만듦
				{
					sum_x[0] += Edge_Line[Edge_Line.size() + l].x;
					sum_y[0] += Edge_Line[Edge_Line.size() + l].y;
				}
				for (int l = 0; l <= n; l++)
				{
					sum_x[0] += Edge_Line[l].x;
					sum_y[0] += Edge_Line[l].y;
				}

				len[0] = (Edge_Line[0].y - sum_y[0] / (2 * n + 1))*(Edge_Line[0].y - sum_y[0] / (2 * n + 1)) + (Edge_Line[0].x - sum_x[0] / (2 * n + 1))*(Edge_Line[0].x - sum_x[0] / (2 * n + 1));

				for (int l = 1; l < Edge_Line.size(); l++)
				{
					sum_x[l] = sum_x[l - 1] + Edge_Line[(l + n) % Edge_Line.size()].x - Edge_Line[(l - n - 1 + Edge_Line.size()) % Edge_Line.size()].x;
					sum_y[l] = sum_y[l - 1] + Edge_Line[(l + n) % Edge_Line.size()].y - Edge_Line[(l - n - 1 + Edge_Line.size()) % Edge_Line.size()].y;

					len[l] = (Edge_Line[l].y - sum_y[l] / (2 * n + 1))*(Edge_Line[l].y - sum_y[l] / (2 * n + 1)) + (Edge_Line[l].x - sum_x[l] / (2 * n + 1))*(Edge_Line[l].x - sum_x[l] / (2 * n + 1));
				}

				for (int l = 0; l < Edge_Line.size(); l++)//새로 만든 외각선과 기존의 외각선의 거리가 먼게 모서리
				{
					if (len[l] >= 8)
					{
						bool check = true;
						for (int x = -5; x <= 5; x++)
						{
							if (len[(l + x + Edge_Line.size()) % Edge_Line.size()] == -1 || len[l] < len[(l + x + Edge_Line.size()) % Edge_Line.size()])
							{
								check = false;
								break;
							}
						}

						if (check)
						{
							struct vertex tmp;
							tmp.point.x = Edge_Line[l].x;
							tmp.point.y = Edge_Line[l].y;
							vertex.push_back(tmp);
							len[l] = -1;
						}
					}
				}

				if (vertex.size() >= 4 && vertex.size() < 10)//모서리 4개씩 골라서 사각형인지 확인
				{
					int Ver[5];
					pair <double, int> V[11];
					struct point center;//이거 정렬 처음에 한 번만 해도 될듯?
					center.x = 0;
					center.y = 0;
					for (int i = 0; i < vertex.size(); i++)
					{
						center.x += vertex[i].point.x;
						center.y += vertex[i].point.y;
					}
					center.x /= vertex.size();
					center.y /= vertex.size();

					for (int i = 0; i < vertex.size(); i++)
					{
						V[i].first = atan2(vertex[i].point.y - center.y, vertex[i].point.x - center.x);
						V[i].second = i;
					}

					sort(&V[0], &V[vertex.size()]);

					for (Ver[0] = 0; Ver[0] < vertex.size() - 3; Ver[0]++)
					{
						for (Ver[1] = Ver[0] + 1; Ver[1] < vertex.size() - 2; Ver[1]++)
						{
							for (Ver[2] = Ver[1] + 1; Ver[2] < vertex.size() - 1; Ver[2]++)
							{
								for (Ver[3] = Ver[2] + 1; Ver[3] < vertex.size(); Ver[3]++)
								{
									if (!Convex_Check(vertex[V[Ver[0]].second].point, vertex[V[Ver[1]].second].point, vertex[V[Ver[2]].second].point, vertex[V[Ver[3]].second].point))break;
									//라벨링한 점이랑 크기가 너무 차이나면 무시, 기준을 기존에 저장된 데이터로 바꿀가 생각중
									Ver[4] = Ver[0];

									double size = SignArea_rect(vertex[V[Ver[0]].second].point, vertex[V[Ver[1]].second].point, vertex[V[Ver[2]].second].point, vertex[V[Ver[3]].second].point);

									double similarity = 1;

									for (int n = 0; n < 4; n++)//점끼리 이으면서 그 위치의 배열이 외각선인지 확인
									{
										int sum = 0;

										double angle = atan2(vertex[V[Ver[n]].second].point.y - vertex[V[Ver[n + 1]].second].point.y, vertex[V[Ver[n]].second].point.x - vertex[V[Ver[n + 1]].second].point.x);
										int r = (int)sqrt((vertex[V[Ver[n]].second].point.x - vertex[V[Ver[n + 1]].second].point.x)*(vertex[V[Ver[n]].second].point.x - vertex[V[Ver[n + 1]].second].point.x) + (vertex[V[Ver[n]].second].point.y - vertex[V[Ver[n + 1]].second].point.y)*(vertex[V[Ver[n]].second].point.y - vertex[V[Ver[n + 1]].second].point.y));
										
										for (int d = 1; d <= r; d++)
										{
											int Y = (int)(vertex[V[Ver[n]].second].point.y - d*sin(angle));
											int X = (int)(vertex[V[Ver[n]].second].point.x - d*cos(angle));
											if (X >= 0 && X < WIDTH_2 && Y >= 0 && Y < HEIGHT_2)
												if (Edge_data[Y][X] == i)
													sum++;
										}
										similarity *= (double)sum / r;
									}

									double size_similarity = size - pre_rectangle.size;
									if (size_similarity > 0)size_similarity /= (double)size;
									else size_similarity /= -(double)pre_rectangle.size;
									similarity *= (1 - size_similarity);

									if (max_similarity < similarity)
									{
										max_similarity = similarity;

										max_vertex[0].x = vertex[V[Ver[0]].second].point.x;
										max_vertex[0].y = vertex[V[Ver[0]].second].point.y;
										max_vertex[1].x = vertex[V[Ver[1]].second].point.x;
										max_vertex[1].y = vertex[V[Ver[1]].second].point.y;
										max_vertex[2].x = vertex[V[Ver[2]].second].point.x;
										max_vertex[2].y = vertex[V[Ver[2]].second].point.y;
										max_vertex[3].x = vertex[V[Ver[3]].second].point.x;
										max_vertex[3].y = vertex[V[Ver[3]].second].point.y;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	else if (!pre_rectangle.check && pre_rectangle.cnt <= 0)//사각형 찾기 시작
	{
		//색기준 라벨링
		for (j = 0; j < HEIGHT_2; j++) { // height
			for (i = 0; i < WIDTH_2; i++) { //width
				if (Labeling_data[j][i] == -1)
				{
					struct Label tmp;
					tmp.R = RGB[j][i][RED];
					tmp.G = RGB[j][i][GREEN];
					tmp.B = RGB[j][i][BLUE];
					tmp.cnt = 0;
					tmp.top.x = i;
					tmp.top.y = j;

					Labeling_color.push_back(tmp);
					Labeling_data[j][i] = Labeling_color.size() - 1;
					Labeling(j, i);
				}
			}
		}

		//라벨링 기준 외각선 검출
		for (int i = 0; i < Labeling_color.size(); i++)
		{
			vertex.clear();
			Edge_Line.clear();
			if (Labeling_color[i].cnt > Min_Size)//작은 건 무시
			{
				Edge_data[Labeling_color[i].top.y][Labeling_color[i].top.x] = i;
				Edge(Labeling_color[i].top.y, Labeling_color[i].top.x, i, 0, 0);//외각선 검출 시작

				for (int d = 0; d < Edge_Line.size(); d++)
					Edge_expansion(Edge_Line[d].y, Edge_Line[d].x, 1);//외각선 팽창

				double *len = new double[Edge_Line.size()];
				int *sum_x = new int[Edge_Line.size()];
				int *sum_y = new int[Edge_Line.size()];

				int n = 15;
				sum_x[0] = 0;
				sum_y[0] = 0;
				for (int l = -n; l < 0; l++)//연속된 점들을 평균내서 새로운 외각선을 만듦
				{
					sum_x[0] += Edge_Line[Edge_Line.size() + l].x;
					sum_y[0] += Edge_Line[Edge_Line.size() + l].y;
				}
				for (int l = 0; l <= n; l++)
				{
					sum_x[0] += Edge_Line[l].x;
					sum_y[0] += Edge_Line[l].y;
				}

				len[0] = (Edge_Line[0].y - sum_y[0] / (2 * n + 1))*(Edge_Line[0].y - sum_y[0] / (2 * n + 1)) + (Edge_Line[0].x - sum_x[0] / (2 * n + 1))*(Edge_Line[0].x - sum_x[0] / (2 * n + 1));

				for (int l = 1; l < Edge_Line.size(); l++)
				{
					sum_x[l] = sum_x[l - 1] + Edge_Line[(l + n) % Edge_Line.size()].x - Edge_Line[(l - n - 1 + Edge_Line.size()) % Edge_Line.size()].x;
					sum_y[l] = sum_y[l - 1] + Edge_Line[(l + n) % Edge_Line.size()].y - Edge_Line[(l - n - 1 + Edge_Line.size()) % Edge_Line.size()].y;

					len[l] = (Edge_Line[l].y - sum_y[l] / (2 * n + 1))*(Edge_Line[l].y - sum_y[l] / (2 * n + 1)) + (Edge_Line[l].x - sum_x[l] / (2 * n + 1))*(Edge_Line[l].x - sum_x[l] / (2 * n + 1));
				}

				for (int l = 0; l < Edge_Line.size(); l++)//새로 만든 외각선과 기존의 외각선의 거리가 먼게 모서리
				{
					if (len[l] >= 8)
					{
						bool check = true;
						for (int x = -5; x <= 5; x++)
						{
							if (len[(l + x + Edge_Line.size()) % Edge_Line.size()] == -1 || len[l] < len[(l + x + Edge_Line.size()) % Edge_Line.size()])
							{
								check = false;
								break;
							}
						}

						if (check)
						{
							struct vertex tmp;
							tmp.point.x = Edge_Line[l].x;
							tmp.point.y = Edge_Line[l].y;
							vertex.push_back(tmp);
							len[l] = -1;
						}
					}
				}

				if (vertex.size() >= 4 && vertex.size() < 10)//모서리 4개씩 골라서 사각형인지 확인
				{
					int Ver[5];
					pair <double, int> V[11];
					struct point center;//이거 정렬 처음에 한 번만 해도 될듯?
					center.x = 0;
					center.y = 0;
					for (int i = 0; i < vertex.size(); i++)
					{
						center.x += vertex[i].point.x;
						center.y += vertex[i].point.y;
					}
					center.x /= vertex.size();
					center.y /= vertex.size();

					for (int i = 0; i < vertex.size(); i++)
					{
						V[i].first = atan2(vertex[i].point.y - center.y, vertex[i].point.x - center.x);
						V[i].second = i;
					}

					sort(&V[0], &V[vertex.size()]);

					for (Ver[0] = 0; Ver[0] < vertex.size() - 3; Ver[0]++)
					{
						for (Ver[1] = Ver[0] + 1; Ver[1] < vertex.size() - 2; Ver[1]++)
						{
							for (Ver[2] = Ver[1] + 1; Ver[2] < vertex.size() - 1; Ver[2]++)
							{
								for (Ver[3] = Ver[2] + 1; Ver[3] < vertex.size(); Ver[3]++)
								{
									if (!Convex_Check(vertex[V[Ver[0]].second].point, vertex[V[Ver[1]].second].point, vertex[V[Ver[2]].second].point, vertex[V[Ver[3]].second].point))break;

									//라벨링한 점이랑 크기가 너무 차이나면 무시, 기준을 기존에 저장된 데이터로 바꿀가 생각중
									Ver[4] = Ver[0];

									double size = SignArea_rect(vertex[V[Ver[0]].second].point, vertex[V[Ver[1]].second].point, vertex[V[Ver[2]].second].point, vertex[V[Ver[3]].second].point);
									double Edge_similarity = 1;

									for (int n = 0; n < 4; n++)//점끼리 이으면서 그 위치의 배열이 외각선인지 확인
									{
										int sum = 0;

										double angle = atan2(vertex[V[Ver[n]].second].point.y - vertex[V[Ver[n + 1]].second].point.y, vertex[V[Ver[n]].second].point.x - vertex[V[Ver[n + 1]].second].point.x);
										int r = (int)sqrt((vertex[V[Ver[n]].second].point.x - vertex[V[Ver[n + 1]].second].point.x)*(vertex[V[Ver[n]].second].point.x - vertex[V[Ver[n + 1]].second].point.x) + (vertex[V[Ver[n]].second].point.y - vertex[V[Ver[n + 1]].second].point.y)*(vertex[V[Ver[n]].second].point.y - vertex[V[Ver[n + 1]].second].point.y));
							
										for (int d = 1; d <= r; d++)
										{
											int Y = (int)(vertex[V[Ver[n]].second].point.y - d*sin(angle));
											int X = (int)(vertex[V[Ver[n]].second].point.x - d*cos(angle));
											if (X >= 0 && X < WIDTH_2 && Y >= 0 && Y < HEIGHT_2)
												if (Edge_data[Y][X] == i)
													sum++;
										}

										Edge_similarity *= (double)sum / r;
									}

									if (Edge_similarity > 0.2)//외각선이 일치 && 크기가 크면 업데이트, 이것도 이전에 사각형이 있었으면 그거랑 비교해서 가장 비슷한 걸로 할까 생각중
									{
										if (max_size < size*Edge_similarity)
										{
											max_size = size*Edge_similarity;
											max_vertex[0].x = vertex[V[Ver[0]].second].point.x;
											max_vertex[0].y = vertex[V[Ver[0]].second].point.y;
											max_vertex[1].x = vertex[V[Ver[1]].second].point.x;
											max_vertex[1].y = vertex[V[Ver[1]].second].point.y;
											max_vertex[2].x = vertex[V[Ver[2]].second].point.x;
											max_vertex[2].y = vertex[V[Ver[2]].second].point.y;
											max_vertex[3].x = vertex[V[Ver[3]].second].point.x;
											max_vertex[3].y = vertex[V[Ver[3]].second].point.y;

											max_similarity = 1;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}


	if (max_similarity > 0.2)//사각형 검출되면 그리는 부분
	{
		Draw_Line(max_vertex[0], max_vertex[1], -2);
		Draw_Line(max_vertex[1], max_vertex[2], -3);
		Draw_Line(max_vertex[2], max_vertex[3], -4);
		Draw_Line(max_vertex[3], max_vertex[0], -5);
		pre_rectangle.check = true;
		pre_rectangle.size = (int)SignArea_rect(max_vertex[0], max_vertex[1], max_vertex[2], max_vertex[3]);
		pre_rectangle.cnt = 10;
		pre_rectangle.center.x = (max_vertex[0].x + max_vertex[1].x + max_vertex[2].x + max_vertex[3].x) / 4;
		pre_rectangle.center.y = (max_vertex[0].y + max_vertex[1].y + max_vertex[2].y + max_vertex[3].y) / 4;
		pre_rectangle.R = RGB[pre_rectangle.center.y][pre_rectangle.center.x][RED];
		pre_rectangle.G = RGB[pre_rectangle.center.y][pre_rectangle.center.x][GREEN];
		pre_rectangle.B = RGB[pre_rectangle.center.y][pre_rectangle.center.x][BLUE];

		Edge_data[pre_rectangle.center.y][pre_rectangle.center.x] = -10;//중간점
	}
	else
	{
		pre_rectangle.check = false;
		if (pre_rectangle.cnt > 0)
			pre_rectangle.cnt--;
		Edge_data[pre_rectangle.center.y][pre_rectangle.center.x] = -11;//사각형 없으면 가장 최근 중간점
	}

	Edge_expansion(pre_rectangle.center.y, pre_rectangle.center.x, 1);//중간점 팽창

	//색칠
	for (j = 0; j < HEIGHT_2; j++) { // height
		for (i = 0; i < WIDTH_2; i++) { //width
			switch (Edge_data[j][i])
			{
			case -2:
				RGB[j][i][RED] = 255;
				RGB[j][i][GREEN] = 0;
				RGB[j][i][BLUE] = 0;
				break;
			case -3:
				RGB[j][i][RED] = 255;
				RGB[j][i][GREEN] = 255;
				RGB[j][i][BLUE] = 0;
				break;
			case -4:
				RGB[j][i][RED] = 0;
				RGB[j][i][GREEN] = 255;
				RGB[j][i][BLUE] = 0;
				break;
			case -5:
				RGB[j][i][RED] = 0;
				RGB[j][i][GREEN] = 0;
				RGB[j][i][BLUE] = 255;
				break;
			case -10:
				RGB[j][i][RED] = 0;
				RGB[j][i][GREEN] = 255;
				RGB[j][i][BLUE] = 0;
				break;
			case -11:
				RGB[j][i][RED] = 255;
				RGB[j][i][GREEN] = 0;
				RGB[j][i][BLUE] = 0;
				break;
			}
		}
	}
	
	//////////////////////////////////////////////////////////////////////////////////

	//확대
	for (j = 0; j < HEIGHT_2; j++) { // height
		for (i = 0; i < WIDTH_2; i++) { //width
			lpVHdr->lpData[(nWidth*j * 2 + i * 2) * 3] = RGB[j][i][BLUE];
			lpVHdr->lpData[(nWidth*j * 2 + i * 2) * 3 + 1] = RGB[j][i][GREEN];
			lpVHdr->lpData[(nWidth*j * 2 + i * 2) * 3 + 2] = RGB[j][i][RED];

			lpVHdr->lpData[(nWidth*j * 2 + i * 2 + 1) * 3] = RGB[j][i][BLUE];
			lpVHdr->lpData[(nWidth*j * 2 + i * 2 + 1) * 3 + 1] = RGB[j][i][GREEN];
			lpVHdr->lpData[(nWidth*j * 2 + i * 2 + 1) * 3 + 2] = RGB[j][i][RED];

			lpVHdr->lpData[(nWidth*(j * 2 + 1) + i * 2) * 3] = RGB[j][i][BLUE];
			lpVHdr->lpData[(nWidth*(j * 2 + 1) + i * 2) * 3 + 1] = RGB[j][i][GREEN];
			lpVHdr->lpData[(nWidth*(j * 2 + 1) + i * 2) * 3 + 2] = RGB[j][i][RED];

			lpVHdr->lpData[(nWidth*(j * 2 + 1) + i * 2 + 1) * 3] = RGB[j][i][BLUE];
			lpVHdr->lpData[(nWidth*(j * 2 + 1) + i * 2 + 1) * 3 + 1] = RGB[j][i][GREEN];
			lpVHdr->lpData[(nWidth*(j * 2 + 1) + i * 2 + 1) * 3 + 2] = RGB[j][i][RED];
		}
	}

	return (LRESULT)true;
}
