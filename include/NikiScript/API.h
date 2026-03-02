#pragma once

#ifdef _WIN32
	#if defined(NS_IMPORT)
		#define NS_API __declspec(dllimport)
	#elif defined(NS_EXPORT)
		#define NS_API __declspec(dllexport)
	#endif
#else
	#if defined(NS_EXPORT)
		#define NS_API __attribute__((visibility("default")))
	#endif
#endif

#ifndef NS_API
#define NS_API
#endif