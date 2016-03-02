#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <opencv2/video.hpp>
#include <iostream>
using namespace cv;
using namespace std;

// Global variables
Mat frame;                          // current frame
Mat fgMaskMOG2;                     // fg mask generated by MOG2 method
Mat ContourImg;                     // fg mask generated by MOG2 method
Mat binaryImg;
Mat blurImg;
Ptr<BackgroundSubtractor> pMOG2;    // MOG2 Background subtractor

void processVideo(int webcam, int structure_size) {
    //create the capture object
    VideoCapture capture;
    capture.open(webcam);
    capture >> frame;

    //create Background Subtractor objects
    pMOG2 = createBackgroundSubtractorMOG2(); //MOG2 approach

    //morphology element
    Mat element = getStructuringElement(MORPH_RECT, Size(structure_size, structure_size), Point(5, 5));

    //read input data. ESC or 'q' for quitting
    for (;;) {
        //read the current frame
        capture >> frame;
        //update the background model
        //blur(frame, blurImg, Size(10, 10));
        pMOG2->apply(frame, fgMaskMOG2);
        
        //morphology
        morphologyEx(fgMaskMOG2, binaryImg, CV_MOP_CLOSE, element);
        threshold(binaryImg, binaryImg, 128, 255, CV_THRESH_BINARY);
        //find contour
        ContourImg = binaryImg.clone();
        
        //less blob delete
        vector<vector<Point> > contours;
        findContours(ContourImg, 
                contours, // a vector of contours
                CV_RETR_EXTERNAL, // retrieve the external contours
                CV_CHAIN_APPROX_NONE); // all pixels of each contours
        vector<Rect> output;
        vector<vector<Point> >::iterator itv = contours.begin();
        
        while (itv!=contours.end()) {
            //create bounding rect of object
            //rect draw on origin image
            Rect mr = boundingRect(Mat(*itv));
            rectangle(frame, mr, CV_RGB(255, 0, 0));
            itv++;
        }
        
        //show the current frame and the fg masks
        imshow("FG Mask MOG 2", fgMaskMOG2);
        imshow("Frame", frame);
        waitKey(30);
    }
    //delete capture object
    capture.release();
}

int main(int argc, char** argv) {
    if(argc != 3) {
        cout << "Usage: ./bg <webcam id> <morphology structure size>" << endl;
        cout << "Notice: this requirs your webcam!" << endl;
        return EXIT_FAILURE;
    }
    //create GUI windows
    namedWindow("Frame");
    namedWindow("FG Mask MOG 2");

    processVideo(atoi(argv[1]), atoi(argv[2]));

    //destroy GUI windows
    destroyAllWindows();
    return EXIT_SUCCESS;
}
