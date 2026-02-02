#include "MineItem.h"

#include "Components/ProgressBar.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

AMineItem::AMineItem()
{
    ExplosionDelay = 2.0f;
    ExplosionRadius = 300.0f;
    ExplosionDamage = 30.0f;
    ItemType = "Mine";
    bHasExploded = false;

    ExplosionCollision = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosionCollision"));
    ExplosionCollision->InitSphereRadius(ExplosionRadius);
    ExplosionCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    ExplosionCollision->SetupAttachment(Scene);
    
    OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
    OverheadWidget->SetupAttachment(Scene);
    OverheadWidget->SetWidgetSpace(EWidgetSpace::Screen);
    OverheadWidget->SetVisibility(false);
}

void AMineItem::BeginPlay()
{
    Super::BeginPlay();
    GetWorld()->GetTimerManager().SetTimer(
        UpdateUITimerHandle,
        this,
        &AMineItem::UpdateOverheadUI,
        0.1f,
        true
    );
}

void AMineItem::ActivateItem(TObjectPtr<AActor> Activator)
{
    if (bHasExploded) return;
    
    Super::ActivateItem(Activator);
    
    OverheadWidget->SetVisibility(true);
    
    GetWorld()->GetTimerManager().SetTimer(
        ExplosionTimerHandle, 
        this, 
        &AMineItem::Explode, 
        ExplosionDelay,
        false
    );
    bHasExploded = true;
}

void AMineItem::Explode()
{
    TObjectPtr<UParticleSystemComponent> Particle = nullptr;
    
    if (ExplosionParticle)
    {
        Particle = UGameplayStatics::SpawnEmitterAtLocation(
            GetWorld(),
            ExplosionParticle,
            GetActorLocation(),
            GetActorRotation(),
            true
        );
    }
    
    if (ExplosionSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            GetWorld(),
            ExplosionSound,
            GetActorLocation()
        );
    }
    
    TArray<AActor*> OverlappingActors;
    ExplosionCollision->GetOverlappingActors(OverlappingActors);

    for (AActor* Actor : OverlappingActors)
    {
        if (Actor && Actor->ActorHasTag("Player"))
        {
            UGameplayStatics::ApplyDamage(
                Actor,
                ExplosionDamage,
                nullptr,
                this,
                UDamageType::StaticClass()
            );
        }
    }

    DestroyItem();
    
    if (Particle)
    {
        FTimerHandle DestroyParticleTimerHandle;
        TWeakObjectPtr<UParticleSystemComponent> WeakParticle = Particle;
						
        GetWorld()->GetTimerManager().SetTimer(
            DestroyParticleTimerHandle,
            [WeakParticle]()
            {
                    if (WeakParticle.IsValid())
                    {
                            WeakParticle->DestroyComponent();
                    }
            },
            2.0f,
            false
        );
    }
}

void AMineItem::UpdateOverheadUI()
{
    if (!OverheadWidget) return;
	
    TObjectPtr<UUserWidget> OverheadWidgetInstance = OverheadWidget->GetUserWidgetObject();
    if (!OverheadWidgetInstance) return;
	
    if (TObjectPtr<UProgressBar> OverHeadUI = Cast<UProgressBar>(OverheadWidgetInstance->GetWidgetFromName(TEXT("ExplosionDelay"))))
    {
        float ExplosionTime = GetWorldTimerManager().GetTimerRemaining(ExplosionTimerHandle);
        OverHeadUI->SetPercent( ExplosionTime / ExplosionDelay);
    }
}

void AMineItem::ExplosionDelayMultiplier(float value)
{
    ExplosionDelay = ExplosionDelay * value;
}

void AMineItem::ExplosionDamageMultiplier(float value)
{
    ExplosionDamage = ExplosionDamage * value;
}
