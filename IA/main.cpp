#include <iostream>
#include <fstream>
#include <vector>
using namespace std;


struct Cidade
{
    int id;
    Cidade **vizinhos;
};

struct No
{
    Cidade *cidade;
    No *pai;
    No **filhos;
    bool expandido = false;
    int iter_filhos;
    float valor=0.0;
};

float **distancia;
float **heuristica;
Cidade **cidades;
int n_cidades;

//Verifica se o No ja possui algum ancestral com o dado ID
bool verif_ancestral(No* no, int id)
{
    No* aux = no->pai;
    while(aux!=NULL)
    {
        if(aux->cidade->id == id) return true;
        aux = aux->pai;
    }
    return false;
}

No* gera_filho(No* atual)
{
    No *filho = new No;
    filho->pai = atual;
    filho->iter_filhos=0;
    filho->filhos= new No*[n_cidades];
    filho->cidade = atual->cidade->vizinhos[atual->iter_filhos];
    atual->filhos[atual->iter_filhos] = filho;
    atual->iter_filhos++;
    return filho;
}

void gera_todos(No* atual, bool *fechados)
{
    atual->expandido = true;
    int i=0;
    while(atual->cidade->vizinhos[atual->iter_filhos]!= NULL)
    {
        int next_id = atual->cidade->vizinhos[atual->iter_filhos]->id;
        if(!verif_ancestral(atual,next_id)&& !fechados[next_id])
        {
            No *filho = new No;
            filho->pai = atual;
            filho->iter_filhos=0;
            filho->filhos= new No*[n_cidades];
            for(int i=0; i<n_cidades;i++) filho->filhos[i] = NULL;
            filho->cidade = atual->cidade->vizinhos[atual->iter_filhos];
            filho->valor = atual->valor + distancia[atual->cidade->id][filho->cidade->id];
            atual->filhos[i] = filho;
            i++;
        }
        atual->iter_filhos++;
    }
    atual->iter_filhos=0;

}


No* backtracking(int inicio, int objetivo)
{
    No *raiz = new No;
    raiz->cidade= cidades[inicio];
    raiz->filhos= new No*[n_cidades];
    raiz->pai = NULL;
    raiz->iter_filhos=0;
    No *atual = raiz;
    if(inicio == objetivo) return raiz;

    while(true)
    {
        if(atual->cidade->vizinhos[atual->iter_filhos]!= NULL)
        {
            if(!verif_ancestral(atual,atual->cidade->vizinhos[atual->iter_filhos]->id))
            {

                No *filho = gera_filho(atual);

                if(filho->cidade->id == objetivo)
                    return filho;
                atual = filho;
            }
            else
            {
                atual->iter_filhos++;
            }
        }

        else if(atual == raiz) return NULL;
        else
        {
            No *aux = atual;
            atual = atual->pai;
            delete aux->filhos;
            delete aux;
        }

    }


}

No* profundidade(int inicio, int objetivo)
{
    No *raiz = new No;
    raiz->cidade= cidades[inicio];

    raiz->filhos= new No*[n_cidades];
    for(int i=0; i<n_cidades;i++) raiz->filhos[i] = NULL;

    raiz->pai = NULL;
    raiz->iter_filhos=0;
    No *atual = raiz;

    bool fechados[n_cidades];
    for(int i=0; i<n_cidades;i++) fechados[i] = false;

    if(inicio == objetivo) return raiz;
    while(true)
    {
        if(!atual->expandido)gera_todos(atual,fechados);

        No* filho = atual->filhos[atual->iter_filhos];

        if(filho != NULL)
        {
            atual->iter_filhos++;

            if(filho->cidade->id == objetivo) return filho;

            atual = filho;

        }
        else
        {
            if(atual->pai == NULL) return NULL;

            fechados[atual->cidade->id] = true;
            No* aux = atual;
            atual = atual->pai;
            delete aux->filhos;
            delete aux;

        }

    }



}

