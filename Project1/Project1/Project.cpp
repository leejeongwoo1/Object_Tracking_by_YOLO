//  Project.cpp: implementation of CProject (main project class you will write)
//	Dept. Software Convergence, Kyung Hee University
//	Prof. Daeho Lee, nize@khu.ac.kr
//
#define _CRT_SECURE_NO_WARNINGS
#include <filesystem>
#include "KhuCvApp.h"
#include "Project.h"

#include <fstream>
#include <string>
#include <algorithm>
#ifdef _MSC_VER
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif
#endif

CProject::CProject() {
	GetExecutionPath();
	char cfgPath[MAX_PATH], weightsPath[MAX_PATH], namesPath[MAX_PATH];
	strcpy(cfgPath, m_ExePath);
	strcat(cfgPath, "/yolov3-tiny.cfg");
	strcpy(weightsPath, m_ExePath);
	strcat(weightsPath, "/yolov3-tiny.weights");
	m_Yolov3 = cv::dnn::readNet(cfgPath, weightsPath);

	layer_names = m_Yolov3.getLayerNames();
	UnconnectedOutLayers = m_Yolov3.getUnconnectedOutLayers();
	std::vector<cv::String> output(UnconnectedOutLayers.size());
	output_layers = output;
	for (int i = 0; i < UnconnectedOutLayers.size(); i++) {
		output_layers[i] = layer_names[UnconnectedOutLayers[i] - 1];
	}
	strcpy(namesPath, m_ExePath);
	strcat(namesPath, "/yolo.names");
	std::ifstream file(namesPath);
	if (!file.is_open()) {

		std::cerr << "Failed to open file." << std::endl;
	}

	std::string line;

	while (std::getline(file, line)) {
		classes.push_back(line);
	}
	file.close();
}

CProject::~CProject() {
}

void CProject::GetExecutionPath() {
	wxFileName f(wxStandardPaths::Get().GetExecutablePath());
	wxString appPath(f.GetPath());

	wcscpy(m_ExePathUnicode, appPath);
	strcpy(m_ExePath, appPath.c_str());
}

float CProject::Iou(cv::Rect rt1, cv::Rect rt2) {
	cv::Rect inter = rt1 & rt2;
	cv::Rect uni = rt1 | rt2;
	if (inter.area() > 0) {
		return inter.area() / float(uni.area());
	}
	else {
		return 0;
	}
}

