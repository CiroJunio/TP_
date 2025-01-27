#include "../include/arvore_b_star.h"

void InsereNaPaginaEstrela(TipoApontador2 Ap,int Reg,TipoApontador2 Apdir,int *comparacoes){
  int k;//insere na pagina indice
  //printf("%d \n",Reg);
      k=Ap->UU.U0.ni;

      short NaoAchouPosicao=0;
      if(k>0)
          NaoAchouPosicao=1;
      while(NaoAchouPosicao){
          (*comparacoes)++;
          if(Reg >= Ap->UU.U0.ri[k-1]){
              NaoAchouPosicao=0;
              break;
          }
          Ap->UU.U0.ri[k]=Ap->UU.U0.ri[k-1];
          Ap->UU.U0.pi[k+1]=Ap->UU.U0.pi[k];
          k--;
          if(k<1)
              NaoAchouPosicao=0;
      }
      Ap->UU.U0.ri[k]=Reg;//insere a chave
      Ap->UU.U0.pi[k+1]=Apdir;//atualiza o ponteiro a direita do novo item
      Ap->UU.U0.ni++;

}

void InsereNaPaginaFolhaEstrela(TipoApontador2 Ap,tipoitem Reg,TipoApontador2 Apdir,int *comparacoes){
  int k;//insere na pagina externa

      k=Ap->UU.U1.ne;
      short NaoAchouPosicao=0;
      if(k>0)
          NaoAchouPosicao=1;
      while(NaoAchouPosicao){
          (*comparacoes)++;
          if(Reg.chave >= Ap->UU.U1.re[k-1].chave){
              NaoAchouPosicao=0;
              break;
          }
          Ap->UU.U1.re[k]=Ap->UU.U1.re[k-1];
          //Ap->p[k+1]=Ap->p[k];
          k--;
          if(k<1)
              NaoAchouPosicao=0;
      }
      Ap->UU.U1.re[k]=Reg;//insere todo o registro
      //Ap->p[k+1]=Apdir;//nao tem ponteiros
      Ap->UU.U1.ne++;


}

void InsEstrela(tipoitem Reg,TipoApontador2 Ap,short *Cresceu,tipoitem *RegRetorno,TipoApontador2 *ApRetorno,int *comparacoes){
  long i=1;

  TipoApontador2 ApTemp;
  //como ja foi criada a raiz, e as folhas nao tem apontadores nao precisa da verificacao do Ap==NULL
  if(Ap->Pt == Interna){//enccontra pagina externa ideal pra inserçao
      while(i < Ap->UU.U0.ni && Reg.chave > Ap->UU.U0.ri[i-1]){
          (*comparacoes)++;
          i++;
      }


      (*comparacoes)++;
      if(Reg.chave < Ap->UU.U0.ri[i-1])
          i--;

      InsEstrela(Reg,Ap->UU.U0.pi[i],Cresceu,RegRetorno,ApRetorno,comparacoes);
  }
  else{
      while(i < Ap->UU.U1.ne && Reg.chave > Ap->UU.U1.re[i-1].chave){//quando na pagina externa precisa decobrir a posicao
          (*comparacoes)++;
          i++;
      }


  }
  if(!(*Cresceu))//verifica se o processo ja finalizou, caso cresceu==True ele continua o programa
      return;

  if(Ap->Pt == Externa)//o Registro da recursao tem que ser atualizado para o item que vai ser inserido na pagina externa
      *RegRetorno=Reg;

  if(Ap->Pt == Interna && Ap->UU.U0.ni < MM){
      InsereNaPaginaEstrela(Ap,RegRetorno->chave,*ApRetorno,comparacoes);
      *Cresceu=0;
      return;
  }

  else if(Ap->Pt == Interna && Ap->UU.U0.ni >= MM){  //cria pagina interna nova 
      ApTemp=malloc(sizeof(TipoPagina2));
      ApTemp->Pt=Interna;
      ApTemp->UU.U0.ni=0;
      ApTemp->UU.U0.pi[0]=NULL;


      if(i< (M+1)){//saber se o item ficara na nova pagina ou na pagina existente
      InsereNaPaginaEstrela(ApTemp,Ap->UU.U0.ri[MM-1],Ap->UU.U0.pi[MM],comparacoes);
      Ap->UU.U0.ni--;
      InsereNaPaginaEstrela(Ap,RegRetorno->chave,*ApRetorno,comparacoes);
      }
      else    
          InsereNaPaginaEstrela(ApTemp,RegRetorno->chave,*ApRetorno,comparacoes);

      for(int j=M+1;j<=MM;j++)
          InsereNaPaginaEstrela(ApTemp,Ap->UU.U0.ri[j-1],Ap->UU.U0.pi[j],comparacoes);

      Ap->UU.U0.ni=M;
      ApTemp->UU.U0.pi[0]=Ap->UU.U0.pi[M+1];//ponteiro a esquerda da nova pagina recebe o ponteiro mais a direita da pagina dividida
      RegRetorno->chave=ApTemp->UU.U0.ri[0];//apenas a chave do item que deve subir
      *ApRetorno=ApTemp;//filho a direita do item que vai subir

  }


  else if(Ap->Pt == Externa && Ap->UU.U1.ne < MM){
          InsereNaPaginaFolhaEstrela(Ap,*RegRetorno,NULL,comparacoes);
          *Cresceu=0;
          return;
      }

  else if(Ap->Pt == Externa && Ap->UU.U1.ne >= MM){//cria nova pagina externa
          ApTemp=malloc(sizeof(TipoPagina2));
          ApTemp->Pt=Externa;
          ApTemp->UU.U1.ne=0;

          if(i< (M+1)){//saber se o item ficara na nova pagina ou na pagina existente
              InsereNaPaginaFolhaEstrela(ApTemp,Ap->UU.U1.re[MM-1],NULL,comparacoes);
              Ap->UU.U1.ne--;
              InsereNaPaginaFolhaEstrela(Ap,*RegRetorno,*ApRetorno,comparacoes);
          }
          else
              InsereNaPaginaFolhaEstrela(ApTemp,*RegRetorno,*ApRetorno,comparacoes);

          for(int j=M+1;j<=MM;j++)
              InsereNaPaginaFolhaEstrela(ApTemp,Ap->UU.U1.re[j-1],NULL,comparacoes);


          Ap->UU.U1.ne=M;
          *RegRetorno=ApTemp->UU.U1.re[0];//o item que vai subir
          *ApRetorno=ApTemp;//filho a direita do que vai subir é a nova pagina    
  }
}

