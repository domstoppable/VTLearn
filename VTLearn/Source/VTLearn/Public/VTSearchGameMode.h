#pragma once

#include "VTLearnGameMode.h"
#include "VTHintDisplayer.h"
#include "PhoneSequenceMatcher.h"

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "VTSearchGameMode.generated.h"


UCLASS(BlueprintType)
class VTLEARN_API AVTSearchGameMode : public AVTLearnGameMode
{
	GENERATED_BODY()

public:
	AVTSearchGameMode();

	virtual void LoadLevelInfo() override;
	virtual void SetupReceivers() override {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TScriptInterface<IVTHintDisplayer> HintDisplayer = nullptr;

	int32 CurrentPhraseIdx = -1;

	UPROPERTY(BlueprintReadWrite)
	UPhoneSequenceMatcher* Matcher;

	UPROPERTY(BlueprintReadWrite)
	FTimerHandle NextPhraseTimerHandle;

	TArray<FString> PhraseKeys;

	TArray<UPhoneticPhrase*> ItemPool;

	UFUNCTION()
	void NextPhrase();

	UFUNCTION()
	void ShowPhraseHint();

	UFUNCTION()
	void ShowHint(FString Hint);

};
