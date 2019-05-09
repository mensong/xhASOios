// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the XHCLIENT_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// XHCLIENT_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef XHCLIENT_EXPORTS
#define XHCLIENT_API __declspec(dllexport)
#else
#define XHCLIENT_API __declspec(dllimport)
#endif

// This class is exported from the xhClient.dll
class XHCLIENT_API CxhClient {
public:
	CxhClient(void);
	// TODO: add your methods here.
};

extern XHCLIENT_API int nxhClient;

XHCLIENT_API int fnxhClient(void);
