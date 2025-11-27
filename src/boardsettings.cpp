// BoardSettings.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "DigitShowBasic.h"
#include "DigitShowBasicDoc.h"

#include "BoardSettings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBoardSettings ダイアログ

//---boards---
extern	int			NUMAD,NUMDA;
extern	short		AdChannels[2],DaChannels[1];		// The Number of Channel
extern	short		AdRange[2],DaRange[1];				// Range of A/D and D/A boards
extern	short		AdResolution[2],DaResolution[1];	// Resolution of A/D and D/A Boards
extern	short		AdInputMethod[2];					// Input Method of A/D

CBoardSettings::CBoardSettings(CWnd* pParent /*=NULL*/)
	: CDialog(CBoardSettings::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBoardSettings)
	m_ADMethod0 = _T("");
	m_ADMethod1 = _T("");
	m_ADResolution0 = _T("");
	m_ADResolution1 = _T("");
	m_ADRange0 = _T("");
	m_ADRange1 = _T("");
	m_ADMaxChannel0 = _T("");
	m_ADMaxChannel1 = _T("");
	m_DAMaxChannel0 = _T("");
	m_DARange0 = _T("");
	m_DAResolution0 = _T("");
	//}}AFX_DATA_INIT
}


void CBoardSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBoardSettings)
	DDX_Text(pDX, IDC_EDIT_ADMethod0, m_ADMethod0);
	DDX_Text(pDX, IDC_EDIT_ADMethod1, m_ADMethod1);
	DDX_Text(pDX, IDC_EDIT_ADResolution0, m_ADResolution0);
	DDX_Text(pDX, IDC_EDIT_ADResolution1, m_ADResolution1);
	DDX_Text(pDX, IDC_EDIT_ADRange0, m_ADRange0);
	DDX_Text(pDX, IDC_EDIT_ADRange1, m_ADRange1);
	DDX_Text(pDX, IDC_EDIT_ADMaxChannel0, m_ADMaxChannel0);
	DDX_Text(pDX, IDC_EDIT_ADMaxChannel1, m_ADMaxChannel1);
	DDX_Text(pDX, IDC_EDIT_DAMaxChannel0, m_DAMaxChannel0);
	DDX_Text(pDX, IDC_EDIT_DARange0, m_DARange0);
	DDX_Text(pDX, IDC_EDIT_DAResolution0, m_DAResolution0);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBoardSettings, CDialog)
	//{{AFX_MSG_MAP(CBoardSettings)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBoardSettings メッセージ ハンドラ
BOOL CBoardSettings::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: この位置に初期化の補足処理を追加してください
	if(NUMAD >0 && AdInputMethod[0]==0) m_ADMethod0="Single Input";
	if(NUMAD >0 && AdInputMethod[0]==1) m_ADMethod0="Differential Input";
	if(NUMAD >1 && AdInputMethod[1]==0) m_ADMethod1="Single Input";
	if(NUMAD >1 && AdInputMethod[1]==1) m_ADMethod1="Differential Input";
	if(NUMAD >0 && AdResolution[0]==12) m_ADResolution0="12 bit";
	if(NUMAD >0 && AdResolution[0]==16) m_ADResolution0="16 bit";
	if(NUMAD >1 && AdResolution[1]==12) m_ADResolution1="12 bit";
	if(NUMAD >1 && AdResolution[1]==16) m_ADResolution1="16 bit";

	if(NUMAD >0 && AdRange[0]==0) m_ADRange0="-10V   +10V";
	if(NUMAD >0 && AdRange[0]==1) m_ADRange0="-5V   +5V";
	if(NUMAD >0 && AdRange[0]==50) m_ADRange0="0V   +10V";
	if(NUMAD >0 && AdRange[0]==51) m_ADRange0="0V   +5V";
	if(NUMAD >1 && AdRange[1]==0) m_ADRange1="-10V   +10V";
	if(NUMAD >1 && AdRange[1]==1) m_ADRange1="-5V   +5V";
	if(NUMAD >1 && AdRange[1]==50) m_ADRange1="0V   +10V";
	if(NUMAD >1 && AdRange[1]==51) m_ADRange1="0V   +5V";

	if(NUMAD >0) m_ADMaxChannel0.Format("%3d",AdChannels[0]/2);
	if(NUMAD >1) m_ADMaxChannel1.Format("%3d",AdChannels[1]/2);
	
	if(NUMDA >0 && DaResolution[0]==12) m_DAResolution0="12 bit";
	if(NUMDA >0 && DaResolution[0]==16) m_DAResolution0="16 bit";

	if(NUMDA >0 && DaRange[0]==0) m_DARange0="-10V   +10V";
	if(NUMDA >0 && DaRange[0]==1) m_DARange0="-5V   +5V";
	if(NUMDA >0 && DaRange[0]==50) m_DARange0="0V   +10V";
	if(NUMDA >0 && DaRange[0]==51) m_DARange0="0V   +5V";
	if(NUMDA >0) m_DAMaxChannel0.Format("%3d",DaChannels[0]);
	UpdateData(FALSE);	
	return TRUE;  // コントロールにフォーカスを設定しないとき、戻り値は TRUE となります
	              // 例外: OCX プロパティ ページの戻り値は FALSE となります
}
