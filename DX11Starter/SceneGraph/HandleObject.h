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
		// Layer for collisions
		int m_layer;

		// Type of collider
		ColliderType m_type;

		// Actual handle
		int m_handle;

		ColliderHandle() {
			m_layer = -1;
			m_type = ColliderType::None;
			m_handle = -1;
		}

		ColliderHandle(int layer, ColliderType type, unsigned int handle) {
			m_layer = layer;
			m_type = type;
			m_handle = handle;
		}

		ColliderHandle(ColliderType type, unsigned int handle) {
			m_layer = 0;
			m_type = type;
			m_handle = handle;
		}
	};
}

struct HandleObject
{
	Colliders2D::ColliderHandle m_collider;

	// Handle for the relevant mesh 
	int m_mesh;

	// Handle for the relevant material
	int m_material;

	HandleObject() 
	{ 
		m_mesh = -1; 
		m_material = -1; 
	}

	~HandleObject() {}
};

