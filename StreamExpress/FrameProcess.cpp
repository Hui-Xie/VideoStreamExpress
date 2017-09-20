
#include "stdafx.h"
#include "FrameProcess.h"
#include "License.c"


FrameProcess::FrameProcess(){

	m_facetsDir = Facet_Dir;
	m_initializtionSuccess = false;
		
#ifdef _WIN32
	//Necessary only with Windows; call before any other FACET call
	EMOTIENT::FacetSDK::InitializeLicensing(FACET); //return 1 is success.
#endif
	
	int retVal = m_frameAnalyzer.Initialize(m_facetsDir, "FrameAnalyzerConfig.json");
	if (EMOTIENT::FacetSDK::SUCCESS == retVal) {
		m_frameAnalyzer.SetChannelActive(EMOTIENT::FacetSDK::ACTION_UNITS, false);//Disable AUs.
		retVal = m_frameAnalyzer.SetMaxThreads(4);//Xeon E5-1620 (Quad Core, 3.6 Ghz, 10MB Cache) for Lab Workstation
		if (EMOTIENT::FacetSDK::SUCCESS == retVal) 	m_initializtionSuccess = true;
		else  AfxMessageBox("Set Facet's max threads Failed.");
	   }
	else { //Failed
		AfxMessageBox("FrameAnalyzer Initialization Failed.");
	}
}

bool FrameProcess::isInitializeSuccess(){
	return m_initializtionSuccess;
}

FrameProcess::~FrameProcess(){

}

void FrameProcess::analyzeFrame(cv::Mat& grayFrame){
	if (grayFrame.empty())  return;
	m_frameAnalyzer.Analyze(grayFrame.data, grayFrame.rows, grayFrame.cols, m_frameAnalysis);
	if (m_frameAnalysis.NumFaces() > 0){
		m_frameAnalysis.LargestFace(m_largestFace);
	}

}

int FrameProcess::getMultiFacesFromAFrame(cv::Mat& grayFrame, FaceContainer& faceContainer){
	if (grayFrame.empty())  return 0;
	m_frameAnalyzer.Analyze(grayFrame.data, grayFrame.rows, grayFrame.cols, m_frameAnalysis);
	int nFaces = m_frameAnalysis.NumFaces();
	if (nFaces > 0){
		m_frameAnalysis.LargestFace(m_largestFace); //for draw face figure
		EMOTIENT::FacetSDK::Face face;
		for (int i = 0; i < nFaces; i++){
			m_frameAnalysis.FaceAtIndex(i, face);
			faceContainer.addFace(face);
		}

	}
	
	return nFaces;
}

void FrameProcess::baseline(){}

EMOTIENT::FacetSDK::Face   FrameProcess::getLargestFace(){
	return m_largestFace;
}

//set the dir of Facets license and Config file, for upgrade 
void FrameProcess::setFacetsDir(std::string dirPath){
	m_facetsDir = dirPath;

}


void FrameProcess::setMinFaceDetectionWidth(int faceWidth){
	m_minFaceWidth = faceWidth;
	m_frameAnalyzer.SetMinFaceDetectionWidth(m_minFaceWidth);
}


FaceCharacter& FrameProcess::getFaceCharacter(int const offsetX, int const offsetY){
	EMOTIENT::FacetSDK::Rectangle rect;
	m_largestFace.FaceLocation(rect);
	m_offsetX = offsetX;
	m_offsetY = offsetY;
	m_faceCharacter.faceX = rect.x + m_offsetX;
	m_faceCharacter.faceY = rect.y + m_offsetY;
	m_faceCharacter.faceW = rect.width;
	m_faceCharacter.faceH = rect.height;

	m_faceCharacter.leftEyeLeteral = cvPoint(m_largestFace.LandmarkLocation(EMOTIENT::FacetSDK::LEFT_EYE_LATERAL));
	m_faceCharacter.leftEyePupil = cvPoint(m_largestFace.LandmarkLocation(EMOTIENT::FacetSDK::LEFT_EYE_PUPIL));
	m_faceCharacter.leftEyeMedial = cvPoint(m_largestFace.LandmarkLocation(EMOTIENT::FacetSDK::LEFT_EYE_MEDIAL));
	m_faceCharacter.rightEyeMedial = cvPoint(m_largestFace.LandmarkLocation(EMOTIENT::FacetSDK::RIGHT_EYE_MEDIAL));
	m_faceCharacter.rightEyePupil = cvPoint(m_largestFace.LandmarkLocation(EMOTIENT::FacetSDK::RIGHT_EYE_PUPIL));
	m_faceCharacter.rightEyeLeteral = cvPoint(m_largestFace.LandmarkLocation(EMOTIENT::FacetSDK::RIGHT_EYE_LATERAL));
	m_faceCharacter.noseTip = cvPoint(m_largestFace.LandmarkLocation(EMOTIENT::FacetSDK::NOSE_TIP));

	return m_faceCharacter;
}

void FrameProcess::getOverallSentiment(cv::Scalar& color, float& intensity){
	//Opencv Scalar(B,G,R)

	using namespace EMOTIENT::FacetSDK;
	float negative_evidence = m_largestFace.EmotionValue(NEGATIVE, EVIDENCE);
	float negative_intencity = m_largestFace.EmotionValue(NEGATIVE, INTENSITY);
	float neutral_evidence = m_largestFace.EmotionValue(NEUTRAL, EVIDENCE);
	float neutral_intencity = m_largestFace.EmotionValue(NEUTRAL, INTENSITY);
	float positive_evidence = m_largestFace.EmotionValue(POSITIVE, EVIDENCE);
	float positive_intencity = m_largestFace.EmotionValue(POSITIVE, INTENSITY);

	if (negative_evidence >= positive_evidence && negative_evidence >= neutral_evidence){
		//color = cv::Scalar(0, 0, 255);//red
		color = cv::Scalar(0,0,180 + negative_evidence * 7.5);// 7.5 = (255-180)/10; Red
		intensity = -negative_intencity;
	} 
	else if (positive_evidence >= negative_evidence && positive_evidence >= neutral_evidence){
		//color = cv::Scalar(0, 255, 255);//yellow
		color = cv::Scalar(0, 230 + positive_evidence * 2.5, 230 + positive_evidence * 2.5);//2.5 =(255-230)/10
		intensity = positive_intencity;
	}
	else if (neutral_evidence >= positive_evidence && neutral_evidence >= negative_evidence){
		//color = cv::Scalar(0, 255, 0);//green
		color = cv::Scalar(0, 180 + neutral_evidence*7.5, 0);//7.5 = (255-180)/10
		intensity = 0;
	}
	else{
		color = cv::Scalar(255, 255, 255);//white
		intensity = 0;
	}

}




cv::Point FrameProcess::cvPoint(EMOTIENT::FacetSDK::Point const point){
	cv::Point temp;
	temp.x = point.x + m_offsetX;
	temp.y = point.y + m_offsetY;
	return temp;
}