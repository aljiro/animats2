#include "../include/contactview.h"

using namespace morph::animats;

ContactView::ContactView( Simulation& s ):SimView(s){

}

void ContactView::drawObject( GeometricObject go[], int colorId){
	if( !go->isVisible() )
		return;

	// Sending vertices to opengl
	glBindVertexArray( go->VAO );
	
	vector<Face*> faces = go->getFaces();	
	const int N = 3*3*faces.size();
	GLfloat pshape[N];
	GLfloat pnormals[N];
	int c = 0;
	bool inc;

	for( int i = 0; i < faces.size(); i++ )
	{
		inc =false;

		for( int j = 0; j < 3; j++ )
			if( faces[i]->points[j]->state == Invalid )
				inc = true;

		if( !inc )
			continue;

		for( int j = 0; j < 3; j++ ){
			pshape[c] = (GLfloat)(faces[i]->points[j]->x[0]);
			pnormals[c] = (GLfloat)(faces[i]->normal[0]);
			pshape[c+1] = (GLfloat)(faces[i]->points[j]->x[1]);
			pnormals[c+1] = (GLfloat)(faces[i]->normal[1]);
			pshape[c+2] = (GLfloat)(faces[i]->points[j]->x[2]);
			pnormals[c+2] = (GLfloat)(faces[i]->normal[2]);

			c+= 3;
		}
	}

	GLfloat shape[c];
	GLfloat normals[c];

	for( int i = 0; i < c; i++ ){
		shape[i] = pshape[i];
		normals[i] = pnormals[i];
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