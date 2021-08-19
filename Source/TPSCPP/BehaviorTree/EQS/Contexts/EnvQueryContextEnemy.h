// Programmer: Khoi Ho

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "EnvQueryContextEnemy.generated.h"

/**
 * 
 */
UCLASS()
class TPSCPP_API UEnvQueryContextEnemy : public UEnvQueryContext
{
	GENERATED_BODY()
	
public:
	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;
};
