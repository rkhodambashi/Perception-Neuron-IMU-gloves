// demo_MFCDlg.cpp : implementation file

#include "stdafx.h"
#include "demo_MFC.h"
#include "demo_MFCDlg.h"
#include "afxdialogex.h"
#include <array>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <fstream>
#include <time.h>
#include <ctime>
#include <stack>
#include <math.h>
//#include "Leap.h"
//#include <myo/myo.hpp>
#include <winsock2.h>
#include <stdio.h>
#include <strsafe.h>
#include <WS2tcpip.h>
//#include "dynamixel.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Load required libraries
#ifdef _WIN64
#pragma comment(lib, "../../x64/NeuronDataReader.lib")//Add Lib
#elif defined _WIN32
#pragma comment(lib, "../../x86/NeuronDataReader.lib")//Add Lib
#pragma comment(lib,"ws2_32.lib")
#endif

//using namespace Leap;
using namespace std;

// UDP set up
#define SERVER "127.0.0.1" //ip address of udp server
#define BUFLEN 512  //Max length of buffer
#define PORT 8888  //The port on which to listen for incoming data

//Dynamixel Setup
// Control table address
#define P_GOAL_POSITION_L		30
#define P_GOAL_POSITION_H		31
#define P_PRESENT_POSITION_L	36
#define P_PRESENT_POSITION_H	37
#define P_MOVING				46

int P_GOAL_POSITION_Thumb;
int P_GOAL_POSITION_Index;
int P_GOAL_POSITION_Middle;
int P_GOAL_POSITION_Ring;
int P_GOAL_POSITION_Pinky;

// Defulat setting
#define DEFAULT_PORTNUM		6 // COM3
#define DEFAULT_BAUDNUM		7 // 1Mbps
#define DEFAULT_ID			4



// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// Cdemo_MFCDlg dialog

Cdemo_MFCDlg::Cdemo_MFCDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(Cdemo_MFCDlg::IDD, pParent)	
	, m_strIPAddress(_T(""))
	, m_strTCPPort(_T(""))
	, m_strUDPPort(_T(""))

    // Calc data display control
    , m_calcPx( _T( "" ) )
    , m_calcPy( _T( "" ) )
    , m_calcPz( _T( "" ) )
           
    , m_calcVx( _T( "" ) )
    , m_calcVy( _T( "" ) )
    , m_calcVz( _T( "" ) )
            
    , m_calcQs( _T( "" ) )
    , m_calcQx( _T( "" ) )
    , m_calcQy( _T( "" ) )
    , m_calcQz( _T( "" ) )
             
    , m_calcAx( _T( "" ) )
    , m_calcAy( _T( "" ) )
    , m_calcAz( _T( "" ) )
            
    , m_calcGx( _T( "" ) )
    , m_calcGy( _T( "" ) )
    , m_calcGz( _T( "" ) )
    
    , m_bvhAngleZ( _T( "" ) )
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	
	sockTCPRef = NULL;
	sockUDPRef = NULL;
	myFile.open("example.csv");

}

