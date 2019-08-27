#ifndef COLLISION_H
#define COLLISION_H 

#include "contact.h"

namespace morph{ namespace animats{

class CollisionManager{
private:
	// Collision spring constant
	static const double Kc = 100000;
public:
	explicit CollisionManager();
	vector<Contact *> findCollisions();
	void pruneContacts( vector<Contact *>& contacts )
	~CollisionManager();
};

}}
#endif