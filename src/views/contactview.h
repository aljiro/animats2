#ifndef CONTACTVIEW_H
#define CONTACTVIEW_H

#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.hpp"
#include "simview.h"
#include "../geometry/gobject.h"

using namespace std;

namespace morph{ namespace animats{


class ContactView : public SimView{
private:
	
public:
	ContactView( Environment *environment );

	// Overwrite
	void drawObject( GeometricObject go[], int colorId);

};

}}

#endif // CONTACTVIEW_H