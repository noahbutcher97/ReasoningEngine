#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Symbolic/Data/RESymbolicTypes.h"
#include "REInferences.generated.h"

// Forward declarations
class UREKnowledge;


/**
 * Logical inference and reasoning engine
 * Derives new facts from existing knowledge using various inference methods
 */
UCLASS(BlueprintType)
class REASONINGENGINE_API UREInferences : public UObject
{
    GENERATED_BODY()
    
private:
    // ========== DEPENDENCIES ==========
    
    UPROPERTY()
    UREKnowledge* KnowledgeBase;
    
    // ========== RULE STORAGE ==========
    
	/** Inference rules organized by category */
	UPROPERTY()
	TMap<FString, FRERuleCollection> RuleCategories;  // Changed to use wrapper
    
    /** All active rules */
    UPROPERTY()
    TArray<FREInferenceRule> ActiveRules;
    
    /** Rule priority map */
    TMap<FName, int32> RulePriorities;
    
    // ========== WORKING MEMORY ==========
    
    /** Current working facts */
    TSet<FREFact> WorkingMemory;
    
    /** Inference history */
    TArray<FREInference> InferenceHistory;
    
    /** Assumption tracking */
    TArray<FREFact> Assumptions;
    
    // ========== STATISTICS ==========
    
    mutable FThreadSafeCounter TotalInferences;
    mutable FThreadSafeCounter SuccessfulInferences;
    mutable FThreadSafeCounter RulesFired;
    
    // ========== INFERENCE METHODS ==========
    
    /** Forward chaining implementation */
    TArray<FREInference> ForwardChain(const FREInferenceContext& Context);
    
    /** Backward chaining implementation */
    TArray<FREInference> BackwardChain(const FREFact& Goal, const FREInferenceContext& Context);
    
    /** Fuzzy logic inference */
    TArray<FREInference> FuzzyInference(const FREInferenceContext& Context);
    
    /** Check if rule conditions are met */
    bool CheckRuleConditions(const FREInferenceRule& Rule, float& OutConfidence) const;
    
    /** Apply rule to generate conclusions */
    TArray<FREFact> ApplyRule(const FREInferenceRule& Rule, float Confidence);
    
public:
    // ========== LIFECYCLE ==========
    
    void Initialize();
    void Shutdown();
    bool IsOperational() const { return true; }
    
    // ========== DEPENDENCIES ==========
    
    void SetKnowledgeBase(UREKnowledge* InKnowledgeBase);
    
    // ========== RULE MANAGEMENT ==========
    
    /**
     * Add an inference rule
     * @param Rule - Rule to add
     * @param Category - Rule category for organization
     */
    UFUNCTION(BlueprintCallable, Category="MM|Inference|Rules",
              meta=(DisplayName="Add Rule"))
    void AddRule(const FREInferenceRule& Rule, const FString& Category = TEXT("Default"));
    
    /**
     * Remove a rule
     * @param RuleID - Rule to remove
     * @return true if rule was removed
     */
    UFUNCTION(BlueprintCallable, Category="MM|Inference|Rules",
              meta=(DisplayName="Remove Rule"))
    bool RemoveRule(FName RuleID);
    
    /**
     * Enable/disable a rule
     * @param RuleID - Rule to modify
     * @param bEnabled - New enabled state
     */
    UFUNCTION(BlueprintCallable, Category="MM|Inference|Rules",
              meta=(DisplayName="Set Rule Enabled"))
    void SetRuleEnabled(FName RuleID, bool bEnabled);
    
    /**
     * Set rule priority
     * @param RuleID - Rule to modify
     * @param Priority - Priority (higher = evaluated first)
     */
    UFUNCTION(BlueprintCallable, Category="MM|Inference|Rules",
              meta=(DisplayName="Set Rule Priority"))
    void SetRulePriority(FName RuleID, int32 Priority);
    
    /**
     * Get all rules in a category
     * @param Category - Category to retrieve
     * @return Array of rules
     */
    UFUNCTION(BlueprintCallable, Category="MM|Inference|Rules",
              meta=(DisplayName="Get Rules By Category"))
    TArray<FREInferenceRule> GetRulesByCategory(const FString& Category) const;
    
    // ========== INFERENCE OPERATIONS ==========
    
