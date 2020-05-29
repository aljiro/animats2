#include "../include/video.h"

using namespace morph::animats;

VideoRecorder::VideoRecorder( SimView* sv ){
	this->sv = sv;
}	

void VideoRecorder::setup( Simulation& s ){
    this->outputVideo.open("video.avi", CV_FOURCC('M','J','P','G'), 20.0f, cv::Size( this->sv->width, this->sv->height ), true);
}

void VideoRecorder::notify( Simulation& s, std::string message ){
    int height = this->sv->height;
    int width = this->sv->width;
    cv::Mat pixels( height, width, CV_8UC3 );
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data );
    
    cv::Mat cv_pixels( height, width, CV_8UC3 );

    for( int y = 0; y < height; y++ ) 
        for( int x=0; x < width; x++ ){
        cv_pixels.at<cv::Vec3b>(y,x)[2] = pixels.at<cv::Vec3b>(height-y-1,x)[0];
        cv_pixels.at<cv::Vec3b>(y,x)[1] = pixels.at<cv::Vec3b>(height-y-1,x)[1];
        cv_pixels.at<cv::Vec3b>(y,x)[0] = pixels.at<cv::Vec3b>(height-y-1,x)[2];
    }
    this->outputVideo << cv_pixels; 
}

void VideoRecorder::notifyEnd(){
    this->outputVideo.release();
}