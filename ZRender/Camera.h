#pragma once

#include "geometry.h"


class Camera
{
public:
	Camera(Vec3f p, Vec3f t, Vec3f u, float a) : pos(p), target(t), up(u), aspect(a) {};

	Vec3f pos;
	Vec3f target;
	Vec3f up;
	
	Matrix lookat();
	float aspect;
};

//handle event
void updata_camera_pos(Camera& camera);
void handle_mouse_events(Camera& camera);