#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "TabletHUDWidget.generated.h"

class UButton;

UCLASS()
class DELIVERYSIMULATOR_API UTabletHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UButton* ExitButton;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UButton* StartOrderButton;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* TimeDisplayText;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* DeliveredPackagesCountText;
	
	UFUNCTION()
	void ExitTablet();
	UFUNCTION()
	void StartOrderButtonClick();
	virtual void NativeConstruct() override;
};
