
#include "stdafx.h"
#include "FaceAnalysis.h"
#include <ShlObj.h>
FaceAnalysis::FaceAnalysis()
{

}

int FaceAnalysis::CalculateQuality()
{
	try
	{
		shared_ptr<FACEQA_LIB::Interface> faceLib = FACEQA_LIB::Interface::getImplementation();
		
		//string configDir = "";
		//string configvalue = "";
		string configDir = "";
		string configvalue = "data\\faceqa.jaxn";
		int zmienna = 5;
		FACEQA::ReturnStatus ret = faceLib->initialize(configDir,configvalue);
		if (ret.code == FACEQA::ReturnCode::Success)
		{
			wchar_t mypicturespath[MAX_PATH] = L"";
			HRESULT result = SHGetFolderPath(NULL, CSIDL_MYPICTURES, NULL, SHGFP_TYPE_CURRENT, mypicturespath);
			wchar_t currentDirAndFile[MAX_PATH] = L"";
			wcscpy(currentDirAndFile, mypicturespath);
			wcscat(currentDirAndFile, L"\\");
			wcscat(currentDirAndFile, L"face.bmp");

			wstring ws(currentDirAndFile);
			
			string fstr(ws.begin(), ws.end());

			FACEQA::Image image;
			
			FACEQA::ReturnStatus retStatus = readImage(fstr, image);
			if (retStatus.code == FACEQA::ReturnCode::Success)
			{
				FACEQA::FaceImageQualityAssessment assessment;
				int resultCode = getQualityAssessmentResults(faceLib, image, assessment);
				if (resultCode == 1)
				{
					QualityResults = exportAssessmentResultsToString(assessment, false);
					return 1;
				}
			}
			else
			{
				return (int)ret.code;
			}

			return 1;
		}
		else
		{
			return (int)ret.code;
		}

	}
	catch (exception& e)
	{
		ExceptionMessage = std::string(e.what());
		
		return -1;
	}
}

FACEQA::ReturnStatus FaceAnalysis::readImage(const std::string& filename, FACEQA::Image& image)
{
	FACEQA::ReturnCode retCode = FACEQA::ReturnCode::Success;
	std::string retStatusInfo{ "" };

	try
	{
		cv::Mat cvImage = cv::imread(filename, cv::IMREAD_COLOR);
		cv::cvtColor(cvImage, cvImage, cv::COLOR_BGR2RGB);
		image.width = cvImage.cols;
		image.height = cvImage.rows;
		image.depth = 24;
#ifdef _WIN32
		//image.data = {new uint8_t[image.size()]};
		uint8_t* data = new uint8_t[image.size()];
		image.data.reset(data);
#else
		uint8_t* data = new uint8_t[image.size()];
		image.data.reset(data);
#endif

		memcpy(image.data.get(), cvImage.data, image.size());
	}
	catch (const std::exception&)
	{
		retCode = FACEQA::ReturnCode::ImageReadingError;
		retStatusInfo = std::string("failed to read image: ") + filename;
	}

	return FACEQA::ReturnStatus(retCode, retStatusInfo);
}

int FaceAnalysis::getQualityAssessmentResults(
	std::shared_ptr<FACEQA_LIB::Interface>& implPtr,
	FACEQA::Image& image,
	FACEQA::FaceImageQualityAssessment& assessments)
{
	

	FACEQA::ReturnStatus retStatus = implPtr->vectorQuality(image, assessments);

	double quality;
	FACEQA::ReturnStatus retStatus2 = implPtr->scalarQuality(image, quality);
	//std::cout << "Image file: '" << inputFile << "' scalar quality: " << quality << std::endl;
	if (retStatus2.code == FACEQA::ReturnCode::Success)
	{
		QualityResults = QualityResults + "Quality: " + std::to_string(quality);
		return retStatus.code == FACEQA::ReturnCode::Success ? 1 : 0;
	}
	else
	{
		return 0;
	}

}

string FaceAnalysis::exportAssessmentResultsToString(
	const FACEQA::FaceImageQualityAssessment& assessments,
	bool doExportScalar)
{
	std::string resultStr;
	char buf[200] = {};
	for (auto const& aResult : assessments.qAssessments)
	{
		//const QualityMeasure& measure = aResult.first;
		const FACEQA::QualityMeasureResult& qaResult = aResult.second;
		double val = doExportScalar ? qaResult.scalar : qaResult.rawScore;
		if (round(val) == val)
			snprintf(buf, 200, "%d;", (int)val);
		else
			snprintf(buf, 200, "%f;", val);
		resultStr += std::string(buf);
	}
	return resultStr;
}


string FaceAnalysis::GetExceptionMessage()
{
	
	return ExceptionMessage;
	
}

string FaceAnalysis::GetQualityResults()
{
	return QualityResults;
}