void Cdemo_MFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TEXT1, m_strIPAddress);
	DDX_Text(pDX, IDC_TEXT2, m_strTCPPort);
	DDX_Text(pDX, IDC_UDP, m_strUDPPort);
	DDX_Text(pDX, IDC_STATIC_AX, m_calcAx);
	DDX_Text(pDX, IDC_STATIC_AY, m_calcAy);
	DDX_Text(pDX, IDC_STATIC_AZ, m_calcAz);
	DDX_Text(pDX, IDC_STATIC_GX, m_calcGx);
	DDX_Text(pDX, IDC_STATIC_GY, m_calcGy);
	DDX_Text(pDX, IDC_STATIC_GZ, m_calcGz);
	DDX_Text(pDX, IDC_STATIC_PX, m_calcPx);
	DDX_Text(pDX, IDC_STATIC_PY, m_calcPy);
	DDX_Text(pDX, IDC_STATIC_QS, m_calcQs);
	DDX_Text(pDX, IDC_STATIC_QX, m_calcQx);
	DDX_Text(pDX, IDC_STATIC_QY, m_calcQy);
	DDX_Text(pDX, IDC_STATIC_QZ, m_calcQz);
	DDX_Text(pDX, IDC_STATIC_VX, m_calcVx);
	DDX_Text(pDX, IDC_STATIC_VY, m_calcVy);
	DDX_Text(pDX, IDC_STATIC_VZ, m_calcVz);
	DDX_Text(pDX, IDC_STATIC_PZ, m_calcPz);
	DDX_Control(pDX, IDC_COMBO_SELECTION_BONE_INDEX, m_wndComBoxBone);
	DDX_Text(pDX, IDC_STATIC_ANGLE_Z, m_bvhAngleZ);
	DDX_Control(pDX, IDC_COMBO_SELECTION_BONE_INDEX2, m_wndComBoxBone2);
}

BEGIN_MESSAGE_MAP(Cdemo_MFCDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDCANCEL, &Cdemo_MFCDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_TCP_CONNECTION, &Cdemo_MFCDlg::OnBnClickedButtonTcpConnection)
	ON_BN_CLICKED(IDC_BUTTON_UDP_CONNECTION, &Cdemo_MFCDlg::OnBnClickedButtonUdpConnection)
	ON_CBN_SELCHANGE(IDC_COMBO_SELECTION_BONE_INDEX, &Cdemo_MFCDlg::OnCbnSelchangeComboSelectionBoneIndex)
    ON_MESSAGE( WM_UPDATE_MESSAGE, OnUpdateMessage )
	ON_CBN_SELCHANGE(IDC_COMBO_SELECTION_BONE_INDEX2, &Cdemo_MFCDlg::OnCbnSelchangeComboSelectionBoneIndex2)
END_MESSAGE_MAP()


// Cdemo_MFCDlg message handlers

BOOL Cdemo_MFCDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
    
	// TODO: Add extra initialization here
	BRRegisterFrameDataCallback(this, bvhFrameDataReceived);

    BRRegisterCalculationDataCallback(this, CalcFrameDataReceive);

	m_strIPAddress = L"127.0.0.1";//Default IP Address
	m_strTCPPort = L"7001";		//Default Port

	m_strUDPPort = L"7001";		// Default UDP Port
	    
    UpdateBvhDataShowUI();	//update the BoneID index when BVH data received
	UpdateCalcDataShowUI();  //update the BoneID index when Calc data received

	UpdateData(FALSE);//update the Default IP Address and Port, and the Default UDP Port
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void __stdcall Cdemo_MFCDlg::bvhFrameDataReceived(void* customedObj, SOCKET_REF sender, BvhDataHeader* header, float* data)
{
	Cdemo_MFCDlg* pthis = (Cdemo_MFCDlg*)customedObj;
	pthis->showBvhBoneInfo(sender, header, data);
}

void __stdcall Cdemo_MFCDlg::CalcFrameDataReceive( void* customedObj, SOCKET_REF sender, CalcDataHeader* header, float* data )
{
    Cdemo_MFCDlg* pthis = (Cdemo_MFCDlg*)customedObj;
    pthis->showCalcBoneInfo( sender, header, data );    
}

