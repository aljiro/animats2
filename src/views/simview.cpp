#include "simview.h"

using namespace morph::animats;

SimView::SimView( Simulation& s ):shaderId(-1), colorId(-1){
	this->width = 1024;
	this->height = 768;
	this->lightColor = new GLfloat[3]{1,1,1};
	// this->lightPower = 0.5;
	this->lightPosition = new GLfloat[3]{3,3,3};

	cParams.position =  glm::vec3( 1, 1, 3 );
	cParams.hAngle =  3.14f;
	cParams.vAngle = 0.0f;
	cParams.fov = 45.0f;
	cParams.speed = 3.0f;
	cParams.direction = -cParams.position;
	cParams.up = glm::vec3( 0, 1, 0 );
	cParams.mouseSpeed = 0.001f;
	cParams.lastTime = glfwGetTime();

	if( this->init() != 0 ){
		cout << "Error initializing the view!" << endl;
		return;
	}
}

int SimView::init(){
	debugger.log(string("Initializing SimView"), GENERAL, "SIMVIEW");
	
	// Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        getchar();
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    this->window = glfwCreateWindow( width, height, "Animats soft body simulation", NULL, NULL);

    if( this->window == NULL ){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        getchar();
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(this->window);
    glewExperimental = true; // Needed for core profile

    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    glfwSetInputMode(this->window, GLFW_STICKY_KEYS, GL_TRUE);
    
    glEnable( GL_DEBUG_OUTPUT );
    glDebugMessageCallback( MessageCallback, 0 );
    glClearColor(0.0,0.0,0.0,0.0);
    glClearColor(0.0f, 0.3f, 0.4f, 0.0f);

 
    return 0;

}

void SimView::setViewPort( vec p ){
	cParams.position = glm::vec3(p(0), p(1), p(2));
	cParams.direction = -cParams.position;
	transformations();
}

void SimView::createBuffer( GeometricObject *go ){

	vector<Face*> faces = go->getFaces();	
	const int N = 3*3*faces.size();
	GLfloat shape[N];
	GLfloat normals[N];
	this->collectObjectShape( go, shape, normals );

	glGenVertexArrays( 1, &(go->VAO) );
	glBindVertexArray( go->VAO );	
	glGenBuffers( 1, &(go->VBO) );	
	glBindBuffer( GL_ARRAY_BUFFER, go->VBO );
	glBufferData( GL_ARRAY_BUFFER, sizeof(shape), shape, GL_STATIC_DRAW );
	glEnableVertexAttribArray( 0 );
	glVertexAttribPointer( 0, 
						   3, 
						   GL_FLOAT, 
						   GL_FALSE, 
						   0, // stride 
						   (void*)0 );	
	glGenBuffers(1, &(go->normalBuffer));
	glBindBuffer(GL_ARRAY_BUFFER, go->normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, go->normalBuffer);
	glVertexAttribPointer(
	 1,                                // attribute
	 3,                                // size
	 GL_FLOAT,                         // type
	 GL_FALSE,                         // normalized?
	 0,                                // stride
	 (void*)0                          // array buffer offset
	);

	glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LESS );
}

void SimView::transformations( ){
	debugger.log(string("Configuring transformation matrices"), LOOP, "SIMVIEW");
    this->P = glm::perspective( glm::radians(cParams.fov), (float)width/(float)height, 0.1f, 100.0f );
	// Camera matrix
	this->V = glm::lookAt(
	    cParams.position,           // Camera is here
	    cParams.position+cParams.direction, // and looks here : at the same position, plus "direction"
	    cParams.up                  // Head is up (set to 0,-1,0 to look upside-down)
	);
	this->M = glm::mat4(0.1f);
    this->MVP = this->P*this->V*this->M;
    debugger.log(string("Matrices configured!"), LOOP, "SIMVIEW");
}

void SimView::computeTransformationsFromInput(){

	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - cParams.lastTime);

	// If control is pressed, change view angle
	if( glfwGetKey( this->window, GLFW_KEY_LEFT_CONTROL ) == GLFW_PRESS ){
		double xpos, ypos;
		glfwGetCursorPos(this->window, &xpos, &ypos);

		cParams.hAngle += cParams.mouseSpeed * deltaTime * float( width/2 - xpos );
		cParams.vAngle += cParams.mouseSpeed * deltaTime * float( height/2 - ypos );
	}

	cParams.direction = glm::vec3(
					    cos(cParams.vAngle) * sin(cParams.hAngle),
					    sin(cParams.vAngle),
					    cos(cParams.vAngle) * cos(cParams.hAngle));
	glm::vec3 right = glm::vec3(
					    sin(cParams.hAngle - 3.14f/2.0f),
					    0,
					    cos(cParams.hAngle - 3.14f/2.0f));

	cParams.up = glm::cross( right, cParams.direction );

	// Move forward
	if (glfwGetKey( this->window, GLFW_KEY_UP ) == GLFW_PRESS){
	    cParams.position += cParams.direction * deltaTime * cParams.speed;
	}
	// Move backward
	if (glfwGetKey( this->window, GLFW_KEY_DOWN ) == GLFW_PRESS){
	    cParams.position -= cParams.direction * deltaTime * cParams.speed;
	}
	// Strafe right
	if (glfwGetKey( this->window, GLFW_KEY_RIGHT ) == GLFW_PRESS){
	    cParams.position += right * deltaTime * cParams.speed;
	}
	// Strafe left
	if (glfwGetKey( this->window, GLFW_KEY_LEFT ) == GLFW_PRESS){
	    cParams.position -= right * deltaTime * cParams.speed;
	}

	cParams.lastTime = currentTime;

}

