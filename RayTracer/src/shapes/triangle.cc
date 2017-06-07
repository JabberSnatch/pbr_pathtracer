#include "shapes/triangle.h"

#include "common_macros.h"
#include "globals.h"
#include "profiler.h"
#include "logger.h"
#include "triangle_mesh.h"
#include "bounds.h"
#include "transform.h"
#include "vector.h"
#include "surface_interaction.h"

namespace raytracer {

Triangle::Triangle(maths::Transform const &_world_transform, bool _flip_normals,
				   TriangleMesh const &_mesh, int32_t _face_index) :
	Shape(_world_transform, _flip_normals),
	mesh_{ _mesh }
{
	YS_ASSERT(_face_index < mesh_.triangle_count);
	vertex_index_ = &mesh_.indices[TriangleMesh::IndexOffset(_face_index)];
}

bool
Triangle::Intersect(maths::Ray const &_ray,
					maths::Decimal &_tHit,
					SurfaceInteraction &_hit_info) const
{
	TIMED_SCOPE(Triangle_Intersect);

	maths::Point3f const	&v0 = mesh_.vertices[vertex_index_[0]];
	maths::Point3f const	&v1 = mesh_.vertices[vertex_index_[1]];
	maths::Point3f const	&v2 = mesh_.vertices[vertex_index_[2]];

	maths::Point3f	p0 = v0;
	maths::Point3f	p1 = v1;
	maths::Point3f	p2 = v2;

	/*	This is fun to read, but it's probably less efficient than an unrolled loop.
	maths::Vector<maths::Vec3f, 3>	vertices{ maths::Vec3f(p0), maths::Vec3f(p1), maths::Vec3f(p2) };
	vertices -= maths::Vec3f(_ray.origin);
	*/

	maths::Vec3f const	ro = maths::Vec3f(_ray.origin);
	p0 -= ro; p1 -= ro; p2 -= ro;

	uint32_t const		kz = maths::MaximumDimension(maths::Abs(_ray.direction));
	uint32_t const		kx = kz + 1 == 3 ? 0 : kz + 1;
	uint32_t const		ky = kx + 1 == 3 ? 0 : kx + 1;
	maths::Vec3f const	rd = maths::Swizzle(_ray.direction, kx, ky, kz);
	p0 = maths::Swizzle(p0, kx, ky, kz);
	p1 = maths::Swizzle(p1, kx, ky, kz);
	p2 = maths::Swizzle(p2, kx, ky, kz);

	maths::Decimal const	sx = -rd.x / rd.z;
	maths::Decimal const	sy = -rd.y / rd.z;
	maths::Decimal const	sz = 1._d / rd.z;
	maths::Vec3f const		shear{ sx, sy, 0._d };
	p0 += shear * p0.z;
	p1 += shear * p1.z;
	p2 += shear * p2.z;

	// Edge function : e(p, p0, p1) = (p1.x - p0.x)(p.y - p0.y) - (p.x - p0.x)(p1.y - p0.y)
	// Returns a positive value if p is on the left side of line p0p1 and a negative value if it's
	// on the right side
	// Thanks to the earlier coordinate system change, p = (0, 0)
	// (p1.x - p0.x)(-p0.y) - (-p0.x)(p1.y - p0.y)
	// p0.y*p0.x - p1.x*p0.y - (p0.y*p0.x - p0.x*p1.y)
	// p0.x*p1.y - p1.x*p0.y
	maths::Decimal const	e0 = p1.x*p2.y - p2.x*p1.y;
	maths::Decimal const	e1 = p2.x*p0.y - p0.x*p2.y;
	maths::Decimal const	e2 = p0.x*p1.y - p1.x*p0.y;
	// NOTE: If any of these is zero, we don't know for sure whether we hit the triangle or not.
	if (e0 == 0._d || e1 == 0._d || e2 == 0._d)
		return false;

	if ((e0 > 0._d || e1 > 0._d || e2 > 0._d) && (e0 < 0._d || e1 < 0._d || e2 < 0._d))
		return false;
	maths::Decimal const	e_sum = e0 + e1 + e2;
	if (e_sum == 0._d)
		return false;

	p0.z *= sz; p1.z *= sz; p2.z *= sz;
	maths::Decimal const	t_scaled = e0*p0.z + e1*p1.z + e2*p2.z;
	if (e_sum < 0._d && (t_scaled >= 0._d || t_scaled < _ray.tMax * e_sum))
		return false;
	if (e_sum > 0._d && (t_scaled <= 0._d || t_scaled > _ray.tMax * e_sum))
		return false;

	maths::Decimal const	e_sum_inverse = 1._d / e_sum;
	maths::Decimal const	b0 = e0 * e_sum_inverse;
	maths::Decimal const	b1 = e1 * e_sum_inverse;
	maths::Decimal const	b2 = e2 * e_sum_inverse;
	maths::Decimal const	t = t_scaled * e_sum_inverse;

	maths::Vec3f			dpdu, dpdv;
	maths::Point2f const	uv0{ uv(0) }, uv1{ uv(1) }, uv2{ uv(2) };
	maths::Vec2f const		duv02 = uv0 - uv2, duv12 = uv1 - uv2;
	maths::Vec3f const		dp02 = v0 - v2, dp12 = v1 - v2;

	maths::Decimal const	matrix_determinant = duv02.x * duv12.y - duv02.y * duv12.x;
	if (matrix_determinant != 0._d)
	{
		maths::Decimal const	matrix_determinant_inverse = 1._d / matrix_determinant;
		dpdu = (duv12.y * dp02 - duv02.y * dp12) * matrix_determinant_inverse;
		dpdv = (duv02.x * dp12 - duv12.x * dp02) * matrix_determinant_inverse;
	}
	else
		maths::OrthonormalBasis(maths::Normalized(maths::Cross(v2 - v0, v1 - v0)), dpdu, dpdv);

	maths::Point3f const	hit_point = b0 * v0 + maths::Vec3f(b1 * v1) + maths::Vec3f(b2 * v2);
	maths::Point2f const	hit_uv = b0 * uv0 + maths::Vec2f(b1 * uv1) + maths::Vec2f(b2 * uv2);

	_hit_info = SurfaceInteraction(hit_point, t, -_ray.direction, this, hit_uv,
								   dpdu, dpdv, maths::Norm3f(0._d), maths::Norm3f(0._d));
	_hit_info.geometry.normal = _hit_info.shading.normal =
		maths::Norm3f(maths::Normalized(maths::Cross(dp02, dp12)));

	bool const	mesh_has_normals = mesh_.has_normals();
	bool const	mesh_has_tangents = mesh_.has_tangents();
	if (mesh_has_normals || mesh_has_tangents)
	{
		// NOTE: I'm really into const variables right now. Let's bench this someday.
		maths::Norm3f const		&n0 = (mesh_has_normals) ?
			mesh_.normals[vertex_index_[0]] :
			_hit_info.geometry.normal;
		maths::Norm3f const		&n1 = (mesh_has_normals) ?
			mesh_.normals[vertex_index_[1]] :
			_hit_info.geometry.normal;
		maths::Norm3f const		&n2 = (mesh_has_normals) ?
			mesh_.normals[vertex_index_[2]] :
			_hit_info.geometry.normal;

		maths::Norm3f const		shading_normal = (mesh_has_normals) ?
			maths::Normalized(b0 * n0 + b1 * n1 + b2 * n2) :
			_hit_info.geometry.normal;

		maths::Vec3f			shading_tangent = (mesh_has_tangents) ?
			maths::Normalized(b0 * mesh_.tangents[vertex_index_[0]] +
							  b1 * mesh_.tangents[vertex_index_[1]] +
							  b2 * mesh_.tangents[vertex_index_[2]]) :
			maths::Normalized(_hit_info.geometry.dpdu);

		maths::Vec3f			shading_bitangent = maths::Cross(shading_normal, shading_tangent);
		if (maths::SqrLength(shading_tangent) > 0._d)
		{
			shading_bitangent = maths::Normalized(shading_bitangent);
			shading_tangent = maths::Cross(shading_bitangent, shading_normal);
		}
		else
			maths::OrthonormalBasis((maths::Vec3f)shading_normal,
									shading_tangent,
									shading_bitangent);

		maths::Norm3f			dndu(0._d), dndv(0._d);
		if (matrix_determinant != 0._d && mesh_has_normals)
		{
			maths::Norm3f const		dn02 = n0 - n2;
			maths::Norm3f const		dn12 = n1 - n2;
			maths::Decimal const	matrix_determinant_inverse = 1._d / matrix_determinant;
			dndu = (duv12.y * dn02 - duv02.y * dn12) * matrix_determinant_inverse;
			dndv = (duv02.x * dn12 - duv12.x * dn02) * matrix_determinant_inverse;
		}

		_hit_info.SetShadingGeometry(shading_tangent, shading_bitangent, dndu, dndv, true);
	} // (mesh_has_normals || mesh_has_tangents)

	if (mesh_has_normals)
		_hit_info.geometry.normal = maths::FaceForward(_hit_info.geometry.normal,
													   _hit_info.shading.normal);
	else if (flip_normals ^ swaps_handedness)
		_hit_info.geometry.normal = _hit_info.shading.normal = -_hit_info.geometry.normal;

	_tHit = t;

	//if (maths::Dot(_ray.direction, _hit_info.shading.normal) > 0._d)
	//	return false;

	return true;
}
bool
Triangle::DoesIntersect(maths::Ray const &_ray) const
{
	LOG_ERROR(tools::kChannelGeneral, "Call to unimplemented Triangle::DoesIntersect. Returning false.");
	YS_ASSERT(false);
	return false;
}

maths::Decimal
Triangle::Area() const
{
	maths::Point3f const	&p0 = mesh_.vertices[vertex_index_[0]];
	maths::Point3f const	&p1 = mesh_.vertices[vertex_index_[1]];
	maths::Point3f const	&p2 = mesh_.vertices[vertex_index_[2]];
	return .5_d * maths::Length(maths::Cross(p1 - p0, p2 - p0));
}

maths::Bounds3f
Triangle::ObjectBounds() const
{
	return maths::Union(
		maths::Bounds3f{
			world_transform(mesh_.vertices[vertex_index_[0]], maths::Transform::kInverse),
			world_transform(mesh_.vertices[vertex_index_[1]], maths::Transform::kInverse)
		},
		world_transform(mesh_.vertices[vertex_index_[2]], maths::Transform::kInverse)
	);
}
maths::Bounds3f
Triangle::WorldBounds() const
{
	return maths::Union(
		maths::Bounds3f{
			mesh_.vertices[vertex_index_[0]],
			mesh_.vertices[vertex_index_[1]]
		},
		mesh_.vertices[vertex_index_[2]]
	);
}

maths::Point2f
Triangle::uv(uint32_t _index) const
{
	YS_ASSERT(_index < 3u && _index >= 0u);
	if (mesh_.has_uv())
		return mesh_.uv[vertex_index_[_index]];
	else
		return maths::Point2f(_index < 1u ? 0._d : 1._d, _index < 2u ? 0._d : 1._d);
}

} // namespace raytracer