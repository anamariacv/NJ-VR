// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Nodo.generated.h"

UCLASS()
class NJVR_API ANodo : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANodo();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    //este conjunto de variables deberia ser privados,
    //id del vertice
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visualization")
    int Id;
    
    //verdadero si es un nodo real, falso si es un nodo virtual
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visualization")
    bool Valid;

    //verdadero si es un nodo real, falso si es un nodo virtual
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visualization")
    bool Selected;

    //la corrdenada x
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visualization")
    float Xcoordinate;//esta es la coordenada original obtenida del xml, es posible que esto se mapee en otro eje, o en otra posici�n

    //la coordenada y
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visualization")
    float Ycoordinate;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visualization")
    float Zcoordinate;
    //el path del archivo
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visualization")
    FString Url;

    //el color que tiene
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visualization")
    int ColorNum;

    //el color que tiene de forma predeterminada com normal, que pdepende y se le asigana, de forma predeterminada sera blanco
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    FLinearColor Color;//este podria cambiar a un arreglo de colores si tengo diferentes formas de clasificar


    //los labels como titulo del documento, y nombre del archivo
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visualization")
    TArray<FString> Labels;

    //id del padre
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visualization")
    int ParentId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visualization")
    ANodo * Parent;

    //id de los hijos
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visualization")
    TArray<int> SonsId;//quiza esto despues se pueda convertir en puntero, para que no necesite acceder al padre u otra cosa

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visualization")
    TArray<ANodo *> Sons;

    //eata es la clase base, no deberia tener componentes importantes aun

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    float Escala;//la escala lo usare en la posicino o escalado en el modo escalado

    //radio de representacion visula del nodo
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    float Radio;

    //Mesh del nodo
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Visualization")
    UStaticMeshComponent * NodoMesh; // debe tener este componente ya que en cualquier tipo de nodo, podre cambiar el color del mesh

    UPROPERTY(VisibleAnywhere, Category = "Visualization")
    USphereComponent * NodoCollision;// el detector de colisiones es diferente en cada nodo por lo tanto se manejan con funciones diferentes, este deberia ser un UShape para poder generalizar, pero por ahora lo dejameros a decision de las subclases

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization", Meta = (BlueprintPublic = "true"))
    UMaterialInstanceDynamic * NodoMaterialDynamic;

    //Cambiar el color del material
    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void CambiarColor(FLinearColor NewColor);

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    virtual void Actualizar();

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Visualization", Meta = (BlueprintPublic = "true"))
    UTextRenderComponent * Nombre;

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    virtual void MostrarNombre();

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    virtual void OcultarNombre();

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Visualization", Meta = (BlueprintPublic = "true"))
    UTextRenderComponent * Numero;

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    virtual void MostrarNumero();

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    virtual void OcultarNumero();

    //para layouts propios

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    float Theta;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    float Phi;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    int Nivel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    int Altura;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    float WInicio;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    float WTam;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    int Hojas;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    int Casilla;//para el layout2

};

//La colision por defecto de esta calse esta como WorldDynamic, sin mebargo le creare su propio perfil llamado Nodo, para rastrear de forma especifica de acruerdo a la necesiadad, y esta coo overlap all dynamic, sin collision physica
//el collision est basicmanete todo en overlap all dynamic, y tod en overlap
//el mesh esta en Block al dynamic, Colision enable, Worldynamic cmo objeto, todo en block