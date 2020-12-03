#include "GamepadButtonImages.h"

#include "GameFramework/InputSettings.h"
#include "GameFramework/PlayerInput.h"

UTexture2D* UGamepadButtonImages::FromKey(FKey Key)
{
	if(Key == EKeys::Gamepad_FaceButton_Top)
	{
		return FaceTop;
	}
	if(Key == EKeys::Gamepad_FaceButton_Right)
	{
		return FaceRight;
	}
	if(Key == EKeys::Gamepad_FaceButton_Bottom)
	{
		return FaceBottom;
	}
	if(Key == EKeys::Gamepad_FaceButton_Left)
	{
		return FaceLeft;
	}

	return nullptr;
}

UTexture2D* UGamepadButtonImages::FromAction(FName Action)
{
	TArray<FInputActionKeyMapping> ActionMappings;

	UInputSettings* InputSettings = UInputSettings::GetInputSettings();
	InputSettings->GetActionMappingByName(Action, ActionMappings);

	for(FInputActionKeyMapping ActionMapping : ActionMappings)
	{
		UTexture2D* Image = FromKey(ActionMapping.Key);
		if(Image)
		{
			return Image;
		}
	}

	return nullptr;
}
