#pragma comment(lib, "vfw32.lib")
//#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")

#include "vfw.h"
#include "afxwin.h"
#include <stdio.h>

#define    _DEF_WEBCAM        1000

LRESULT CALLBACK CallbackOnFrame(HWND hWnd, LPVIDEOHDR lpVHdr);

// CImageProcessBaseDlg 대화 상자
class CImageProcessBaseDlg : public CDialog
{
// 생성입니다.
public:
	CImageProcessBaseDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.
	void OnDestroy();
	afx_msg void OnBnClickedOk();

	HWND m_Cap;
	CStatic m_stDisplay;

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_IMAGEPROCESSBASE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
};
