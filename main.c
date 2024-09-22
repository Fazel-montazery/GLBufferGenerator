#include <stdio.h>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define MAX_FILENAME_LENGTH 128

char* generate_filename(const char* name, int postfix) ;
int process_model(const char* path);
void process_mesh(C_STRUCT aiMesh *mesh);
void process_node(C_STRUCT aiNode *node, const C_STRUCT aiScene* scene);

int main(int argc, char** argv)
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s [model file]", argv[0]);
		return -1;
	}

	if(!process_model(argv[1])) {
		return -1;
	};

	return 0;
}

char* generate_filename(const char* name, int postfix) 
{
    static char filename[MAX_FILENAME_LENGTH];
    snprintf(filename, sizeof(filename), "%s%d.txt", name, postfix);
    return filename;
}

int process_model(const char* path)
{
	const C_STRUCT aiScene* scene = aiImportFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_OptimizeMeshes);

	if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		fprintf(stderr, "ERROR::ASSIMP::%s\n", aiGetErrorString());
		return 0;
	}

	process_node(scene->mRootNode, scene);
	aiReleaseImport(scene);

	return 1;
}

static int meshCount = 0;
void process_mesh(C_STRUCT aiMesh *mesh)
{
	FILE* meshFile = fopen(generate_filename("Mesh", meshCount), "w");
	fprintf(meshFile, "####################################\n"
			  "# Generated with GLBufferGenerator #\n"
			  "####################################\n\n");
	// Vertex positions
	fprintf(meshFile, "##Vertices positions##\n");
	for(unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		// process vertex positions, normals
		float x = mesh->mVertices[i].x;
		float y = mesh->mVertices[i].y;
		float z = mesh->mVertices[i].z; 

		fprintf(meshFile, "%.1f, %.1f, %.1f,\n", x, y, z);
	}

	// Vertex normals
	fprintf(meshFile, "\n##Vertices Normals##\n");
	for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
		float x = mesh->mNormals[i].x;
		float y = mesh->mNormals[i].y;
		float z = mesh->mNormals[i].z; 
		fprintf(meshFile, "%.1f, %.1f, %.1f,\n", x, y, z);
	}

	// Combined
	fprintf(meshFile, "\n##Combined##\n");
	for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
		float xp = mesh->mNormals[i].x;
		float yp = mesh->mNormals[i].y;
		float zp = mesh->mNormals[i].z; 
		float xn = mesh->mNormals[i].x;
		float yn = mesh->mNormals[i].y;
		float zn = mesh->mNormals[i].z; 
		fprintf(meshFile, "%.1f, %.1f, %.1f, %.1f, %.1f, %.1f,\n",
				xp, yp, zp, xn, yn, zn);
	}

	// Indices
	fprintf(meshFile, "\n##Indices##\n");
	const int width = 3;
	int indexCount = 0;
	for(unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		C_STRUCT aiFace face = mesh->mFaces[i];
		for(unsigned int j = 0; j < face.mNumIndices; j++) {
			indexCount++;
			unsigned int index = face.mIndices[j];
			fprintf(meshFile, "%u, ", index);
			if (indexCount % width == 0) {
				fprintf(meshFile, "\n");
			}
		}
	}
	
	fclose(meshFile);
}

void process_node(C_STRUCT aiNode *node, const C_STRUCT aiScene* scene)
{
	// process all the node's meshes
	for(unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		C_STRUCT aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		process_mesh(mesh);
	}
	// then do the same for each of its children
	for(unsigned int i = 0; i < node->mNumChildren; i++)
	{
		process_node(node->mChildren[i], scene);
	}
}
