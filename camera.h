#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>

#include <glew.h>
#include "maths_funcs.h"


enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

#define	YAW				-90.0f
#define PITCH			0.0f
#define SPEED			3.0f
#define SENSITIVTY		0.25f
#define ZOOM			45.0f

class Camera
{
public:
	vec3 Position,Front,Up,Right,WorldUp;
	GLfloat Yaw,Pitch,MovementSpeed,MouseSensitivity,Zoom;
	

	Camera(vec3 position = vec3(0.0f, 0.0f, 0.0f), vec3 up = vec3(0.0f, 1.0f, 0.0f), GLfloat yaw = YAW, GLfloat pitch = PITCH) : Front(vec3(0.0f, 0.0f, 0.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
	{
		this->Position = position;
		this->WorldUp = up;
		this->Yaw = yaw;
		this->Pitch = pitch;
		this->updateFront();
	}

	void setWorldPosition(vec3 pos)
	{
		this->Position = pos;
	}

	vec3 getWorldPosition()
	{
		return this->Position;
	}

	mat4 GetViewMatrix()
	{
		return look_at(this->Position, this->Position + this->Front, this->Up);
	}

	void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime)
	{
		GLfloat velocity = this->MovementSpeed * deltaTime;
		if (direction == FORWARD)
			this->Position += this->Front * velocity;
		if (direction == BACKWARD)
			this->Position -= this->Front * velocity;
		if (direction == LEFT)
			this->Position -= this->Right * velocity;
		if (direction == RIGHT)
			this->Position += this->Right * velocity;

		this->Position.v[1] =3.0; //Bound camera to y so there's no clipping
	}
void ProcessMouse(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true)
	{
		xoffset *= this->MouseSensitivity;
		yoffset *= this->MouseSensitivity;

		this->Yaw += xoffset;
		this->Pitch += yoffset;

		if (constrainPitch)
		{
			if (this->Pitch > 89.0f)
				this->Pitch = 89.0f;
			if (this->Pitch < -89.0f)
				this->Pitch = -89.0f;
		}

		this->updateFront();
	}
	vec3 calculateNextPosition(Camera_Movement direction, GLfloat deltaTime)
	{
		vec3 position;
		vec3 position2 = this->Position;

		GLfloat velocity = this->MovementSpeed * deltaTime;
		if (direction == FORWARD)
		{
			position = this->Front * velocity;
			return position2 += position;
		}

		if (direction == BACKWARD)
		{
			position = this->Front * velocity;
			return position2 -= position;
		}
			
		if (direction == LEFT)
		{
			position = this->Right * velocity;
			return position2 -= position;
		}
			
		if (direction == RIGHT)
		{
			position = this->Right * velocity;
			return position2 += position;
		}
			
	}
	float radians(float input)	
	{
		return ONE_DEG_IN_RAD * input;
	}

private:
	void updateFront()
	{
		this->Front = normalise(vec3(cos(radians(this->Yaw)) * cos(radians(this->Pitch)),sin(radians(this->Pitch)),sin(radians(this->Yaw)) * cos(radians(this->Pitch))));
		//Normalise the right and up vectors to ensure that there is no sluggish movement of the camera
		this->Right = normalise(cross(this->Front, this->WorldUp));  
		this->Up = normalise(cross(this->Right, this->Front));
	}
};