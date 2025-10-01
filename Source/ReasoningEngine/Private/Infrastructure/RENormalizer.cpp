// Source/ReasoningEngine/Private/Infrastructure/RENormalizer.cpp
#include "Infrastructure/RENormalizer.h"
#include "ReasoningEngine.h"
#include "Internationalization/Regex.h"

// ========== PRIMARY NORMALIZATION ==========

FString RENormalizer::NormalizeText(const FString& Text)
{
    // Default configuration
    FRENormalizationConfig DefaultConfig;
    return NormalizeTextWithConfig(Text, DefaultConfig);
}

FString RENormalizer::NormalizeTextWithConfig(const FString& Text, const FRENormalizationConfig& Config)
{
    if (Text.IsEmpty())
    {
        return Text;
    }
    
    FString Result = Text;
    
    // Apply normalization modes in specific order for best results
    
    // 1. Unicode normalization (if needed)
    if (Config.UnicodeForm != EREUnicodeNormalizationForm::NFC)
    {
        Result = NormalizeUnicode(Result, Config.UnicodeForm);
    }
    
    // 2. Remove custom characters
    if (!Config.CustomRemoveChars.IsEmpty())
    {
        for (TCHAR Char : Config.CustomRemoveChars)
        {
            Result = Result.Replace(&Char, TEXT(""));
        }
    }
    
    // 3. Apply case conversion
    if (Config.HasMode(ERENormalizationMode::Lowercase))
    {
        Result = ToLowercase(Result);
    }
    else if (Config.HasMode(ERENormalizationMode::Uppercase))
    {
        Result = ToUppercase(Result);
    }
    
    // 4. Remove accents
    if (Config.HasMode(ERENormalizationMode::RemoveAccents))
    {
        Result = RemoveAccents(Result);
    }
    
    // 5. Remove numbers
    if (Config.HasMode(ERENormalizationMode::RemoveNumbers))
    {
        Result = RemoveNumbers(Result);
    }
    
    // 6. Remove punctuation
    if (Config.HasMode(ERENormalizationMode::RemovePunctuation))
    {
        Result = RemovePunctuation(Result);
    }
    
    // 7. Collapse whitespace
    if (Config.HasMode(ERENormalizationMode::CollapseWhitespace))
    {
        Result = CollapseWhitespace(Result);
    }
    
    // 8. Trim whitespace
    if (Config.HasMode(ERENormalizationMode::TrimWhitespace))
    {
        Result = TrimWhitespace(Result);
    }
    
    // 9. Convert to ASCII if requested
    if (Config.bConvertToAscii)
    {
        Result = ToAscii(Result, Config.AsciiReplacementChar);
    }
    
    // 10. Apply length constraints
    if (Config.MinLength > 0 && Result.Len() < Config.MinLength)
    {
        UE_LOG(LogReasoningEngine, Warning, TEXT("Normalized text too short: %d < %d"), Result.Len(), Config.MinLength);
        return TEXT("");
    }
    
    if (Config.MaxLength > 0 && Result.Len() > Config.MaxLength)
    {
        Result = Result.Left(Config.MaxLength);
    }
    
    return Result;
}

// ========== SPECIFIC NORMALIZATIONS ==========

FString RENormalizer::ToLowercase(const FString& Text)
{
    return Text.ToLower();
}

FString RENormalizer::ToUppercase(const FString& Text)
{
    return Text.ToUpper();
}

FString RENormalizer::TrimWhitespace(const FString& Text)
{
    return Text.TrimStartAndEnd();
}

FString RENormalizer::RemovePunctuation(const FString& Text)
{
    FString Result;
    Result.Reserve(Text.Len());
    
    for (TCHAR Char : Text)
    {
        if (!FChar::IsPunctuation(Char))
        {
            Result.AppendChar(Char);
        }
    }
    
    return Result;
}

