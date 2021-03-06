// Fill out your copyright notice in the Description page of Project Settings.

#include "NJVR.h"
#include "VRVisualization.h"
#include "Nodo.h"
#include "Arista.h"
#include "VRPawn.h"
#include "MotionControllerComponent.h"
#include "Kismet/GameplayStatics.h"
#include <queue>

// Sets default values
AVRVisualization::AVRVisualization()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));

    Escala = 1.0f;
    RadioNodos = 3.0f;
    RadioNodosVirtuales = 2.0f;
    RadioAristas = 1.0f;
    DistanciaLaserMaxima = 500.0f;
    DistanciaLaser = DistanciaLaserMaxima;
    //static ConstructorHelpers::FClassFinder<ANodo> NodoClass(TEXT("/Script/NJVR.Nodo"));
    //static ConstructorHelpers::FClassFinder<ANodo> NodoClass(TEXT("Class'/Script/NJVR.NodoEsfera'"));
    static ConstructorHelpers::FClassFinder<ANodo> NodoClass(TEXT("BlueprintGeneratedClass'/Game/Visualization/Blueprints/Elements/NodoEsfera_BP.NodoEsfera_BP_C'"));
    if (NodoClass.Succeeded()) {
        if(GEngine)//no hacer esta verificación provocaba error al iniciar el editor
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("TipoNodo encontrado."));
        TipoNodo = NodoClass.Class;
    }

    //Para buscar la clase arista que sera default en este codigo
    //static ConstructorHelpers::FClassFinder<AArista> AristaClass(TEXT("/Script/NJVR.Arista"));
    //static ConstructorHelpers::FClassFinder<AArista> AristaClass(TEXT("Class'/Script/NJVR.AristaCilindro'"));
    static ConstructorHelpers::FClassFinder<AArista> AristaClass(TEXT("BlueprintGeneratedClass'/Game/Visualization/Blueprints/Elements/AristaCilindro_BP.AristaCilindro_BP_C'"));
    if (AristaClass.Succeeded()) {
        if(GEngine)//no hacer esta verificación provocaba error al iniciar el editor
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("TipoArista encontrado."));
        TipoArista = AristaClass.Class;
    }

    IntensidadColor = 0.6f;
    ColorSeleccion = FLinearColor::Green;
    ColorVirtual = FLinearColor::White;
    SetVisualizationMode(EVRVisualizationMode::ENoMode);// como inicia en ste mdo deberia parecer marcado, el boton correspondiente,
    SetVisualizationTask(EVRVisualizationTask::ENoTask);//esta bien que empiece en ninguno, asi ningun boton tarea estara marcado
    DataSets.Add(FString("D:/UnrealProjects/NJVR/Content/Resources/cbr-ilp-ir-son.xml"));
    DataSets.Add(FString("D:/UnrealProjects/NJVR/Content/Resources/Sincbr-ilp-ir.xml"));
    DataSets.Add(FString("D:/UnrealProjects/NJVR/Content/Resources/imagensCorel.xml"));
    DataSets.Add(FString("D:/UnrealProjects/NJVR/Content/Resources/SinimagensCorel.xml"));
    DataSetSeleccionado = 0;
}

