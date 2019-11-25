#pragma once
#include "pch.h"

#include "CollisionVolume.h"
#include "ConvexHull.h"

#include "math/Vector3.h"
using math::Vector3;

#include "world/Collision.h"

namespace geom {
	struct SupportPoint {
		Vector3 v; // the minkowski difference point

		// the individual support points
		Vector3 sup_a;
		Vector3 sup_b; // not actually necessary

		bool operator==(const SupportPoint &r) const { return v == r.v; }
	};
	struct Simplex {
		Simplex() : size(0), a(vertices[0]), b(vertices[1]), c(vertices[2]), d(vertices[3]) {}
		SupportPoint vertices[4];
		unsigned int size;
		SupportPoint & a;
		SupportPoint & b;
		SupportPoint & c;
		SupportPoint & d;
		void Push(SupportPoint vertex) {
			for (unsigned int i = size; i > 0; i--) {
				vertices[i] = vertices[i - 1];
			}
			vertices[0] = vertex;
			size++;
		}
		void Set(SupportPoint p_a) { a = p_a; size = 1; }
		void Set(SupportPoint p_a, SupportPoint p_b) { a = p_a; b = p_b; size = 2; }
		void Set(SupportPoint p_a, SupportPoint p_b, SupportPoint p_c) { a = p_a; b = p_b; c = p_c; size = 3;  }
	};
	struct GjkIntersection {
		GjkIntersection() {}
		Simplex Simplex;
		Vector3 Direction;
	};
	
	struct Triangle {
		SupportPoint points[3];
		Vector3 n;

		Triangle( SupportPoint a,  SupportPoint b,  SupportPoint c) {
			points[0] = a;
			points[1] = b;
			points[2] = c;
			n = ((b.v - a.v).Cross(c.v - a.v));
			n.Normalize();
		}
	};

	struct Edge {
		SupportPoint points[2];

