#pragma once

#include "CoreMinimal.h"
#include "Infrastructure//Data/REInfrastructureTypes.h"
#include "RENormalizer.generated.h"

/**
 * Core text normalization utility
 * Provides universal text cleanup that is safe for all systems
 * Lives in Core as it's infrastructure, not interpretation
 */
UCLASS(BlueprintType)
class REASONINGENGINE_API URENormalizer : public UObject
{
    GENERATED_BODY()
    
private:
    // Accent mapping for Latin characters
    static TMap<TCHAR, TCHAR> AccentMap;
    static bool bAccentMapInitialized;
    
    // Initialize accent mappings
    static void InitializeAccentMap();
    
public:
    // ========== NORMALIZATION METHODS ==========
    
    /**
     * Normalize text with default configuration
     * @param Text - Input text to normalize
     * @return Normalized text
     */
    UFUNCTION(BlueprintCallable, Category="RE|Core|Normalizer",
              meta=(DisplayName="Normalize"))
    static FString Normalize(const FString& Text);
    
    /**
     * Normalize text with custom configuration
     * @param Text - Input text
     * @param Config - Normalization settings
     * @return Normalized text
     */
    UFUNCTION(BlueprintCallable, Category="RE|Core|Normalizer",
              meta=(DisplayName="Normalize With Config"))
    static FString NormalizeWithConfig(const FString& Text, 
                                       const FRENormalizationConfig& Config);
    
    /**
     * Normalize text with specific mode
     * @param Text - Input text
     * @param Mode - Normalization mode
     * @return Normalized text
     */
    UFUNCTION(BlueprintCallable, Category="RE|Core|Normalizer",
              meta=(DisplayName="Normalize With Mode"))
    static FString NormalizeWithMode(const FString& Text, 
                                     ERENormalizationMode Mode);
    
    // ========== INDIVIDUAL OPERATIONS ==========
    
    /**
     * Convert to lowercase
     * @param Text - Input text
     * @return Lowercase text
     */
    UFUNCTION(BlueprintPure, Category="RE|Core|Normalizer",
              meta=(DisplayName="To Lowercase"))
    static FString ToLowercase(const FString& Text);
    
    /**
     * Convert to uppercase
     * @param Text - Input text
     * @return Uppercase text
     */
    UFUNCTION(BlueprintPure, Category="RE|Core|Normalizer",
              meta=(DisplayName="To Uppercase"))
    static FString ToUppercase(const FString& Text);
    
    /**
     * Remove leading/trailing whitespace
     * @param Text - Input text
     * @return Trimmed text
     */
    UFUNCTION(BlueprintPure, Category="RE|Core|Normalizer",
              meta=(DisplayName="Trim Whitespace"))
    static FString TrimWhitespace(const FString& Text);
    
    /**
     * Collapse multiple spaces to single space
     * @param Text - Input text
     * @return Text with collapsed whitespace
     */
    UFUNCTION(BlueprintPure, Category="RE|Core|Normalizer",
              meta=(DisplayName="Collapse Whitespace"))
    static FString CollapseWhitespace(const FString& Text);
    
    /**
     * Remove accents from Latin characters (é → e)
     * @param Text - Input text
     * @return Text without accents
     */
    UFUNCTION(BlueprintPure, Category="RE|Core|Normalizer",
              meta=(DisplayName="Remove Accents"))
    static FString RemoveAccents(const FString& Text);
    
    /**
     * Remove punctuation marks
     * @param Text - Input text
     * @param bKeepSpaces - Preserve spaces
     * @return Text without punctuation
     */
    UFUNCTION(BlueprintPure, Category="RE|Core|Normalizer",
              meta=(DisplayName="Remove Punctuation"))
    static FString RemovePunctuation(const FString& Text, bool bKeepSpaces = true);
    
    /**
     * Remove numeric characters
     * @param Text - Input text
     * @return Text without numbers
     */
    UFUNCTION(BlueprintPure, Category="RE|Core|Normalizer",
              meta=(DisplayName="Remove Numbers"))
    static FString RemoveNumbers(const FString& Text);
    
    /**
     * Remove non-alphanumeric characters
     * @param Text - Input text
     * @param bKeepSpaces - Preserve spaces
     * @return Alphanumeric text only
     */
    UFUNCTION(BlueprintPure, Category="RE|Core|Normalizer",
              meta=(DisplayName="Keep Alphanumeric"))
    static FString KeepAlphanumeric(const FString& Text, bool bKeepSpaces = true);
    
    // ========== CHARACTER OPERATIONS ==========
    
    /**
     * Normalize a single character
     * @param Char - Character to normalize
     * @param Config - Normalization settings
     * @return Normalized character(s) - may be empty or multiple chars
     */
    static FString NormalizeChar(TCHAR Char, const FRENormalizationConfig& Config);
    
    /**
     * Remove accent from a character
     * @param Char - Character to process
     * @return Character without accent
     */
    static TCHAR RemoveAccentFromChar(TCHAR Char);
    
    /**
     * Check if character is accent
     * @param Char - Character to check
     * @return true if accented character
     */
    static bool IsAccentedChar(TCHAR Char);
    
    // ========== UTILITIES ==========
    
    /**
     * Get default normalization config
     * @return Default configuration
     */
    UFUNCTION(BlueprintPure, Category="RE|Core|Normalizer",
              meta=(DisplayName="Get Default Config"))
    static FRENormalizationConfig GetDefaultConfig();
    
    /**
     * Get aggressive normalization config (maximum normalization)
     * @return Aggressive configuration
     */
    UFUNCTION(BlueprintPure, Category="RE|Core|Normalizer",
              meta=(DisplayName="Get Aggressive Config"))
    static FRENormalizationConfig GetAggressiveConfig();
    
    /**
     * Get minimal normalization config (preserve most features)
     * @return Minimal configuration
     */
    UFUNCTION(BlueprintPure, Category="RE|Core|Normalizer",
              meta=(DisplayName="Get Minimal Config"))
    static FRENormalizationConfig GetMinimalConfig();
};