// (c) 2021 Dominic Canare <dom@dominiccanare.com>

#pragma once

#include "PhoneticPhrase.h"

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "PhoneticPhraseFactory.generated.h"

UCLASS()
class VTLEARNED_API UPhoneticPhraseFactory : public UFactory
{
	GENERATED_BODY()

	UPhoneticPhraseFactory();

	virtual UObject* FactoryCreateBinary(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, const TCHAR* Type, const uint8*& Buffer, const uint8* BufferEnd, FFeedbackContext* Warn) override;
	virtual UObject* FactoryCreateFile(UClass * InClass, UObject * InParent, FName InName, EObjectFlags Flags, const FString & Filename, const TCHAR * Parms, FFeedbackContext * Warn, bool & bOutOperationCanceled) override;

	UPhoneticPhrase* LoadPhoneticPhrase(const uint8*& Buffer, const uint8* BufferEnd, UPhoneticPhrase* ExistingPhrase = nullptr);

};