		Edge(const SupportPoint &a, const SupportPoint &b) {
			points[0] = a;
			points[1] = b;
		}
	};
	inline SupportPoint MinkowskiDifference(Vector3 dir, CollisionVolume& volumeA, CollisionVolume& volumeB) {
		SupportPoint diff;
		diff.sup_a = volumeA.Support(dir);
		diff.sup_b = volumeB.Support(-dir);
		diff.v = diff.sup_a - diff.sup_b;
		return diff;
	}
	inline void NearestSimplexHelper(GjkIntersection & result, SupportPoint & a, SupportPoint & b, Vector3 & ao, Vector3 & ab) {
		if (ab.Dot(ao) > 0) {
			result.Simplex.Set(a,b);
			result.Direction = ab.Cross(ao).Cross(ab);
		}
		else {
			result.Simplex.Set(a);
			result.Direction = ao;
		}
	}
	#define gjk_simtest(v) (v.Dot(ao) > 0)
	inline bool NearestSimplex(GjkIntersection & result) {
		Vector3 & dir = result.Direction;
		Simplex & sim = result.Simplex;
		const Vector3 ao = -sim.a.v;
		// simplex tests
		if (result.Simplex.size == 2) {
			// simplex is a line, being here means that the early out was passed, and thus
			// the origin must be between point a and b
			// search direction is perpendicular to ab and coplanar with ao
			const Vector3 ab = (sim.b.v - sim.a.v);
			dir = ab.Cross(ao).Cross(ab);
			return false;
		}
		else if (result.Simplex.size == 3) {
			// simplex is a triangle, meaning that the origin must be
			const Vector3 ab = (sim.b.v - sim.a.v);
			const Vector3 ac = (sim.c.v - sim.a.v);
			const Vector3 ad = (sim.d.v - sim.a.v);
			const Vector3 abc = ab.Cross(ac);

			if (gjk_simtest(ab.Cross(abc))) {
				// origin is outside the triangle, near the edge ab
				// reset the simplex to the line ab and continue
				// search direction is perpendicular to ab and coplanar with ao
				sim.Set(sim.a, sim.b);
				dir = ab.Cross(ao).Cross(ab);
				return false;
			}

			if (gjk_simtest(abc.Cross(ac))) {
				// origin is outside the triangle, near the edge ac
				// reset the simplex to the line ac and continue
				// search direction is perpendicular to ac and coplanar with ao
				sim.Set(sim.a, sim.c);
				dir = ac.Cross(ao).Cross(ac);
				return false;
			}

			// origin is within the triangular prism defined by the triangle
			// determine if it is above or below
			if (gjk_simtest(abc)) {
				// origin is above the triangle, so the simplex is not modified,
				// the search direction is the triangle's face normal
				dir = abc;
				return false;
			}

			// origin is below the triangle, so the simplex is rewound the opposite direction
			// the search direction is the new triangle's face normal
			sim.Set(sim.a, sim.c, sim.b);
			dir = -abc;
			return false;
		}
		else { // == 4
		 // the simplex is a tetrahedron, must check if it is outside any of the side triangles, (abc, acd, adb)
		 // if it is then Set the simplex equal to that triangle and continue, otherwise we know
		 // there is an intersection and exit

		 // check the triangles (abc,acd,adb), scoped as the temporary variables used here
		 // will no longer be valid afterward
			; {
				const Vector3 ab = (sim.b.v - sim.a.v);
				const Vector3 ac = (sim.c.v - sim.a.v);

				if (gjk_simtest(ab.Cross(ac))) {
					// origin is in front of triangle abc, simplex is already what it needs to be
					// go to jmp_face
					goto jmp_face;
				}

				const Vector3 ad = (sim.d.v - sim.a.v);

				if (gjk_simtest(ac.Cross(ad))) {
					// origin is in front of triangle acd, simplex is Set to this triangle
					// go to jmp_face
					sim.Set(sim.a, sim.c, sim.d);
					goto jmp_face;
				}

				if (gjk_simtest(ad.Cross(ab))) {
					// origin is in front of triangle adb, simplex is Set to this triangle
					// go to jmp_face
					sim.Set(sim.a, sim.d, sim.b);
					goto jmp_face;
				}

				// intersection confirmed, break from the loop
				return true;
			}

		jmp_face:
			// the simplex is equal to the triangle that the origin is in front of
			// this is exactly the same as the triangular simplex test except that we know
			// that the origin is not behind the triangle
			const Vector3 ab = (result.Simplex.b.v - result.Simplex.a.v);
			const Vector3 ac = (result.Simplex.c.v - result.Simplex.a.v);
			const Vector3 abc = ab.Cross(ac);

			if (gjk_simtest(ab.Cross(abc))) {
				result.Simplex.Set(result.Simplex.a, result.Simplex.b);
				dir = ab.Cross(ao).Cross(ab);
				return false;
			}

			if (gjk_simtest(abc.Cross(ac))) {
				result.Simplex.Set(result.Simplex.a, result.Simplex.c);
				dir = ac.Cross(ao).Cross(ac);
				return false;
			}

			sim.Set(result.Simplex.a, result.Simplex.b, result.Simplex.c);
			dir = abc;
			return false;
		}
	}
	
