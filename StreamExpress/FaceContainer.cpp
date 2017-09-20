
#include "stdafx.h"
#include "FaceContainer.h"
#include <iostream>
#include <string>

FaceContainer::FaceContainer()
{
	m_vectorFaces.clear();
	m_vectorFrameProperty.clear();

}

FaceContainer::~FaceContainer()
{
	
}

void FaceContainer::clearAllVector(){
	m_vectorFaces.clear();
	m_vectorFrameProperty.clear();
}
bool FaceContainer::isEmpty(){
	return m_vectorFaces.empty();
}

void FaceContainer::internalPrint(const std::string& buff){
	m_cFile.Write(buff.c_str(), buff.size());
	m_cFile.Flush();
}

void FaceContainer::printBlankComma(const int num){
	std::string buff;
	buff.clear();
	for (int i = 0; i < num; i++){
		 buff +=" ,";
	}
	internalPrint(buff);
}

void FaceContainer::addFrameProperty(const FrameProperty frameProp){
	m_vectorFrameProperty.push_back(frameProp);
}

void FaceContainer::addFace(const EMOTIENT::FacetSDK::Face face){
	m_vectorFaces.push_back(face);
}
void FaceContainer::openOutputFile(std::string path){
		
	std::string shortDir = path.substr(path.find_last_of("\\") + 1);
	
	SetCurrentDirectory(path.c_str());
	std::string fileName = "FacetOutput_" + shortDir + ".csv";
	
	bool isOpen = m_cFile.Open(fileName.c_str(), CFile::modeCreate | CFile::modeWrite);
	if (!isOpen) {
		std::cout << "Output File can't be created." << std::endl;
	}
}

void FaceContainer::openStreamOutputFile(CString path){
	SetCurrentDirectory(path);
	int nFrames = m_vectorFrameProperty.size();
	char fileName[50];
	SYSTEMTIME startTime = m_vectorFrameProperty.at(0).m_time;
	SYSTEMTIME endTime = m_vectorFrameProperty.at(nFrames-1).m_time;

	//filename example: StreamOutput_20150317_185300to195460.csv
	int n = 0;
	n = sprintf_s(fileName, 50, "StreamOutput_%4d%02d%02d_%02d%02d%02dto%02d%02d%02d.csv", 
		startTime.wYear, startTime.wMonth, startTime.wDay,startTime.wHour, startTime.wMinute, startTime.wSecond, 
		endTime.wHour, endTime.wMinute, endTime.wSecond);
	*(fileName + n) = '\0';
	
	bool isOpen = m_cFile.Open(fileName, CFile::modeCreate | CFile::modeWrite);
	if (!isOpen) {
		AfxMessageBox("Stream Output File can't be created.");
	}
}

void FaceContainer::closeOutputFile(){
	m_cFile.Close();

}


void FaceContainer::printCSVHead(){ 
	std::string csvHead = ""
		"FileName, Frame#, Time, "
		"FaceboxX, FaceboxY, FaceboxW, FaceboxH, "
		"Pitch, Yaw, Roll, "
		"Glasses(Evidence), Glasses(Probability), "
		"Gender(Evidence), Gender(Probability), "
		"Anger(Evidence), Anger(Intensity), Contempt(Evidence), Contempt(Intensity),	Disgust(Evidence), Disgust(Intensity), Joy(Evidence), Joy(Intensity), "
		"Fear(Evidence), Fear(Intensity), Sadness(Evidence), Sadness(Intensity), Surprise(Evidence), Surprise(Intensity), "
		"Frustration(Evidence), Frustration(Intensity), Confusion(Evidence), Confusion(Intensity), "
		"Negative(Evidence), Negative(Intensity), Neutral(Evidence), Neutral(Intensity), Positive(Evidence), Positive(Intensity), "
		"AU1, AU2, AU4, AU5, AU6, AU7, AU9, AU10, AU12, AU14, "
		"AU15, AU17, AU18, AU20, AU23, AU24, AU25, AU26, AU28, ";
	csvHead.append("\r\n");
	internalPrint(csvHead);
}