// Called when the game starts or when spawned
void AVRVisualization::BeginPlay()
{
	Super::BeginPlay();

    AVRPawn * MyVRPawn = Cast<AVRPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
    if (MyVRPawn) {
        RightController = MyVRPawn->MotionControllerRight;
        LeftController = MyVRPawn->MotionControllerLeft;
        MyVRPawn->Visualization = this;
        Interaction = MyVRPawn->Interaction;
        Document = MyVRPawn->Document;
        Usuario = MyVRPawn;
    }
    //FString path = FString("D:\\UnrealProjects\\NJVR\\Content\\Resources\\cbr-ilp-ir-son.xml");//de esta forma funciona
    //Este funciona FString path = FString("D:/UnrealProjects/NJVR/Content/Resources/cbr-ilp-ir-son.xml");//de esta forma tambien funciona
    //FString path = FString("D:/UnrealProjects/NJVR/Content/Resources/Sincbr-ilp-ir.xml");//de esta forma tambien funciona
    //FString path = FString("D:/UnrealProjects/NJVR/Content/Resources/imagensCorel.xml");//de esta forma tambien funciona
    //FString path = FString("D:/UnrealProjects/NJVR/Content/Resources/SinimagensCorel.xml");//de esta forma tambien funciona
    //FString path = FString("/Game/Resources/cbr-ilp-ir-son.xml");//esto no funciona
    //bool cargado = XmlSource.LoadFile(path, EConstructMethod::ConstructFromFile);//para construirlo como archivo
    bool cargado = XmlSource.LoadFile(DataSets[DataSetSeleccionado], EConstructMethod::ConstructFromFile);//para construirlo como archivo
    if (GEngine) {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Probando."));
        if(cargado)
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Cargado."));
        else
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("No cargado."));
    }
    if (cargado) {
        //LoadNodos();
        CreateNodos();
        CreateAristas();
    }

	
}

// Called every frame
void AVRVisualization::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AVRVisualization::LoadNodos() {
    FXmlNode * rootnode = XmlSource.GetRootNode();
    if (GEngine) {
        //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, rootnode->GetTag());
    }
    TArray<FXmlNode *> XMLnodos = rootnode->GetChildrenNodes();
    TArray<FXmlNode*> XMLvertexs;
    //for(FXmlNode* nodo : nodos){
        //if (nodo->GetTag() == "vertex") {
            //vertexs.Add(nodo);
        //}
    for (int i = 0; i < XMLnodos.Num(); ++i) {
        if (XMLnodos[i]->GetTag() == "vertex") {
            XMLvertexs.Add(XMLnodos[i]);
        }
    }
    //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::FromInt(vertexs.Num()));
    //tengo todos los vertices en ese array
    for (int i = 0; i < XMLvertexs.Num(); ++i) {
        //obteniendo el id
        FString id = XMLvertexs[i]->GetAttribute(FString("id"));//devuelve el valor del atributo que le doy, hay otra funocin que me devuelve todos los atributos en un arrya de un obejto especial//quiza deba esto guardarlo como int cuando genere la clase Vertex
        //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, id);
        UE_LOG(LogClass, Log, TEXT("Vertex id = %s"), *id);
        
        //obteniendo el valid
        FXmlNode * nodevalid = XMLvertexs[i]->FindChildNode(FString("valid"));
        FString valid = nodevalid->GetAttribute("value");
        //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, valid);
        UE_LOG(LogClass, Log, TEXT("valid = %s"), *valid);

        //obteniendo la x-coordinate
        FXmlNode * nodex = XMLvertexs[i]->FindChildNode(FString("x-coordinate"));
        FString xcoordinate = nodex->GetAttribute("value");
        //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, xcoordinate);
        UE_LOG(LogClass, Log, TEXT("x-coordinate = %s"), *xcoordinate);

        //obteniendo la y-coordinate
        FXmlNode * nodey = XMLvertexs[i]->FindChildNode(FString("y-coordinate"));
        FString ycoordinate = nodey->GetAttribute("value");
        //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, ycoordinate);
        UE_LOG(LogClass, Log, TEXT("y-coordinate = %s"), *ycoordinate);

        //obteniendo url
        FXmlNode * nodeurl = XMLvertexs[i]->FindChildNode(FString("url"));
        FString url = nodeurl->GetAttribute("value");
        //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, url);
        UE_LOG(LogClass, Log, TEXT("url = %s"), *url);

        //obteniendo parent 
        FXmlNode * nodeparent = XMLvertexs[i]->FindChildNode(FString("parent"));//quiza no sean necesario usar FString
        FString parent = nodeparent->GetAttribute("value");
        //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, parent);
        UE_LOG(LogClass, Log, TEXT("parent = %s"), *parent);

        //los hijos no estan dentro de un array por lo tanto es necesario reccorrer todos los child nose, es decir lo de aqui arruba fue por las puras, jeje
        TArray<FString> sons;
        TArray<FXmlNode*> childs = XMLvertexs[i]->GetChildrenNodes();
        for (int j = 0; j < childs.Num(); j++) {
            if (childs[j]->GetTag() == "son") {
                sons.Add(childs[j]->GetAttribute("value"));
            }
        }
        for (int j = 0; j < sons.Num(); j++) {
            //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, sons[j]);
            UE_LOG(LogClass, Log, TEXT("son = %s"), *sons[j]);
        }

        FXmlNode * nodelabels = XMLvertexs[i]->FindChildNode(FString("labels"));//quiza no sean necesario usar FString
        TArray<FXmlNode*> labelschilds = nodelabels->GetChildrenNodes();
        TArray<FString> labels;
        for (int j = 0; j < labelschilds.Num(); j++) {
            labels.Add(labelschilds[j]->GetAttribute("value"));
            //aqui faltaria definir que label es cada uno, para poder ponerlo en la variable que corresponda en el la calse vertex que creare
        }
        for (int j = 0; j < labels.Num(); j++) {
            //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, labels[j]);
            UE_LOG(LogClass, Log, TEXT("label = %s"), *labels[j]);
        }
        //el contenido de los nodos, es lo que hay en trexto plano dentro del tag de apertura y de cierre

        //TArray<FXmlNode*> childs = vertexs[i]->GetChildrenNodes();//para el caso de los vertexs sus hijos son unicos o son un array por lo tanto podria usar la funcion findchildren, para encontrar los que necesito

    }
}

