#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Symbolic/Data/RESymbolicTypes.h"  // UPDATED: Was "Data/REPatternTypes.h"
#include "REKnowledge.generated.h"

// Forward declarations
class URECache;

/**
 * Semantic knowledge base with graph structure
 * Stores facts, concepts, and relations for reasoning
 */
UCLASS(BlueprintType)
class REASONINGENGINE_API UREKnowledge : public UObject
{
    GENERATED_BODY()
    
private:
    // ========== DEPENDENCIES ==========
    
    UPROPERTY()
    URECache* CacheManager;
    
    // ========== KNOWLEDGE STORAGE ==========
    
	/** Facts organized by namespace */
	UPROPERTY()
	TMap<FName, FREFactCollection> FactsByNamespace;  // Changed to use wrapper
    
    /** Concepts in the knowledge graph */
    UPROPERTY()
    TMap<FName, FREKnowledgeNode> KnowledgeGraph;
    
    /** Relations between concepts */
    UPROPERTY()
    TArray<FRERelation> Relations;
    
    /** Concept hierarchies */
    TMultiMap<FName, FName> ConceptHierarchy;
    
    // ========== INDEXING ==========
    
    /** Subject index for fast lookup */
    TMultiMap<FString, FREFact*> SubjectIndex;
    
    /** Predicate index */
    TMultiMap<FString, FREFact*> PredicateIndex;
    
    /** Object index */
    TMultiMap<FString, FREFact*> ObjectIndex;
    
    /** Relation index by type */
    TMultiMap<FString, FRERelation*> RelationTypeIndex;
    
    // ========== STATISTICS ==========
    
    mutable FThreadSafeCounter TotalFacts;
    mutable FThreadSafeCounter TotalConcepts;
    mutable FThreadSafeCounter TotalRelations;
    mutable FThreadSafeCounter QueryCount;
    
    // ========== HELPERS ==========
    
    /** Rebuild indices after bulk changes */
    void RebuildIndices();
    
    /** Find path between concepts */
    TArray<FName> FindConceptPath(FName FromConcept, FName ToConcept, int32 MaxDepth = 5) const;
    
    /** Spread activation through graph */
    void SpreadActivation(FName StartNode, float ActivationStrength, int32 MaxHops = 3);
    
public:
    // ========== LIFECYCLE ==========
    
    void Initialize();
    void Shutdown();
    bool IsOperational() const { return true; }
    
    // ========== DEPENDENCIES ==========
    
    void SetCacheManager(URECache* InCacheManager);
    
    // ========== FACT MANAGEMENT ==========
    
    /**
     * Add a fact to the knowledge base
     * @param Fact - Fact to add
     * @param Namespace - Namespace for organization
     */
    UFUNCTION(BlueprintCallable, Category="MM|Knowledge|Facts",
              meta=(DisplayName="Add Fact"))
    void AddFact(const FREFact& Fact, FName Namespace = NAME_None);
    
    /**
     * Remove a fact
     * @param Fact - Fact to remove
     * @param Namespace - Namespace to search
     * @return true if fact was removed
     */
    UFUNCTION(BlueprintCallable, Category="MM|Knowledge|Facts",
              meta=(DisplayName="Remove Fact"))
    bool RemoveFact(const FREFact& Fact, FName Namespace = NAME_None);
    
    /**
     * Query facts
     * @param Query - Query parameters
     * @return Array of matching facts
     */
    UFUNCTION(BlueprintCallable, Category="MM|Knowledge|Facts",
              meta=(DisplayName="Query Facts"))
    TArray<FREFact> QueryFacts(const FREKnowledgeQuery& Query);
    
    /**
     * Check if fact exists
     * @param Subject - Fact subject
     * @param Predicate - Fact predicate
     * @param Object - Fact object
     * @return true if fact exists
     */
    UFUNCTION(BlueprintPure, Category="MM|Knowledge|Facts",
              meta=(DisplayName="Has Fact"))
    bool HasFact(const FString& Subject,
                const FString& Predicate,
                const FString& Object) const;
    
    // ========== CONCEPT MANAGEMENT ==========
    
    /**
     * Add a concept to the knowledge graph
     * @param Concept - Concept to add
     */
    UFUNCTION(BlueprintCallable, Category="MM|Knowledge|Concepts",
              meta=(DisplayName="Add Concept"))
    void AddConcept(const FREConcept& Concept);
    
    /**
     * Remove a concept
     * @param ConceptID - Concept to remove
     * @return true if concept was removed
     */
    UFUNCTION(BlueprintCallable, Category="MM|Knowledge|Concepts",
              meta=(DisplayName="Remove Concept"))
    bool RemoveConcept(FName ConceptID);
    
    /**
     * Get a concept by ID
     * @param ConceptID - Concept identifier
     * @param OutConcept - Retrieved concept
     * @return true if concept found
     */
    UFUNCTION(BlueprintCallable, Category="MM|Knowledge|Concepts",
              meta=(DisplayName="Get Concept"))
    bool GetConcept(FName ConceptID, FREConcept& OutConcept) const;
    
    /**
     * Find concepts by name or description
     * @param SearchText - Text to search for
     * @param MaxResults - Maximum results to return
     * @return Array of matching concepts
     */
    UFUNCTION(BlueprintCallable, Category="MM|Knowledge|Concepts",
              meta=(DisplayName="Find Concepts"))
    TArray<FREConcept> FindConcepts(const FString& SearchText,
                                    int32 MaxResults = 10);
    
    // ========== RELATION MANAGEMENT ==========
    