void FaceContainer::printAllResult(){
	int numFaces = m_vectorFaces.size();
	int numFrame = m_vectorFrameProperty.size();
	ASSERT(numFaces == numFrame);

	for (int i = 0; i < numFaces; i++){
		const EMOTIENT::FacetSDK::Face& face = m_vectorFaces.at(i);
		const FrameProperty& frameProp = m_vectorFrameProperty.at(i);
		printFrameProperty(frameProp);
		printFaceSize(face, frameProp.m_offsetX, frameProp.m_offsetY);
		printPose(face);
		printProperty(face);
		printDemographic(face);
		printPrimaryEmotions(face);
		printAdvancedEmotions(face);
		printOverallSentiment(face);
		printNewLineChar();
		
	}
	
}

//"FileName, Frame#, Time, "
void FaceContainer::printFrameProperty(const FrameProperty& frameProp){
	std::string buff;
	buff.clear();
	
	//extract filename for ImageExpress
	//std::string fileName = frameProp.m_frameName.substr(frameProp.m_frameName.find_last_of("\\") + 1);
	//buff.append(fileName + ", ");
	
	//For StreamExpress, There is no filename cell.
	buff.append(" , ");
	
	buff.append(std::to_string((long double)frameProp.m_frameNO) + ", ");
	
	//time print
	char* timeString = new char[50];
	int n = 0;
	//CTime for ImageExpress
	//n= sprintf(timeString, "%4d%02d%02d %02d%02d%02d %03d", frameProp.m_time.GetYear(), frameProp.m_time.GetMonth(), frameProp.m_time.GetDay(),
	//	frameProp.m_time.GetHour(), frameProp.m_time.GetMinute(), frameProp.m_time.GetSecond(), frameProp.m_millisecond);
	
	//SYSTEMTIME for StreamExpress
	n = sprintf_s(timeString, 50,"%4d%02d%02d %02d%02d%02d %03d", frameProp.m_time.wYear, frameProp.m_time.wMonth, frameProp.m_time.wDay,
		frameProp.m_time.wHour, frameProp.m_time.wMinute, frameProp.m_time.wSecond, frameProp.m_time.wMilliseconds);

	*(timeString+n) = '\0';

	buff.append(timeString);
	buff.append(", ");
	delete[] timeString;
	internalPrint(buff);
}

//"FaceboxX, FaceboxY, FaceboxW, FaceboxH, "
void FaceContainer::printFaceSize(const EMOTIENT::FacetSDK::Face& face, int offsetX, int offsetY, bool nonBlank){
	if (!nonBlank) {
		printBlankComma(4);
		return;
	}

	std::string buff;
	buff.clear();

	EMOTIENT::FacetSDK::Rectangle faceLocation;
	face.FaceLocation(faceLocation);
	buff.append (std::to_string((long double)(faceLocation.x+offsetX)) + ", " 
	           + std::to_string((long double)(faceLocation.y+offsetY)) + ", "
	           + std::to_string((long double)faceLocation.width) + ", "
		       + std::to_string((long double)faceLocation.height) + ", ");
	internalPrint(buff);
}

//"Pitch, Yaw, Roll, "
//YAW = 0, PITCH = 1, ROLL = 2
void FaceContainer::printPose(const EMOTIENT::FacetSDK::Face& face, bool nonBlank){
	if (!nonBlank) {
		printBlankComma(3);
		return;
	}

	std::string buff;
	buff.clear();
	using namespace EMOTIENT::FacetSDK;
	buff = std::to_string((long double)face.PoseValue(PITCH)) + ", "
		+ std::to_string((long double)face.PoseValue(YAW)) + ", "
		+ std::to_string((long double)face.PoseValue(ROLL)) + ", ";
	internalPrint(buff);

} 

//"Glasses(Evidence), Glasses(Probability), "
// HAS_GLASSES = 0,
//                                        ATTRIBUTE_EVIDENCE = 1, ATTRIBUTE_PROBABILITY = 2,
void FaceContainer::printProperty(const EMOTIENT::FacetSDK::Face& face, bool nonBlank){
	if (!nonBlank) {
		printBlankComma(2);
		return;
	}
	std::string buff;
	buff.clear();
	using namespace EMOTIENT::FacetSDK;
	buff = std::to_string((long double)face.AttributeValue(HAS_GLASSES, ATTRIBUTE_EVIDENCE)) + ", "
		+ std::to_string((long double)face.AttributeValue(HAS_GLASSES, ATTRIBUTE_PROBABILITY)) + ", ";
	internalPrint(buff);

}

