#ifndef VIDEO_H
#define VIDEO_H

#include <iostream>

#include "view.h"
// #include "simview.h"
#include "simulation.h"
#include "opencv/cv.h"
#include "opencv2/opencv.hpp"

namespace morph{ namespace animats{

class SimView;

class VideoRecorder : public View{

protected:
	cv::VideoWriter outputVideo;
	SimView* sv;
public:
	VideoRecorder( SimView* sv );
	void setup( Simulation& s );
	void notify( Simulation& s, std::string message );
	void notifyEnd();
};

}}

#endif