void InsereEstrela(tipoitem Reg,TipoApontador2 *Ap,int *comparacoes){
  short Cresceu=1;//inicia com a criaçao de pagina nova
  tipoitem RegRetorno;
  TipoPagina2 *ApRetorno,*ApTemp,*ApTemp2;
  if(*Ap == NULL ){//ja inicializa primeira pagina como externa

      ApTemp2 = malloc(sizeof(TipoPagina2));
      ApTemp2->Pt=Externa;
      ApTemp2->UU.U1.ne=1;
      ApTemp2->UU.U1.re[0]=Reg;

      *Ap=ApTemp2;

  }
  else{
      InsEstrela(Reg,*Ap,&Cresceu,&RegRetorno,&ApRetorno,comparacoes);
      if(Cresceu){
          ApTemp=malloc(sizeof(TipoPagina2));
          ApTemp->Pt=Interna;
          ApTemp->UU.U0.ni=1;
          ApTemp->UU.U0.ri[0]=RegRetorno.chave;
          ApTemp->UU.U0.pi[0]=*Ap;
          ApTemp->UU.U0.pi[1]=ApRetorno;
          *Ap=ApTemp;    
  }
  }
 
}

int PesquisaEstrela(tipoitem *x,TipoApontador2 *Ap,int *comparacoes){
  int i;
  TipoApontador2 Pag;
  Pag=*Ap;
  if((*Ap)->Pt == Interna){
      i=1;
      (*comparacoes)++;
      while(i < Pag->UU.U0.ni && x->chave > Pag->UU.U0.ri[i-1])
          i++; 

      (*comparacoes)++;       
      if(x->chave < Pag->UU.U0.ri[i-1])
          return PesquisaEstrela(x,&Pag->UU.U0.pi[i-1],comparacoes);
      else    
          return PesquisaEstrela(x,&Pag->UU.U0.pi[i],comparacoes);
  }
  i=1;
  while(i < Pag->UU.U1.ne && x->chave > Pag->UU.U1.re[i-1].chave){
      (*comparacoes)++;
      i++;
  }

  if(x->chave == Pag->UU.U1.re[i-1].chave){
      (*comparacoes)++;
      return 1;
  }

  else
      return 0;
}

void processaArvoreEstrela(const char *nomeArquivo, int qtd, int chave,dados *cont) {
    FILE *arq = fopen(nomeArquivo, "rb");  // Abre o arquivo no modo leitura binária
    if (arq == NULL) {
        printf("Erro ao abrir o arquivo: %s\n", nomeArquivo);
        return;
    }

    tipoitem temp, pesquisado;
    pesquisado.chave = chave;  // A chave que será pesquisada na árvore
    TipoApontador2 Ap = NULL;  // Ponteiro para a árvore (inicialmente NULL)
    int contreg = 0;

    // Insere os registros na árvore
    while (fread(&temp, sizeof(tipoitem), 1, arq) == 1 && contreg < qtd) {
        InsereEstrela(temp, &Ap, &cont->comparacoes);  // Insere na árvore
        contreg++;
        cont->acessos++;  // Conta os acessos
    }
    
    rewind(arq);  // Volta o ponteiro do arquivo para o início
    fclose(arq);  // Fecha o arquivo após a inserção

    // Pesquisa o registro na árvore
    int retorno = PesquisaEstrela(&pesquisado, &Ap, &cont->comparacoes);  // Pesquisa pelo registro
    if (retorno) {
        printf("Registro de código %d foi localizado\n", chave);
    } else {
        printf("Registro de código %d não foi localizado\n", chave);
    }
}