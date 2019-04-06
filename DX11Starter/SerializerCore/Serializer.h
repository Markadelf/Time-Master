#pragma once
#include "Buffer.h"
#include <cmath>
#include <WS2tcpip.h>
#include <winsock2.h>
#pragma comment( lib, "wsock32.lib" )

// Basic Serialization Logic for Primitives and a design by contract serializer for everything else
namespace Serializer
{
	// Endian Swap
	static unsigned __int32 SwapL(unsigned __int32 input) {
		return
			((input & 0xFF) << 24) |
			((input & 0xFF00) << 8) |
			((input & 0xFF0000) >> 8) |
			((input & 0xFF000000) >> 24);
	}

	static unsigned __int16 SwapS(unsigned __int16 input) {
		return
			((input & 0xFF) << 8) |
			((input & 0xFF00) >> 8);
	}

	static bool SerializeBool(Buffer& buffer, bool value) {
		return buffer.WriteBits(&value, 1);
	}

	static bool DeserializeBool(Buffer& buffer, bool& value) {
		return buffer.ReadBits(&value, 1);
	}

	// Integers are treated as being serialized as an offset between two values.
	// This avoids possible issues with signed vs unsigned integers
	template<__int32 min, __int32 max>
	static bool SerializeInteger(Buffer& buffer, __int32 value)
	{
		if (min > max || value > max || value < min) {
			return false;
		}
		const int bits = (min == max) ? 0 : (int)log2(max - min) + 1;
		unsigned __int32 val = SwapL(htonl(value - min));

		return buffer.WriteBits(&val, bits);
	}	
	template<__int32 min, __int32 max>
	static bool DeserializeInteger(Buffer& buffer, __int32& value)
	{
		if (min > max) {
			return false;
		}
		const int bits = (min == max) ? 0 : (int)log2(max - min) + 1;
		unsigned __int32 val = 0;
		if (buffer.ReadBits(&val, bits))
		{
			value = ntohl(SwapL(val)) + min;
			return true;
		}
		else
		{
			return false;
		}
	}
	// Specific serializers unsigned ints
	static bool SerializeUINT16(Buffer& buffer, unsigned __int16 value)
	{
		unsigned __int16 val = SwapS(htons(value));
		return buffer.WriteBits(&val, 16);
	}
	static bool DeserializeUINT16(Buffer& buffer, unsigned __int16& value)
	{
		unsigned __int16 val = 0;
		if (buffer.ReadBits(&val, 16))
		{
			value = ntohs(SwapS(val));
			return true;
		}
		else
		{
			return false;
		}
	}
	static bool SerializeUINT32(Buffer& buffer, unsigned __int32 value)
	{
		unsigned __int32 val = SwapL(htonl(value));
		return buffer.WriteBits(&val, 32);
	}
	static bool DeserializeUINT32(Buffer& buffer, unsigned __int32& value)
	{
		unsigned __int32 val = 0;
		if (buffer.ReadBits(&val, 32))
		{
			value = ntohl(SwapL(val));
			return true;
		}
		else
		{
			return false;
		}
	}

	// Floats are serialized as integers, but also use a resolution parameter
	// EITHER resNum or resDen should be one at all time
	// if resNum is > 1 the number is less precise
	// if resDen is < 1 the number is more precise
	template<int min, int max, int resNum, int resDen>
	static bool SerializeFloat(Buffer& buffer, float value)
	{
		if (min > max || value > max || value < min) {
			return false;
		}
		const float res = (float)resNum / resDen;
		const float range = max - min;
		const float values = range / res;
		const __int32 bits = (min == max) ? 0 : (int)log2(values) + 1;
		
		if (bits <= 0)
		{
			return false;
		}
		unsigned __int32 val = SwapL(htonl((int)floor(((value - min) / range) * values + .5f)));

		return buffer.WriteBits(&val, bits);
	}
	template<int min, int max, int resNum, int resDen>
	static bool DeserializeFloat(Buffer& buffer, float& value)
	{
		if (min > max) {
			return false;
		}
		const float res = (float)resNum / resDen;
		const float range = max - min;
		const float values = range / res;
		const __int32 bits = (min == max) ? 0 : (int)log2(values) + 1;
		
		if (bits <= 0)
		{
			return false;
		}

		unsigned __int32 val = 0;
		if (buffer.ReadBits(&val, bits))
		{
			value = min + (ntohl(SwapL(val)) * range / values);
			return true;
		}
		else
		{
			return false;
		}
	}

	// Serialize the length followed by the data
	template<int maxLength>
	static bool SerializeString(Buffer& buffer, const char* data)
	{
		int length = (int) strlen(data);
		if (length > maxLength) {
			return false;
		}

		if (!SerializeInteger<0, maxLength>(buffer, length)) {
			return false;
		}

		return buffer.WriteBytes(data, length);
	}

	template<int maxLength>
	static bool DeserializeString(Buffer& buffer, char* data)
	{
		int length = 0;
		int bits = DeserializeInteger<0, maxLength>(buffer, length);
		if (length > maxLength) {
			return false;
		}

		if (buffer.ReadBytes(data, length))
		{
			data[length] = NULL;
			return true;
		}
		else
		{
			return false;
		}
	}

	// Design by contract
	template <class T>
	static bool Serialize(Buffer& buffer, const T* obj) {
		return obj->Serialize(buffer);
	}

	template <class T>
	static bool Deserialize(Buffer& buffer, T* obj) {
		return obj->Deserialize(buffer);
	}
};

