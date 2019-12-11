#pragma once
#include <Pch/Pch.h>

struct Transform {
	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;

	Transform()
	{
		position = glm::vec3();
		rotation = glm::quat();
		scale = glm::vec3(1.0f);
	}
};

struct Vertex
{
	glm::vec3 position;
	glm::vec2 UV;
	glm::vec3 Normals;
	glm::vec3 Tangent;
};

struct Vertex2
{
	glm::vec3 position;
	glm::vec2 UV;
	glm::vec3 Normals;

	glm::ivec4 bone;
	glm::vec4 weight;
};

struct Face
{
	int indices[3];
};

struct Material
{
	std::string name;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	int rimLighting;
	bool texture;
	bool normalMap;
	std::vector<GLuint> textureID;	
	GLuint normalMapID;
};

struct Buffers
{
	GLuint vao;
	GLuint vbo;
	GLuint ibo;
	int nrOfFaces;
};

struct Skeleton
{
	struct Joint
	{
		std::string name;
		int parentIndex;
		glm::mat4 invBindPose;
	};

	std::string name;
	std::vector<Joint> joints;

	Skeleton()
	{
		name = "";
	}
};
struct AnimationState
{
	bool running = false;
	bool casting = false;
	bool jumping = false;
	bool deflecting = false;
	bool idle = false;
	bool casTripple = false;
	bool castPotion = false;
	
};


struct Animation
{
	// Skeleton animation
	struct skKeyframe
	{
		struct skTransform
		{
			int jointid;
			Transform transform;
		};
		int id;
		std::vector<skTransform> local_joint_t;
	};

	// Standards
	std::string name;
	int keyframeFirst;
	int keyframeLast;
	float duration;
	float rate;
	std::vector<skKeyframe> keyframes;

};

struct BonePalleteBuffer
{
	glm::mat4 bones[64]{ glm::mat4(1.0f) };
};

struct DelaunayTriangulation
{
	std::vector<glm::vec2> vertices;
	std::vector<int> faces;

	std::vector<Vertex> GetAsVertices()
	{
		std::vector<Vertex> result;
		result.resize(vertices.size());
		for (int i = 0; i < (int)vertices.size(); i++)
		{
			glm::vec3 pos = glm::vec3(vertices[i].x, vertices[i].y, 0.0f);
			result[i].position = pos;
		}

		return result;
	}

	std::vector<Face> GetAsFaces()
	{
		std::vector<Face> result;
		result.resize((int)faces.size() / 3);
		for (int i = 0; i < (int)faces.size(); i += 3)
		{
			result[i / 3].indices[0] = faces[i];
			result[i / 3].indices[1] = faces[i + 1];
			result[i / 3].indices[2] = faces[i + 2];
		}

		return result;
	}

	void Clear()
	{
		vertices.clear();
		vertices.shrink_to_fit();
		faces.clear();
		faces.shrink_to_fit();
	}
	
};

struct PointTriangle
{
	int point;
	int triangle;

	PointTriangle(int p, int t)
	{
		point = p;
		triangle = t;
	}
};

struct Edge
{
	enum EdgeType
	{
		Line,
		RayCCW,
		RayCW,
		Segment
	};

	EdgeType type;

	int site;
	int vertex_0;
	int vertex_1;

	glm::vec2 direction;

	Edge(EdgeType o_type, int o_site, int o_vertex_0, int o_vertex_1, glm::vec2 o_direction)
	{
		type = o_type;
		site = o_site;
		vertex_0 = o_vertex_0;
		vertex_1 = o_vertex_1;
		direction = o_direction;
	}

};

struct VoroniDiagram
{
	DelaunayTriangulation triangulation;

	std::vector<glm::vec2> sites;
	std::vector<glm::vec2> vertices;
	std::vector<Edge> edges;

	std::vector<int> firstEdgeBySite;

	VoroniDiagram()
	{
		sites = triangulation.vertices;
	}

	void Clear()
	{
		triangulation.Clear();

		sites.clear();
		sites.shrink_to_fit();
		vertices.clear();
		vertices.shrink_to_fit();
		edges.clear();
		edges.shrink_to_fit();
		firstEdgeBySite.clear();
		firstEdgeBySite.shrink_to_fit();
	}


};