    /**
     * Add a relation between concepts
     * @param Relation - Relation to add
     */
    UFUNCTION(BlueprintCallable, Category="MM|Knowledge|Relations",
              meta=(DisplayName="Add Relation"))
    void AddRelation(const FRERelation& Relation);
    
    /**
     * Remove relations between concepts
     * @param FromConcept - Source concept
     * @param ToConcept - Target concept
     * @param RelationType - Type of relation (empty = all)
     * @return Number of relations removed
     */
    UFUNCTION(BlueprintCallable, Category="MM|Knowledge|Relations",
              meta=(DisplayName="Remove Relation"))
    int32 RemoveRelation(FName FromConcept,
                        FName ToConcept,
                        const FString& RelationType = TEXT(""));
    
    /**
     * Get relations from a concept
     * @param ConceptID - Source concept
     * @param RelationType - Filter by type (empty = all)
     * @return Array of relations
     */
    UFUNCTION(BlueprintCallable, Category="MM|Knowledge|Relations",
              meta=(DisplayName="Get Relations From"))
    TArray<FRERelation> GetRelationsFrom(FName ConceptID,
                                         const FString& RelationType = TEXT("")) const;
    
    /**
     * Get relations to a concept
     * @param ConceptID - Target concept
     * @param RelationType - Filter by type (empty = all)
     * @return Array of relations
     */
    UFUNCTION(BlueprintCallable, Category="MM|Knowledge|Relations",
              meta=(DisplayName="Get Relations To"))
    TArray<FRERelation> GetRelationsTo(FName ConceptID,
                                       const FString& RelationType = TEXT("")) const;
    
    // ========== GRAPH OPERATIONS ==========
    
    /**
     * Find related concepts
     * @param ConceptID - Starting concept
     * @param MaxDepth - Maximum graph traversal depth
     * @param RelationType - Filter by relation type
     * @return Array of related concepts
     */
    UFUNCTION(BlueprintCallable, Category="MM|Knowledge|Graph",
              meta=(DisplayName="Find Related Concepts"))
    TArray<FREConcept> FindRelatedConcepts(FName ConceptID,
                                           int32 MaxDepth = 2,
                                           const FString& RelationType = TEXT(""));
    
    /**
     * Calculate semantic distance between concepts
     * @param ConceptA - First concept
     * @param ConceptB - Second concept
     * @return Semantic distance (lower = more similar)
     */
    UFUNCTION(BlueprintPure, Category="MM|Knowledge|Graph",
              meta=(DisplayName="Calculate Semantic Distance"))
    float CalculateSemanticDistance(FName ConceptA, FName ConceptB) const;
    
    /**
     * Activate concept and spread activation
     * @param ConceptID - Concept to activate
     * @param ActivationStrength - Initial activation level
     * @param SpreadDepth - How far to spread activation
     */
    UFUNCTION(BlueprintCallable, Category="MM|Knowledge|Graph",
              meta=(DisplayName="Activate Concept"))
    void ActivateConcept(FName ConceptID,
                        float ActivationStrength = 1.0f,
                        int32 SpreadDepth = 2);
    
    /**
     * Get most activated concepts
     * @param Count - Number of concepts to return
     * @return Array of activated concepts
     */
    UFUNCTION(BlueprintCallable, Category="MM|Knowledge|Graph",
              meta=(DisplayName="Get Activated Concepts"))
    TArray<FREConcept> GetActivatedConcepts(int32 Count = 10) const;
    
    // ========== IMPORT/EXPORT ==========
    
    /**
     * Load knowledge from JSON
     * @param JsonString - JSON representation
     * @return true if loaded successfully
     */
    UFUNCTION(BlueprintCallable, Category="MM|Knowledge|IO",
              meta=(DisplayName="Load From JSON"))
    bool LoadFromJSON(const FString& JsonString);
    
    /**
     * Export knowledge to JSON
     * @return JSON representation
     */
    UFUNCTION(BlueprintCallable, Category="MM|Knowledge|IO",
              meta=(DisplayName="Export To JSON"))
    FString ExportToJSON() const;
    
    /**
     * Load knowledge from asset
     * @param KnowledgeAsset - Knowledge data asset
     */
    UFUNCTION(BlueprintCallable, Category="MM|Knowledge|IO",
              meta=(DisplayName="Load From Asset"))
    void LoadFromAsset(class UDataAsset* KnowledgeAsset);
    
    // ========== UTILITIES ==========
    
    /**
     * Clear all knowledge
     */
    UFUNCTION(BlueprintCallable, Category="MM|Knowledge|Utils",
              meta=(DisplayName="Clear All"))
    void ClearAll();
    
    /**
     * Get memory usage
     * @return Memory in bytes
     */
    int64 GetMemoryUsage() const;
    
    /**
     * Get knowledge statistics
     * @param OutFactCount - Number of facts
     * @param OutConceptCount - Number of concepts
     * @param OutRelationCount - Number of relations
     */
    UFUNCTION(BlueprintCallable, Category="MM|Knowledge|Utils",
              meta=(DisplayName="Get Statistics"))
    void GetStatistics(int32& OutFactCount,
                      int32& OutConceptCount,
                      int32& OutRelationCount) const;
    
    /**
     * Validate knowledge consistency
     * @param OutErrors - Validation errors found
     * @return true if knowledge is consistent
     */
    UFUNCTION(BlueprintCallable, Category="MM|Knowledge|Utils",
              meta=(DisplayName="Validate Knowledge"))
    bool ValidateKnowledge(TArray<FString>& OutErrors);
};