	inline bool GJK(CollisionVolume & volumeA, CollisionVolume& volumeB, GjkIntersection & result) {
		Vector3 initialAxis = Vector3::UnitX;
		SupportPoint a = MinkowskiDifference(initialAxis, volumeA, volumeB);
		result.Simplex.Set(a);
		result.Direction = -a.v;
		
		
		for (int i = 0; i < 20;i++) {
			result.Direction.Normalize();
			a = MinkowskiDifference(result.Direction,volumeA,volumeB);
			if (a.v.Dot(result.Direction) < 0) {
				return false;
			}
			result.Simplex.Push(a);
			if (NearestSimplex(result)) {
				return true;
			}
		}
		return false;
	}
	inline void Barycentric(Vector3 p, Vector3 a, Vector3 b, Vector3 c, float &u, float &v, float &w)
	{
		Vector3 v0 = b - a, v1 = c - a, v2 = p - a;
		float d00 = v0.Dot(v0);
		float d01 = v0.Dot(v1);
		float d11 = v1.Dot(v1);
		float d20 = v2.Dot(v0);
		float d21 = v2.Dot(v1);
		float denom = d00 * d11 - d01 * d01;
		v = (d11 * d20 - d01 * d21) / denom;
		w = (d00 * d21 - d01 * d20) / denom;
		u = 1.0f - v - w;
	}
	inline bool ExtrapolateContactInformation(const Triangle* triangle, world::Collision::Contact & contactData) {
		//const Matrix& colliderA_toWorld = contactData->colliders[0]->collision_detection.mtxColliderToWorld;
		//const Matrix& colliderB_toWorld = contactData->colliders[1]->collision_detection.mtxColliderToWorld;

		const float distanceFromOrigin = triangle->n.Dot(triangle->points[0].v);

		// calculate the barycentric coordinates of the closest triangle with respect to
		// the projection of the origin onto the triangle
		float bary_u, bary_v, bary_w;
		Barycentric(triangle->n * distanceFromOrigin,
			triangle->points[0].v,
			triangle->points[1].v,
			triangle->points[2].v,
			bary_u,
			bary_v,
			bary_w);

		// barycentric can fail and generate invalid coordinates, if this happens return false
		//if (!is_valid(bary_u) || !is_valid(bary_v) || !is_valid(bary_w)) return false;

		// if any of the barycentric coefficients have a magnitude greater than 1, then the origin is not within the triangular prism described by 'triangle'
		// thus, there is no collision here, return false
		if (std::fabs(bary_u) > 1.0f || std::fabs(bary_v) > 1.0f || std::fabs(bary_w) > 1.0f) return false;

		// collision point on object a in world space
		contactData.Point = Vector3(
			((triangle->points[0].sup_a) * bary_u) +
			((triangle->points[1].sup_a) * bary_v) +
			((triangle->points[2].sup_a) * bary_w));

		// collision normal
		contactData.Normal = -triangle->n;

		// penetration depth
		contactData.PenetrationDepth = distanceFromOrigin;


		/*for (uint8 i = 0; i < 2; i++)
			for (uint8 j = 0; j < 3; j++)
				contactData->triangleSupports_local[i][j] = triangle->points[j].localSupports[i];*/

		return true;
	}
	inline bool EPA(CollisionVolume& volumeA, CollisionVolume& volumeB, GjkIntersection & intersection, world::Collision::Contact& contactInfo) {
		
		//Expanding Polytope Algorithm (EPA) as described here: http://allenchou.net/2013/12/game-physics-contact-generation-epa/
		const float _EXIT_THRESHOLD = 0.001f;
		const unsigned _EXIT_ITERATION_LIMIT = 50;
		unsigned _EXIT_ITERATION_CUR = 0;

		std::list<Triangle> lst_triangles;
		std::list<Edge> lst_edges;

		// process the specified edge, if another edge with the same points in the
		// opposite order exists then it is removed and the new point is also not added
		// this ensures only the outermost ring edges of a cluster of triangles remain
		// in the list
		auto lam_addEdge = [&]( SupportPoint &a, SupportPoint &b)->void {
			for (auto it = lst_edges.begin(); it != lst_edges.end(); it++) {
				if (it->points[0] == b && it->points[1] == a) {
					//opposite edge found, remove it and do not add new one
					lst_edges.erase(it);
					return;
				}
			}
			lst_edges.emplace_back(a, b);
		};

		// add the GJK simplex triangles to the list
		lst_triangles.emplace_back(intersection.Simplex.a, intersection.Simplex.b, intersection.Simplex.c);
		lst_triangles.emplace_back(intersection.Simplex.a, intersection.Simplex.c, intersection.Simplex.d);
		lst_triangles.emplace_back(intersection.Simplex.a, intersection.Simplex.d, intersection.Simplex.b);
		lst_triangles.emplace_back(intersection.Simplex.b, intersection.Simplex.d, intersection.Simplex.c);

		while (true) {
			if (_EXIT_ITERATION_CUR++ >= _EXIT_ITERATION_LIMIT) return false;

			// find closest triangle to origin
			std::list<Triangle>::iterator entry_cur_triangle_it = lst_triangles.begin();
			float entry_cur_dst = FLT_MAX;
			for (auto it = lst_triangles.begin(); it != lst_triangles.end(); it++) {
				float dst = fabs(it->n.Dot(it->points[0].v));
				if (dst < entry_cur_dst) {
					entry_cur_dst = dst;
					entry_cur_triangle_it = it;
				}
			}

			// get the next support point in front of the current triangle, away from the origin
			const SupportPoint entry_cur_support = MinkowskiDifference(entry_cur_triangle_it->n,volumeA,volumeB);

			// checks how much further this new point will takes us from the origin
			// if it is not far enough then we assume we have found the closest triangle
			// on the hull from the origin
			if (entry_cur_triangle_it->n.Dot(entry_cur_support.v) - entry_cur_dst < _EXIT_THRESHOLD) {
				//////////////////////////////////////////////////////////////////////////
				//////////////////////////////////////////////////////////////////////////
				// GENERATE CONTACT INFO AND RETURN

				return ExtrapolateContactInformation(&*entry_cur_triangle_it,contactInfo);
			}

			for (auto it = lst_triangles.begin(); it != lst_triangles.end(); ) {
				// can this face be 'seen' by entry_cur_support?
				if (it->n.Dot(entry_cur_support.v - it->points[0].v) > 0) {
					lam_addEdge(it->points[0], it->points[1]);
					lam_addEdge(it->points[1], it->points[2]);
					lam_addEdge(it->points[2], it->points[0]);
					it = lst_triangles.erase(it);
					continue;
				}
				it++;
			}

			// create new triangles from the edges in the edge list
			for (auto it = lst_edges.begin(); it != lst_edges.end(); it++) {
				lst_triangles.emplace_back(entry_cur_support, it->points[0], it->points[1]);
			}

			lst_edges.clear();
		}

		return false;
	}
	struct SatResult {
		SatResult() {}
		Vector3 Axis;

	};
	