FString RENormalizer::RemoveAccents(const FString& Text)
{
    FString Result;
    Result.Reserve(Text.Len());
    
    // Common accent replacements
    static const TMap<TCHAR, TCHAR> AccentMap = {
        {TEXT('à'), TEXT('a')}, {TEXT('á'), TEXT('a')}, {TEXT('â'), TEXT('a')}, {TEXT('ã'), TEXT('a')}, {TEXT('ä'), TEXT('a')}, {TEXT('å'), TEXT('a')},
        {TEXT('è'), TEXT('e')}, {TEXT('é'), TEXT('e')}, {TEXT('ê'), TEXT('e')}, {TEXT('ë'), TEXT('e')},
        {TEXT('ì'), TEXT('i')}, {TEXT('í'), TEXT('i')}, {TEXT('î'), TEXT('i')}, {TEXT('ï'), TEXT('i')},
        {TEXT('ò'), TEXT('o')}, {TEXT('ó'), TEXT('o')}, {TEXT('ô'), TEXT('o')}, {TEXT('õ'), TEXT('o')}, {TEXT('ö'), TEXT('o')}, {TEXT('ø'), TEXT('o')},
        {TEXT('ù'), TEXT('u')}, {TEXT('ú'), TEXT('u')}, {TEXT('û'), TEXT('u')}, {TEXT('ü'), TEXT('u')},
        {TEXT('ý'), TEXT('y')}, {TEXT('ÿ'), TEXT('y')},
        {TEXT('ñ'), TEXT('n')}, {TEXT('ç'), TEXT('c')},
        // Uppercase
        {TEXT('À'), TEXT('A')}, {TEXT('Á'), TEXT('A')}, {TEXT('Â'), TEXT('A')}, {TEXT('Ã'), TEXT('A')}, {TEXT('Ä'), TEXT('A')}, {TEXT('Å'), TEXT('A')},
        {TEXT('È'), TEXT('E')}, {TEXT('É'), TEXT('E')}, {TEXT('Ê'), TEXT('E')}, {TEXT('Ë'), TEXT('E')},
        {TEXT('Ì'), TEXT('I')}, {TEXT('Í'), TEXT('I')}, {TEXT('Î'), TEXT('I')}, {TEXT('Ï'), TEXT('I')},
        {TEXT('Ò'), TEXT('O')}, {TEXT('Ó'), TEXT('O')}, {TEXT('Ô'), TEXT('O')}, {TEXT('Õ'), TEXT('O')}, {TEXT('Ö'), TEXT('O')}, {TEXT('Ø'), TEXT('O')},
        {TEXT('Ù'), TEXT('U')}, {TEXT('Ú'), TEXT('U')}, {TEXT('Û'), TEXT('U')}, {TEXT('Ü'), TEXT('U')},
        {TEXT('Ý'), TEXT('Y')}, {TEXT('Ñ'), TEXT('N')}, {TEXT('Ç'), TEXT('C')}
    };
    
    for (TCHAR Char : Text)
    {
        if (const TCHAR* Replacement = AccentMap.Find(Char))
        {
            Result.AppendChar(*Replacement);
        }
        else
        {
            Result.AppendChar(Char);
        }
    }
    
    return Result;
}

FString RENormalizer::CollapseWhitespace(const FString& Text)
{
    FString Result;
    Result.Reserve(Text.Len());
    
    bool bInWhitespace = false;
    
    for (TCHAR Char : Text)
    {
        if (FChar::IsWhitespace(Char))
        {
            if (!bInWhitespace)
            {
                Result.AppendChar(TEXT(' '));
                bInWhitespace = true;
            }
        }
        else
        {
            Result.AppendChar(Char);
            bInWhitespace = false;
        }
    }
    
    return Result;
}

FString RENormalizer::RemoveNumbers(const FString& Text)
{
    FString Result;
    Result.Reserve(Text.Len());
    
    for (TCHAR Char : Text)
    {
        if (!FChar::IsDigit(Char))
        {
            Result.AppendChar(Char);
        }
    }
    
    return Result;
}

FString RENormalizer::RemoveSpecialCharacters(const FString& Text)
{
    FString Result;
    Result.Reserve(Text.Len());
    
    for (TCHAR Char : Text)
    {
        if (FChar::IsAlnum(Char) || FChar::IsWhitespace(Char))
        {
            Result.AppendChar(Char);
        }
    }
    
    return Result;
}

// ========== TEXT ANALYSIS ==========

bool RENormalizer::IsAlphabetic(const FString& Text)
{
    if (Text.IsEmpty()) return false;
    
    for (TCHAR Char : Text)
    {
        if (!FChar::IsAlpha(Char))
        {
            return false;
        }
    }
    
    return true;
}

bool RENormalizer::IsAlphanumeric(const FString& Text)
{
    if (Text.IsEmpty()) return false;
    
    for (TCHAR Char : Text)
    {
        if (!FChar::IsAlnum(Char))
        {
            return false;
        }
    }
    
    return true;
}

bool RENormalizer::IsNumeric(const FString& Text)
{
    if (Text.IsEmpty()) return false;
    
    for (TCHAR Char : Text)
    {
        if (!FChar::IsDigit(Char))
        {
            return false;
        }
    }
    
    return true;
}

bool RENormalizer::ContainsPunctuation(const FString& Text)
{
    for (TCHAR Char : Text)
    {
        if (FChar::IsPunctuation(Char))
        {
            return true;
        }
    }
    
    return false;
}

bool RENormalizer::ContainsWhitespace(const FString& Text)
{
    for (TCHAR Char : Text)
    {
        if (FChar::IsWhitespace(Char))
        {
            return true;
        }
    }
    
    return false;
}

// ========== UNICODE HANDLING ==========

