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
                    return raiz;
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

            if(filho->cidade->id == objetivo) return raiz;

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
        if(fila[i]->cidade->id == objetivo) return raiz;
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


    No* result = largura(7,6);
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
    cout<<soma;


    return 0;
}
