// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "XmlParser.h"
#include "GameFramework/Actor.h"
#include "Visualization.generated.h"

UCLASS()
class NJVR_API AVisualization : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVisualization();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    TSubclassOf<class ANodo> TipoNodo;//esto no es practio llenarlo en el cosntructor, cuando esta clase pase a bluprint sera mejor

    //el tipo de nodo que se instancia
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    TSubclassOf<class AArista> TipoArista;//esto no es practio llenarlo en el cosntructor, cuando esta clase pase a bluprint sera mejor

    TArray<class ANodo*> Nodos;

    TArray<class AArista*> Aristas;
	

    FXmlFile XmlSource;// o podria tener un puntero e ir genereando nuevos FXmlFile, todo debepnde, eso por que el contructor podria recibir el path, al ser creado, 
	
    void LoadNodos();//solo esto por que solo me interesa leer la informacion de los vertex, para saber quienes son hijos y padres, por eso tal vez no se trabaje como unity o si, probar
    void CreateNodos();
    void CreateAristas();
	
};

//por ahora usar los nodos blueprint, ya que estos pueden hacer el cambio de color de forma adecuada, pero usar la variable MaterialDynaimc para almacenar la referencai, y no promover variables