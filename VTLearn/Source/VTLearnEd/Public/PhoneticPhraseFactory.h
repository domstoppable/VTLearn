// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PhoneticPhrase.h"

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "PhoneticPhraseFactory.generated.h"

/**
 *
 */
UCLASS()
class VTLEARNED_API UPhoneticPhraseFactory : public UFactory
{
	GENERATED_BODY()

	UPhoneticPhraseFactory();

	virtual UObject* FactoryCreateBinary(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, const TCHAR* Type, const uint8*& Buffer, const uint8* BufferEnd, FFeedbackContext* Warn) override;
	UPhoneticPhrase* LoadPhoneticPhrase(const uint8*& Buffer, const uint8* BufferEnd, UPhoneticPhrase* ExistingPhrase = nullptr);

};