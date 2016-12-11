#include "globals.h"
#include "mesh.h"
#include "camera.h"
#include "skybox.h"
#include "pumpkin.h"

using namespace std;
GLuint shaderProgramID;
GLuint skyboxShaderID;
int width = 800;
int height = 600;
Mesh terrain, tree,creepy,house;
Pumpkin pumpkin;
Skybox skybox;
BYTE pHeightMap [1920*1339];
Camera camera;
bool fMouse;
enum ObjectType {
	TREE,
	PUMPKIN,
	HOUSE,
	CREEPY_TREE,
	COFFIN
};
//Middle of screen
GLfloat lastX = width / 2;
GLfloat lastY = height / 2;
std::vector<vec4> pump_pos;
float camX = 5;
float camY =3.0;
float camZ = 5;
int walkingSpeed=20;
float  delta;

// Shader Functions- click on + to expand
#pragma region SHADER_FUNCTIONS

// Create a NULL-terminated string by reading the provided file
char* readShaderSource(const char* shaderFile) {   
    FILE* fp = fopen(shaderFile, "rb"); //!->Why does binary flag "RB" work and not "R"... wierd msvc thing?

    if ( fp == NULL ) { return NULL; }

    fseek(fp, 0L, SEEK_END);
    long size = ftell(fp);

    fseek(fp, 0L, SEEK_SET);
    char* buf = new char[size + 1];
    fread(buf, 1, size, fp);
    buf[size] = '\0';

    fclose(fp);

    return buf;
}


static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
	// create a shader object
    GLuint ShaderObj = glCreateShader(ShaderType);

    if (ShaderObj == 0) {
        fprintf(stderr, "Error creating shader type %d\n", ShaderType);
        exit(0);
    }
	const char* pShaderSource = readShaderSource( pShaderText);

	// Bind the source code to the shader, this happens before compilation
	glShaderSource(ShaderObj, 1, (const GLchar**)&pShaderSource, NULL);
	// compile the shader and check for errors
    glCompileShader(ShaderObj);
    GLint success;
	// check for shader related errors using glGetShaderiv
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar InfoLog[1024];
        glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
        fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
        exit(1);
    }
	// Attach the compiled shader object to the program object
    glAttachShader(ShaderProgram, ShaderObj);
}

