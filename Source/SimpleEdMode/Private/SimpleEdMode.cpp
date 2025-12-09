#include "SimpleEdMode.h"
#include "SimpleEdModeEdMode.h"
#include "EditorModeRegistry.h"
#include "SimpleEdModeStyle.h"

#define LOCTEXT_NAMESPACE "FSimpleEdModeModule"

void FSimpleEdModeModule::StartupModule()
{
	FSimpleEdModeStyle::Initialize();
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	FEditorModeRegistry::Get().RegisterMode<FSimpleEdMode>(FSimpleEdMode::EM_SimpleEdModeId, LOCTEXT("SimpleEdModeEdModeName", "SimpleEdModeEdMode"), FSlateIcon(FSimpleEdModeStyle::GetStyleSetName(), "SimpleEdMode.ToolsIcon"), true);
}

void FSimpleEdModeModule::ShutdownModule()
{
	FSimpleEdModeStyle::Shutdown();
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FEditorModeRegistry::Get().UnregisterMode(FSimpleEdMode::EM_SimpleEdModeId);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSimpleEdModeModule, SimpleEdMode)