    /**
     * Make inferences from facts
     * @param Facts - Input facts
     * @param Method - Inference method to use
     * @param Context - Inference context
     * @return Array of inferences made
     */
    UFUNCTION(BlueprintCallable, Category="MM|Inference",
              meta=(DisplayName="Make Inferences"))
    TArray<FREInference> MakeInferences(const TArray<FREFact>& Facts,
                                        EREInferenceMethod Method = EREInferenceMethod::ForwardChaining,
                                        const FREInferenceContext& Context = FREInferenceContext());
    
    /**
     * Prove a hypothesis
     * @param Hypothesis - Hypothesis to prove
     * @param Method - Inference method to use
     * @return Updated hypothesis with confidence
     */
    UFUNCTION(BlueprintCallable, Category="MM|Inference",
              meta=(DisplayName="Prove Hypothesis"))
    FREHypothesis ProveHypothesis(const FREHypothesis& Hypothesis,
                                  EREInferenceMethod Method = EREInferenceMethod::BackwardChaining);
    
    /**
     * Check if a fact can be inferred
     * @param Fact - Fact to check
     * @param OutConfidence - Confidence if inferable
     * @return true if fact can be inferred
     */
    UFUNCTION(BlueprintCallable, Category="MM|Inference",
              meta=(DisplayName="Can Infer Fact"))
    bool CanInferFact(const FREFact& Fact, float& OutConfidence);
    
    /**
     * Explain how a fact was inferred
     * @param Fact - Fact to explain
     * @return Inference chain explanation
     */
    UFUNCTION(BlueprintCallable, Category="MM|Inference",
              meta=(DisplayName="Explain Inference"))
    TArray<FREInference> ExplainInference(const FREFact& Fact);
    
    // ========== WORKING MEMORY ==========
    
    /**
     * Add fact to working memory
     * @param Fact - Fact to add
     */
    UFUNCTION(BlueprintCallable, Category="MM|Inference|Memory",
              meta=(DisplayName="Add To Working Memory"))
    void AddToWorkingMemory(const FREFact& Fact);
    
    /**
     * Clear working memory
     */
    UFUNCTION(BlueprintCallable, Category="MM|Inference|Memory",
              meta=(DisplayName="Clear Working Memory"))
    void ClearWorkingMemory();
    
    /**
     * Get current working memory
     * @return Array of facts in working memory
     */
    UFUNCTION(BlueprintCallable, Category="MM|Inference|Memory",
              meta=(DisplayName="Get Working Memory"))
    TArray<FREFact> GetWorkingMemory() const;
    
    // ========== CONFLICT RESOLUTION ==========
    
    /**
     * Resolve conflicts between inferences
     * @param Inferences - Conflicting inferences
     * @return Resolved inferences
     */
    UFUNCTION(BlueprintCallable, Category="MM|Inference|Conflict",
              meta=(DisplayName="Resolve Conflicts"))
    TArray<FREInference> ResolveConflicts(const TArray<FREInference>& Inferences);
    
	/**
	 * Check for contradictions
	 * @param Facts - Facts to check
	 * @param OutContradictions - Found contradictions
	 * @return true if contradictions found
	 */
	UFUNCTION(BlueprintCallable, Category="MM|Inference|Conflict",
			  meta=(DisplayName="Check Contradictions"))
	bool CheckContradictions(const TArray<FREFact>& Facts,
							TArray<FREFactContradiction>& OutContradictions);  // Changed from TPair
    
    // ========== UTILITIES ==========
    
    /**
     * Get inference statistics
     * @param OutTotal - Total inferences made
     * @param OutSuccessful - Successful inferences
     * @param OutRulesFired - Number of rules fired
     */
    UFUNCTION(BlueprintCallable, Category="MM|Inference|Stats",
              meta=(DisplayName="Get Inference Stats"))
    void GetInferenceStats(int32& OutTotal,
                          int32& OutSuccessful,
                          int32& OutRulesFired) const;
    
    /**
     * Get memory usage
     * @return Memory in bytes
     */
    int64 GetMemoryUsage() const;
    
    /**
     * Reset inference engine
     */
    UFUNCTION(BlueprintCallable, Category="MM|Inference",
              meta=(DisplayName="Reset Engine"))
    void ResetEngine();
    
    /**
     * Load default inference rules
     */
    void LoadDefaultRules();
};
