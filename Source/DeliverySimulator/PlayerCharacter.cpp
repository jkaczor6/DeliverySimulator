#include "PlayerCharacter.h"

#include "DayTimeManager.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/SceneComponent.h"
#include "DeliveryPackage.h"
#include "House.h"
#include "Truck.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TabletHUDWidget.h"
#include "Warehouse.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));
	
	FirstPersonMesh->SetupAttachment(GetMesh());
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));
	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FirstPersonCameraComponent->SetupAttachment(FirstPersonMesh, FName("head"));
	FirstPersonCameraComponent->SetRelativeLocationAndRotation(FVector(-2.8f, 5.89f, 0.0f), FRotator(0.0f, 90.0f, -90.0f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->bEnableFirstPersonFieldOfView = true;
	FirstPersonCameraComponent->bEnableFirstPersonScale = true;
	FirstPersonCameraComponent->FirstPersonFieldOfView = 70.0f;
	FirstPersonCameraComponent->FirstPersonScale = 0.6f;

	BoxSocket = CreateDefaultSubobject<USceneComponent>(TEXT("Box Socket"));
	BoxSocket->SetupAttachment(RootComponent);
	
	// configure the character comps
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

	GetCapsuleComponent()->SetCapsuleSize(34.0f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->AirControl = 0.5f;
	
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	TArray<AActor*> HouseActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AHouse::StaticClass(), HouseActors);
	for (AActor* HouseActor : HouseActors)
	{
		AHouse* House = Cast<AHouse>(HouseActor);
		if (House)
		{
			Houses.Add(House);
		}
	}
	
	Truck = Cast<ATruck>(UGameplayStatics::GetActorOfClass(GetWorld(), ATruck::StaticClass()));
	DayTimeManager = Cast<ADayTimeManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ADayTimeManager::StaticClass()));
	Warehouse = Cast<AWarehouse>(UGameplayStatics::GetActorOfClass(GetWorld(), AWarehouse::StaticClass()));
	
	TabletHUDWidget = CreateWidget<UTabletHUDWidget>(UGameplayStatics::GetPlayerController(GetWorld(), 0), TabletHUDWidgetClass);
	if (TabletHUDWidget)
	{
		TabletHUDWidget->AddToPlayerScreen();
		TabletHUDWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Movement
		EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::MoveInput);
		
		// Looking
		EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::LookInput);
		
		// Jumping
		EIC->BindAction(JumpAction, ETriggerEvent::Started, this, &APlayerCharacter::DoJumpStart);
		EIC->BindAction(JumpAction, ETriggerEvent::Completed, this, &APlayerCharacter::DoJumpEnd);
		
		// Interacting
		EIC->BindAction(InteractAction, ETriggerEvent::Started, this, &APlayerCharacter::InteractInput);
		
		// Tablet
		EIC->BindAction(TabletAction, ETriggerEvent::Started, this, &APlayerCharacter::TabletInput);
	}
	
}

void APlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	if (APlayerController* APC = Cast<APlayerController>(NewController))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(APC->GetLocalPlayer()))
		{
			Subsystem->ClearAllMappings();
			Subsystem->AddMappingContext(DefaultIMC, 0);
		}
	}
}

void APlayerCharacter::MoveInput(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	
	AddMovementInput(GetActorRightVector(), MovementVector.X);
	AddMovementInput(GetActorForwardVector(), MovementVector.Y);
}

void APlayerCharacter::LookInput(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	
	AddControllerYawInput(-LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void APlayerCharacter::InteractInput(const FInputActionValue& Value)
{
	if (Truck->PlayerOverlappingWithTrunk)
	{
		if (!Truck->HasPackageAtTrunk && IsHoldingPackage)
		{
			HidePackage();
			Truck->HasPackageAtTrunk = true;	
		}
		else if (Truck->HasPackageAtTrunk && !IsHoldingPackage)
		{
			PullOutPackage();
			Truck->HasPackageAtTrunk = false;
		}
	}
	if (Truck->PlayerOverlappingWithCabin)
	{
		if (!IsHoldingPackage && !IsInTruck)
		{
			EnterTruck();
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("You cant enter Truck while holding package")));
		}
	}
}

void APlayerCharacter::StartNewOrder()
{
	if (HasActiveOrder) return;
	if (Houses.Num() == 0) return;
	if (DayTimeManager->bIsDayFinished) return;
	if (!Warehouse->PlayerOverlapping) return;
	
	int32 RandomHouseIndex = FMath::RandRange(0, Houses.Num() - 1);
	if (Houses[RandomHouseIndex])
	{	
		Houses[RandomHouseIndex]->DeliveryPoint->SetVisibility(true);
		Houses[RandomHouseIndex]->HouseLocator->SetVisibility(true);
		Houses[RandomHouseIndex]->OnPackageDeliveredDelegate.AddDynamic(this, &APlayerCharacter::DeliverPackage);
	}
	
	FTransform SpawnTransform = BoxSocket->GetComponentTransform();
	DeliveryPackage = GetWorld()->SpawnActor<ADeliveryPackage>(BoxClass, SpawnTransform);
	
	DeliveryPackage->AttachToComponent(BoxSocket, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	if (DeliveryPackage)
	{
		IsHoldingPackage = true;
	}
	
	HasActiveOrder = true;
}

void APlayerCharacter::TabletInput(const FInputActionValue& Value)
{
	if (!TabletHUDWidget) return;
	if (TabletHUDWidget->GetVisibility() == ESlateVisibility::Visible) return;
	
	TabletHUDWidget->SetVisibility(ESlateVisibility::Visible);
	
	APlayerController* APC = UGameplayStatics::GetPlayerController(GetWorld(),0);
	if (!APC) return;
	
	APC->SetShowMouseCursor(true);
	
	FInputModeUIOnly Mode;
	Mode.SetWidgetToFocus(TabletHUDWidget->TakeWidget());
	APC->SetInputMode(Mode);
}

void APlayerCharacter::DoJumpStart(const FInputActionValue& Value)
{
	Jump();
}

void APlayerCharacter::DoJumpEnd(const FInputActionValue& Value)
{
	StopJumping();
}

void APlayerCharacter::DeliverPackage()
{
	if (HasActiveOrder && IsHoldingPackage)
	{
		DeliveryPackage->Destroy();
		DeliveredPackages++;
		HasActiveOrder = false;
		IsHoldingPackage = false;
		
		FString Str = FString::Printf(TEXT("%d"), DeliveredPackages);
		TabletHUDWidget->DeliveredPackagesCountText->SetText(FText::FromString(Str));
	}
}

void APlayerCharacter::HidePackage()
{
	DeliveryPackage->BoxMesh->SetVisibility(false);
	IsHoldingPackage = false;
}

void APlayerCharacter::PullOutPackage()
{
	DeliveryPackage->BoxMesh->SetVisibility(true);
	IsHoldingPackage = true;
}

void APlayerCharacter::EnterTruck()
{
	if (!Truck) return;

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;
	
	AttachToActor(Truck, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	
	PC->Possess(Truck);
	IsInTruck = true;
}