void CProject::Run(cv::Mat Input, cv::Mat& Output, bool bFirstRun, bool bVerbose) {

	///// Example code - Begin
	//cv::Mat Input1;
	//cv::resize(Input, Input1, cv::Size(Input.cols / 2., Input.rows / 2.), 0, 0);
	try {
		cv::Mat Output1 = Input.clone();
		cv::Mat inputBlob = cv::dnn::blobFromImage(Input, 1 / 255., cv::Size(416, 416), cv::Scalar(0, 0, 0), true);
		m_Yolov3.setInput(inputBlob);
		std::vector<cv::Mat> features(output_layers.size());
		for (int i = 0; i < output_layers.size(); i++) {
			features[i] = m_Yolov3.forward(output_layers[i]);
		}
		//DlgPrintf("%s", output_layers[0]);
		//DlgPrintf("%s", output_layers[1]);
		//DlgPrintf("features size: %d", features.size());

		//DlgPrintf("rows: %d", features[0].rows);//300
		//DlgPrintf("cols: %d", features[0].cols);//85
		//DlgPrintf("rows: %d", features[1].rows);//300
		//DlgPrintf("cols: %d", features[1].cols);//85
		std::vector<int> cls_ids;
		std::vector<float> confidences;
		std::vector<cv::Rect> boxes;

		for (int i = 1; i < features.size(); i++) {
			for (int j = 0; j < features[i].rows; j++) {
				cv::Mat info = features[i].row(j);
				cv::Mat scores = info(cv::Range(0, 1), cv::Range(5, 85));
				float x = 0;
				//DlgPrintf("scores cols: %d", scores.cols);//80
				cv::Point classLoc;
				double maxVal = 0;
				for (int k = 0; k < scores.cols; k++) {
					float value = scores.at<float>(0, k);
					if (value > maxVal) {
						maxVal = value;
						classLoc = cv::Point(k, 0);
					}
				}
				float confidence = scores.at<float>(classLoc);

				if (confidence > 0.5) {
					for (int p = 0; p < scores.cols; p++) {
						x = x + scores.at<float>(0, p);
					}
					//DlgPrintf("class total: %lf", x);
					//DlgPrintf("confidence: %lf", confidence);
					int center_x = info.at<float>(0, 0) * Input.cols;
					int center_y = info.at<float>(0, 1) * Input.rows;
					int dw = info.at<float>(0, 2) * Input.cols;
					int dh = info.at<float>(0, 3) * Input.rows;
					int x = center_x - dw / 2.;
					int y = center_y - dh / 2.;
					cv::Rect box(x, y, dw, dh);
					boxes.push_back(box);
					confidences.push_back(confidence);
					cls_ids.push_back(classLoc.x);
				}
			}
		}
		//DlgPrintf("boxes len: %d", boxes.size());
		//DlgPrintf("confidences len: %d", confidences.size());
		//DlgPrintf("cls ids len: %d", cls_ids.size());
		std::vector<int> indices;
		cv::dnn::NMSBoxes(boxes, confidences, 0, 0.4, indices);
		//DlgPrintf("indices length: %d", indices.size());
		for (int i = 0; i < peopleVec.size(); i++) {
			peopleVec[i].untracked = true;
		}
		for (int i = 0; i < carVec.size(); i++) {
			carVec[i].untracked = true;
		}
		for (int i = 0; i < indices.size(); i++) {
			cv::Rect detectRect = boxes[indices[i]];
			std::string label = classes[cls_ids[indices[i]]];
			float score = confidences[indices[i]];
			bool tracked = false;
			Tracker detect(detectRect, false);
			if (label == "person") {
				for (int j = 0; j < peopleVec.size(); j++) {
					if (Iou(detectRect, peopleVec[j].detectRec) > 0.5) {
						peopleVec[j] = detect;
						tracked = true;
						break;
					}
				}
				if (!tracked) {
					peopleVec.push_back(detect);
				}
			}
			else if (label == "car") {
				for (int j = 0; j < carVec.size(); j++) {
					if (Iou(detectRect, carVec[j].detectRec) > 0.5) {
						carVec[j] = detect;
						tracked = true;
						break;
					}
				}
				if (!tracked) {
					carVec.push_back(detect);
				}

			}
		}
		std::erase_if(peopleVec, [](Tracker tracker) {return tracker.untracked; });
		std::erase_if(carVec, [](Tracker tracker) {return tracker.untracked; });
		
		for (int i = 0; i < peopleVec.size(); i++) {
			cv::rectangle(Output1, peopleVec[i].detectRec, cv::Scalar(0, 0, 255), 2);
			std::string tracker = "people" + std::to_string(i);
			cv::putText(Output1, tracker, cv::Point(peopleVec[i].detectRec.x, peopleVec[i].detectRec.y - 20), cv::FONT_ITALIC, 0.5, cv::Scalar(255, 255, 255), 1.5);
		}
		for (int i = 0; i < carVec.size(); i++) {
			cv::rectangle(Output1, carVec[i].detectRec, cv::Scalar(0, 0, 255), 2);
			std::string tracker = "car" + std::to_string(i);
			cv::putText(Output1, tracker, cv::Point(carVec[i].detectRec.x, carVec[i].detectRec.y - 20), cv::FONT_ITALIC, 0.5, cv::Scalar(255, 255, 255), 1.5);
		}
	
		DisplayImage(Output1, Input.cols, 0, false, true);
	}
	catch (cv::Exception& e) {
		DlgPrintf(e.what());
	}
}
