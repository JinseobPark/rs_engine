#ifndef DLL_HEADER_H_
#define DLL_HEADER_H_

	#ifdef DLL_EXPORTER

	#define SPH_VIEWER __declspec(dllexport)

	#define SPH_VIEWER_C extern "C" __declspec(dllexport)

	#else

	#define SPH_VIEWER __declspec(dllimport)

	#define SPH_VIEWER_C extern "C" __declspec(dllimport)
	#endif // DLL_EXPORTER

#endif // !DLL_HEADER_H_