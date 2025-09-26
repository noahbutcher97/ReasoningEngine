#include "Misc/AutomationTest.h"
#include "Semantic/RESimilarity.h"
#include "Semantic/Data/RESemanticTypes.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRESimilarityHarnessTest,
	"ReasoningEngine.Similarity.Harness",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRESimilarityHarnessTest::RunTest(const FString& Parameters)
{
	FString A = TEXT("apple");
	FString B = TEXT("applesauce");

	// Loop over all similarity metrics
	for (int32 MetricIndex = 0; MetricIndex < static_cast<int32>(ERESimilarityMetric::Auto) + 1; ++MetricIndex)
	{
		ERESimilarityMetric Metric = static_cast<ERESimilarityMetric>(MetricIndex);

		// Dispatcher call (currently a stub in URESimilarity)
		FRESimilarityResult Result = URESimilarity::CompareStringsWithAlgo(A, B, Metric, true);

		FString MetricName = StaticEnum<ERESimilarityMetric>()->GetNameStringByValue(MetricIndex);

		// Log representative values
		UE_LOG(LogTemp, Display,
			TEXT("Metric: %s | Overall=%.3f | Fuzzy=%.3f | Token=%.3f | Vector=%.3f | Flags(F=%d,T=%d,V=%d)"),
			*MetricName,
			Result.OverallScore,
			Result.FuzzyScore,
			Result.TokenScore,
			Result.VectorScore,
			Result.bUsedFuzzy,
			Result.bUsedTokens,
			Result.bUsedVectors);

		// Basic sanity checks
		TestTrue(FString::Printf(TEXT("Similarity %s Overall valid"), *MetricName),
				 Result.OverallScore >= 0.f && Result.OverallScore <= 1.f);

		TestTrue(FString::Printf(TEXT("Similarity %s UsedMetric correct"), *MetricName),
				 Result.UsedMetric == Metric);
	}

	return true;
}
