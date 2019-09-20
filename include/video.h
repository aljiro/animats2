#ifndef SIMVIEW_H
#define SIMVIEW_H

#include "view.h"

namespace morph{ namespace animats{

class VideoRecorder : public View{
public:
	VideoRecorder();
	void setup( Simulation& s );
	void notify( Simulation& s, std::string message );

};
}

#endif