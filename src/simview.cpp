#include "../include/simview.h"

using namespace morph::animats;

SimView::SimView( Simulation& s ):shaderId(-1), colorId(-1){
	if( this->init() != 0 ){
		cout << "Error initializing the view!" << endl;
		return;
	}
	Debug::log(string("Setting up the SimView"));	
	this->setup( s );
}

int SimView::init(){
	Debug::log(string("Initializing SimView"));
	this->width = 1024;
	this->height = 768;
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
    glClearColor(0.0f, 0.3f, 0.4f, 0.0f);
    glEnable( GL_DEBUG_OUTPUT );
    glDebugMessageCallback( MessageCallback, 0 );

    return 0;

}

void SimView::createBuffer( GeometricObject *go ){

	vector<Face*> faces = go->getFaces();	
	const int N = 3*3*faces.size();
	GLfloat shape[N];
	int c = 0;

	for( int i = 0; i < faces.size(); i++ )
	{
		for( int j = 0; j < 3; j++ ){
			shape[c] = (GLfloat)(faces[i]->points[j]->x[0]);
			shape[c+1] = (GLfloat)(faces[i]->points[j]->x[1]);
			shape[c+2] = (GLfloat)(faces[i]->points[j]->x[2]);

			c+= 3;
		}
	}

	glGenVertexArrays( 1, &(go->VAO) );
	cout << "VAO: " << go->VAO <<endl;
	glGenBuffers( 1, &(go->VBO) );	
	cout << "VBO: " << go->VBO <<endl;
	glBindBuffer( GL_ARRAY_BUFFER, go->VBO );
	glBufferData( GL_ARRAY_BUFFER, sizeof(shape), shape, GL_STATIC_DRAW );
	glBindVertexArray( go->VAO );	

	glVertexAttribPointer( 0, 
						   3, 
						   GL_FLOAT, 
						   GL_FALSE, 
						   0, // stride 
						   (void*)0 );
	

	glEnableVertexAttribArray( 0 );
	glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LESS );
}

void SimView::transformations( GLuint programId ){
    glm::mat4 P = glm::perspective( glm::radians(45.0f), (float)width/(float)height, 0.1f, 100.0f );
    glm::mat4 V = glm::lookAt( glm::vec3(1, 1, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0) );
    glm::mat4 M = glm::mat4(0.1f);
    this->mvp = P*V*M;
}

// Drawing routines
void SimView::setup( Simulation &s ){
	Debug::log(string("Creating buffers"));
	// Initializing buffers for each object
	vector<SoftBody *>& softBodies = s.getSoftBodies();
	vector<RigidBody *>& rigidBodies = s.getRigidBodies();

	for( SoftBody *sb : softBodies ){
		createBuffer( sb );
	}

	for( RigidBody *rb : rigidBodies ){
		createBuffer( rb );	
	}

	Debug::log(string("Loading shaders"));
	// Loading shaders
	this->shaderId = LoadShaders( "shaders/AnimatVertexShader.vsh", 
							 	  "shaders/AnimatFragmentShader.fsh" );
	glUseProgram( this->shaderId );
	transformations( this->shaderId );
	this->colorId = glGetUniformLocation( this->shaderId, "color" );
	this->matrixId = glGetUniformLocation( this->shaderId, "MVP" );
}

GLfloat* SimView::collectObjectShape( GeometricObject *go ){

	vector<Point*> points = go->getPoints();	
	const int N = 3*points.size();

	GLfloat *shape = (GLfloat *)malloc(N*sizeof(GLfloat));
	int c = 0;

	for( int i = 0; i < points.size(); i++ )
	{
		shape[c] = (GLfloat)(points[i]->x[0]);
		shape[c+1] = (GLfloat)(points[i]->x[1]);
		shape[c+2] = (GLfloat)(points[i]->x[2]);

		c+= 3;
	}

	return  shape;
}


void SimView::drawObject( GeometricObject *go , int colorId){
	Debug::log(string("Collecting object shape"), LOOP);

	// GLfloat shape[] = {
	//    -1.0f, -1.0f, 0.0f,
	//    -1.0f, 1.0f, 0.0f,
	//     0.8f, -1.0f, 0.0f,
	//     1.0f, -1.0f, 0.0f,
	//     1.0f, 1.0f, 0.0f,
	//     -0.8f, 1.0f, 0.0f
	// };

	// GLfloat *shape = collectObjectShape( go );	

	vector<Face*> faces = go->getFaces();	
	const int N = 3*3*faces.size();
	GLfloat shape[N];
	int c = 0;

	for( int i = 0; i < faces.size(); i++ )
	{
		for( int j = 0; j < 3; j++ ){
			shape[c] = (GLfloat)(faces[i]->points[j]->x[0]);
			shape[c+1] = (GLfloat)(faces[i]->points[j]->x[1]);
			shape[c+2] = (GLfloat)(faces[i]->points[j]->x[2]);

			c+= 3;
		}
	}

	glEnableVertexAttribArray(0);

	Debug::log(string("Binding and initializing buffer"), LOOP);
	glBindBuffer( GL_ARRAY_BUFFER, go->VBO );
	glBufferData( GL_ARRAY_BUFFER, sizeof(shape), shape, GL_STATIC_DRAW );
	

	Debug::log(string("Passing variables to the shader"), LOOP);
	GLfloat color_soft[4] = {0.5f, 0.1f, 0.0f, 1.0f}; // Set according to heat
	GLfloat color_hard[4] = {0.5f, 0.5f, 0.5f, 1.0f}; // Set according to heat
	
	if( colorId == 1 )
		glUniform4fv( this->colorId, 1, color_soft );
	else
		glUniform4fv( this->colorId, 1, color_hard );

	glUniformMatrix4fv( this->matrixId, 1, GL_FALSE, &mvp[0][0] );
	Debug::log(string("Drawing..."), LOOP);

	glBindVertexArray( go->VAO );
	glDrawArrays( GL_TRIANGLES, 0, faces.size()*3 );
	glBindVertexArray(0);
}

void SimView::notify( Simulation& s, std::string message ){
	Debug::log(string("SimView Notified."), LOOP);
	this->draw( s );
	glfwSetWindowTitle(this->window, message.c_str() );
}

void SimView::draw( Simulation& s ){
	Debug::log(string("Drawing objects: setting up"), LOOP);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc( GL_SRC_ALPHA, GL_ONE );

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	// Drawing rigid bodies first
	vector<SoftBody *>& softBodies = s.getSoftBodies();
	vector<RigidBody *>& rigidBodies = s.getRigidBodies();
	Debug::log(string("Drawing objects: Rigid bodies"), LOOP);

	
	for( RigidBody *rb : rigidBodies ){
		drawObject( rb, 0 );
	}
	

	Debug::log(string("Drawing objects: Soft bodies"), LOOP);
	for( SoftBody *sb : softBodies ){
		drawObject( sb, 1 );
	}

	Debug::log(string("Drawing objects: Swap and Poll"), LOOP);
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