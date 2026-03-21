#include "DayTimeManager.h"
#include "Components/DirectionalLightComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerCharacter.h"
#include "TabletHUDWidget.h"

ADayTimeManager::ADayTimeManager()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ADayTimeManager::BeginPlay()
{
	Super::BeginPlay();
	
	Player = Cast<APlayerCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass()));
	
}

void ADayTimeManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!DirectionalLight) return;
	
	if (bIsDayFinished) return;
	
	TimeOfDay += DeltaTime / DayLength;
	if (TimeOfDay >= 1.0f)
	{
		TimeOfDay = 1.0f;
		bIsDayFinished = true;
	}
	float StartHour = 6.0f;
	float EndHour = 20.0f;
	float DayHours = EndHour - StartHour;
	
	float CurrentHourFloat = StartHour + TimeOfDay * DayHours;
	
	int32 Hours = FMath::FloorToInt(CurrentHourFloat);
	int32 Minutes = FMath::FloorToInt((CurrentHourFloat - Hours) * 60.0f);
	
	CurrentHour = FString::Printf(TEXT("%02d:%02d"), Hours, Minutes);
	
	float SunAngle = TimeOfDay * 180.0f + 180.0f;
	DirectionalLight->SetWorldRotation(FRotator(SunAngle, 0.0f, 0.0f));
}

void ADayTimeManager::EndDayAndReset()
{
	TimeOfDay = 0.0f;
	bIsDayFinished = false;
	Player->DeliveredPackages = 0;
	FString Str = FString::Printf(TEXT("%d"), Player->DeliveredPackages);
	Player->TabletHUDWidget->DeliveredPackagesCountText->SetText(FText::FromString(Str));
}