GLuint CompileShaders(const char* vertex, const char* frag)
{
	//Start the process of setting up our shaders by creating a program ID
	//Note: we will link all the shaders together into this ID
    GLuint shaderID = glCreateProgram();
    if (shaderID == 0) {
        fprintf(stderr, "Error creating shader program\n");
        exit(1);
    }

	// Create two shader objects, one for the vertex, and one for the fragment shader
    AddShader(shaderID, vertex, GL_VERTEX_SHADER);
    AddShader(shaderID, frag, GL_FRAGMENT_SHADER);

    GLint Success = 0;
    GLchar ErrorLog[1024] = { 0 };
	// After compiling all shader objects and attaching them to the program, we can finally link it
    glLinkProgram(shaderID);
	// check for program related errors using glGetProgramiv
    glGetProgramiv(shaderID, GL_LINK_STATUS, &Success);
	if (Success == 0) {
		glGetProgramInfoLog(shaderID, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
        exit(1);
	}

	// program has been successfully linked but needs to be validated to check whether the program can execute given the current pipeline state
    glValidateProgram(shaderID);
	// check for program related errors using glGetProgramiv
    glGetProgramiv(shaderID, GL_VALIDATE_STATUS, &Success);
    if (!Success) {
        glGetProgramInfoLog(shaderID, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
        exit(1);
    }
	// Finally, use the linked shader program
	// Note: this program will stay in effect for all draw calls until you replace it with another or explicitly disable its use
    //glUseProgram(shaderID);
	return shaderID;
}
#pragma endregion SHADER_FUNCTIONS


void drawSkybox()
{
	glUseProgram(skyboxShaderID);
	glDepthMask(GL_FALSE);
	mat4 view = camera.GetViewMatrix();
	view.m[12]=0;
	view.m[13]=0;
	view.m[14]=0;
	mat4 persp_proj =  perspective(80.0, (float)width/(float)height, 0.1, 100.0);
	int sky_view_mat_location = glGetUniformLocation(skyboxShaderID, "view");
	int sky_proj_mat_location = glGetUniformLocation(skyboxShaderID, "proj");
	glUniformMatrix4fv(sky_proj_mat_location, 1, GL_FALSE, persp_proj.m);
	glUniformMatrix4fv(sky_view_mat_location, 1, GL_FALSE, view.m);
	glBindVertexArray(skybox.vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.texCube);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthMask(GL_TRUE);
}


void display(){

	// tell GL to only draw onto a pixel if the shape is closer to the viewer
	glEnable (GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc (GL_LESS); // depth-testing interprets a smaller value as "closer"
	glClearColor (0.5f, 0.5f, 0.5f, 1.0f);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	drawSkybox();
	glUseProgram (shaderProgramID);

	int matrix_location = glGetUniformLocation (shaderProgramID, "model");
	int view_mat_location = glGetUniformLocation (shaderProgramID, "view");
	int proj_mat_location = glGetUniformLocation (shaderProgramID, "proj");

	mat4 view = camera.GetViewMatrix();
	mat4 persp_proj =  perspective(80.0, (float)width/(float)height, 0.1, 100.0);
	mat4 model = identity_mat4 ();

	glUniformMatrix4fv (proj_mat_location, 1, GL_FALSE, persp_proj.m);
	glUniformMatrix4fv (view_mat_location, 1, GL_FALSE, view.m);
	glUniformMatrix4fv (matrix_location, 1, GL_FALSE, model.m);

	terrain.draw(0,0,0);
	house.draw(0,0,0,1);
	creepy.draw(-5,0,8,1);
	for(int i=-26;i<=28;i+=2){
		tree.draw(26,0,i);
		tree.draw(-28,0,i);
		tree.draw(i,0,-27);
		tree.draw(i,0,27);
	}
	pumpkin.draw();
	glutSwapBuffers();
}


void updateScene() {	

	static DWORD  last_time = 0;
	DWORD  curr_time = timeGetTime();
	delta = (curr_time - last_time) * 0.001f;
	if (delta > 0.03f)
		delta = 0.03f;
	last_time = curr_time;

	glutPostRedisplay();
}




void init()
{
	// Set up the shaders
	
	shaderProgramID = CompileShaders(VERTEX_SHADER, FRAGMENT_SHADER);
	skyboxShaderID = CompileShaders(SKYBOX_VERTEX_SHADER, SKYBOX_FRAG_SHADER);
	skybox.init();
	camera.setWorldPosition(vec3(camX,camY,camZ));
	
	terrain.loadTexture(GROUND_TEX);
	terrain.generateBuffer(GROUND_MESH);
	tree.loadTexture(TREE_TEX);
	tree.generateBuffer(TREE_MESH);
	creepy.loadTexture(CREEPY_TEX);
	creepy.generateBuffer(CREEPY_MESH);
	pumpkin.loadTexture(PUMPKIN_TEX);
	pumpkin.generateBuffer(PUMPKIN_MESH);
	house.loadTexture(HOUSE_TEX);
	house.generateBuffer(HOUSE_MESH);
}
float calcDistance(float pX, float pZ, float oX, float oZ)
	{
		float distX = oX - pX;
		float distZ = oZ - pZ;

		float dist = sqrt((pow(distX, 2) + pow(distZ, 2)));
		return dist;
	}
bool detectCollision(ObjectType type, std::vector<vec3> objectPositions, vec3 playerPos)
	{
		float playerX = playerPos.v[0];
		float playerZ = playerPos.v[2];

		for (int i = 0; i < objectPositions.size(); i++)
		{
			float objX = objectPositions[i].v[0];
			float objZ = objectPositions[i].v[2];

			float dist = calcDistance(playerX, playerZ, objX, objZ);

			if(dist <= (pumpkin.box.size.v[0])/2)
			{
				if (type== PUMPKIN)
				{
					//Remove medkit from the screen
					pumpkin.removePumpkin(i);
					return true;
				}
				return true;
			}
		}
		return false;
	}
void keypress(unsigned char key, int x, int y) 
{

	if(key=='w')
	{
		vec3 pos = camera.calculateNextPosition(FORWARD, delta * walkingSpeed);
		if (detectCollision(PUMPKIN, pumpkin.getWorldPositions(), pos))
		{
			camera.ProcessKeyboard(FORWARD, delta * walkingSpeed);
		}
		else{
		camera.ProcessKeyboard(FORWARD, delta * walkingSpeed);
		}
		}
	if (key == 's')
	{
		vec3 pos = camera.calculateNextPosition(FORWARD, delta * walkingSpeed);
		if (detectCollision(PUMPKIN, pumpkin.getWorldPositions(), pos))
		{			
			camera.ProcessKeyboard(BACKWARD, delta * walkingSpeed);
		}
		else{
		camera.ProcessKeyboard(BACKWARD, delta * walkingSpeed);
		}
	}
	if (key == 'd')
	{ 
			vec3 pos = camera.calculateNextPosition(FORWARD, delta * walkingSpeed);
		if (detectCollision(PUMPKIN, pumpkin.getWorldPositions(), pos))
		{		
			camera.ProcessKeyboard(RIGHT, delta * walkingSpeed);
		}
		else{
		camera.ProcessKeyboard(RIGHT, delta * walkingSpeed);
		}
	}
	if (key == 'a') 
	{
		vec3 pos = camera.calculateNextPosition(FORWARD, delta * walkingSpeed);
		if (detectCollision(PUMPKIN, pumpkin.getWorldPositions(), pos))
		{	
			camera.ProcessKeyboard(LEFT, delta * walkingSpeed);
		}
		else{
			camera.ProcessKeyboard(LEFT, delta * walkingSpeed);
		}
	}
}

void handleMouseMove(int x, int y)
{
	  if (fMouse)
        {
            lastX = x;
            lastY = y;
            fMouse = false;
        }

        GLfloat xOff = x - lastX;
        GLfloat yOff = lastY - y;

        lastX = x;
        lastY = y;

        camera.ProcessMouse(xOff, yOff);
}

int main(int argc, char** argv){

	// Set up the window
	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
    glutInitWindowSize(width, height);
    glutCreateWindow("Pumpkins");
	// Tell glut where the display function is
	glutDisplayFunc(display);
	glutIdleFunc(updateScene);
	glutKeyboardFunc(keypress);
	glutPassiveMotionFunc(handleMouseMove);

	 // A call to glewInit() must be done after glut is initialized!
    GLenum res = glewInit();
	// Check for any errors
    if (res != GLEW_OK) {
      fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
      return 1;
    }
	// Set up your objects and shaders
	init();
	// Begin infinite event loop
	glutMainLoop();
    return 0;
}