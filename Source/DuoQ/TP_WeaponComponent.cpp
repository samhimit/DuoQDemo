// Copyright Epic Games, Inc. All Rights Reserved.


#include "TP_WeaponComponent.h"
#include "DuoQCharacter.h"
#include "DuoQProjectile.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/DamageEvents.h"

// Sets default values for this component's properties
UTP_WeaponComponent::UTP_WeaponComponent()
{
	// Default offset from the character location for projectiles to spawn
	MuzzleOffset = FVector(100.0f, 0.0f, 10.0f);
}


void UTP_WeaponComponent::Fire()
{
	// if (Character == nullptr || Character->GetController() == nullptr)
	// {
	// 	return;
	// }

	// Try and fire a projectile
	UWorld* const World = GetWorld();
	if (World != nullptr)
	{
		FRotator SpawnRotation;
		APawn* Pawn = Cast<APawn>(GetOwner());
		SpawnRotation = Pawn->GetController()->GetControlRotation();
		// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
		const FVector SpawnLocation = GetOwner()->GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);
		const FVector EndLocation = GetOwner()->GetActorLocation() + SpawnRotation.RotateVector(FVector(10000.0f,0.0f,0.0f));
		
		FHitResult HitResult(ForceInit);
		FCollisionQueryParams CollisionParams;
		CollisionParams.bDebugQuery = true;
		bool DidHitObject = World->LineTraceSingleByChannel(HitResult,SpawnLocation,EndLocation,ECC_WorldDynamic);
		DrawDebugLine(GetWorld(), SpawnLocation, EndLocation, FColor::Green, true, 2.f, false, 4.f);
		if(GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Shooting!"));
		if(DidHitObject)
		{
			if(GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Some debug message!"));
			HitResult.GetActor()->TakeDamage(1.0f,FDamageEvent(),Pawn->GetController(),GetOwner());
		}
		if(FireEffect != nullptr)
			UGameplayStatics::SpawnEmitterAttached(FireEffect, this,TEXT("Muzzle"),FVector(),FRotator(),EAttachLocation::KeepRelativeOffset, true);
			//UGameplayStatics::SpawnEmitterAttached(FireEffect, this,"Muzzle",SpawnLocation + SpawnRotation.RotateVector(FVector(0.0f,100.0f,-20.0f)), FRotator::ZeroRotator,EAttachLocation::KeepRelativeOffset, true);
	}
	
	// Try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetOwner()->GetActorLocation());
	}
	
	
	
	// Try and play a firing animation if specified
	// if (FireAnimation != nullptr)
	// {
	// 	// Get the animation object for the arms mesh
	// 	UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance();
	// 	if (AnimInstance != nullptr)
	// 	{
	// 		AnimInstance->Montage_Play(FireAnimation, 1.f);
	// 	}
	// }
}

void UTP_WeaponComponent::AttachWeapon(ADuoQCharacter* TargetCharacter)
{
	Character = TargetCharacter;
	if (Character == nullptr)
	{
		return;
	}

	// Attach the weapon to the First Person Character
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));
	
	// switch bHasRifle so the animation blueprint can switch to another animation set
	Character->SetHasRifle(true);

	// Set up action bindings
	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Set the priority of the mapping to 1, so that it overrides the Jump action with the Fire action when using touch input
			Subsystem->AddMappingContext(FireMappingContext, 1);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			// Fire
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &UTP_WeaponComponent::Fire);
		}
	}
}

void UTP_WeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (Character == nullptr)
	{
		return;
	}

	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(FireMappingContext);
		}
	}
}