	inline bool DoProjectionsOverlap(Vector3 & axis, shared_ptr<CollisionVolume> & volumeA, shared_ptr<CollisionVolume> & volumeB) {
		auto projectionA = volumeA->Project(axis);
		auto projectionB = volumeB->Project(axis);
		return (projectionA.second >= projectionB.first && projectionB.second >= projectionA.first);
	}
	// generate axes from boxes
	inline std::vector<Vector3> GenerateSatAxes(vector<Vector3> & axesA, vector<Vector3> & axesB) {
		std::vector<Vector3> axes;
		for (auto & a : axesA) {
			for (auto & b : axesB) {
				Vector3 cross = a.Cross(b);
				if (cross.LengthSquared() > 0) {
					cross.Normalize();
					axes.push_back(cross);
				}
			}
		}
		return axes;
	}

	inline static shared_ptr<CollisionVolume> CreateBox(Vector3 center, Vector3 size) {
		auto hull = std::make_shared<ConvexHull>();
		for (float x = center.X - size.X / 2.f; x <= center.X + size.X / 2;  x += size.X) {
			for (float y = center.Y - size.Y / 2.f; y <= center.Y + size.Y / 2; y += size.Y) {
				for (float z = center.Z - size.Z / 2.f; z <= center.Z + size.Z / 2; z += size.Z) {
					hull->AddVertex(Vector3(x, y, z));
				}
			}
		}
		return hull;
	}
}