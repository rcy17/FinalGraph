#ifndef OCTREE_HPP
#define OCTREE_HPP
#include <vecmath.h>
#include "ray.hpp"

class Mesh;

struct Box
{
	Vector3f mn, mx;
	Box() {}
	Box(const Vector3f &a, const Vector3f &b) : mn(a), mx(b)
	{
	}
	Box(float mnx, float mny, float mnz,
		float mxx, float mxy, float mxz) : mn(Vector3f(mnx, mny, mnz)),
										   mx(Vector3f(mxx, mxy, mxz))
	{
	}
};

struct OctNode
{
	OctNode *child[8];
	OctNode()
	{
		child[0] = nullptr;
	}
	///@brief is this terminal
	bool isTerm()
	{
		return child[0] == nullptr;
	}
	std::vector<int> obj;
};
struct IntersectRecorder
{
	Mesh *m;
	const Ray &ray;
	Hit &hit;
	float t_min;
	bool result;
	IntersectRecorder(Mesh *m, const Ray &ray, Hit &hit, float t_min)
		: m(m), ray(ray), hit(hit), t_min(t_min), result(false)
	{
	}
	bool operator()(int idx);
};

struct Octree
{
	//if a node contains more than 7 triangles and it
	//hasn't reached the max level yet,
	///split
	static const int max_trig = 7;
	int maxLevel;
	OctNode root;
	Octree(int level = 8) : maxLevel(level)
	{
	}
	Box box;
	void build(const Mesh &m);
	void buildNode(OctNode &parent, const Box &pbox,
				   const std::vector<int> &trigs,
				   const Mesh &m, int level);

	///@brief indexing
	void proc_subtree(float tx0, float ty0, float tz0, float tx1, float ty1, float tz1,
					  OctNode *node, unsigned char aa, IntersectRecorder &f);

	void intersect(const Ray &ray, IntersectRecorder &f);
};
Octree buildOctree(const Mesh &m, int maxLevel = 7);
#endif