#include "raytracer/shapes/triangle_mesh.h"

#include <numeric>

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

#include "maths/ray.h"
#include "maths/transform.h"
#include "raytracer/shapes/triangle.h"
#include "raytracer/surface_interaction.h"


namespace raytracer {


template class TriangleMesh<InstancingPolicyClass::SharedSource>;
template class TriangleMesh<InstancingPolicyClass::Transformed>;


template <typename InstancingPolicy>
TriangleMesh<InstancingPolicy>::TriangleMesh(maths::Transform const &_world_transform,
											 bool _flip_normals,
											 TriangleMeshData const &_data) :
	Shape(_world_transform, _flip_normals),
	data_{ _data }
{}


template <>
bool
TriangleMesh<InstancingPolicyClass::Transformed>::Intersect(maths::Ray const &_ray,
															maths::Decimal &_tHit,
															SurfaceInteraction &_hit_info) const
{
	maths::Ray bvh_ray{ _ray };
	bool const result = data_.bvh().Intersect(bvh_ray, _hit_info);
	_tHit = (result) ? bvh_ray.tMax : _tHit;
	return result;
}

template <>
bool
TriangleMesh<InstancingPolicyClass::SharedSource>::Intersect(maths::Ray const &_ray,
															 maths::Decimal &_tHit,
															 SurfaceInteraction &_hit_info) const
{
	maths::Ray bvh_ray{ world_transform(_ray, maths::Transform::kInverse) };
	bool const result = data_.bvh().Intersect(bvh_ray, _hit_info);
	if (result)
	{
		_tHit = bvh_ray.tMax;
		_hit_info = world_transform(_hit_info, maths::Transform::kForward);
	}
	return result;
}


template <>
bool
TriangleMesh<InstancingPolicyClass::Transformed>::DoesIntersect(maths::Ray const &_ray) const
{
	return data_.bvh().DoesIntersect(_ray);
}

template <>
bool
TriangleMesh<InstancingPolicyClass::SharedSource>::DoesIntersect(maths::Ray const &_ray) const
{
	return data_.bvh().DoesIntersect(world_transform(_ray, maths::Transform::kInverse));
}


template <typename InstancingPolicy>
maths::Decimal
TriangleMesh<InstancingPolicy>::Area() const
{
	TriangleMeshData::TriangleContainer_t const &triangles = data_.triangles();
	return std::accumulate(triangles.cbegin(), triangles.cend(), 0._d,
						   [](maths::Decimal const &_acc, Triangle const* const _triangle) {
		return _acc + _triangle->Area();
	});
}


template <typename InstancingPolicy>
Shape::SurfacePoint
TriangleMesh<InstancingPolicy>::SampleSurface(maths::Vec2f const &_ksi) const
{
	YS_ASSERT(false);
	return Shape::SurfacePoint();
}


template <>
maths::Bounds3f
TriangleMesh<InstancingPolicyClass::Transformed>::ObjectBounds() const
{
	return world_transform(data_.bounds(), maths::Transform::kInverse);
}

template <>
maths::Bounds3f
TriangleMesh<InstancingPolicyClass::SharedSource>::ObjectBounds() const
{
	return data_.bounds();
}


template <>
maths::Bounds3f
TriangleMesh<InstancingPolicyClass::Transformed>::WorldBounds() const
{
	return data_.bounds();
}

template <>
maths::Bounds3f
TriangleMesh<InstancingPolicyClass::SharedSource>::WorldBounds() const
{
	return world_transform(data_.bounds(), maths::Transform::kForward);
}


#if 0
TriangleMesh*
ReadTriangleMeshFromFile(std::string const &_path,
						 maths::Transform const &_world_transform,
						 bool _flip_normals,
						 core::MemoryRegion &_mem_region)
{
	TriangleMesh *result = nullptr;
	uint32_t const		load_flags =
		aiProcess_Triangulate |
		aiProcess_PreTransformVertices |
		aiProcess_JoinIdenticalVertices;
	Assimp::Importer	importer;
	aiScene const		*scene = importer.ReadFile(_path.c_str(), load_flags);
	if (scene != nullptr)
	{
		uint32_t const		mesh_count = scene->mNumMeshes;

		YS_ASSERT(mesh_count > 0u);

		int32_t						out_triangle_count{ 0 };
		std::vector<int32_t>		out_indices;
		std::vector<maths::Point3f>	out_vertices;
		std::vector<maths::Norm3f>	out_normals;

		uint32_t					indices_capacity{ 0u };
		uint32_t					vertices_capacity{ 0u };
		bool						load_normals = true;

		aiMesh *const				*in_meshes = scene->mMeshes;

		for (uint32_t mesh_index = 0; mesh_index < mesh_count; ++mesh_index)
			load_normals &= in_meshes[mesh_index]->HasNormals();

		for (uint32_t mesh_index = 0; mesh_index < mesh_count; ++mesh_index)
		{
			aiMesh const		*in_mesh = in_meshes[mesh_index];
			aiFace const		*in_faces = in_mesh->mFaces;
			aiVector3D const	*in_vertices = in_mesh->mVertices;
			aiVector3D const	*in_normals = in_mesh->mNormals;

			out_triangle_count += in_mesh->mNumFaces;
			indices_capacity += in_mesh->mNumFaces * 3;
			vertices_capacity += in_mesh->mNumVertices;
			out_indices.reserve(indices_capacity);
			out_vertices.reserve(vertices_capacity);

			if (load_normals)
				out_normals.reserve(vertices_capacity);

			for (uint32_t i = 0; i < in_mesh->mNumFaces; ++i)
			{
				aiFace const	&face = in_faces[i];
				YS_ASSERT(face.mNumIndices == 3);
				for (uint32_t j = 0; j < face.mNumIndices; ++j)
				{
					uint32_t const	base_index = static_cast<uint32_t>(face.mIndices[j]);
					uint32_t const	offset_index =
						base_index + static_cast<uint32_t>(out_vertices.size());
					YS_ASSERT(offset_index >= base_index);
					out_indices.emplace_back(offset_index);
				}
			}

			for (uint32_t i = 0; i < in_mesh->mNumVertices; ++i)
			{
				aiVector3D const	&vertex = in_vertices[i];
				out_vertices.emplace_back(vertex.x, vertex.y, vertex.z);
				if (load_normals)
				{
					aiVector3D const	&normal = in_normals[i];
					maths::Decimal const x = std::isnan(normal.x) ? 0._d : normal.x;
					maths::Decimal const y = std::isnan(normal.y) ? 0._d : normal.y;
					maths::Decimal const z = std::isnan(normal.z) ? 0._d : normal.z;
					out_normals.emplace_back(normal.x, normal.y, normal.z);
				}
			}
		}

#ifndef YS_NO_LOGS
		std::string	const	info_message =
			"Loading " + std::to_string(mesh_count) + " submesh from " + _path + ". " +
			std::to_string(out_triangle_count) + " total triangles." +
			((load_normals) ? " Normals provided by file." : "");
		LOG(tools::kChannelGeneral, tools::kLevelInfo, info_message);
#endif

		TriangleMeshData const* const mesh_data = new (_mem_region)
			TriangleMeshData(_world_transform,
							 out_triangle_count,
							 out_indices,
							 out_vertices,
							 (load_normals ? &out_normals : nullptr));
		result = new (_mem_region) TriangleMesh(_world_transform,
												_flip_normals,
												*mesh_data,
												_mem_region);
	}
	else
	{
		LOG_WARNING(tools::kChannelGeneral, "could not load file " + _path);
	}
	return result;
}
#endif


} // namespace raytracer
