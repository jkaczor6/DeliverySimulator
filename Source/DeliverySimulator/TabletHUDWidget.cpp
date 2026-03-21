#include "TabletHUDWidget.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "DayTimeManager.h"
#include "PlayerCharacter.h"

void UTabletHUDWidget::ExitTablet()
{
	SetVisibility(ESlateVisibility::Hidden);
	
	APlayerController* APC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!APC) return;
	
	APC->SetShowMouseCursor(false);
	FInputModeGameOnly Mode;
	APC->SetInputMode(Mode);
}

void UTabletHUDWidget::StartOrderButtonClick()
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (!Player) return;
	
	Player->StartNewOrder();
	ExitTablet();
}
void UTabletHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	ExitButton->OnClicked.Clear();
	ExitButton->OnClicked.AddDynamic(this, &UTabletHUDWidget::ExitTablet);
	
	StartOrderButton->OnClicked.Clear();
	StartOrderButton->OnClicked.AddDynamic(this, &UTabletHUDWidget::StartOrderButtonClick);
	
	DayTimeManager = Cast<ADayTimeManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ADayTimeManager::StaticClass()));
}

void UTabletHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	TimeDisplayText->SetText(FText::FromString(DayTimeManager->CurrentHour));
}
