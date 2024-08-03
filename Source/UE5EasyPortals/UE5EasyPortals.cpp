#include "UE5EasyPortals.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

class FUE5EasyPortalsModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override {}
	virtual void ShutdownModule() override {}
};

IMPLEMENT_MODULE(FUE5EasyPortalsModule, UE5EasyPortals)