void AVRVisualization::CreateNodos() {

}

void AVRVisualization::CreateAristas() {

}

int AVRVisualization::NivelMasDenso() {//de todo el arbol
    int NivelDensoMaximo = 0;
    int CantidadNodosMaximo = 0;
    int CantidadNodosNivelActual = 0;
    int NivelActual = 1;
    std::queue<ANodo *> Cola;
    //la raiz es el ultimo nodo
    ANodo * Root = Nodos[Nodos.Num() - 1];
    //UE_LOG(LogClass, Log, TEXT("Root id = %d, (%f,%f,%f)"), Root->Id, Root->Xcoordinate, Root->Ycoordinate, Root->Zcoordinate);
    Cola.push(Root->Parent);
    for (int i = 0; i < Root->Sons.Num(); i++) {
        Cola.push(Root->Sons[i]);
    }
    CantidadNodosNivelActual = Cola.size();
    if (CantidadNodosNivelActual >= CantidadNodosMaximo) {// deberia camniar si estoy ma abajo o arriba??
        CantidadNodosMaximo = CantidadNodosNivelActual;
        NivelDensoMaximo = NivelActual;
    }
    NivelActual++;//iniciaria el bucle con 2, pero los nodos ubicados en la cola son de nivel 1
    while(!Cola.empty()){
        while (!Cola.empty() && Cola.front()->Nivel < NivelActual) {
            ANodo * V;
            V = Cola.front();
            Cola.pop();
            for (int i = 0; i < V->Sons.Num(); i++) {
                Cola.push(V->Sons[i]);
            }
        }
        CantidadNodosNivelActual = Cola.size();
        if (CantidadNodosNivelActual >= CantidadNodosMaximo) {// deberia camniar si estoy ma abajo o arriba??
            CantidadNodosMaximo = CantidadNodosNivelActual;
            NivelDensoMaximo = NivelActual;
        }
        NivelActual++;
    }
    return NivelDensoMaximo;
}

