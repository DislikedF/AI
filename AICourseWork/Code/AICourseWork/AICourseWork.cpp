// AICourseWork.cpp : Defines the entry point for the console application.
//
//C:\Users\ryanf\Documents\AICourseWork\x64\Debug\AICourseWork.exe C:\Users\ryanf\Documents\AICourseWork\csvFile.csv C:\Users\ryanf\Documents\AICourseWork\csvFileTest.csv
#include "stdafx.h"

#include "opencv2/core/core.hpp"
#include "opencv2/face.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv.hpp"
#include "opencv2/imgproc.hpp"


#include <iostream>
#include <fstream>
#include <sstream>

using namespace cv;
using namespace cv::face;
using namespace std;

static Mat norm_0_255(InputArray _src) {
	Mat src = _src.getMat();
	// Create and return normalized image:
	Mat dst;
	switch (src.channels()) {
	case 1:
		cv::normalize(_src, dst, 0, 255, NORM_MINMAX, CV_8UC1);
		break;
	case 3:
		cv::normalize(_src, dst, 0, 255, NORM_MINMAX, CV_8UC3);
		break;
	default:
		src.copyTo(dst);
		break;
	}
	return dst;
}

// Read CSV files
static void read_csv(const string& filename, const string& testfilename, vector<Mat>& images, vector<Mat>& testimages, vector<int>& labels, vector<int>& testlabels, char separator = ';') {
	std::ifstream file(filename.c_str(), ifstream::in);
	if (!file) {
		string error_message = "No valid input file was given, please check the given filename.";
		CV_Error(CV_StsBadArg, error_message);
	}
	string line, path, classlabel;
	while (getline(file, line)) {
		stringstream liness(line);
		getline(liness, path, separator);
		getline(liness, classlabel);
		if (!path.empty() && !classlabel.empty()) {
			images.push_back(imread(path, 0));
			labels.push_back(atoi(classlabel.c_str()));
		}
	}
	std::ifstream testfile(testfilename.c_str(), ifstream::in);
	if (!testfile) {
		string error_message = "No valid input file was given, please check the given filename.";
		CV_Error(CV_StsBadArg, error_message);
	}
	string testline, testpath, testclasslabel;
	while (getline(testfile, testline)) {
		stringstream liness(testline);
		getline(liness, testpath, separator);
		getline(liness, testclasslabel);
		if (!testpath.empty() && !testclasslabel.empty()) {
			testimages.push_back(imread(testpath, 0));
			testlabels.push_back(atoi(testclasslabel.c_str()));
		}
	}
}

int main(int argc, const char *argv[]) {
	// Check for valid command line arguments, print usage
	// if no arguments were given.
	if (argc < 2) {
		cout << "usage: " << argv[0] << " <csv.ext> <output_folder> " << endl;
		exit(1);
	}
	
	// Get the path to your CSV.
	string fn_csv = string(argv[1]);
	string testCsv = string(argv[2]);
	//Get path to test image CSV.
	// These vectors hold the images and corresponding labels.
	vector<Mat> images;
	vector<int> labels;
	vector<Mat> testImages;
	vector<int> testLabels;
	// Read in the data.
	try {
		read_csv(fn_csv, testCsv, images, testImages, labels, testLabels);
	}
	catch (cv::Exception& e) {
		cerr << "Error opening file \"" << fn_csv << "\". Reason: " << e.msg << endl;
		exit(1);
	}
	
	//histogram training images
	for (int i = 0; i < images.size() - 1; i++)
	{
		equalizeHist(images[i], images[i]);
	}
	// Create FsiherFace recognizer and train with the tarining data
	Ptr<FisherFaceRecognizer> model = FisherFaceRecognizer::create();
	model->train(images, labels);
	
	//Set test images and predict emotion from all test images
	Mat testSample;
	int correct = 0;
	int wrong = 0;

	// Go through all images
	for (int i = 0; i < testImages.size() - 1; i++)
	{
		// histogram test images
		
			equalizeHist(testImages[i], testImages[i]);
		
		testSample = testImages[i];
		if (!testSample.data)                              // Check for invalid input
		{
			cout << "Could not open or find the image" << std::endl;
			return -1;
		}
		int testLabel = testLabels[i];
		int predictedLabel = model->predict(testSample);
		
		predictedLabel = -1;
		double confidence = 0.0;
		model->predict(testSample, predictedLabel, confidence);
		
		// Add to Correct or Wrong lists
		if (predictedLabel == testLabel)
		{
			correct++;
		}
		else
		{
			wrong++;
		}

		// Determin predicted class name
		string predictClass = "None";
		string testClass = "None";
		switch (predictedLabel)
		{
		case 0:
			predictClass = "Anger";
			testClass = "Anger";
			break;
		case 1:
			predictClass = "Disgust";
			testClass = "Disgust";
			break;
		case 2:
			predictClass = "Fear";
			break;
		case 3:
			predictClass = "Joy";
			break;
		case 4:
			predictClass = "Sadness";
			break;
		case 5:
			predictClass = "Surprise";
			break;
		}

		switch (testLabel)
		{
		case 0:
			
			testClass = "Anger";
			break;
		case 1:
			
			testClass = "Disgust";
			break;
		case 2:
			testClass = "Fear";
			break;
		case 3:
			testClass = "Joy";
			break;
		case 4:
			testClass = "Sadness";
			break;
		case 5:
			testClass = "Surprise";
			break;
		}
		
		cout << "Actual Class - " << testClass << " - Predicted Class " << predictClass << " - Confidence " << confidence << endl;
	}

	// Show how many correct or wrong predictions

	cout << " Correct - " << correct << endl;
	cout << " Wrong - " << wrong;

	
	
	return 0;
}