// Drawing routines
void SimView::setup( Simulation &s ){
	debugger.log(string("Setting up graphics"), GENERAL, "SIMVIEW");
	// Initializing buffers for each object
	vector<SoftBody *>& softBodies = s.getSoftBodies();
	vector<RigidBody *>& rigidBodies = s.getRigidBodies();

	for( SoftBody *sb : softBodies ){
		createBuffer( sb );
	}

	for( RigidBody *rb : rigidBodies ){
		createBuffer( rb );	
	}

	debugger.log(string("Loading shaders"), GENERAL, "SIMVIEW");
	// Loading shaders
	this->shaderId = LoadShaders( "shaders/AnimatVertexShader.vsh", 
							 	  "shaders/AnimatFragmentShader.fsh" );
	glUseProgram( this->shaderId );
	transformations();

	this->colorId = glGetUniformLocation( this->shaderId, "color" );
	this->matrixId = glGetUniformLocation( this->shaderId, "MVP" );
	this->viewId = glGetUniformLocation( this->shaderId, "V" );
	this->modelId = glGetUniformLocation( this->shaderId, "M" );
	// this->lightPowerId = glGetUniformLocation( this->shaderId, "lightColor" );
	this->lightColorId = glGetUniformLocation( this->shaderId, "lightColor" );
	this->lightPositionId = glGetUniformLocation( this->shaderId, "lightPosition" );
	
}

void SimView::collectObjectShape( GeometricObject *go, GLfloat *shape, GLfloat *normals ){

	vector<Face*> faces = go->getFaces();	
	const int N = 3*3*faces.size();
	int c = 0;

	for( int i = 0; i < faces.size(); i++ )
	{
		for( int j = 0; j < 3; j++ ){
			shape[c] = (GLfloat)(faces[i]->points[j]->x[0]);
			normals[c] = (GLfloat)(faces[i]->normal[0]);
			shape[c+1] = (GLfloat)(faces[i]->points[j]->x[1]);
			normals[c+1] = (GLfloat)(faces[i]->normal[1]);
			shape[c+2] = (GLfloat)(faces[i]->points[j]->x[2]);
			normals[c+2] = (GLfloat)(faces[i]->normal[2]);

			c+= 3;
		}
	}
}