void AVRVisualization::NivelMasDenso(int & NivelDenso, int & CantidadNodos) {
    int NivelDensoMaximo = 0;
    int CantidadNodosMaximo = 0;
    int CantidadNodosNivelActual = 0;
    int NivelActual = 1;
    std::queue<ANodo *> Cola;
    //la raiz es el ultimo nodo
    ANodo * Root = Nodos[Nodos.Num() - 1];
    //UE_LOG(LogClass, Log, TEXT("Root id = %d, (%f,%f,%f)"), Root->Id, Root->Xcoordinate, Root->Ycoordinate, Root->Zcoordinate);
    Cola.push(Root->Parent);
    for (int i = 0; i < Root->Sons.Num(); i++) {
        Cola.push(Root->Sons[i]);
    }
    CantidadNodosNivelActual = Cola.size();
    if (CantidadNodosNivelActual >= CantidadNodosMaximo) {// deberia camniar si estoy ma abajo o arriba??
        CantidadNodosMaximo = CantidadNodosNivelActual;
        NivelDensoMaximo = NivelActual;
    }
    NivelActual++;//iniciaria el bucle con 2, pero los nodos ubicados en la cola son de nivel 1
    while(!Cola.empty()){
        while (!Cola.empty() && Cola.front()->Nivel < NivelActual) {
            ANodo * V;
            V = Cola.front();
            Cola.pop();
            for (int i = 0; i < V->Sons.Num(); i++) {
                Cola.push(V->Sons[i]);
            }
        }
        CantidadNodosNivelActual = Cola.size();
        if (CantidadNodosNivelActual >= CantidadNodosMaximo) {// deberia camniar si estoy ma abajo o arriba??
            CantidadNodosMaximo = CantidadNodosNivelActual;
            NivelDensoMaximo = NivelActual;
        }
        NivelActual++;
    }
    NivelDenso = NivelDensoMaximo;
    CantidadNodos = CantidadNodosMaximo;
}

int AVRVisualization::NivelMasDensoRama(ANodo * Nodo) {//de todo el arbol, deeria tomarme en cuenta, osea el nodo inicial,
    int NivelDensoMaximo = Nodo->Nivel;
    int CantidadNodosMaximo = 1;
    int CantidadNodosNivelActual = 1;
    int NivelActual = Nodo->Nivel + 1;//estaba solo 1
    std::queue<ANodo *> Cola;
    Cola.push(Nodo);
    while(!Cola.empty()){
        while (!Cola.empty() && Cola.front()->Nivel < NivelActual) {
            ANodo * V;
            V = Cola.front();
            Cola.pop();
            for (int i = 0; i < V->Sons.Num(); i++) {
                Cola.push(V->Sons[i]);
            }
        }
        CantidadNodosNivelActual = Cola.size();
        if (CantidadNodosNivelActual >= CantidadNodosMaximo) {// deberia camniar si estoy ma abajo o arriba??
            CantidadNodosMaximo = CantidadNodosNivelActual;
            NivelDensoMaximo = NivelActual;
        }
        NivelActual++;
    }
    return NivelDensoMaximo;//el valor de retorno es respecto al arbol general, no de forma relativba a la rama
}

EVRVisualizationTask AVRVisualization::GetVisualizationTask() {
    return CurrentVisualizationTask;
}

void AVRVisualization::SetVisualizationTask(EVRVisualizationTask NewVisualizationTask) {
    CurrentVisualizationTask = NewVisualizationTask;
}

EVRVisualizationMode AVRVisualization::GetVisualizationMode() {
    return CurrentVisualizationMode;
}

void AVRVisualization::SetVisualizationMode(EVRVisualizationMode NewVisualizationMode) {
    CurrentVisualizationMode = NewVisualizationMode;
}

//algunas de destas funciones son la misma para todos asl visualizaciones, otras no, esas sobrescribir las que estan aqui
void AVRVisualization::SeleccionarNodo(ANodo * NodoSeleccionado) {
    NodoSeleccionado->Selected = true;
    NodoSeleccionado->CambiarColor(ColorSeleccion);
    NodosSeleccionados.Add(NodoSeleccionado);
}
void AVRVisualization::DeseleccionarNodo(ANodo * NodoSeleccionado) {
    NodoSeleccionado->Selected = false;
    NodoSeleccionado->CambiarColor(NodoSeleccionado->Color);
    NodosSeleccionados.Remove(NodoSeleccionado);
}

void AVRVisualization::SeleccionarRama(ANodo * NodoSeleccionado) {
    TQueue<ANodo*> Cola;
    Cola.Enqueue(NodoSeleccionado);
    while (!Cola.IsEmpty()) {
        Cola.Dequeue(NodoSeleccionado);
        NodoSeleccionado->Selected = true;
        NodoSeleccionado->CambiarColor(ColorSeleccion);
        NodosSeleccionados.Add(NodoSeleccionado);
        for (int i = 0; i < NodoSeleccionado->Sons.Num(); i++) {
            Cola.Enqueue(NodoSeleccionado->Sons[i]);
        }
    }
}

