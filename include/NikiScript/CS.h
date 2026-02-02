#pragma once
#ifdef NS_CS_API
#include <stdint.h>

#define NS_CS_UNORDERED_MAP(name, keyType, valueType) \
	bool name##Contains(keyType key); \
	valueType name##GetValue(keyType key); \
	void name##Erase(keyType key); \
	void name##Clear(); \
	size_t name##Count()

#define NS_CS_UNORDERED_MAP_IMPLEMENTATION(name, variable, keyType, valueType, _namespace) \
	bool _namespace##name##Contains(keyType key) { \
		return variable.count(key) != 0; \
	} \
	valueType _namespace##name##GetValue(keyType key) { \
		return variable[key]; \
	} \
	void _namespace##name##Erase(keyType key) { \
		variable.erase(key); \
	} \
	void _namespace##name##Clear() { \
		variable.clear(); \
	} \
	size_t _namespace##name##Count() { \
		return variable.size(); \
	}


#define NS_CS_VECTOR(name, type, pushType) \
	type name##Get(size_t index); \
	void name##Pop(size_t index); \
	void name##Push(pushType value); \
	void name##Clear(); \
	void* name##Data(); \
	size_t name##Count()

#define NS_CS_VECTOR_IMPLEMENTATION(name, variable, type, pushType, _namespace) \
	type _namespace##name##Get(size_t index) { \
		return variable[index]; \
	} \
	void _namespace##name##Pop(size_t index) { \
		variable.erase(variable.begin()+index); \
	} \
	void _namespace##name##Push(pushType value) { \
		variable.push_back(value); \
	} \
	void _namespace##name##Clear() { \
		variable.clear(); \
	} \
	void* _namespace##name##Data() { \
		return (void*)variable.data(); \
	} \
	size_t _namespace##name##Count() { \
		return variable.size(); \
	}

#define NS_CS_VECTOR(name, type, pushType) \
	type name##Get(size_t index); \
	void name##Pop(size_t index); \
	void name##Push(pushType value); \
	void name##Clear(); \
	void* name##Data(); \
	size_t name##Count()

#define NS_CS_VECTOR_NO_PUSH(name, type, pushType) \
	type name##Get(size_t index); \
	void name##Pop(size_t index); \
	void name##Clear(); \
	void* name##Data(); \
	size_t name##Count()

// requires NS_CS_VECTOR_NO_PUSH
#define NS_CS_VECTOR_POINTER_PAIR_IMPLEMENTATION(name, variable, type, pushType, _namespace) \
	type _namespace##name##Get(size_t index) { \
		return variable[index]->second; \
	} \
	void _namespace##name##Pop(size_t index) { \
		variable.erase(variable.begin()+index); \
	} \
	void _namespace##name##Clear() { \
		variable.clear(); \
	} \
	void* _namespace##name##Data() { \
		return (void*)variable.data(); \
	} \
	size_t _namespace##name##Count() { \
		return variable.size(); \
	}
#else
#define NS_CS_UNORDERED_MAP(...)
#define NS_CS_VECTOR_NO_PUSH(...)
#define NS_CS_VECTOR(...)
#define NS_CS_UNORDERED_MAP_IMPLEMENTATION(...)
#define NS_CS_VECTOR_IMPLEMENTATION(...)
#define NS_CS_VECTOR_POINTER_PAIR_IMPLEMENTATION(...)
#endif