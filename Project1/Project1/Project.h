//  Project.h: interface of CProject (main project class you will write)
//	Dept. Software Convergence, Kyung Hee University
//	Prof. Daeho Lee, nize@khu.ac.kr
//

#pragma once

class Tracker {
public:
	Tracker(cv::Rect detectRec, bool untracked) :detectRec(detectRec), untracked(untracked) {}
	cv::Rect detectRec;
	bool untracked;
};

class CProject
{
	cv::Mat m_PreviousImage;
public:
	char m_ExePath[256];
	wchar_t m_ExePathUnicode[256];

	CProject();
	~CProject();
	void GetExecutionPath();
	void Run(cv::Mat Input, cv::Mat& Output, bool bFirstRun, bool bVerbose);
	cv::dnn::Net m_Yolov3;
	std::vector<cv::String> layer_names;
	std::vector<int> UnconnectedOutLayers;
	std::vector<cv::String> output_layers;
	std::vector<std::string> classes;

	std::vector<Tracker> peopleVec;
	std::vector<Tracker> carVec;

	float Iou(cv::Rect rt1, cv::Rect rt2);
};