//"Gender(Evidence), Gender(Probability), "
//IS_MALE = 0
//                                        ATTRIBUTE_EVIDENCE = 1, ATTRIBUTE_PROBABILITY = 2,
void FaceContainer::printDemographic(const EMOTIENT::FacetSDK::Face& face, bool nonBlank){
	if (!nonBlank) {
		printBlankComma(2);
		return;
	}

	std::string buff;
	buff.clear();
	using namespace EMOTIENT::FacetSDK;
	buff = std::to_string((long double)face.DemographicValue(IS_MALE, ATTRIBUTE_EVIDENCE)) + ", "
		+ std::to_string((long double)face.DemographicValue(IS_MALE, ATTRIBUTE_PROBABILITY)) + ", ";
	internalPrint(buff);
}

//"Anger(Evidence), Anger(Intensity), Contempt(Evidence), Contempt(Intensity),	Disgust(Evidence), Disgust(Intensity), Joy(Evidence), Joy(Intensity), "
//"Fear(Evidence), Fear(Intensity), Sadness(Evidence), Sadness(Intensity), Surprise(Evidence), Surprise(Intensity), "
//  ANGER = 0, CONTEMPT = 1, DISGUST = 2, JOY = 3, 
//  FEAR = 4,  SADNESS = 8, SURPRISE = 9, 
//                                       EVIDENCE = 2, INTENSITY = 1
void FaceContainer::printPrimaryEmotions(const EMOTIENT::FacetSDK::Face& face, bool nonBlank){
	if (!nonBlank) {
		printBlankComma(14);
		return;
	}

	
	std::string buff;
	buff.clear();
	using namespace EMOTIENT::FacetSDK;
	buff = std::to_string((long double)face.EmotionValue(ANGER, EVIDENCE)) + ", "
		+ std::to_string((long double)face.EmotionValue(ANGER, INTENSITY)) + ", "
		+ std::to_string((long double)face.EmotionValue(CONTEMPT, EVIDENCE)) + ", "
		+ std::to_string((long double)face.EmotionValue(CONTEMPT, INTENSITY)) + ", "
		+ std::to_string((long double)face.EmotionValue(DISGUST, EVIDENCE)) + ", "
		+ std::to_string((long double)face.EmotionValue(DISGUST, INTENSITY)) + ", "
		+ std::to_string((long double)face.EmotionValue(JOY, EVIDENCE)) + ", "
		+ std::to_string((long double)face.EmotionValue(JOY, INTENSITY)) + ", "
		+ std::to_string((long double)face.EmotionValue(FEAR, EVIDENCE)) + ", "
		+ std::to_string((long double)face.EmotionValue(FEAR, INTENSITY)) + ", "
		+ std::to_string((long double)face.EmotionValue(SADNESS, EVIDENCE)) + ", "
		+ std::to_string((long double)face.EmotionValue(SADNESS, INTENSITY)) + ", "
		+ std::to_string((long double)face.EmotionValue(SURPRISE, EVIDENCE)) + ", "
		+ std::to_string((long double)face.EmotionValue(SURPRISE, INTENSITY)) + ", ";
	internalPrint(buff);
}

//"Frustration(Evidence), Frustration(Intensity), Confusion(Evidence), Confusion(Intensity), "
//FRUSTRATION = 11, CONFUSION = 10, 
//                                       EVIDENCE = 2, INTENSITY = 1
void FaceContainer::printAdvancedEmotions(const EMOTIENT::FacetSDK::Face& face, bool nonBlank){
	if (!nonBlank) {
		printBlankComma(4);
		return;
	}

	std::string buff;
	buff.clear();
	using namespace EMOTIENT::FacetSDK;
	buff = std::to_string((long double)face.EmotionValue(FRUSTRATION, EVIDENCE)) + ", "
		+ std::to_string((long double)face.EmotionValue(FRUSTRATION, INTENSITY)) + ", "
		+ std::to_string((long double)face.EmotionValue(CONFUSION, EVIDENCE)) + ", "
		+ std::to_string((long double)face.EmotionValue(CONFUSION, INTENSITY)) + ", ";
	internalPrint(buff);

}

