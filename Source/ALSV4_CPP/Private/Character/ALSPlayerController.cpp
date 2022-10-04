// Copyright:       Copyright (C) 2022 Doğa Can Yanıkoğlu
// Source Code:     https://github.com/dyanikoglu/ALS-Community


#include "Character/ALSPlayerController.h"

#include "AI/ALSAIController.h"
#include "Character/ALSCharacter.h"
#include "Character/ALSPlayerCameraManager.h"
#include "Components/ALSDebugComponent.h"
#include "Kismet/GameplayStatics.h"

void AALSPlayerController::OnPossess(APawn* NewPawn)
{
	Super::OnPossess(NewPawn);
	PossessedCharacter = Cast<AALSBaseCharacter>(NewPawn);
	if (!IsRunningDedicatedServer())
	{
		// Servers want to setup camera only in listen servers.
		SetupCamera();
	}

#if ENABLE_ALS_DEBUG_COMPONENT
	if (UALSDebugComponent* DebugComp = PossessedCharacter->FindComponentByClass<UALSDebugComponent>())
	{
		DebugComp->OnPlayerControllerInitialized(this);
	}
#endif
	
}

void AALSPlayerController::OnRep_Pawn()
{
	Super::OnRep_Pawn();
	PossessedCharacter = Cast<AALSBaseCharacter>(GetPawn());
	SetupCamera();

#if ENABLE_ALS_DEBUG_COMPONENT
	if (UALSDebugComponent* DebugComp = Cast<UALSDebugComponent>(PossessedCharacter->GetComponentByClass(UALSDebugComponent::StaticClass())))
	{
		DebugComp->OnPlayerControllerInitialized(this);
	}
#endif
}

void AALSPlayerController::ServerUpdateCamera_Implementation(const FVector_NetQuantize CamLoc, const int32 CamPitchAndYaw)
{
	Super::ServerUpdateCamera_Implementation(CamLoc, CamPitchAndYaw);

	if (!PossessedCharacter)
	{
		return;
	}
	
#if ENABLE_DRAW_DEBUG
	if ( PlayerCameraManager->bDebugClientSideCamera )
	{
		const float Yaw = FRotator::DecompressAxisFromShort( (CamPitchAndYaw >> 16) & 65535 );
		const float Pitch = FRotator::DecompressAxisFromShort(CamPitchAndYaw & 65535);

		DrawDebugCone(GetWorld(), PossessedCharacter->GetFirstPersonCameraTarget(), -FRotator(Pitch, Yaw, 0.f).Vector(), 50.0f,
			30.0f, 30.0f, 8, FColor::Orange, false, -1.0,SDPG_World, 3);

	}
#endif
}

void AALSPlayerController::SetupCamera() const
{
	// Call "OnPossess" in Player Camera Manager when possessing a pawn
	if (AALSPlayerCameraManager* CastedMgr = Cast<AALSPlayerCameraManager>(PlayerCameraManager);
		PossessedCharacter && CastedMgr)
	{
		CastedMgr->OnPossess(PossessedCharacter);
	}
}
