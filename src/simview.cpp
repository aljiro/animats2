#include "simview.h"

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

}

void SimView::createBuffer( GeometricObject *go ){

	GLfloat shape[]{
		0.0f, 1.0f, 0.0f,
	    1.0f, 0.0f, 0.0f,
	    0.0f, 0.0f, 0.0f,

	    0.0f, 1.0f, 0.0f,
	    1.0f, 1.0f, 0.0f,
	    1.0f, 0.0f, 0.0f
	};

	glGenVertexArrays( 1, &(go->VAO) );
	glGenBuffers( 1, &go->VBO );	
	glBindBuffer( GL_ARRAY_BUFFER, go->VBO );
	glBufferData( GL_ARRAY_BUFFER, sizeof(shape), shape, GL_STATIC_DRAW );
	glBindVertexArray( go->VAO );	
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (void*)0 );
	glEnableVertexAttribArray( 0 );
}

// Drawing routines
void SimView::setup( Simulation &s ){
	cout << "Initializing the simulation" << endl;

	// Initializing buffers for each object
	vector<SoftBody *>& softBodies = s.getSoftBodies();

	for( SoftBody *sb : softBodies ){
		createBuffer( sb );
	}


	cout << "Generating data for OpenGL" <<endl;
	glGenVertexArrays( 1, &(this->VAO) );
	glGenBuffers( 1, &this->VBO );	
	glBindBuffer( GL_ARRAY_BUFFER, this->VBO );
	glBufferData( GL_ARRAY_BUFFER, sizeof(particle_shape), particle_shape, GL_STATIC_DRAW );
	glBindVertexArray( this->VAO );
	
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (void*)0 );
	glEnableVertexAttribArray( 0 );

	cout << "Loading the shaders" << endl;
	// Loading shaders
	this->shaderId = LoadShaders( "shaders/AnimatVertexShader.vsh", 
							 	  "shaders/AnimatFragmentShader.fsh" );
	this->offsetId = glGetUniformLocation( this->shaderId, "offset");
	this->colorId = glGetUniformLocation( this->shaderId, "color");

	cout << "Initializing the particle system" << endl;
	// Setting up the particle system
	ps = new ParticleSystem( this->n );
	ps->init();
}


void SimView::draw( ParticleSystem *ps ){
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc( GL_SRC_ALPHA, GL_ONE );
	glUseProgram( this->shaderId );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	GLfloat color[4] = {1.0f, 0.0f, 0.0f, 1.0f};

	for( Particle *p : ps->particles ){
		color[3] = 1-sqrt((pow(p->x[0],2) + pow(p->x[1],2)));
		color[1] = p->life/2.0;

		glUniform3fv( this->offsetId, 1, p->x );
		glUniform4fv( this->colorId, 1, color );
		glBindVertexArray( this->VAO );
		glDrawArrays( GL_TRIANGLES, 0, 6 );
		glBindVertexArray(0);
	}

	
}
