#pragma once
namespace Colliders2D {
	// Collider type is used to define collider handles
	enum ColliderType
	{
		None,
		Circle,
		Rectangle
	};

	// Collider Handles are used by the handle
	struct ColliderHandle
	{
		ColliderType m_type;
		int m_handle;

		ColliderHandle() {
			m_type = ColliderType::None;
			m_handle = -1;
		}

		ColliderHandle(ColliderType type, unsigned int handle) {
			m_type = type;
			m_handle = handle;
		}
	};
}

struct HandleObject
{
	Colliders2D::ColliderHandle m_collider;

	// Layer for collisions
	int m_layer;

	// Handle for the relevant mesh 
	int m_mesh;

	// Handle for the relevant material
	int m_material;

	HandleObject() {m_layer = 0; m_mesh = -1; m_material = -1; }
	~HandleObject() {}
};

