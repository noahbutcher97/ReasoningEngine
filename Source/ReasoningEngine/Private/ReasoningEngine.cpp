#include "ReasoningEngine.h"
#include "Core/RECore.h"

#define LOCTEXT_NAMESPACE "FReasoningEngineModule"

/**
 * Log category definition for MM Semantic Engine
 * This is the definition that was missing
 */
DEFINE_LOG_CATEGORY(LogReasoningEngine);

void FReasoningEngineModule::StartupModule()
{
	UE_LOG(LogReasoningEngine, Log, TEXT("MM Semantic Engine: Module starting up"));
    
	// Initialize the semantic engine singleton
	// This ensures it's available as soon as the module loads
	URECore::InitializeSingleton();
    
	UE_LOG(LogReasoningEngine, Log, TEXT("MM Semantic Engine: Module startup complete"));
}

void FReasoningEngineModule::ShutdownModule()
{
	UE_LOG(LogReasoningEngine, Log, TEXT("MM Semantic Engine: Module shutting down"));
    
	// Properly destroy the singleton on shutdown
	URECore::DestroySingleton();
    
	UE_LOG(LogReasoningEngine, Log, TEXT("MM Semantic Engine: Module shutdown complete"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FReasoningEngineModule, ReasoningEngine)
