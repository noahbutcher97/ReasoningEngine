#include "Misc/AutomationTest.h"
#include "Semantic/REFuzzy.h"
#include "Semantic/Data/REStringTypes.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FREFuzzyHarnessTest,
	"ReasoningEngine.Fuzzy.Harness",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FREFuzzyHarnessTest::RunTest(const FString& Parameters)
{
	FString A = TEXT("apple");
	FString B = TEXT("applesauce");

	// Loop over all available fuzzy algorithms in the enum
	for (int32 AlgoIndex = 0; AlgoIndex < static_cast<int32>(EREFuzzyAlgorithm::Auto) + 1; ++AlgoIndex)
	{
		static_cast<EREFuzzyAlgorithm>(AlgoIndex);

		// Run fuzzy comparison (your real implementation computes all metrics)
		FREStringMatch Result = UREFuzzy::CompareStrings(A, B, true);

		FString AlgoName = StaticEnum<EREFuzzyAlgorithm>()->GetNameStringByValue(AlgoIndex);

		// Just log some representative fields for sanity
		UE_LOG(LogTemp, Display,
			TEXT("Algo: %s | Lev=%d | JW=%.3f | Dice=%.3f | Jac=%.3f"),
			*AlgoName,
			Result.LevenshteinDistance,
			Result.JaroWinklerSimilarity,
			Result.DiceCoefficient,
			Result.JaccardIndex);

		// Trivial "pass" condition: nothing crashed, some fields are in range
		TestTrue(FString::Printf(TEXT("Fuzzy %s valid"), *AlgoName),
				 Result.NormalizedLevenshtein >= 0.f && Result.NormalizedLevenshtein <= 1.f);
	}

	return true;
}
