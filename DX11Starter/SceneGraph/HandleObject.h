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
		unsigned int m_handle;

		ColliderHandle() {
			m_type = ColliderType::None;
			m_handle = 0;
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
	int m_teamID;
	// Add render handles
	// Mesh Handle
	// Material Handle

	HandleObject() { m_teamID = -1; }
	~HandleObject() {}
};

