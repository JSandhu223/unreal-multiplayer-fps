#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Actor.h"
#include "HealthComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHealthChanged, UHealthComponent*, HealthComponent, float, OldValue, float, NewValue, AActor*, Instigator);


UENUM(BlueprintType)
enum EDeathState : uint8
{
	NotDead,
	DeathStarted,
	DeathFinished
};


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FPS_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHealthComponent();
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(BlueprintPure, Category="FPS|Health")
	static UHealthComponent* FindHealthComponent(const AActor* Actor)
	{
		return IsValid(Actor) ? Actor->FindComponentByClass<UHealthComponent>() : nullptr;
	}
	
	UFUNCTION(BlueprintCallable)
	float GetHealthNormalized() const;
	
	// Returns true if lethal
	bool ChangeHealthByAmount(float Amount, AActor* Instigator);
	void ChangeMaxHealthByAmount(float Amount, AActor* Instigator);
	
	UPROPERTY(ReplicatedUsing=OnRep_DeathState)
	TEnumAsByte<EDeathState> DeathState;
	
	UPROPERTY(ReplicatedUsing=OnRep_Health, EditDefaultsOnly, Category="FPS|Health")
	float Health;
	
	UPROPERTY(ReplicatedUsing=OnRep_Health, EditDefaultsOnly, Category="FPS|Health")
	float MaxHealth;
	
	UPROPERTY(BlueprintAssignable)
	FOnHealthChanged OnHealthChanged;
	
	UPROPERTY(BlueprintAssignable)
	FOnHealthChanged OnMaxHealthChanged;

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnRep_DeathState(EDeathState OldDeathState);
	
	UFUNCTION()
	void OnRep_Health(float OldValue);
	
	UFUNCTION()
	void OnRep_MaxHealth(float OldValue);
};
