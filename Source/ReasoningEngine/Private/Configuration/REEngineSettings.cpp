#include "Configuration/REEngineSettings.h"

UREEngineSettings::UREEngineSettings()
{
}

UREEngineSettings* UREEngineSettings::Get()
{
    return nullptr;
}

class UREEngineConfiguration* UREEngineSettings::LoadDefaultConfiguration() const
{
    return nullptr;
}

class UREEngineConfiguration* UREEngineSettings::GetContextConfiguration(FName Context) const
{
    return nullptr;
}

void UREEngineSettings::ApplySettings(class URECore* Engine) const
{
}

#if WITH_EDITOR
FText UREEngineSettings::GetSectionText() const
{
    return FText();
}

FText UREEngineSettings::GetSectionDescription() const
{
    return FText();
}

void UREEngineSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
}
#endif

FName UREEngineSettings::GetCategoryName() const
{
    return FName();
}
