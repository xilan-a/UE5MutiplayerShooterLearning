// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Projectile.h"

void AProjectileWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	APawn* InstigatorPawn = Cast<APawn>(GetOwner());
	 const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	 UWorld* World = GetWorld();
	 if (MuzzleFlashSocket && World)
	 {
		 FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		 //Ŀ��λ�ü�ȥǹ��λ�õ����м��������ٵõ���������ķ�����Ϊ�ӵ�����
		 FVector ToTarget = HitTarget - SocketTransform.GetLocation();
		 FRotator TargetRotation = ToTarget.Rotation();

		 FActorSpawnParameters SpawnParams;
		 SpawnParams.Owner = GetOwner();
		 SpawnParams.Instigator = InstigatorPawn;

		 AProjectile* SpawnedProjectile = nullptr;
		 if (bUseServerSideRewind)
		 {
			 if (InstigatorPawn->HasAuthority())//�ڷ�����
			 {
				 if (InstigatorPawn->IsLocallyControlled())//���������� ���ɸ����ӵ�
				 {
					 SpawnedProjectile = World->SpawnActor<AProjectile>(ProjectileClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
					 SpawnedProjectile->bUseServerSideRewind = false;
					 SpawnedProjectile->Damage = Damage;
					 SpawnedProjectile->HeadShotDamage = HeadShotDamage;
				 }
				 else//�����������Ǳ��ؿ���,���ɲ����Ƶ��ӵ���ʹ�õ���
				 {
					 SpawnedProjectile = World->SpawnActor<AProjectile>(ServerSideRewindProjectileClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
					 SpawnedProjectile->bUseServerSideRewind = true;
				 }
			 }
			 else//�ͻ���ʹ�õ���
			 {
				 if (InstigatorPawn->IsLocallyControlled())//�ͻ��˱��ؿ���,���ɲ����Ƶ��ӵ���ʹ�õ���
				 {
					 SpawnedProjectile = World->SpawnActor<AProjectile>(ServerSideRewindProjectileClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
					 SpawnedProjectile->bUseServerSideRewind = true;
					 SpawnedProjectile->TraceStart = SocketTransform.GetLocation();
					 SpawnedProjectile->InitialVelocity = SpawnedProjectile->GetActorForwardVector() * SpawnedProjectile->InitialSpeed;
					 //SpawnedProjectile->Damage = Damage;//��ʵд��д����ν
				 }
				 else//�ͻ��˲��Ǳ��ؿ���,����һ���Ǹ����ӵ������õ���
				 {
					 SpawnedProjectile = World->SpawnActor<AProjectile>(ServerSideRewindProjectileClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
					 SpawnedProjectile->bUseServerSideRewind = false;
				 }
			 }
		 }
		 else//������ʹ�õ���
		 {
			 if (InstigatorPawn->HasAuthority())
			 {
				 SpawnedProjectile = World->SpawnActor<AProjectile>(ProjectileClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
				 SpawnedProjectile->bUseServerSideRewind = false;
				 SpawnedProjectile->Damage = Damage;
				 SpawnedProjectile->HeadShotDamage = HeadShotDamage;
			 }
		 }
	 }
}
