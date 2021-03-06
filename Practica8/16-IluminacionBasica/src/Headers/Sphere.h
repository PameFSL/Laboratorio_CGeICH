#ifndef SPHERE_H
#define SPHERE_H
#define _USE_MATH_DEFINES
#include <cmath>
#include "Abstractmodel.h"

class Sphere : public AbstractModel
{
public:
	Sphere(int slices, int stacks, float ratio = 0.5);
	~Sphere();
	virtual bool rayPicking(glm::vec3 init, glm::vec3 end, glm::vec3 &intersection);
private:
	SBB sbb;
};

#endif // SPHERE_H