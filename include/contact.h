#ifndef CONTACT_H
#define CONTACT_H 

namespace morph{ namespace animats{

class ContactList{
private:
	
public:
	addPoint( ContactInformation ci );
};

class Contact{
private:

public:
	explicit Contact( );
	
	virtual void solveContactRegion() = 0;
	virtual void resolve() = 0;
	bool isResting();
	~Contact();
};

class SignoriniContact: public Contact{
private:
	vector<Point*> points;
	vector<double> displacements;
public:
	SignoriniContact( SoftBody *sb, RigidBody *rb );
	void updateDisplacements();
	bool evaluateRegion();

	void solveContactRegion();
	void resolve();
};

class DeformableContact: public Contact{
	private:
	vector<Point*> pointsA;
	vector<Point*> pointsB;
	vector<double> u_A;
	vector<double> u_B;
	double delta;
public:
	SignoriniContact( SoftBody *sb, RigidBody *rb );
	void updateDisplacements();
	bool evaluateRegion();
	void solveContactRegion();
	void resolve();
};

}}

#endif