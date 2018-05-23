#ifdef _MSC_VER
	// Remove warning C4251
	#pragma warning(disable: 4251)

	#ifdef _DLL
	#define PBODLL_API __declspec(dllexport)
	#else
	#define PBODLL_API __declspec(dllimport)
	#endif
#else
#define PBODLL_API
#endif