void AVRVisualization::DeseleccionarRama(ANodo * NodoSeleccionado) {
    TQueue<ANodo*> Cola;
    Cola.Enqueue(NodoSeleccionado);
    while (!Cola.IsEmpty()) {
        Cola.Dequeue(NodoSeleccionado);
        NodoSeleccionado->Selected = false;
        NodoSeleccionado->CambiarColor(NodoSeleccionado->Color);
        NodosSeleccionados.Remove(NodoSeleccionado);
        for (int i = 0; i < NodoSeleccionado->Sons.Num(); i++) {
            Cola.Enqueue(NodoSeleccionado->Sons[i]);
        }
    }
}

void AVRVisualization::SeleccionarTodo() {
    NodosSeleccionados = Nodos;// esta accion borra todos los elementos y le copia los elementos del segundo array //pero ahora debo marcarlos y 
    for (int i = 0; i < NodosSeleccionados.Num(); i++) {
        NodosSeleccionados[i]->Selected = true;
        NodosSeleccionados[i]->CambiarColor(ColorSeleccion);
    }
}

void AVRVisualization::DeseleccionarTodo() {
    for (int i = 0; i < NodosSeleccionados.Num(); i++) {
        NodosSeleccionados[i]->Selected = false;
        NodosSeleccionados[i]->CambiarColor(NodosSeleccionados[i]->Color);
    }
    NodosSeleccionados.Empty();
}

void AVRVisualization::AplicarTraslacion(FVector Traslacion) {

}

/*void AVRVisualization::AplicarEscala(float NuevaEscala) {
    for (int i = 0; i < Nodos.Num(); i++) {
        Nodos[i]->Escala = NuevaEscala;
        Nodos[i]->Actualizar();//este deberia actualziar tal vez también la posicion del nodo, o calcularlo afuera
        FTransform NewTransform = Nodos[i]->GetActorTransform();
        //NewTransform.SetLocation(Nodos[i]->GetTransform().GetLocation() * NuevaEscala);
        NewTransform.SetLocation(PosicionesNodosInicialEscala[i] * NuevaEscala);
        Nodos[i]->SetActorTransform(NewTransform);
    }
    for (int i = 0; i < Aristas.Num(); i++) {
        Aristas[i]->Escala = NuevaEscala;
        Aristas[i]->Actualizar();
    }
}*/
//Hya dos formas de aplicar la escala, una seria la que esta expresada, que los elemnetos y sus posiciones esten escalados respecto a esta clase
//la otra es que se escale esta clase, como los nodos y las aristas estan dentro, como hijos, tambien deberian ser escalados, eso evita mucho calculo y correciones
void AVRVisualization::AplicarEscala(float NuevaEscala) {
    //esta forma es menos costosa, y parece mas sencialla de manipular todos los elementos, solo hya que hacer que todo tenga concordancia, no como la arista actual
    SetActorScale3D(FVector(NuevaEscala));
    //FTransform NuevoTransform = GetTransform();
    //NuevoTransform.SetScale3D(FVector(NuevaEscala));
    //SetActorTransform(NuevoTransform);
}

void AVRVisualization::AplicarRotacionRelativaANodo(ANodo* NodoReferencia, FVector PuntoReferencia) {

}

FString AVRVisualization::ObtenerContenidoNodo(ANodo* Nodo) {
    FString contenido;
    FString archivo("D:/UnrealProjects/NJVR/Content/Resources/cbr-ilp-ir-son/");
    archivo += Nodo->Url;
    FFileHelper::LoadFileToString(contenido, *archivo);
    return contenido;
}

void AVRVisualization::MostrarNumeracion() {
    for (int i = 0; i < Nodos.Num(); i++)
        Nodos[i]->Numero->SetVisibility(true);

}

