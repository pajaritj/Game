#include "globals.h"
#include "pumpkin.h"
Pumpkin::Pumpkin()
	{
		generateRandomPositionCoords();
	}

	
bool Pumpkin::load_mesh(const char* file_name) {
		const aiScene* scene = aiImportFile(file_name, aiProcess_Triangulate|aiProcess_FlipUVs); // TRIANGLES!
																			 
		if (!scene) {
			fprintf(stderr, "ERROR: reading mesh %s\n", file_name);
			return false;
		}
	
		for (unsigned int m_i = 0; m_i < scene->mNumMeshes; m_i++) 
		{
			const aiMesh* mesh = scene->mMeshes[m_i];
				
			this->point_count = mesh->mNumVertices;
			for (unsigned int v_i = 0; v_i < mesh->mNumVertices; v_i++) 
			{
				if (mesh->HasPositions()) 
				{
					const aiVector3D* vp = &(mesh->mVertices[v_i]);
					//printf ("      vp %i (%f,%f,%f)\n", v_i, vp->x, vp->y, vp->z);
					this->vp.push_back(vp->x);
					this->vp.push_back(vp->y);
					this->vp.push_back(vp->z);
				}
				if (mesh->HasNormals()) 
				{
					const aiVector3D* vn = &(mesh->mNormals[v_i]);
					//printf ("      vn %i (%f,%f,%f)\n", v_i, vn->x, vn->y, vn->z);
					this->vn.push_back(vn->x);
					this->vn.push_back(vn->y);
					this->vn.push_back(vn->z);
				}
				if (mesh->HasTextureCoords(0)) 
				{
					const aiVector3D* vt = &(mesh->mTextureCoords[0][v_i]);
					//printf ("      vt %i (%f,%f)\n", v_i, vt->x, vt->y);
					this->vt.push_back(vt->x);
					this->vt.push_back(vt->y);
				}
			}
			box.findBox(mesh);
		}
		aiReleaseImport(scene);
		return true;
	}

std::vector<vec3> Pumpkin:: getWorldPositions()
	{
		return this->positions;
	}

void Pumpkin::generateBuffer(const char* mesh_loc)
	{
		load_mesh(mesh_loc);
		box.init();
		unsigned int vp_vbo = 0;
		this->loc1 = glGetAttribLocation(shaderProgramID, "vertex_position");
		this->loc2 = glGetAttribLocation(shaderProgramID, "vertex_normal");
		this->loc3 = glGetAttribLocation(shaderProgramID, "vertex_texture");

		glGenVertexArrays(1, &this->vao);
		glBindVertexArray(this->vao);

		glGenBuffers(1, &vp_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);

		glBufferData(GL_ARRAY_BUFFER, this->point_count * 3 * sizeof(float), &this->vp[0], GL_STATIC_DRAW);
		unsigned int vn_vbo = 0;
		glGenBuffers(1, &vn_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vn_vbo);
		glBufferData(GL_ARRAY_BUFFER, this->point_count * 3 * sizeof(float), &this->vn[0], GL_STATIC_DRAW);

		unsigned int vt_vbo = 0;
		glGenBuffers(1, &vt_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vt_vbo);
		glBufferData(GL_ARRAY_BUFFER, this->point_count * 2 * sizeof(float), &this->vt[0], GL_STATIC_DRAW);

		//unsigned int vao = 0;
		glBindVertexArray(this->vao);

		//Points
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

		//Normals 
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, vn_vbo);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

		//Textures
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, vt_vbo);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

	}

void Pumpkin:: loadTexture(const char *filename)
	{
		//GLuint texture;
		unsigned char *data;

		int width, height;
		int n = 0;

		data = il.load_image(filename, &width, &height, &n, 0);

		glGenTextures(1, &this->tex);
		glBindTexture(GL_TEXTURE_2D, this->tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glGenerateMipmap(GL_TEXTURE_2D);

		il.free_image(data);
	}

void Pumpkin:: removePumpkin(int index)
	{
		this->positions.erase(positions.begin()+index);

	}

void Pumpkin:: generateRandomPositionCoords()
	{
		for (int i = 0; i < PUMPKIN_AMOUNT i++)
		{
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_real_distribution<float> dis(-24, 24);

			float x = dis(gen);
			float z = dis(gen);
			vec3 pos=vec3(x,0,z);
			this->positions.push_back(pos);
		}
	}

void Pumpkin:: draw()
	{
		int model_location = glGetUniformLocation(shaderProgramID, "model");
		mat4 model = identity_mat4();
		for (int i = 0; i < positions.size(); i++)
		{
			model = identity_mat4();
			model = translate(model, positions.at(i));
			model.m[13]=0;

			glBindVertexArray(this->vao);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, this->tex);

			glUniformMatrix4fv(model_location, 1, GL_FALSE, model.m);
			glDrawArrays(GL_TRIANGLES, 0, this->point_count);
			glBindVertexArray(0);
			
		}
	}