FString RENormalizer::NormalizeUnicode(const FString& Text, EREUnicodeNormalizationForm Form)
{
    // Note: Unreal doesn't have built-in Unicode normalization
    // This would require ICU or similar library for full implementation
    // For now, return the text unchanged
    
    UE_LOG(LogReasoningEngine, Verbose, TEXT("Unicode normalization not fully implemented, returning original text"));
    return Text;
}

FString RENormalizer::ToAscii(const FString& Text, const FString& ReplacementChar)
{
    FString Result;
    Result.Reserve(Text.Len());
    
    for (TCHAR Char : Text)
    {
        if (Char <= 127) // ASCII range
        {
            Result.AppendChar(Char);
        }
        else if (!ReplacementChar.IsEmpty())
        {
            Result.Append(ReplacementChar);
        }
        // else skip non-ASCII characters
    }
    
    return Result;
}

// ========== CASE CONVERSION ==========

FString RENormalizer::ToTitleCase(const FString& Text)
{
    FString Result;
    Result.Reserve(Text.Len());
    
    bool bNextIsTitle = true;
    
    for (TCHAR Char : Text)
    {
        if (FChar::IsWhitespace(Char))
        {
            Result.AppendChar(Char);
            bNextIsTitle = true;
        }
        else if (bNextIsTitle && FChar::IsAlpha(Char))
        {
            Result.AppendChar(FChar::ToUpper(Char));
            bNextIsTitle = false;
        }
        else
        {
            Result.AppendChar(FChar::ToLower(Char));
        }
    }
    
    return Result;
}

FString RENormalizer::ToSentenceCase(const FString& Text)
{
    FString Result = Text.ToLower();
    
    if (Result.Len() > 0)
    {
        Result[0] = FChar::ToUpper(Result[0]);
        
        // Find sentence endings and capitalize next letter
        static const FString SentenceEnders = TEXT(".!?");
        
        bool bCapitalizeNext = false;
        for (int32 i = 1; i < Result.Len(); i++)
        {
            if (bCapitalizeNext && FChar::IsAlpha(Result[i]))
            {
                Result[i] = FChar::ToUpper(Result[i]);
                bCapitalizeNext = false;
            }
            else if (SentenceEnders.Contains(FString::Chr(Result[i])))
            {
                bCapitalizeNext = true;
            }
        }
    }
    
    return Result;
}

FString RENormalizer::FromCamelCase(const FString& Text)
{
    FString Result;
    Result.Reserve(Text.Len() * 2);
    
    for (int32 i = 0; i < Text.Len(); i++)
    {
        TCHAR Char = Text[i];
        
        // Add space before uppercase letters (except first char)
        if (i > 0 && FChar::IsUpper(Char))
        {
            // Don't add space if previous char was already uppercase (acronym)
            if (i == 1 || !FChar::IsUpper(Text[i-1]))
            {
                Result.AppendChar(TEXT(' '));
            }
        }
        
        Result.AppendChar(FChar::ToLower(Char));
    }
    
    return Result;
}

FString RENormalizer::FromSnakeCase(const FString& Text)
{
    return Text.Replace(TEXT("_"), TEXT(" "));
}

FString RENormalizer::FromKebabCase(const FString& Text)
{
    return Text.Replace(TEXT("-"), TEXT(" "));
}

// ========== UTILITY FUNCTIONS ==========

ERECharacterType RENormalizer::GetCharacterType(TCHAR Char)
{
    if (FChar::IsAlpha(Char))
    {
        if (IsVowel(Char))
        {
            return ERECharacterType::Vowel;
        }
        else
        {
            return ERECharacterType::Consonant;
        }
    }
    else if (FChar::IsDigit(Char))
    {
        return ERECharacterType::Digit;
    }
    else if (FChar::IsWhitespace(Char))
    {
        return ERECharacterType::Whitespace;
    }
    else if (FChar::IsPunctuation(Char))
    {
        return ERECharacterType::Punctuation;
    }
    else if (FChar::IsControl(Char))
    {
        return ERECharacterType::Control;
    }
    else
    {
        return ERECharacterType::Symbol;
    }
}

bool RENormalizer::IsVowel(TCHAR Char)
{
    TCHAR Upper = FChar::ToUpper(Char);
    return Upper == TEXT('A') || Upper == TEXT('E') || Upper == TEXT('I') || 
           Upper == TEXT('O') || Upper == TEXT('U') || Upper == TEXT('Y');
}

bool RENormalizer::IsConsonant(TCHAR Char)
{
    return FChar::IsAlpha(Char) && !IsVowel(Char);
}

FString RENormalizer::GetComparisonForm(const FString& Text)
{
    // Aggressive normalization for comparison
    FRENormalizationConfig Config;
    Config.Modes = static_cast<uint8>(ERENormalizationMode::Full);
    Config.bConvertToAscii = true;
    
    return NormalizeTextWithConfig(Text, Config);
}