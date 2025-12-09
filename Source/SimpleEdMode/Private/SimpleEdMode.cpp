#include "SimpleEdMode.h"
#include "SimpleEdModeEdMode.h"
#include "EditorModeRegistry.h"

#define LOCTEXT_NAMESPACE "FSimpleEdModeModule"

void FSimpleEdModeModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	FEditorModeRegistry::Get().RegisterMode<FSimpleEdMode>(FSimpleEdMode::EM_SimpleEdModeId, LOCTEXT("SimpleEdModeEdModeName", "SimpleEdModeEdMode"), FSlateIcon(), true);
}

void FSimpleEdModeModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FEditorModeRegistry::Get().UnregisterMode(FSimpleEdMode::EM_SimpleEdModeId);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSimpleEdModeModule, SimpleEdMode)