void AVRVisualization::OcultarNumeracion() {
    for (int i = 0; i < Nodos.Num(); i++)
        Nodos[i]->Numero->SetVisibility(false);
}

void AVRVisualization::RotarVisualizacion() {
    FRotator DeltaRotation = RightController->GetComponentRotation() - InitialRotationController;
    SetActorRotation(InitialRotation + DeltaRotation);
}

void AVRVisualization::CalcularEscalaTemporal() {
    float DistanciaEntreControles = (RightController->GetComponentLocation() - LeftController->GetComponentLocation()).Size();
    EscalaTemp = Escala + (DistanciaEntreControles - DistanciaInicialControles) / DistanciaInicialControles;
}

FVector AVRVisualization::BuscarNodo(ANodo * &NodoEncontrado) {//en realidad dbe hacer asignaciones, o poner null si no encuentra nada
    FCollisionQueryParams NodoTraceParams = FCollisionQueryParams(FName(TEXT("TraceNodo")), true, this);
    FVector PuntoInicial = RightController->GetComponentLocation();//lo mismo que en teorioa, GetComponentTransfor().GetLocation();
    FVector Vec = RightController->GetForwardVector();
    FVector PuntoFinal = PuntoInicial + Vec*DistanciaLaserMaxima;
    //PuntoInical = PuntoInicial + Vec * 10;//para que no se choque con lo que quiero, aun que no deberia importar
    TArray<TEnumAsByte<EObjectTypeQuery> > TiposObjetos;
    TiposObjetos.Add(EObjectTypeQuery::ObjectTypeQuery8);//Nodo
    //TiposObjetos.Add(EObjectTypeQuery::ObjectTypeQuery2);//World dynamic, separado esta funcionando bien, supongo que tendre que hacer oto trace par saber si me estoy chocando con la interfaz, y no tener encuenta esta busqueda
    //podria agregar los world static y dynamic, para asi avitar siempre encontrar algun nodo que este destrar de algun menu, y que por seleccionar en el menu tambien le de click a el
    TArray<AActor*> vacio;
    FHitResult Hit;
    bool trace = UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), PuntoInicial, PuntoFinal, TiposObjetos, false, vacio, EDrawDebugTrace::None, Hit, true, FLinearColor::Blue);//el none es para que no se dibuje nada
    //hit se supone que devovera al actor y el punto de impacto si encontró algo, castearlo a nodo, y listo
    if (trace) {
        //solo que al agregar el worldynamic ,tengo que castear y verificar
        NodoEncontrado = Cast<ANodo>(Hit.Actor.Get());
        /*if (NodoEncontrado) {//no estaba
            //en que momento debo incluir la seccion del label? despues de todo esto, en otra funcion, o en este mismo codigo?
            return Hit.ImpactPoint;
        }*/
        //DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 1.5f, 6, FColor::Black, false, 0.0f, 0, 0.25f);
        
        return Hit.ImpactPoint;//si quito toto el if anterior debo activar esta linea, y liesto
    }
    //y si esta funcion es solo para esto, y luego ya verifico si es tal o cual cosa, en otra parte del codigo?

    //DrawDebugLine(GetWorld(), SourceNodo->GetActorLocation(), TargetNodo->GetActorLocation(), FColor::Black, false, -1.0f, 0, Radio*Escala);
    NodoEncontrado = nullptr;
    return FVector::ZeroVector;// los casos manejarlos afuera
}