void Cdemo_MFCDlg::showBvhBoneInfo(SOCKET_REF sender, BvhDataHeader* header, float* data)
{
	USES_CONVERSION;
	// show frame index
	char strFrameIndex[60];
	_itoa_s(header->FrameIndex, strFrameIndex, 10);		//int transform into string by decimalism
	GetDlgItem(IDC_STATIC_FRAME_INDEX)->SetWindowText(A2W(strFrameIndex));

	// ADDED

	struct sockaddr_in si_other;
	int s, slen = sizeof(si_other);
	char buf[BUFLEN];
	char message[BUFLEN];
	WSADATA wsa;

	//Initialise winsock
	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	printf("Initialised.\n");

	//create socket
	//if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET)
	{
		printf("socket() failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	char str[INET_ADDRSTRLEN];
	//setup address structure
	memset((char *)&si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(PORT);
	//si_other.sin_addr.S_un.S_addr = inet_pton(AF_INET, SERVER, &(si_other.sin_addr));
	inet_pton(AF_INET, SERVER, &(si_other.sin_addr.S_un.S_addr));
	/*if (inet_pton(AF_INET, SERVER, &(si_other.sin_addr.S_un.S_addr)))
	{
	inet_ntop(AF_INET, &(si_other.sin_addr.S_un.S_addr), str, INET_ADDRSTRLEN);
	std::cout << ("%s\n", str);
	}*/

	// END ADDED

	// calculate data index for selected bone
	int dataIndex = 0;
    int curSel = m_wndComBoxBone.GetCurSel();   // Gets the currently selected option in the drop down box
    if ( curSel == CB_ERR ) return;

	if (header->WithDisp)
	{
		dataIndex = curSel * 6;
		if (header->WithReference)
		{
			dataIndex += 6;
		}
		// Displacement VALUES HERE
		float dispX = data[dataIndex + 0];
		float dispY = data[dataIndex + 1];
		float dispZ = data[dataIndex + 2];
		//myFile << dispX;
		//myFile << ",";
		//myFile << dispY;
		//myFile << ",";
		//myFile << dispZ;
		//myFile << "\n";

		char strBuff[32];
		sprintf_s(strBuff, sizeof(strBuff), "%0.3f", dispX);
		GetDlgItem(IDC_STATIC_DISP_X)->SetWindowText(A2W(strBuff));

		sprintf_s(strBuff, sizeof(strBuff), "%0.3f", dispY);
		GetDlgItem(IDC_STATIC_DISP_Y)->SetWindowText(A2W(strBuff));

		sprintf_s(strBuff, sizeof(strBuff), "%0.3f", dispZ);
		GetDlgItem(IDC_STATIC_DISP_Z)->SetWindowText(A2W(strBuff));

		// Rotational VALUES HERE
		float angX = data[dataIndex + 4];
		float angY = data[dataIndex + 3];
		float angZ = data[dataIndex + 5];

		
		// Write Rotational Values
		
		//dxl_write_word(DEFAULT_ID, P_GOAL_POSITION_L, fingDis[1]);

		// Left Arm
		/*myFile << data[37 + 4];
		myFile << ",";
		myFile << data[37 + 3];
		myFile << ",";
		myFile << data[37 + 5];
		myFile << ",";

		// Left Forearm
		myFile << data[38 + 4];
		myFile << ",";
		myFile << data[38 + 3];
		myFile << ",";
		myFile << data[38 + 5];
		myFile << ",";

		// Left Hand
		myFile << data[39 + 4];
		myFile << ",";
		myFile << data[39 + 3];
		myFile << ",";
		myFile << data[39 + 5];
		myFile << ",";

		// Left Hand Thumb 1
		myFile << data[40 + 4];
		myFile << ",";
		myFile << data[40 + 3];
		myFile << ",";
		myFile << data[40 + 5];
		myFile << ",";;

		// Left Hand Thumb 2
		myFile << data[41 + 4];
		myFile << ",";
		myFile << data[41 + 3];
		myFile << ",";
		myFile << data[41 + 5];
		myFile << ",";;

		// Left Hand Thumb 3
		myFile << data[42 + 4];
		myFile << ",";
		myFile << data[42 + 3];
		myFile << ",";
		myFile << data[42 + 5];
		myFile << ",";

		// Left Hand Index 
		myFile << data[43 + 4];
		myFile << ",";
		myFile << data[43 + 3];
		myFile << ",";
		myFile << data[43 + 5];
		myFile << ",";

		// Left Hand Index 1
		myFile << data[44 + 4];
		myFile << ",";
		myFile << data[44 + 3];
		myFile << ",";
		myFile << data[44 + 5];
		myFile << ",";

		// Left Hand Index 2
		myFile << data[45 + 4];
		myFile << ",";
		myFile << data[45 + 3];
		myFile << ",";
		myFile << data[45 + 5];
		myFile << ",";

		// Left Hand Index 3
		myFile << data[46 + 4];
		myFile << ",";
		myFile << data[46 + 3];
		myFile << ",";
		myFile << data[46 + 5];
		myFile << ",";

		// Left Hand Middle 
		myFile << data[47 + 4];
		myFile << ",";
		myFile << data[47 + 3];
		myFile << ",";
		myFile << data[47 + 5];
		myFile << ",";

		// Left Hand Middle 1
		myFile << data[48 + 4];
		myFile << ",";
		myFile << data[48 + 3];
		myFile << ",";
		myFile << data[48 + 5];
		myFile << "\n";

		// Left Hand Middle 2
		myFile << data[49 + 4];
		myFile << ",";
		myFile << data[49 + 3];
		myFile << ",";
		myFile << data[49 + 5];
		myFile << ",";

		// Left Hand Middle 3
		myFile << data[50 + 4];
		myFile << ",";
		myFile << data[50 + 3];
		myFile << ",";
		myFile << data[50 + 5];
		myFile << ",";

		// Left Hand Ring
		myFile << data[51 + 4];
		myFile << ",";
		myFile << data[51 + 3];
		myFile << ",";
		myFile << data[51 + 5];
		myFile << ",";

		// Left Hand Ring 1
		myFile << data[52 + 4];
		myFile << ",";
		myFile << data[52 + 3];
		myFile << ",";
		myFile << data[52 + 5];
		myFile << ",";

		// Left Hand Ring 2
		myFile << data[53 + 4];
		myFile << ",";
		myFile << data[53 + 3];
		myFile << ",";
		myFile << data[53 + 5];
		myFile << ",";

		// Left Hand Ring 3
		myFile << data[54 + 4];
		myFile << ",";
		myFile << data[54 + 3];
		myFile << ",";
		myFile << data[54 + 5];
		myFile << ",";

		// Left Hand Pinky
		myFile << data[55 + 4];
		myFile << ",";
		myFile << data[55 + 3];
		myFile << ",";
		myFile << data[55 + 5];
		myFile << ",";

		// Left Hand Pinky 1
		myFile << data[56 + 4];
		myFile << ",";
		myFile << data[56 + 3];
		myFile << ",";
		myFile << data[56 + 5];
		myFile << ",";

		// Left Hand Pinky 2
		myFile << data[57 + 4];
		myFile << ",";
		myFile << data[57 + 3];
		myFile << ",";
		myFile << data[57 + 5];
		myFile << ",";

		// Left Hand Pinky 3
		myFile << data[58 + 4];
		myFile << ",";
		myFile << data[58 + 3];
		myFile << ",";
		myFile << data[58 + 5];
		myFile << "\n";*/

//		hub.run(1000/20);


		sprintf_s(strBuff, sizeof(strBuff), "%0.3f", angX);
		GetDlgItem(IDC_STATIC_ANGLE_X)->SetWindowText(A2W(strBuff));

		sprintf_s(strBuff, sizeof(strBuff), "%0.3f", angY);
		GetDlgItem(IDC_STATIC_ANGLE_Y)->SetWindowText(A2W(strBuff));

		sprintf_s(strBuff, sizeof(strBuff), "%0.3f", angZ);
		GetDlgItem(IDC_STATIC_ANGLE_Z)->SetWindowText(A2W(strBuff));

		char strBvhData[64];
		sprintf_s(strBvhData, "%0.3f\t", dispX);
		sprintf_s(strBvhData, "%0.3f\t", dispY);
		sprintf_s(strBvhData, "%0.3f\t", dispZ);

		sprintf_s(strBvhData, "%0.3f\t", angX);
		sprintf_s(strBvhData, "%0.3f\t", angY);
		sprintf_s(strBvhData, "%0.3f\t", angZ);
	}
	else
	{
		if (curSel == 0)
		{
			dataIndex = 0;
			if (header->WithReference)
			{
				dataIndex += 6;
			}

			// show hip's displacement
			float dispX = data[dataIndex + 0];
			float dispY = data[dataIndex + 1];
			float dispZ = data[dataIndex + 2];

			char strBuff[32];
			sprintf_s(strBuff, sizeof(strBuff), "%0.3f", dispX);
			GetDlgItem(IDC_STATIC_DISP_X)->SetWindowText(A2W(strBuff));

			sprintf_s(strBuff, sizeof(strBuff), "%0.3f", dispY);
			GetDlgItem(IDC_STATIC_DISP_Y)->SetWindowText(A2W(strBuff));

			sprintf_s(strBuff, sizeof(strBuff), "%0.3f", dispZ);
			GetDlgItem(IDC_STATIC_DISP_Z)->SetWindowText(A2W(strBuff));

			// show hip's angle
			float angX = data[dataIndex + 4];
			float angY = data[dataIndex + 3];
			float angZ = data[dataIndex + 5];

			sprintf_s(strBuff, sizeof(strBuff), "%0.3f", angX);
			GetDlgItem(IDC_STATIC_ANGLE_X)->SetWindowText(A2W(strBuff));

			sprintf_s(strBuff, sizeof(strBuff), "%0.3f", angY);
			GetDlgItem(IDC_STATIC_ANGLE_Y)->SetWindowText(A2W(strBuff));

			sprintf_s(strBuff, sizeof(strBuff), "%0.3f", angZ);
			GetDlgItem(IDC_STATIC_ANGLE_Z)->SetWindowText(A2W(strBuff));


			char strBvhData[64];
			sprintf_s(strBvhData, "%0.3f\t", dispX);
			sprintf_s(strBvhData, "%0.3f\t", dispY);
			sprintf_s(strBvhData, "%0.3f\t", dispZ);

			sprintf_s(strBvhData, "%0.3f\t", angX);
			sprintf_s(strBvhData, "%0.3f\t", angY);
			sprintf_s(strBvhData, "%0.3f\t", angZ);
		}
		else
		{
			dataIndex = 3 + curSel * 3;
			if (header->WithReference)
			{
				dataIndex += 6;
			}

			// show displacement
			char strBuff[32];
			sprintf_s(strBuff, sizeof(strBuff), "NaN");
			GetDlgItem(IDC_STATIC_DISP_X)->SetWindowText(A2W(strBuff));
			GetDlgItem(IDC_STATIC_DISP_Y)->SetWindowText(A2W(strBuff));
			GetDlgItem(IDC_STATIC_DISP_Z)->SetWindowText(A2W(strBuff));

			// show angle
			float angX = data[dataIndex + 1];
			float angY = data[dataIndex + 0];
			float angZ = data[dataIndex + 2];

			// ADDED CODE 2
			float forearm_x = data[38 * 3 + 10], forearm_y = data[38 * 3 + 9], forearm_z = data[38 * 3 + 11];
			float hand_x = data[39 * 3 + 10], hand_y = data[39 * 3 + 9], hand_z = data[39 * 3 + 11];
			float thumb_y = data[42 * 3 + 10];
			float index_z = data[45 * 3 + 11];
			float middle_z = data[50 * 3 + 11];
			float ring_z = data[54 * 3 + 11];
			float pinky_z = data[58 * 3 + 11];

			//send over UDP
			string tmp = to_string(forearm_x) + " " + to_string(forearm_y) + " " + to_string(forearm_z) + " " + to_string(hand_x) + " " + to_string(hand_y) + " " + to_string(hand_z) + " " + to_string(thumb_y) + " " + to_string(index_z) + " " + to_string(middle_z) + " " + to_string(ring_z) + " " + to_string(pinky_z);
			//string tmp = "data";
			strcpy_s(message, tmp.c_str());
			//send message
			if (sendto(s, message, strlen(message), 0, (struct sockaddr *) &si_other, slen) == SOCKET_ERROR)
			{
				//printf("sendto() failed with error code : %d", WSAGetLastError());
				exit(EXIT_FAILURE);
			}
			// END ADDED CODE 2

			sprintf_s(strBuff, sizeof(strBuff), "%0.3f", angX);
			GetDlgItem(IDC_STATIC_ANGLE_X)->SetWindowText(A2W(strBuff));

			sprintf_s(strBuff, sizeof(strBuff), "%0.3f", angY);
			GetDlgItem(IDC_STATIC_ANGLE_Y)->SetWindowText(A2W(strBuff));

			sprintf_s(strBuff, sizeof(strBuff), "%0.3f", angZ);
			GetDlgItem(IDC_STATIC_ANGLE_Z)->SetWindowText(A2W(strBuff));
			
			char strBvhData[64];
			
			sprintf_s(strBvhData, "%0.3f\t", angX);
			sprintf_s(strBvhData, "%0.3f\t", angY);
			sprintf_s(strBvhData, "%0.3f\t", angZ);
		}
	}
}

LRESULT Cdemo_MFCDlg::OnUpdateMessage( WPARAM wParam, LPARAM lParam )
{
    UpdateData( FALSE );
    return 0;
}

void Cdemo_MFCDlg::showCalcBoneInfo( SOCKET_REF sender, CalcDataHeader* header, float* data )
{
    USES_CONVERSION;

    // show frame index
    char strFrameIndex[60];
    _itoa_s( header->FrameIndex, strFrameIndex, 10 );		//int transform into string by decimalism
    GetDlgItem( IDC_STATIC_FRAME_INDEX )->SetWindowText( A2W( strFrameIndex ) );
    
    int dataIndex = 0;
    int curSel = m_wndComBoxBone2.GetCurSel(); //Gets the currently selected option in the drop down box
    if ( curSel == CB_ERR ) return;

    if ( curSel > CalcBoneCount ) return;

    dataIndex = 16 * curSel;
    
    CString tmpData;
    tmpData.Format( L"%0.3f", data[dataIndex + 0] );
    m_calcPx = tmpData;
    tmpData.Format( L"%0.3f", data[dataIndex + 1] );
    m_calcPy = tmpData;
    tmpData.Format( L"%0.3f", data[dataIndex + 2] );
    m_calcPz = tmpData;
    tmpData.Format( L"%0.3f", data[dataIndex + 3] );
    m_calcVx = tmpData;
    tmpData.Format( L"%0.3f", data[dataIndex + 4] );
    m_calcVy = tmpData;
    tmpData.Format( L"%0.3f", data[dataIndex + 5] );
    m_calcVz = tmpData;
    tmpData.Format( L"%0.3f", data[dataIndex + 6] );
    m_calcQs = tmpData;
    tmpData.Format( L"%0.3f", data[dataIndex + 7] );
    m_calcQx = tmpData;
    tmpData.Format( L"%0.3f", data[dataIndex + 8] );
    m_calcQy = tmpData;
    tmpData.Format( L"%0.3f", data[dataIndex + 9] );
    m_calcQz = tmpData;
    tmpData.Format( L"%0.3f", data[dataIndex + 10] );
    m_calcAx = tmpData;
    tmpData.Format( L"%0.3f", data[dataIndex + 11] );
    m_calcAy = tmpData;
    tmpData.Format( L"%0.3f", data[dataIndex + 12] );
    m_calcAz = tmpData;
    tmpData.Format( L"%0.3f", data[dataIndex + 13] );
    m_calcGx = tmpData;
    tmpData.Format( L"%0.3f", data[dataIndex + 14] );
    m_calcGy = tmpData;
    tmpData.Format( L"%0.3f", data[dataIndex + 15] );
    m_calcGz = tmpData;

    PostMessage(WM_UPDATE_MESSAGE,0,0);//OK － UpdateDate
    
}

void Cdemo_MFCDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void Cdemo_MFCDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR Cdemo_MFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void Cdemo_MFCDlg::OnBnClickedCancel()
{
	CDialogEx::OnCancel();
}

void Cdemo_MFCDlg::OnBnClickedButtonTcpConnection()
{
	UpdateData();

	if (sockTCPRef)
	{
		// close socket
		BRCloseSocket(sockTCPRef);

		// reconnect
		sockTCPRef = NULL;

		// change the title of button
		GetDlgItem(IDC_BUTTON_TCP_CONNECTION)->SetWindowText(L"Connect");
	}
	else
	{
		USES_CONVERSION;
		
		// get port number
		char* port = W2A(m_strTCPPort);
		long nPort = 0;
		try
		{
			nPort = atoi(port);
		}
		catch (CException* e)
		{
			AfxMessageBox(L"Port number error", MB_OK);
			return;
		}
		
		// connect to remote server
		sockTCPRef = BRConnectTo(W2A(m_strIPAddress), nPort);

		// if success, change the title of button
		if (sockTCPRef)
		{
			GetDlgItem(IDC_BUTTON_TCP_CONNECTION)->SetWindowText(L"Disconnect");
		}
		else
		{
			// if failed, show message
			AfxMessageBox(A2W(BRGetLastErrorMessage()), MB_OK);
		}
	}
}


void Cdemo_MFCDlg::OnBnClickedButtonUdpConnection()
{
	UpdateData();

	if (sockUDPRef)
	{
		// close socket
		BRCloseSocket(sockUDPRef);

		// reconnect
		sockUDPRef = NULL;

		// change the title of button
		GetDlgItem(IDC_BUTTON_UDP_CONNECTION)->SetWindowText(L"Connect");
	}
	else
	{
		USES_CONVERSION;

		// get port number
		char* UDPport = W2A(m_strUDPPort);
		long nUDPPort = 0;
		try
		{
			nUDPPort = atoi(UDPport);
		}
		catch (CException* e)
		{
			AfxMessageBox(L"UDPPort number error", MB_OK);
			return;
		}

		// connect to remote server
		sockUDPRef = BRStartUDPServiceAt(nUDPPort);

		// if success, change the title of button
		if (sockUDPRef)
		{
			GetDlgItem(IDC_BUTTON_UDP_CONNECTION)->SetWindowText(L"Disconnect");
		}
		else
		{
			// if failed, show message
			AfxMessageBox(A2W(BRGetLastErrorMessage()), MB_OK);
		}
	}
}


void Cdemo_MFCDlg::UpdateBvhDataShowUI()
{
    CString BoneID;
    m_wndComBoxBone.ResetContent();		//重置列表框里的内容
    for ( int i = 0; i < BVHBoneCount; i++ )
    {
        BoneID.Format( L"%s%d", L"Bone", i );
        m_wndComBoxBone.AddString( BoneID );
    }
    m_wndComBoxBone.SetCurSel( 0 );
}

void Cdemo_MFCDlg::UpdateCalcDataShowUI()
{
    CString BoneID;
    m_wndComBoxBone2.ResetContent();
    for ( int i = 0; i < CalcBoneCount; i++ )
    {
        BoneID.Format( L"%s%d", L"Bone", i );
        m_wndComBoxBone2.AddString( BoneID );
    }
    m_wndComBoxBone2.SetCurSel( 0 );
}


void Cdemo_MFCDlg::OnCbnSelchangeComboSelectionBoneIndex()
{
	UpdateData();
}


void Cdemo_MFCDlg::OnCbnSelchangeComboSelectionBoneIndex2()
{
	UpdateData();
}

