#include "globals.h"
#include "bounding_box.h"

#define PUMPKIN_AMOUNT 20;
class Pumpkin{
	ImageLoader il;
public:
	std::vector<float> vp, vn, vt;
	int point_count;
	GLuint loc1, loc2, loc3;
	GLuint vao;
	GLuint vbo;

	GLuint tex;
	std::vector<vec3> positions;
	BoundingBox box;
	vec3 position;
	Pumpkin();

	bool load_mesh(const char* file_name);

	std::vector<vec3> getWorldPositions();

	void generateBuffer(const char* mesh_loc);

	void loadTexture(const char *filename);

	void generateRandomPositionCoords();

	void removePumpkin(int index);

	void draw();
};