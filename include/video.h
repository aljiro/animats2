#ifndef VIDEO_H
#define VIDEO_H

#include <iostream>
#include "simulation.h"
#include "view.h"

namespace morph{ namespace animats{

class VideoRecorder : public View{
public:
	VideoRecorder();
	void setup( Simulation& s );
	void notify( Simulation& s, std::string message );

};

}}

#endif