//"Negative(Evidence), Negative(Intensity), Neutral(Evidence), Neutral(Intensity), Positive(Evidence), Positive(Intensity), "
//NEGATIVE = 5, NEUTRAL = 6, POSITIVE = 7,
//                                       EVIDENCE = 2, INTENSITY = 1
void FaceContainer::printOverallSentiment(const EMOTIENT::FacetSDK::Face& face, bool nonBlank){
	if (!nonBlank) {
		printBlankComma(6);
		return;
	}

	std::string buff;
	buff.clear();
	using namespace EMOTIENT::FacetSDK;
	buff = std::to_string((long double)face.EmotionValue(NEGATIVE, EVIDENCE)) + ", "
		+ std::to_string((long double)face.EmotionValue(NEGATIVE, INTENSITY)) + ", "
		+ std::to_string((long double)face.EmotionValue(NEUTRAL, EVIDENCE)) + ", "
		+ std::to_string((long double)face.EmotionValue(NEUTRAL, INTENSITY)) + ", "
		+ std::to_string((long double)face.EmotionValue(POSITIVE, EVIDENCE)) + ", "
		+ std::to_string((long double)face.EmotionValue(POSITIVE, INTENSITY)) + ", ";
	internalPrint(buff);

}

//"AU1, AU2, AU4, AU5, AU6, AU7, AU9, AU10, AU12, AU14, "
//"AU15, AU17, AU18, AU20, AU23, AU24, AU25, AU26, AU28, "
// AUn = n, total 19 AUs
//                                       EVIDENCE = 2, 
//Note that the INTENSITY normalization type is not yet supported for action units (though it may be in later versions of the SDK). 
void FaceContainer::printAU(const EMOTIENT::FacetSDK::Face& face, bool nonBlank){
	if (!nonBlank) {
		printBlankComma(19);
		return;
	}

	std::string buff;
	buff.clear();
	using namespace EMOTIENT::FacetSDK;
	buff = std::to_string((long double)face.ActionUnitValue(AU1, EVIDENCE)) + ", "
		+ std::to_string((long double)face.ActionUnitValue(AU2, EVIDENCE)) + ", "
		+ std::to_string((long double)face.ActionUnitValue(AU4, EVIDENCE)) + ", "
		+ std::to_string((long double)face.ActionUnitValue(AU5, EVIDENCE)) + ", "
		+ std::to_string((long double)face.ActionUnitValue(AU6, EVIDENCE)) + ", "
		+ std::to_string((long double)face.ActionUnitValue(AU7, EVIDENCE)) + ", "
    	+ std::to_string((long double)face.ActionUnitValue(AU9, EVIDENCE)) + ", "
		+ std::to_string((long double)face.ActionUnitValue(AU10, EVIDENCE)) + ", "
		+ std::to_string((long double)face.ActionUnitValue(AU12, EVIDENCE)) + ", "
		+ std::to_string((long double)face.ActionUnitValue(AU14, EVIDENCE)) + ", "
		+ std::to_string((long double)face.ActionUnitValue(AU15, EVIDENCE)) + ", "
		+ std::to_string((long double)face.ActionUnitValue(AU17, EVIDENCE)) + ", "
		+ std::to_string((long double)face.ActionUnitValue(AU18, EVIDENCE)) + ", "
		+ std::to_string((long double)face.ActionUnitValue(AU20, EVIDENCE)) + ", "
		+ std::to_string((long double)face.ActionUnitValue(AU23, EVIDENCE)) + ", "
		+ std::to_string((long double)face.ActionUnitValue(AU24, EVIDENCE)) + ", "
	    + std::to_string((long double)face.ActionUnitValue(AU25, EVIDENCE)) + ", "
		+ std::to_string((long double)face.ActionUnitValue(AU26, EVIDENCE)) + ", "
		+ std::to_string((long double)face.ActionUnitValue(AU28, EVIDENCE)) + ", ";
	//buff.append("\r\n");
	internalPrint(buff);
}


void FaceContainer::printNewLineChar(){
	std::string buff;
	buff.append("\r\n");
	internalPrint(buff);
}

