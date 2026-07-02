#pragma once

#include "CoreMinimal.h"
#include "Semantic/Data/RESemanticTypes.h"

class REASONINGENGINE_API RESimilarity
{
public:
    static FRESimilarityResult CompareStringsWithAlgo(
        const FString& A,
        const FString& B,
        ERESimilarityMetric Metric = ERESimilarityMetric::Auto,
        bool bNormalize = true
    );
};