void AVRVisualization::BuscandoNodoConLaser() {
    ANodo * NodoEncontrado;
    FVector PuntoImpacto = BuscarNodo(NodoEncontrado);
    if (NodoEncontrado && !Interaction->IsOverHitTestVisibleWidget()) {//comprobamos la interaccion para que no se detecte lo que este detras del menu
        if (HitNodo && HitNodo != NodoEncontrado) {//quiza se pueda hacer con el boleano, debo ocultar si es que es diferente al de ahora,
            if (MostrarLabel) {
                HitNodo->OcultarNombre();
                NodoEncontrado->MostrarNombre();
            }
            //HitNodo = NodoEncontrado; bHitNodo = true; ImpactPoint = PuntoImpacto;
        }
        else {
            if (MostrarLabel) {
                NodoEncontrado->MostrarNombre();
            }
            //HitNodo = NodoEncontrado; bHitNodo = true; ImpactPoint = PuntoImpacto;
        }
        HitNodo = NodoEncontrado;//podria dejar ests 3 lineas, y borrar las de adentro
        bHitNodo = true;
        ImpactPoint = PuntoImpacto;
    }
    else {//si no estoy golpeando algun nodo
        if (HitNodo) {
            if (MostrarLabel) {
                HitNodo->OcultarNombre();
            }
            HitNodo = nullptr;
            bHitNodo = false;
            ImpactPoint = PuntoImpacto;
        }
        //el caso contrario, seria encontrar como lo deje con el if anterior, asi que no se hace nada
    }
    //todo esto podria ser una sola funcion
    //hasta aqui he verificado si encontre algun nodo, pero no si encotnre un, menu, y tampoo he ejecutado los cambios visuales
    //hagamos algo visual, antes de incluir los menus
    if (bHitNodo) {//quiza la verificacion que hago sobre si hubo cambio o no de hit nodo, ayude a evitar ciertos calculos, tal vez, por ejemplo si el laser siempre esa al maximo, no tiene mucho sentido seimpre setear con el mismo valor
        Usuario->CambiarPuntoFinal(ImpactPoint);
    }
    else {
        Usuario->CambiarPuntoFinal(RightController->GetComponentLocation() + RightController->GetForwardVector()*DistanciaLaserMaxima);//debieria tener un punto por defecto, pero mejor lo dejamos asi
        //esta funcion deberia administrar le punto recbido, y verficar si acutalmente el puntero de interaccion esta sobre el menu, y tomar el adecuado para cada situacion
    }
    //creo que la parte de interacion con el menu, deberia estar manajedo por el pawn, asi dentro de la funcion cambiar punto final, evaluo o verifico que no este primero algun menu
    //la pregunta es como hare con los clicks digamos para el contenido, si estoy buscando algun nodo, quiza igual deberia evitar que de algun click, si tengo algun overlap en ferente, evaluar la mejor forma de hacer todo esto
    //o usar esto en lugar de un trace solo que debo hacer esto antes de que haga cambios visuales, obtener el punto y evaluar,  antes de setear lo de hit nodo y dema
}

FVector AVRVisualization::InterseccionLineaSuperficie() {
    return FVector();
}

void AVRVisualization::TraslacionConNodoGuia() {

}

void AVRVisualization::TraslacionVisualizacion() {//esta es una funcion gloabl, ponerlo en la clase padre, analizar estas cosas
    FVector PuntoInicial = RightController->GetComponentLocation();//lo mismo que en teorioa, GetComponentTransfor().GetLocation();
    FVector Vec = RightController->GetForwardVector();
    FVector PuntoFinal = PuntoInicial + Vec*DistanciaLaser;
    if(Usuario->LaserActual() != 6){
        Usuario->CambiarLaser(6);
    }
    Usuario->CambiarPuntoFinal(PuntoFinal);
    SetActorLocation(PuntoFinal + OffsetToPointLaser);
}

void AVRVisualization::TrasladarNodoPressed() {//esta es una funcion gloabl, ponerlo en la clase padre, analizar estas cosas
    if (bHitNodo) {
        SeleccionarNodo(HitNodo);
        NodoGuia = HitNodo;
        bNodoGuia = true;
    }
}

void AVRVisualization::VisualizarNodoPressed() {//esta es una funcion gloabl, ponerlo en la clase padre, analizar estas cosas
    if (bHitNodo) {
        SeleccionarNodo(HitNodo);
        HitNodo->MostrarNombre();
        //Cast<UControlMenu2VR>(Document->GetUserWidgetObject());
    }
}

void AVRVisualization::TrasladarRamaPressed() {//esta es una funcion gloabl, ponerlo en la clase padre, analizar estas cosas
    if (bHitNodo) {
        SeleccionarRama(HitNodo);
        NodoGuia = HitNodo;
        bNodoGuia = true;
    }
}