void SimView::drawObject( GeometricObject *go , int colorId){
	
	
	if( !go->isVisible() )
		return;

	// Sending vertices to opengl
	glBindVertexArray( go->VAO );
	
	vector<Face*> faces = go->getFaces();	
	const int N = 3*3*faces.size();
	GLfloat shape[N];
	GLfloat normals[N];
	int c = 0;

	for( int i = 0; i < faces.size(); i++ )
	{
		for( int j = 0; j < 3; j++ ){
			shape[c] = (GLfloat)(faces[i]->points[j]->x[0]);
			normals[c] = (GLfloat)(faces[i]->normal[0]);
			shape[c+1] = (GLfloat)(faces[i]->points[j]->x[1]);
			normals[c+1] = (GLfloat)(faces[i]->normal[1]);
			shape[c+2] = (GLfloat)(faces[i]->points[j]->x[2]);
			normals[c+2] = (GLfloat)(faces[i]->normal[2]);

			c+= 3;
		}
	}

	glEnableVertexAttribArray(0);

	glBindBuffer( GL_ARRAY_BUFFER, go->VBO );
	glBufferData( GL_ARRAY_BUFFER, sizeof(shape), shape, GL_STATIC_DRAW );
	
	glVertexAttribPointer( 0, 
						   3, 
						   GL_FLOAT, 
						   GL_FALSE, 
						   0, // stride 
						   (void*)0 );
	
	// Sending normals
	// Debug::log(string("Collecting normals"), LOOP);
	// const int n = go->faces.size();
	// vector<glm::vec3> normals;

	// for( int i = 0; i < n; i++ ){
	// 	Face *f = go->faces[i];

	// 	if( f->normal.n_elem != 3 ){
	// 		Debug::log(string("WARNING: defective normal found!"));		
	// 		f->normal = {0, 1, 0};
	// 	}

	// 	glm::vec3 cn;
	// 	cn.x = f->normal(0);
	// 	cn.y = f->normal(1);
	// 	cn.z = f->normal(2);

	// 	normals.push_back( cn ); 
	// }

	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, go->normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);

	glVertexAttribPointer(
	 1,                                // attribute
	 3,                                // size
	 GL_FLOAT,                         // type
	 GL_FALSE,                         // normalized?
	 0,                                // stride
	 (void*)0                          // array buffer offset
	);

	GLfloat color_soft[4] = {0.5f, 0.1f, 0.0f, 1.0f}; // Set according to heat

	if( go->getId() == 2 ){
		color_soft[0] = 0.0f;
		color_soft[2] = 0.5f;
		color_soft[3] = 0.0f;
	}

	GLfloat color_hard[4] = {0.5f, 0.5f, 0.5f, 1.0f}; // Set according to heat
	
	//glUseProgram( this->shaderId );

	// Setting uniforms
	if( colorId == 1 )
		glUniform4fv( this->colorId, 1, color_soft );
	else
		glUniform4fv( this->colorId, 1, color_hard );

	glUniformMatrix4fv( this->matrixId, 1, GL_FALSE, &MVP[0][0] );
	glUniformMatrix4fv( this->viewId, 1, GL_FALSE, &(this->V[0][0]) );
	glUniformMatrix4fv( this->modelId, 1, GL_FALSE, &(this->M[0][0]) );
	// glUniform4fv( this->lightPowerId, 1, &this->lightPower );
	glUniform3fv( this->lightColorId, 1, this->lightColor );
	glUniform3fv( this->lightPositionId, 1, this->lightPosition );
	
	glDrawArrays( GL_TRIANGLES, 0, faces.size()*3 );
}

void SimView::notify( Simulation& s, std::string message ){
	debugger.log(string("SimView Noti/fied."), LOOP, "SIMVIEW");

	if (glfwGetKey( this->window, GLFW_KEY_P ) == GLFW_PRESS){
		s.pause = true;
	}

	if (glfwGetKey( this->window, GLFW_KEY_U ) == GLFW_PRESS){
		s.pause = false;
	}

	computeTransformationsFromInput();
	transformations();
	this->draw( s );
	glfwSetWindowTitle(this->window, message.c_str() );

	if( glfwWindowShouldClose(window) != 0 )
		s.close();
}

void SimView::draw( Simulation& s ){
	debugger.log(string("Drawing objects: setting up"), LOOP, "SIMVIEW");
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// glBlendFunc( GL_SRC_ALPHA, GL_ONE );

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	// Drawing rigid bodies first
	vector<SoftBody *>& softBodies = s.getSoftBodies();
	vector<RigidBody *>& rigidBodies = s.getRigidBodies();
	debugger.log(string("Drawing objects: Rigid bodies"), LOOP, "SIMVIEW");

	for( RigidBody *rb : rigidBodies ){
		drawObject( rb, 0 );
	}
	
	debugger.log(string("Drawing objects: Soft bodies"), LOOP, "SIMVIEW");
	for( SoftBody *sb : softBodies ){
		drawObject( sb, 1 );
	}

	debugger.log(string("Drawing objects: Swap and Poll"), LOOP, "SIMVIEW");
	glfwSwapBuffers(this->window);
    glfwPollEvents();
}

char* SimView::checkErrors(){
	while( true ){
		const GLenum err = glGetError();

		switch (err)
	    {
		    case GL_NO_ERROR:          
		    	return "No error";
		    	break;
		    case GL_INVALID_ENUM:      
		    	return "Invalid enum" ;
		    	break;
		    case GL_INVALID_VALUE:    
		    	return "Invalid value";
		    	break;
		    case GL_INVALID_OPERATION: 
		    	return "Invalid operation";
		    	break;
		    case GL_STACK_OVERFLOW:    
		    	return  "Stack overflow";
		    	break;
		    case GL_STACK_UNDERFLOW:   
		    	return "Stack underflow";
		    	break;
		    case GL_OUT_OF_MEMORY:     
		    	return  "Out of memory";
		    	break;
		    default:                  
		    	return "Unknown error";
	    }

	    if (GL_NO_ERROR == err)
	        break;
	}
}

SimView::~SimView(){
    glDeleteProgram(this->shaderId);
    // Close OpenGL window and terminate GLFW
    glfwTerminate();
}

// Utility function
void GLAPIENTRY morph::animats::MessageCallback( GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam )
{
  fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
           ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            type, severity, message );
}