No* largura(int inicio, int objetivo)
{
    No *raiz = new No;
    raiz->cidade= cidades[inicio];
    raiz->filhos= new No*[n_cidades];
    for(int i=0; i<n_cidades;i++) raiz->filhos[i] = NULL;
    raiz->pai = NULL;
    raiz->iter_filhos=0;

    if(inicio == objetivo) return raiz;

    No *atual = raiz;
    int tam = 0;
    bool fechados[n_cidades];
    for(int i=0; i<n_cidades;i++) fechados[i] = false;

    vector<No*> fila;


    //Inicia lista com filhos da raiz
    gera_todos(raiz,fechados);
    for(int i=0; i<n_cidades;i++)
    {
        if(atual->filhos[i]!= NULL)
            fila.push_back(atual->filhos[i]);
    }
    tam = fila.size();

    for(int i = 0; i <tam; i++)
    {
        fila[i]->pai->iter_filhos++;
        if(fila[i]->cidade->id == objetivo) return fila[i];
        if(!fila[i]->expandido)gera_todos(fila[i],fechados);
        for(int j=0; j<n_cidades;j++)
            {
                if(fila[i]->filhos[j]!= NULL)
                    fila.push_back(fila[i]->filhos[j]);
            }
        tam = fila.size();

    }
    return NULL;

}


No* ordenada (int inicio, int objetivo)
{
     No *raiz = new No;
    raiz->cidade= cidades[inicio];
    raiz->filhos= new No*[n_cidades];
    for(int i=0; i<n_cidades;i++) raiz->filhos[i] = NULL;
    raiz->pai = NULL;
    raiz->iter_filhos=0;

    if(inicio == objetivo) return raiz;

    No *atual = raiz;
    int tam = 0;
    bool fechados[n_cidades];
    for(int i=0; i<n_cidades;i++) fechados[i] = false;

    vector<No*> fila;


    //Inicia lista com filhos da raiz
    gera_todos(raiz,fechados);
    for(int i=0; i<n_cidades;i++)
    {
        if(atual->filhos[i]!= NULL)
            fila.push_back(atual->filhos[i]);
    }
    tam = fila.size();

    int menor;
    int id;
    while(true)
    {
        if(fila.empty()) return NULL;
        menor = fila[0]->valor;
        id=0;
        for(int i=1; i<tam; i++)
        {
            if(menor > fila[i]->valor)
            {
                menor = fila[i]->valor;
                id = i;

            }

        }

        if(fila[id]->cidade->id == objetivo) return fila[id];

        else
        {

            atual = fila[id];
            if(!atual->expandido)gera_todos(atual,fechados);
            fila.erase(fila.begin()+id);
                for(int i=0; i<n_cidades;i++)
                    {
                        if(atual->filhos[i]!= NULL)
                        fila.push_back(atual->filhos[i]);
                    }
            if(tam == fila.size()) fechados[id] = true;
            else tam = fila.size();
        }


    }
}


No* gulosa (int inicio, int objetivo)
{
     No *raiz = new No;
    raiz->cidade= cidades[inicio];
    raiz->filhos= new No*[n_cidades];
    for(int i=0; i<n_cidades;i++) raiz->filhos[i] = NULL;
    raiz->pai = NULL;
    raiz->iter_filhos=0;

    if(inicio == objetivo) return raiz;

    No *atual = raiz;
    int tam = 0;
    bool fechados[n_cidades];
    for(int i=0; i<n_cidades;i++) fechados[i] = false;

    vector<No*> fila;


    //Inicia lista com filhos da raiz
    gera_todos(raiz,fechados);
    for(int i=0; i<n_cidades;i++)
    {
        if(atual->filhos[i]!= NULL)
            fila.push_back(atual->filhos[i]);
    }
    tam = fila.size();

    float menor;
    int id;
    while(true)
    {
        if(fila.empty()) return NULL;
        menor = heuristica[fila[0]->cidade->id][objetivo];
        id=0;
        for(int i=1; i<tam; i++)
        {
            if(menor > heuristica[fila[i]->cidade->id][objetivo])
            {
                menor = heuristica[fila[i]->cidade->id][objetivo];
                id = i;

            }

        }

        cout<<menor<<endl;
        if(fila[id]->cidade->id == objetivo) return fila[id];

        else
        {

            atual = fila[id];
            if(!atual->expandido)gera_todos(atual,fechados);
            fila.erase(fila.begin()+id);
                for(int i=0; i<n_cidades;i++)
                    {
                        if(atual->filhos[i]!= NULL)
                        fila.push_back(atual->filhos[i]);
                    }
            if(tam == fila.size()) fechados[id] = true;
            else tam = fila.size();
        }


    }
}



