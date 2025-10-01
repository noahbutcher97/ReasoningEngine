// Source/ReasoningEngine/Public/BlueprintLibraries/UREFuzzyBlueprintLibrary.h
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Semantic/Data/RESemanticTypes.h"
#include "UREFuzzyBlueprintLibrary.generated.h"

UCLASS()
class REASONINGENGINE_API UREFuzzyBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="ReasoningEngine|Fuzzy", meta=(DisplayName="Calculate Levenshtein Distance"))
	static int32 BP_CalculateLevenshteinDistance(const FString& Source, const FString& Target);
    
	UFUNCTION(BlueprintCallable, Category="ReasoningEngine|Fuzzy", meta=(DisplayName="Calculate Jaro-Winkler Similarity"))
	static float BP_CalculateJaroWinkler(const FString& Source, const FString& Target);
    
	UFUNCTION(BlueprintCallable, Category="ReasoningEngine|Fuzzy", meta=(DisplayName="Batch Fuzzy Match"))
	static TArray<FREFuzzyResult> BP_BatchFuzzyMatch(
		const TArray<FString>& Candidates, 
		const FString& Query,
		EREFuzzyAlgorithm Algorithm = EREFuzzyAlgorithm::JaroWinkler,
		float MinScore = 0.0f
	);
    
	UFUNCTION(BlueprintCallable, Category="ReasoningEngine|Fuzzy", meta=(DisplayName="Generate Soundex"))
	static FString BP_GenerateSoundex(const FString& Input);
};