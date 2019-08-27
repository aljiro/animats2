#include "simview.h"

SimView::SimView():shaderId(-1), colorId(-1){
	if( this->init() != 0 )
		cout << "Error initializing the view!" << endl;
}

int SimView::init(){
	int width = 1024;
	int height = 768;
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
    this->window = glfwCreateWindow( width, height, "Playground", NULL, NULL);

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
    return 0;

}

void SimView::createBuffer( GeometricObject *go ){

	GLfloat* shape = collectObjectShape( go );
	glGenVertexArrays( 1, &(go->VAO) );
	glGenBuffers( 1, &(go->VBO) );	
	glBindBuffer( GL_ARRAY_BUFFER, go->VBO );
	glBufferData( GL_ARRAY_BUFFER, sizeof(shape), shape, GL_STATIC_DRAW );
	glBindVertexArray( go->VAO );	
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (void*)0 );
	glEnableVertexAttribArray( 0 );
}

// Drawing routines
void SimView::setup( Simulation &s ){
	cout << "Generating data for OpenGL" <<endl;

	// Initializing buffers for each object
	vector<SoftBody *>& softBodies = s.getSoftBodies();
	vector<RigidBody *>& rigidBodies = s.getRigidBodies();

	for( SoftBody *sb : softBodies ){
		createBuffer( sb );
	}

	for( RigidBody *rb : rigidBodies ){
		createBuffer( rb );	
	}

	cout << "Loading the shaders" << endl;
	// Loading shaders
	this->shaderId = LoadShaders( "shaders/AnimatVertexShader.vsh", 
							 	  "shaders/AnimatFragmentShader.fsh" );
	this->offsetId = glGetUniformLocation( this->shaderId, "offset");
	this->colorId = glGetUniformLocation( this->shaderId, "color");

}

GLfloat* SimView::collectObjectShape( GeometricObject *go ){
	vector<Point*> points = go->getPoints();	
	const int N = 3*points.size();
	GLfloat shape[N];

	for( int i = 0; i < N; i+=3 )
	{
		shape[i] = points[i]->x[0];
		shape[i+1] = points[i]->x[1];
		shape[i+2] = points[i]->x[2];
	}

	return shape;
}


void SimView::drawObject( GeometricObject *go ){
	GLfloat *shape = collectObjectShape( go );
	GLfloat color[4] = {1.0f, 0.0f, 0.0f, 1.0f}; // Set according to heat

	glBindBuffer( GL_ARRAY_BUFFER, go->VBO );
	glBufferData( GL_ARRAY_BUFFER, sizeof(shape), shape, GL_STATIC_DRAW );
	
	glUniform4fv( this->colorId, 1, color );
	glBindVertexArray( this->VAO );
	glDrawArrays( GL_TRIANGLES, 0, go->getPoints().size() );
	glBindVertexArray(0);
}

void SimView::draw( Simulation *s ){
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc( GL_SRC_ALPHA, GL_ONE );
	glUseProgram( this->shaderId );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	// Drawing rigid bodies first
	vector<SoftBody *>& softBodies = s.getSoftBodies();
	vector<RigidBody *>& rigidBodies = s.getRigidBodies();

	for( RigidBody *rb : rigidBodies ){
		drawObject( rb );
	}

	for( SoftBody *sb : softBodies ){
		drawObject( sb );
	}

	glfwSwapBuffers(this->window);
    glfwPollEvents();	
}

SimView::~SimView(){
	// Cleanup VBO and shader
	// TO-DO: It might be better to have a function "getAllBodies"
	vector<SoftBody *>& softBodies = s.getSoftBodies();
	vector<RigidBody *>& rigidBodies = s.getRigidBodies();

	for( RigidBody *rb : rigidBodies ){
		glDeleteBuffers(1, &(rb->VBO) );
    	glDeleteVertexArrays(1, &(rb->VAO) );
	}

	for( Softody *sb : softBodies ){
		glDeleteBuffers(1, &(sb->VBO) );
    	glDeleteVertexArrays(1, &(sb->VAO) );
	}

    glDeleteProgram(this->shaderId);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();
}