No* a_estrela (int inicio, int objetivo)
{
     No *raiz = new No;
    raiz->cidade= cidades[inicio];
    raiz->filhos= new No*[n_cidades];
    for(int i=0; i<n_cidades;i++) raiz->filhos[i] = NULL;
    raiz->pai = NULL;
    raiz->iter_filhos=0;

    if(inicio == objetivo) return raiz;

    No *atual = raiz;
    int tam = 0;
    bool fechados[n_cidades];
    for(int i=0; i<n_cidades;i++) fechados[i] = false;

    vector<No*> fila;


    //Inicia lista com filhos da raiz
    gera_todos(raiz,fechados);
    for(int i=0; i<n_cidades;i++)
    {
        if(atual->filhos[i]!= NULL)
            fila.push_back(atual->filhos[i]);
    }
    tam = fila.size();

    float menor;
    int id;
    while(true)
    {
        if(fila.empty()) return NULL;
        menor = heuristica[fila[0]->cidade->id][objetivo]+fila[0]->valor;
        id=0;
        for(int i=1; i<tam; i++)
        {

            if(menor > heuristica[fila[i]->cidade->id][objetivo]+ fila[i]->valor)
            {
                menor = heuristica[fila[i]->cidade->id][objetivo] + fila[i]->valor;
                id = i;


            }

        }
        if(fila[id]->cidade->id == objetivo) return fila[id];

        else
        {

            atual = fila[id];
            if(!atual->expandido)gera_todos(atual,fechados);
            fila.erase(fila.begin()+id);
                for(int i=0; i<n_cidades;i++)
                    {
                        if(atual->filhos[i]!= NULL)
                        fila.push_back(atual->filhos[i]);
                    }
            if(tam == fila.size()) fechados[id] = true;
            else tam = fila.size();
        }


    }
}







int main()
{

    fstream file;
    file.open("dist.txt", std::ios::in | std::ios::out);
    file.clear();
    file.seekg(0, std::ios::beg);
    //Get n_cidades;
    file>>n_cidades;
    //Cria vetor de cidades
    cidades = new Cidade*[n_cidades];
    for(int i=0; i<n_cidades;i++)
    {
        cidades[i] = new Cidade;
        cidades[i]->id = i;
        cidades[i]->vizinhos = new Cidade*[n_cidades];
        for(int j=0; j<n_cidades;j++)
            cidades[i]->vizinhos[j] = NULL;

    }

    //Enche matriz
    distancia = new float* [n_cidades];
    for(int i=0; i<n_cidades;i++)
        distancia[i] = new float [n_cidades];
    int iter;
    for(int i=0; i<n_cidades;i++)
    {
        iter=0;
        for(int j=0; j<n_cidades;j++)
          {
            file>>distancia[i][j];
            if(distancia[i][j] > 0)
                {
                    cidades[i]->vizinhos[iter] = cidades[j];
                    iter++;
                }
          }
    }

    //Enche heuristica
    heuristica = new float* [n_cidades];
    for(int i=0; i<n_cidades;i++)
        heuristica[i] = new float [n_cidades];

    for(int i=0; i<n_cidades;i++)
    {
        for(int j=0; j<n_cidades;j++)
            file>>heuristica[i][j];
    }


  /*  No* result = largura(7,6);
    float soma = 0;
    int id = result->cidade->id;
    while(result!=NULL)
    {
        cout<<result->cidade->id<<" ";
        soma+= distancia[id][result->cidade->id];
        id = result->cidade->id;
        if(result->iter_filhos==0) result = NULL;
        else
        result = result->filhos[result->iter_filhos-1];

    }
    cout<<soma;*/

    No* result = a_estrela(7,6);
    float soma = 0;
    while(result->pai!=NULL)
    {
        cout<<result->cidade->id<<" <- ";
        soma+= distancia[result->pai->cidade->id][result->cidade->id];
        result = result->pai;
    }
    cout<<result->cidade->id<<" Total:";
    cout<<soma;




    return 0;
}
