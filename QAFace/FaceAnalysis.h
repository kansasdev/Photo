#pragma once
#include <exception>
#include <string>
#include <fstream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "faceqa_lib.h"

class FaceAnalysis
{
private:
	std::string ExceptionMessage;
	std::string QualityResults;
	FACEQA::ReturnStatus readImage(const std::string& filename, FACEQA::Image& image);
	int getQualityAssessmentResults(std::shared_ptr<FACEQA_LIB::Interface>& implPtr, FACEQA::Image& image, FACEQA::FaceImageQualityAssessment& assessments);
	std::string exportAssessmentResultsToString(const FACEQA::FaceImageQualityAssessment& assessments, bool doExportScalar);

public:
	FaceAnalysis();
	int CalculateQuality();
	std::string GetExceptionMessage();
	std::string GetQualityResults();
};