void AVRVisualization::RotarRamaPressed() {//esta es una funcion gloabl, ponerlo en la clase padre, analizar estas cosas
    if (bHitNodo) {
        SeleccionarRama(HitNodo);
        NodoGuia = HitNodo;
        bNodoGuia = true;
    }
}

void AVRVisualization::TrasladarTodoPressed() {//esta es una funcion gloabl, ponerlo en la clase padre, analizar estas cosas
    SeleccionarTodo();
    if (bHitNodo) {
        NodoGuia = HitNodo;
        bNodoGuia = true;
    }
}

void AVRVisualization::TrasladarVisualizacionPressed() {//esta es una funcion gloabl, ponerlo en la clase padre, analizar estas cosas
    if(bHitNodo){
        DistanciaLaser = (ImpactPoint - RightController->GetComponentLocation()).Size();
        OffsetToPointLaser = GetActorLocation() - ImpactPoint;
        bNodoGuia = true;
    }
}

void AVRVisualization::RotarVisualizacionPressed() {//esta es una funcion gloabl, ponerlo en la clase padre, analizar estas cosas
    InitialRotationController = RightController->GetComponentRotation();
    InitialRotation = GetActorRotation();
    Rotar = true;
}

void AVRVisualization::TrasladarNodoReleased() {//esta es una funcion gloabl, ponerlo en la clase padre, analizar estas cosas
    DeseleccionarTodo();
    bNodoGuia = false;
    for (int i = 0; i < Aristas.Num(); i++) {
        Aristas[i]->ActualizarCollision();
    }
    Usuario->CambiarLaser(0);
    //Usuario->CambiarPuntoFinal(GetTransform().TransformPosition(ImpactPoint));
}

void AVRVisualization::VisualizarNodoReleased() {//esta es una funcion gloabl, ponerlo en la clase padre, analizar estas cosas
    for (int i = 0; i < NodosSeleccionados.Num(); i++) {
        NodosSeleccionados[i]->OcultarNombre();
    }
    DeseleccionarTodo();
    Usuario->CambiarLaser(0);
}

void AVRVisualization::TrasladarRamaReleased() {//esta es una funcion gloabl, ponerlo en la clase padre, analizar estas cosas
    DeseleccionarRama(NodoGuia);//con el deseleccionar todo tambien bastaria
    bNodoGuia = false;
    for (int i = 0; i < Aristas.Num(); i++) {
        Aristas[i]->ActualizarCollision();
    }
    Usuario->CambiarLaser(0);
}

void AVRVisualization::RotarRamaReleased() {//esta es una funcion gloabl, ponerlo en la clase padre, analizar estas cosas
    bNodoGuia = false;
    for (int i = 0; i < Aristas.Num(); i++) {
        Aristas[i]->ActualizarCollision();
    }
    DeseleccionarTodo();
    Usuario->CambiarLaser(0);
}

void AVRVisualization::TrasladarVisualizacionReleased() {//esta es una funcion gloabl, ponerlo en la clase padre, analizar estas cosas
    bNodoGuia = false;
    Usuario->CambiarLaser(0);
}

void AVRVisualization::RotarVisualizacionReleased() {//esta es una funcion gloabl, ponerlo en la clase padre, analizar estas cosas
    Rotar = false;
    Usuario->CambiarLaser(0);
}

void AVRVisualization::TrasladarTodoReleased() {//esta es una funcion gloabl, ponerlo en la clase padre, analizar estas cosas
    DeseleccionarTodo();
    bNodoGuia = false;
    Usuario->CambiarLaser(0);
}

int AVRVisualization::mod(int a, int b) {
    int d = a / b;
    int m = a - b*d;
    if (m < 0)
        m += b;
    return m;
}

float AVRVisualization::modFloat(float a, float b) {
    //por ahora es solo para el exceso en resta, para el esferico nunca exceden el doblre asi que sera solo la suma para hacerlo positivo
    if (a > b) {
        return a - b;
    }
    if (a < 0) {
        return a + b;
    }
    return a;
}