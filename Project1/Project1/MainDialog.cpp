//  MainDialog.cpp: implementation of CMainDialog (main dialog of KhuCvApp)
//	Dept. Software Convergence, Kyung Hee University
//	Prof. Daeho Lee, nize@khu.ac.kr
//

#include "KhuCvApp.h"

#include <chrono>


#ifdef _MSC_VER
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif
#endif

BEGIN_EVENT_TABLE(CMainDialog, wxDialog)
EVT_BUTTON(IDC_SEL_FILE_FOLDER, CMainDialog::OnSelSrcFileFolder)
EVT_BUTTON(IDC_SEL_DES_FOLDER, CMainDialog::OnSelDesFileFolder)
EVT_LIST_ITEM_ACTIVATED(IDC_FILE_LIST_CTRL, CMainDialog::OnActivatedFileListCtrl)
EVT_BUTTON(IDC_RUN, CMainDialog::OnRun)
EVT_BUTTON(IDC_PAUSE, CMainDialog::OnPause)
EVT_BUTTON(IDC_EXAMPLE, CMainDialog::OnExample)
EVT_TIMER(-1, CMainDialog::OnTimer)
END_EVENT_TABLE()

CMainDialog::CMainDialog(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
	: wxDialog(parent, id, title, pos, size, style),
	m_SequenceRunTimer(this, ID_TIMER_SEQUENCE_RUN), m_VideoRunTimer(this, ID_TIMER_VIDEO_RUN), m_CamRunTimer(this, ID_TIMER_CAM_RUN)
{
	m_pVbox = new wxBoxSizer(wxVERTICAL);
	
	for(int i = 0 ;i < H_BOX_SIZER_NUM; ++i)
		m_pHbox[i] = new wxBoxSizer(wxHORIZONTAL);

	m_pSrcFolderSelButton = new wxButton(this, IDC_SEL_FILE_FOLDER, wxT("..."), wxDefaultPosition, wxSize(30, 20));
	m_pDisplaySrcPathText = new wxStaticText(this, IDC_DISPLAY_SRC_PATH, wxT(""), wxDefaultPosition, wxSize(180, 20), wxST_NO_AUTORESIZE);
	m_pVideoFileCheck = new wxCheckBox(this, IDC_VIDEO_FILE_CHECK, wxT("Video file"), wxDefaultPosition, wxSize(90, 20));

	m_pHbox[0]->Add(m_pSrcFolderSelButton, 0);
	m_pHbox[0]->Add(m_pDisplaySrcPathText, 1, wxLEFT, 5);
	m_pHbox[0]->Add(m_pVideoFileCheck, 0, wxLEFT, 5);

	m_pDesFolderSelButton = new wxButton(this, IDC_SEL_DES_FOLDER, wxT("..."), wxDefaultPosition, wxSize(30, 20));
	m_pDisplayDesPathText = new wxStaticText(this, IDC_DISPLAY_DES_PATH, wxT(""), wxDefaultPosition, wxSize(180, 20), wxST_NO_AUTORESIZE);
	m_pSaveFrameCheck = new wxCheckBox(this, IDC_SAVE_FRAME_CHECK, wxT("Frame save"), wxDefaultPosition, wxSize(90, 20));

	m_pHbox[1]->Add(m_pDesFolderSelButton, 0);
	m_pHbox[1]->Add(m_pDisplayDesPathText, 1, wxLEFT, 5);
	m_pHbox[1]->Add(m_pSaveFrameCheck, 0, wxLEFT, 5);


	m_pListCtrl = new wxListCtrl(this, IDC_FILE_LIST_CTRL, wxDefaultPosition, wxSize(320, 150), 
		wxLC_REPORT | wxLC_SINGLE_SEL | wxSUNKEN_BORDER);
	long indx1 = m_pListCtrl->InsertColumn(0, "Num", wxLIST_FORMAT_LEFT, 50);
	long indx2 = m_pListCtrl->InsertColumn(1, "File name", wxLIST_FORMAT_LEFT, 270);

	m_pHbox[2]->Add(m_pListCtrl, 1);

	m_pStartNum = new wxTextCtrl(this, IDC_START_NUM, wxT(" "), wxDefaultPosition, wxSize(70, 20), wxTE_RIGHT);
	m_pEndNum = new wxTextCtrl(this, IDC_END_NUM, wxT(" "), wxDefaultPosition, wxSize(70, 20), wxTE_RIGHT);

	m_pHbox[3]->Add(m_pStartNum, 1);
	m_pHbox[3]->Add(m_pEndNum, 0, wxLEFT, 5);

	cv::VideoCapture camera;
	int device_counts = 0;
	while (true) {
#ifdef _MSC_VER
		if (!camera.open(device_counts, cv::CAP_DSHOW)) {
#else
		if (!camera.open(device_counts)) {
#endif
			camera.release();
			break;
		}
		DlgPrintf("WebCam(%d): %s", device_counts, camera.getBackendName().c_str());
		device_counts++;
		camera.release();
	}

	wxArrayString CamString;

	CamString.Add("No-Cam");
	for (int i = 0; i < device_counts; ++i) {
		char ListName[100] = { 0, };
		sprintf(ListName, "%d-Cam", i);
		CamString.Add(ListName);
	}

	m_pSelCam = new wxComboBox(this, IDC_SEL_CAM, wxEmptyString, wxDefaultPosition, wxSize(60, 20), CamString, wxCB_DROPDOWN | wxCB_READONLY);
	m_pSelCam->SetSelection(0);
	m_pRunButton = new wxButton(this, IDC_RUN, wxT("Run"), wxDefaultPosition, wxSize(50, 20));
	m_pPauseButton = new wxButton(this, IDC_PAUSE, wxT("Pause"), wxDefaultPosition, wxSize(50, 20));
	m_pStepCheck = new wxCheckBox(this, IDC_SEL_STEP_CHECK, wxT("Step"), wxDefaultPosition, wxSize(20, 20));
	m_pVerboseCheck = new wxCheckBox(this, IDC_SEL_VERBOSE_CHECK, wxT("Verbose"), wxDefaultPosition, wxSize(60, 20));
	m_pVerboseCheck->SetValue(true);

	m_pHbox[4]->Add(m_pSelCam, 1);
	m_pHbox[4]->Add(m_pRunButton, 1, wxLEFT, 5);
	m_pHbox[4]->Add(m_pPauseButton, 1, wxLEFT, 5);
	m_pHbox[4]->Add(m_pStepCheck, 1, wxLEFT, 5);
	m_pHbox[4]->Add(m_pVerboseCheck, 0, wxLEFT, 5);

	m_pExampleButton = new wxButton(this, IDC_EXAMPLE, wxT("Example"), wxDefaultPosition, wxSize(70, 20));
	m_pHbox[5]->Add(m_pExampleButton, 1);

	m_pVbox->Add(m_pHbox[0], 1, wxALIGN_CENTER | wxTOP, 5);
	m_pVbox->Add(m_pHbox[1], 1, wxALIGN_CENTER | wxTOP, 5);
	m_pVbox->Add(m_pHbox[2], 1, wxALIGN_CENTER | wxTOP, 5);
	m_pVbox->Add(m_pHbox[3], 1, wxALIGN_CENTER | wxTOP, 5);
	m_pVbox->Add(m_pHbox[4], 1, wxALIGN_CENTER | wxTOP, 5);
	m_pVbox->Add(m_pHbox[5], 1, wxALIGN_CENTER | wxTOP, 5);

	SetSizer(m_pVbox);
	
	m_bRunTimer = false;
	m_bRunPause = false;
}

CMainDialog::~CMainDialog() {
	m_SequenceRunTimer.Stop();
	m_VideoRunTimer.Stop();
	m_CamRunTimer.Stop();

	m_VideoProcessingVc.release();
	m_CamProcessingVc.release();
}

void CMainDialog::OnSelSrcFileFolder(wxCommandEvent& event) {
	if (m_bRunTimer) {
		wxMessageBox("Timer is running!!");
		return;
	}

	m_pStartNum->Clear();
	*m_pStartNum << 0;

	bool bFileMode = m_pVideoFileCheck->GetValue();
	
	if (!bFileMode) {
		wxDirDialog dirDialog(this, "Folder Selection", "", wxDD_DEFAULT_STYLE, wxDefaultPosition, wxDefaultSize,
			wxDirDialogNameStr);

		if (dirDialog.ShowModal() == wxID_CANCEL) return;

		m_pListCtrl->DeleteAllItems();

		m_pDisplaySrcPathText->SetLabelText(dirDialog.GetPath());

		wxDir dir(dirDialog.GetPath());

		if (!dir.IsOpened()) return;
		
		wxString filename;
		bool cont = dir.GetFirst(&filename);
		int nNum = 0;
		char NumString[100];
		while (cont) {
			wxString filePath = dir.GetNameWithSep() + filename;

			sprintf(NumString, "%d", nNum++);
			int len = m_pListCtrl->GetItemCount();
			long index = m_pListCtrl->InsertItem(len, NumString);
			m_pListCtrl->SetItem(index, 1, filePath);

			cont = dir.GetNext(&filename);
		}

		m_pEndNum->Clear();
		*m_pEndNum << nNum-1;
	}
	else {
		 wxFileDialog openFileDialog(this, "Open Video file", "", "",
			 "Mp4 files(*.mp4)|*.mp4|All files(*.*)|*.*", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

		 if (openFileDialog.ShowModal() == wxID_CANCEL) return;

		 m_pDisplaySrcPathText->SetLabelText(openFileDialog.GetPath());

		 strcpy(m_VideoFileName, openFileDialog.GetPath());

		 m_VideoProcessingVc.open(m_VideoFileName);

		 if (m_VideoProcessingVc.isOpened()) {
			 cv::Mat videoFrame;

			 float videoFPS = m_VideoProcessingVc.get(cv::CAP_PROP_FPS);
			 int videoWidth = m_VideoProcessingVc.get(cv::CAP_PROP_FRAME_WIDTH);
			 int videoHeight = m_VideoProcessingVc.get(cv::CAP_PROP_FRAME_HEIGHT);

			 m_pListCtrl->DeleteAllItems();

			 int nNum = 0;
			 char NumString[100];

			 m_nVideoFileFrameCnt = m_VideoProcessingVc.get(cv::CAP_PROP_FRAME_COUNT);
			 for (int i = 0; i < m_nVideoFileFrameCnt; ++i) {
				 sprintf(NumString, "%d", nNum++);
				 int len = m_pListCtrl->GetItemCount();
				 long index = m_pListCtrl->InsertItem(len, NumString);
				 m_pListCtrl->SetItem(index, 1, m_VideoFileName);
			 }

			 m_pEndNum->Clear();
			 *m_pEndNum << m_nVideoFileFrameCnt - 1;
		 }
	}
}

void CMainDialog::OnSelDesFileFolder(wxCommandEvent& event) {
	if (m_bRunTimer) {
		wxMessageBox("Timer is running!!");
		return;
	}

	wxDirDialog dirDialog(this, "Folder Selection", "", wxDD_DEFAULT_STYLE, wxDefaultPosition, wxDefaultSize,
		wxDirDialogNameStr);

	if (dirDialog.ShowModal() == wxID_CANCEL) return;

	m_pDisplayDesPathText->SetLabelText(dirDialog.GetPath());
}

void CMainDialog::OnActivatedFileListCtrl(wxListEvent& event) {
	int nSel = m_pListCtrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

	wxString pathNameLc = m_pListCtrl->GetItemText(nSel, 1);
	m_pListCtrl->SetItemState(nSel, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
	m_pListCtrl->EnsureVisible(nSel);

	bool bFileMode = m_pVideoFileCheck->GetValue();

	m_pStartNum->Clear();
	*m_pStartNum << nSel;

	if (!bFileMode) {
		char fileName[256];
		strcpy(fileName, pathNameLc);

		auto start = std::chrono::steady_clock::now();

		cv::Mat image;
		image = cv::imread(fileName, cv::IMREAD_COLOR);

		CMainFrame* pMainFrame = (CMainFrame*)GetParent();
		CChildFrame* pChildFrame = (CChildFrame*)(pMainFrame->GetActiveChild());

		if (!image.empty()) {

			if (pChildFrame) {
				DisplayImage(image, 0, 0, false, false);
			}
			else {
				NewFileOpen("New Image", image);
			}
		}
	}
	else {
		if (m_VideoProcessingVc.isOpened()) {
			cv::Mat videoFrame;
			
			m_VideoProcessingVc.set(cv::CAP_PROP_POS_FRAMES, nSel);
			m_VideoProcessingVc.grab();
			m_VideoProcessingVc.retrieve(videoFrame);
			m_VideoProcessingVc.set(cv::CAP_PROP_POS_FRAMES, m_nProcessingNum);
			m_VideoProcessingVc.grab();

			CMainFrame* pMainFrame = (CMainFrame*)GetParent();
			CChildFrame* pChildFrame = (CChildFrame*)(pMainFrame->GetActiveChild());

			if (!videoFrame.empty()) {

				if (pChildFrame) {
					DisplayImage(videoFrame, 0, 0, false, false);
				}
				else {
					NewFileOpen("New Image", videoFrame);
				}
			}
		}
	}
}

void CMainDialog::OnTimer(wxTimerEvent& event) {
	if (m_bRunPause) return;

	char fileName[256], frameInfo[256];
	cv::Mat CurrentImage;
	bool bLoaded = false;;

	if (event.GetId() == ID_TIMER_CAM_RUN) {
		m_CamProcessingVc >> CurrentImage;

		sprintf(frameInfo, "Cam frame: %5.3lf", m_nProcessingNum/30.);

		if (CurrentImage.empty()) {
			m_nProcessingNum++;

			return;
		}

		if (m_pSaveFrameCheck->GetValue()) {
			char SaveFileName[256];

			char FolderName[256];
			strcpy(FolderName, m_pDisplayDesPathText->GetLabelText());

			sprintf(SaveFileName, "%s/frame%05d.jpg", FolderName, m_nProcessingNum);
			cv::imwrite(SaveFileName, CurrentImage);
		}

		bLoaded = true;
	}
	else if (event.GetId() == ID_TIMER_SEQUENCE_RUN) {
		int nEnd = 0;
		m_pEndNum->GetLineText(0).ToInt(&nEnd, 10);
		if (nEnd < 0) nEnd = 0;

		if (m_nProcessingNum >= m_pListCtrl->GetItemCount() || m_nProcessingNum > nEnd) {
			m_bRunTimer = false;
			m_pRunButton->SetLabelText("Run");
			m_SequenceRunTimer.Stop();
			return;
		}

		wxString pathNameLc = m_pListCtrl->GetItemText(m_nProcessingNum, 1);
		m_pListCtrl->SetItemState(m_nProcessingNum, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
		m_pListCtrl->EnsureVisible(m_nProcessingNum);

		strcpy(fileName, pathNameLc);

		CurrentImage = cv::imread(fileName, cv::IMREAD_COLOR);

		if (CurrentImage.empty()) {
			m_nProcessingNum++;

			return;
		}

		bLoaded = true;
	}
	else if (event.GetId() == ID_TIMER_VIDEO_RUN) {
		int nEnd = 0;
		m_pEndNum->GetLineText(0).ToInt(&nEnd, 10);
		if (nEnd < 0) nEnd = 0;

		if (m_nProcessingNum >= m_pListCtrl->GetItemCount() || m_nProcessingNum > nEnd) {
			m_bRunTimer = false;
			m_pRunButton->SetLabelText("Run");
			m_VideoRunTimer.Stop();
			return;
		}

		if (m_VideoProcessingVc.isOpened()) {
			float videoFPS = m_VideoProcessingVc.get(cv::CAP_PROP_FPS);
			int videoWidth = m_VideoProcessingVc.get(cv::CAP_PROP_FRAME_WIDTH);
			int videoHeight = m_VideoProcessingVc.get(cv::CAP_PROP_FRAME_HEIGHT);

			int nFrameCnt = m_VideoProcessingVc.get(cv::CAP_PROP_FRAME_COUNT);

			m_pListCtrl->SetItemState(m_nProcessingNum, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
			m_pListCtrl->EnsureVisible(m_nProcessingNum);

			m_VideoProcessingVc >> CurrentImage;

			sprintf(frameInfo, "Video frame: %5.3lf", m_nProcessingNum/videoFPS);

			if (CurrentImage.empty()) {
				m_nProcessingNum++;

				return;
			}

			if (m_pSaveFrameCheck->GetValue()) {
				char SaveFileName[256];

				char FolderName[256];
				strcpy(FolderName, m_pDisplayDesPathText->GetLabelText());

				sprintf(SaveFileName, "%s/frame%05d.jpg", FolderName, m_nProcessingNum);
				cv::imwrite(SaveFileName, CurrentImage);
			}

			bLoaded = true;
		}
	}

	if(bLoaded) {
		CMainFrame* pMainFrame = (CMainFrame*)GetParent();
		CChildFrame* pChildFrame = (CChildFrame*)(pMainFrame->GetActiveChild());

		if (!CurrentImage.empty()) {

			if (pChildFrame) {
				DisplayImage(CurrentImage, 0, 0, false, true);
			}
			else {
				NewFileOpen("New Image", CurrentImage);
			}
		}

		auto start = std::chrono::steady_clock::now();
		cv::Mat Output;
		
		// Project Run;
		m_Project.Run(CurrentImage, Output, m_bFirstRun, m_pVerboseCheck->GetValue());
		m_bFirstRun = false;

		auto end = std::chrono::steady_clock::now();
		double processingTime = static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()) / 1000;

		if (!CurrentImage.empty()) {
			if (!Output.empty()) DisplayImage(Output, CurrentImage.cols, 0, false, true);
			
			if (event.GetId() == ID_TIMER_CAM_RUN) {
				DlgPrintf("%05d: %s, %10.5lfms, %7dx%4d", m_nProcessingNum, frameInfo, processingTime, CurrentImage.cols, CurrentImage.rows);
			}
			else if (event.GetId() == ID_TIMER_SEQUENCE_RUN) {
				DlgPrintf("%05d: %s, %10.5lfms, %7dx%4d", m_nProcessingNum, fileName, processingTime, CurrentImage.cols, CurrentImage.rows);
			}
			else if (event.GetId() == ID_TIMER_VIDEO_RUN) {
				DlgPrintf("%05d: %s, %10.5lfms, %7dx%4d", m_nProcessingNum, frameInfo, processingTime, CurrentImage.cols, CurrentImage.rows);
			}
		}
		else
		{
			if (event.GetId() == ID_TIMER_CAM_RUN) {
				DlgPrintf("%05d: %s - load error", m_nProcessingNum, frameInfo);
			}
			else if (event.GetId() == ID_TIMER_SEQUENCE_RUN) {
				DlgPrintf("%05d: %s - load error", m_nProcessingNum, fileName);
			}
			else if (event.GetId() == ID_TIMER_VIDEO_RUN) {
				DlgPrintf("%05d: %s - load error", m_nProcessingNum, frameInfo);
			}
		}

		m_nProcessingNum++;
	}
	
	if (m_pStepCheck->GetValue()) m_bRunPause = true;
}

void CMainDialog::OnPause(wxCommandEvent& event) {
	m_bRunPause = !m_bRunPause;
}

void CMainDialog::OnRun(wxCommandEvent& event) {
	bool bVidieFileMode = m_pVideoFileCheck->GetValue();
	bool bCamMode;

	int nCamNum = m_pSelCam->GetSelection()-1;
	bCamMode = nCamNum>=0?true:false;

	if (bCamMode) {
		if (!m_bRunTimer) {
			int nStart = 0;
#ifdef _MSC_VER			
			m_CamProcessingVc.open(nCamNum, cv::CAP_DSHOW);
#else
			m_CamProcessingVc.open(nCamNum);
#endif
			
			if (m_CamProcessingVc.isOpened()) {
				m_nProcessingNum = nStart;

				m_bRunTimer = true;
				m_bRunPause = false;

				m_bFirstRun = true;
				m_pRunButton->SetLabelText("Stop");
				m_CamRunTimer.Start(10);
			}
		}
		else {
			m_bRunTimer = false;
			m_pRunButton->SetLabelText("Run");
			m_CamRunTimer.Stop();

			m_CamProcessingVc.release();
		}
	}
	else if (!bVidieFileMode) {
		if (!m_bRunTimer && m_pListCtrl->GetItemCount() > 0) {
			m_bRunTimer = true;
			m_bRunPause = false;
			int nStart = 0;
			m_pStartNum->GetLineText(0).ToInt(&nStart, 10);
			if (nStart < 0) nStart = 0;
			if (nStart >= m_pListCtrl->GetItemCount()) nStart = m_pListCtrl->GetItemCount() - 1;
			m_nProcessingNum = nStart;

			m_bFirstRun = true;
			m_pRunButton->SetLabelText("Stop");
			m_SequenceRunTimer.Start(10);
		}
		else {
			m_bRunTimer = false;
			m_pRunButton->SetLabelText("Run");
			m_SequenceRunTimer.Stop();
		}
	}
	else {
		if (!m_bRunTimer) {
			m_VideoProcessingVc.open(m_VideoFileName);

			int nStart = 0;
			m_pStartNum->GetLineText(0).ToInt(&nStart, 10);
			if (nStart < 0) nStart = 0;
			if (nStart >= m_pListCtrl->GetItemCount()) nStart = m_pListCtrl->GetItemCount() - 1;

			if (m_VideoProcessingVc.isOpened()) {
				m_VideoProcessingVc.set(cv::CAP_PROP_POS_FRAMES, nStart);
				m_VideoProcessingVc.grab();

				m_nProcessingNum = nStart;

				m_bRunTimer = true;
				m_bRunPause = false;
				m_bFirstRun = true;

				m_pRunButton->SetLabelText("Stop");
				m_VideoRunTimer.Start(10);
			}
		}
		else {
			m_bRunTimer = false;
			m_pRunButton->SetLabelText("Run");
			m_VideoRunTimer.Stop();
		}
	}
}

void CMainDialog::OnExample(wxCommandEvent& event) {
	/*CKcImage kcImage = GetLastSelImage();
	if (kcImage.cvImage.empty()) return;

	cv::Mat cvImage = kcImage.cvImage.clone();

	cv::Mat img_gray;
	cv::cvtColor(cvImage, img_gray, cv::COLOR_BGR2GRAY);

	cv::Mat img_threshold;
	threshold(img_gray, img_threshold, 0, 255, cv::THRESH_BINARY + cv::THRESH_OTSU);

	cv::Mat img_labels, stats, centroids;
	int numOfLables = cv::connectedComponentsWithStats(img_threshold, img_labels, stats, centroids, 8, CV_32S);

	DisplayImage(img_threshold, kcImage.pos.x + kcImage.cvImage.cols, kcImage.pos.y, true, false);
	DisplayImage(img_labels, kcImage.pos.x + kcImage.cvImage.cols * 2, kcImage.pos.y, true, false);
	*/
	CKcImage kcImage = GetLastSelImage();
	if (kcImage.cvImage.empty()) return;

	cv::Mat cvImage = kcImage.cvImage.clone();

	cv::Mat Output;

	m_Project.Run(cvImage, Output, false, m_pVerboseCheck->GetValue());
}
