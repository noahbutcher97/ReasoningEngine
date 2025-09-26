#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

/**
 * Log category for MM Semantic Engine
 * This is the declaration that was missing
 */
DECLARE_LOG_CATEGORY_EXTERN(LogReasoningEngine, Log, All);

/**
 * Main module class for MM Semantic Engine
 * Manages singleton lifecycle and module initialization
 */
class REASONINGENGINE_API  FReasoningEngineModule  : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	virtual bool SupportsDynamicReloading() override { return false; } // Singleton doesn't support hot reload
    
	/**
	 * Check if the semantic engine module is loaded
	 * @return true if module is available
	 */
	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("ReasoningEngine");
	}
    
	/**
	 * Get the semantic engine module instance
	 * @return Reference to the module
	 */
	static inline FReasoningEngineModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FReasoningEngineModule>("ReasoningEngine");
	}
};
