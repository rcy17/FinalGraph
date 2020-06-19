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
	Box(double mnx, double mny, double mnz,
		double mxx, double mxy, double mxz) : mn(Vector3f(mnx, mny, mnz)),
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
	bool isTerm() const
	{
		return child[0] == nullptr;
	}
	std::vector<int> obj;
};
struct IntersectRecorder
{
	const Mesh *m;
	const Ray &ray;
	Hit &hit;
	double t_min;
	bool result;
	IntersectRecorder(const Mesh *m, const Ray &ray, Hit &hit, double t_min)
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
	void proc_subtree(double tx0, double ty0, double tz0, double tx1, double ty1, double tz1,
					  const OctNode *node, unsigned char aa, IntersectRecorder &f) const;

	void intersect(const Ray &ray, IntersectRecorder &f) const;
};
Octree buildOctree(const Mesh &m, int maxLevel = 7);
#endif