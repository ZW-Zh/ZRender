#include "Camera.h"
#include "Window.h"
#include "global.h"

Matrix Camera::lookat() {
	//看向模型的向量
	Vec3f z = (pos - target).normalize();

	Vec3f x = cross(up, z).normalize();
	//up
	Vec3f y = cross(z, x).normalize();
	//单位矩阵
	Matrix res = Matrix::identity();
	for (int i = 0; i < 3; i++) {
		res[0][i] = x[i];
		res[1][i] = y[i];
		res[2][i] = z[i];
		//相机中心平移
		res[i][3] = -target[i];
	}
	return res;
}


Vec3f rotate(Vec3f axis,Vec3f v, float radius) {
	mat<3,3,float> m;
	float c = cos(radius);
	float s = sin(radius);
	m[0][0] = axis.x * axis.x * (1 - c) + c;
	m[0][1] = axis.x * axis.y * (1 - c) - axis.z * s;
	m[0][2] = axis.x * axis.z * (1 - c) + axis.y * s;
	m[1][0] = axis.x * axis.y * (1 - c) + axis.z * s;
	m[1][1] = axis.y * axis.y * (1 - c) + c;
	m[1][2] = axis.y * axis.z * (1 - c) - axis.x * s;
	m[2][0] = axis.x * axis.z * (1 - c) - axis.y * s;
	m[2][1] = axis.y * axis.z * (1 - c) + axis.x * s;
	m[2][2] = axis.z * axis.z * (1 - c) + c;
	
	return m * v;
}

void updata_camera_pos(Camera& camera)
{

	//计算当前的
	Vec3f from_target = camera.pos - camera.target;			// vector point from target to camera's position
	float distance = from_target.norm();
	//水平旋转角
	float phi = (float)atan2(from_target[0], from_target[2]); // azimuth angle(方位角), angle between from_target and z-axis，[-pi, pi]
	//垂直旋转角
	float theta = (float)asin(from_target[1] / distance);		  // zenith angle(天顶角), angle between from_target and y-axis, [0, pi]
	//鼠标在x,y上移动的距离
	float x_delta = window->mouse_info.orbit_delta[0] / window->width;
	float y_delta = window->mouse_info.orbit_delta[1] / window->height;

	// for mouse wheel
	distance *= (float)pow(0.95, window->mouse_info.wheel_delta);

	//加上位移的
	float factor = 1.5 * PI;
	// for mouse left button
	phi += x_delta * factor;
	theta += y_delta * factor;
	if (theta > PI) theta = PI - 1e-5f * 100;
	if (theta < 0)  theta = 1e-5f * 100;

	camera.pos[0] = camera.target[0] + distance * sin(phi) * sin(theta);
	camera.pos[1] = camera.target[1] + distance * cos(theta);
	camera.pos[2] = camera.target[2] + distance * sin(theta) * cos(phi);
	

}

void handle_mouse_events(Camera& camera)
{
	if (window->buttons[0])
	{
		
		Vec2f cur_pos = get_mouse_pos();
		window->mouse_info.orbit_delta = window->mouse_info.orbit_pos - cur_pos;
		window->mouse_info.orbit_pos = cur_pos;
		updata_camera_pos(camera);
		window_reset();
	}
}

