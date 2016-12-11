#include "globals.h"

class Skybox{
	ImageLoader il;
public:
	GLuint vao;
	GLuint vbo;
	GLuint texCube;
//Convient helper function which does all of the initialization work for you
	void init()
	{
		GLfloat cube_points[108]= {-SKY_SIZE,  SKY_SIZE, -SKY_SIZE,
		-SKY_SIZE, -SKY_SIZE, -SKY_SIZE,
		SKY_SIZE, -SKY_SIZE, -SKY_SIZE,
		SKY_SIZE, -SKY_SIZE, -SKY_SIZE,
		SKY_SIZE,  SKY_SIZE, -SKY_SIZE,
		-SKY_SIZE,  SKY_SIZE, -SKY_SIZE,

		-SKY_SIZE, -SKY_SIZE,  SKY_SIZE,
		-SKY_SIZE, -SKY_SIZE, -SKY_SIZE,
		-SKY_SIZE,  SKY_SIZE, -SKY_SIZE,
		-SKY_SIZE,  SKY_SIZE, -SKY_SIZE,
		-SKY_SIZE,  SKY_SIZE,  SKY_SIZE,
		-SKY_SIZE, -SKY_SIZE,  SKY_SIZE,

		SKY_SIZE, -SKY_SIZE, -SKY_SIZE,
		SKY_SIZE, -SKY_SIZE,  SKY_SIZE,
		SKY_SIZE,  SKY_SIZE,  SKY_SIZE,
		SKY_SIZE,  SKY_SIZE,  SKY_SIZE,
		SKY_SIZE,  SKY_SIZE, -SKY_SIZE,
		SKY_SIZE, -SKY_SIZE, -SKY_SIZE,

		-SKY_SIZE, -SKY_SIZE,  SKY_SIZE,
		-SKY_SIZE,  SKY_SIZE,  SKY_SIZE,
		SKY_SIZE,  SKY_SIZE,  SKY_SIZE,
		SKY_SIZE,  SKY_SIZE,  SKY_SIZE,
		SKY_SIZE, -SKY_SIZE,  SKY_SIZE,
		-SKY_SIZE, -SKY_SIZE,  SKY_SIZE,

		-SKY_SIZE,  SKY_SIZE, -SKY_SIZE,
		SKY_SIZE,  SKY_SIZE, -SKY_SIZE,
		SKY_SIZE,  SKY_SIZE,  SKY_SIZE,
		SKY_SIZE,  SKY_SIZE,  SKY_SIZE,
		-SKY_SIZE,  SKY_SIZE,  SKY_SIZE,
		-SKY_SIZE,  SKY_SIZE, -SKY_SIZE,

		-SKY_SIZE, -SKY_SIZE, -SKY_SIZE,
		-SKY_SIZE, -SKY_SIZE,  SKY_SIZE,
		SKY_SIZE, -SKY_SIZE, -SKY_SIZE,
		SKY_SIZE, -SKY_SIZE, -SKY_SIZE,
		-SKY_SIZE, -SKY_SIZE,  SKY_SIZE,
		SKY_SIZE, -SKY_SIZE,  SKY_SIZE};

		glGenVertexArrays(1, &this->vao);
		glGenBuffers(1, &vbo);
		glBindVertexArray(this->vao);
		glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cube_points), &cube_points, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);
		glBindVertexArray(0);
		createCubeMap(SKY_BACK, SKY_FRONT, SKY_TOP, SKY_BOTTOM, SKY_LEFT,
			SKY_RIGHT, &this->texCube);
	}


	bool loadCubeMapSide(GLuint texture, GLenum side_target, const char* filename)
	{
		
		int width, height, n;
		int force_channels = 4;

		unsigned char* data;
		data=il.load_image(filename, &width, &height, &n, force_channels);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
		if (!data) {
			fprintf(stderr, "ERROR: could not load %s\n", filename);
			return false;
		}
		glTexImage2D(side_target, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		il.free_image(data);
		return true;
	}

	void createCubeMap(const char* front,const char* back,const char* top,const char* bottom,const char* left,const char* right,GLuint* texCube
		)
	{
		glActiveTexture(GL_TEXTURE0);
		glGenTextures(1, &this->texCube);
		this->loadCubeMapSide(this->texCube, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, front);
		this->loadCubeMapSide(this->texCube, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, back);
		this->loadCubeMapSide(this->texCube, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, top);
		this->loadCubeMapSide(this->texCube, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, bottom);
		this->loadCubeMapSide(this->texCube, GL_TEXTURE_CUBE_MAP_POSITIVE_X, right);
		this->loadCubeMapSide(this->texCube, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, left);

		// format cube map texture
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);  
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	void drawSky()
	{
		glBindVertexArray(this->vao);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, this->texCube);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthMask(GL_TRUE);
	}
};