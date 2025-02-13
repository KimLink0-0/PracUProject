// Fill out your copyright notice in the Description page of Project Settings.


#include "PracGameMode.h"
#include "PracGameState.h"
#include "PracPlayerController.h"
#include "PracRPGCharacter.h"

APracGameMode::APracGameMode()
{
	DefaultPawnClass = APracRPGCharacter::StaticClass();
	PlayerControllerClass = APracPlayerController::StaticClass();
	GameStateClass = APracGameState::StaticClass();
	
}
