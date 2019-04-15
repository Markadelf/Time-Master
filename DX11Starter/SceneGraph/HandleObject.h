#pragma once
#include "Serializer.h"

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

		bool Serialize(Buffer& buffer) const {
			return Serializer::SerializeInteger<-1, 100>(buffer, m_layer) &&
				Serializer::SerializeInteger<ColliderType::None, ColliderType::Rectangle>(buffer, m_type) &&
				Serializer::SerializeInteger<-1, 100>(buffer, m_handle);

		}
		bool Deserialize(Buffer& buffer) {
			int colType;
			bool ret = Serializer::DeserializeInteger<-1, 100>(buffer, m_layer) &&
				Serializer::DeserializeInteger<ColliderType::None, ColliderType::Rectangle>(buffer, colType) &&
				Serializer::DeserializeInteger<-1, 100>(buffer, m_handle);
			m_type = (ColliderType) colType;
			return ret;
		}
	};
}

struct HandleObject
{
	Colliders2D::ColliderHandle m_collider;

	// Handle for the relevant mesh 
	int m_mesh = -1;

	// Handle for the relevant material
	int m_material = -1;

	// X, Y, Z
	float m_scale[3] = {1, 1, 1};

	float m_yPos = 0;

	// Convenience method
	void SetUniformScale(float val) 
	{
		m_scale[0] = val;
		m_scale[1] = val;
		m_scale[2] = val;
	}

	bool Serialize(Buffer& buffer) const {
		return m_collider.Serialize(buffer) &&
			Serializer::SerializeInteger<-1, 100>(buffer, m_mesh) &&
			Serializer::SerializeInteger<-1, 100>(buffer, m_material);
	}

	bool Deserialize(Buffer& buffer) {
		return m_collider.Deserialize(buffer) &&
			Serializer::DeserializeInteger<-1, 100>(buffer, m_mesh) &&
			Serializer::DeserializeInteger<-1, 100>(buffer, m_material);
	}

};

