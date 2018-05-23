#ifdef PBODLL_EXPORTS
#define PBODLL_API __declspec(dllexport)
#else
#define PBODLL_API __declspec(dllimport)
#endif