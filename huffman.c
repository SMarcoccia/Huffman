
#include "huffman.h"


#define SLOT_Occ_Void (1<<31)-1
#define SIZE_Tab_Used -1
#define BITS 8

#define FILENAME "test.txt"
#define FILENAME_Compressed "test2.txt"
#define FILENAME_UnCompressed "test3.txt" 
#define FLAG_FILENAME_w "w"
#define FLAG_FILENAME_r "r"
#define FLAG_FILENAME_rb "rb"
#define FLAG_FILENAME_wb "wb"
#define FLAG_FILENAME_rbPlus "rb+"
#define FLAG_FILENAME_a "a"
#define FLAG_FILENAME_ab "ab"
#define FLAG_FILENAME_abPlus "ab+"
#define MASK 0xFF

FILE *gpFile = NULL;
u32 gnNbStructDicoUsed = 0; // Compte le nombre de struct gDico utilisées.
struct SDico *gDicoUsed = NULL;
u32 *gpTxtCompress = NULL; // Pour récupéré le texte compressé.
u32 gnNbBitsTotal = 0; // Nombre de bits total utilisé pour la compression du fichier texte.
u32 gnNbBytesUsed = 0; // Valeur max d'octet pour le fichier texte compressé.
u32 gnNbCaraInFile = 0; // Nombre de caractères dans le fichier.



void Huffman_TxtUnCompressedToFileTxt(u8 *pTxt)
{
  gpFile = fopen(FILENAME_UnCompressed, FLAG_FILENAME_wb);
  fwrite(pTxt, sizeof(pTxt[0]), gnNbCaraInFile, gpFile);
  fclose(gpFile);
}

void Huffman_UncompressTxt(void)
{
  u32 i = 0, j = 0, k = 0, nMask = MASK, nTxtCompress = 0, nPartCodeCara = 0, nNbBitsRemaining = 0; 
  u32 nNbBitsDone = 0, nCountNbBits = 0;
  // Compte à rebours pour savoir si on à utilisé tous les bits de l'octet.
  s32 nCountDownBits = BITS, nNbBits = BITS; 
  
  // Reçois les caractères qui seront mis ultérieurement dans le fichier texte.
  u8 *pTxt = malloc(FILE_Lg * sizeof(u8)); 
  memset(pTxt, 0, FILE_Lg);

  for(i = 0; i < gnNbBytesUsed; i++)
  {
    nTxtCompress = gpTxtCompress[i];
    if(nCountDownBits < 0)
    {
      nTxtCompress |= (nPartCodeCara << 8);
    }
    else if(nCountDownBits == 0)
      nNbBitsRemaining = 0;
    
    // Ici faire une fonction pour les bits supplémentaires.
    nNbBits = nCountDownBits = BITS + nNbBitsRemaining; 
    nMask |= (nMask << nNbBitsRemaining); 

    while(nCountDownBits > 0)
    {  
      for(j = 0; j < gnNbStructDicoUsed; j++)
      { 
        nNbBitsRemaining = nNbBits;
        nNbBits -= gDicoUsed[j].nNbBits; 

        if(nNbBits < 0)
        {
          nCountDownBits = nNbBits;
          nPartCodeCara = nTxtCompress;
          break;
        }
        
        if((nTxtCompress >> (nNbBits)) == gDicoUsed[j].nCodeCara)
        {
          // if(k > FILE_Lg)
          // Faire un malloc. Ou message d'erreur taille capacité de stockage atteinte.
          pTxt[k] = gDicoUsed[j].nIdx;
          k++;
          nCountDownBits -= gDicoUsed[j].nNbBits;
          nCountNbBits += gDicoUsed[j].nNbBits;
          nNbBitsDone = gDicoUsed[j].nNbBits; // Pour nNbBits superieur à 8.
          
          if(nCountDownBits >= 0)
          {
            nMask >>= nNbBitsDone;
            nTxtCompress &= (nMask); 
          }
          break;
        } 
        nNbBits = nCountDownBits; // Init à la valeur d'origine.
      }
      nNbBits = nCountDownBits;

      if(gnNbBitsTotal == nCountNbBits)
        break;
    }
    nMask = MASK;
    nNbBits = BITS;
  }
  Huffman_TxtUnCompressedToFileTxt(pTxt);
  free(pTxt);
}

void Huffman_FreeArrayTxtCompress(void)
{
  free(gpTxtCompress);
}

u32 * Huffman_InitArrayTxtCompress(u32 gnNbBytesUsed)
{
  return gpTxtCompress = malloc(gnNbBytesUsed * sizeof(u32));
}

void Huffman_GetTxtCompress(void)
{
  gnNbBytesUsed = Huffman_GetNbBytesFileCompress();
  gpTxtCompress = Huffman_InitArrayTxtCompress(gnNbBytesUsed); 

  // +4 pour tenir compte du dernier caractère qui est en u32 (le nombre de byte pour le texte compressé).
  fseek(gpFile, -(gnNbBytesUsed + 4), SEEK_CUR); 
  for(u32 i = 0; i < gnNbBytesUsed; i++)
    {
      gpTxtCompress[i] = fgetc(gpFile); // Mettre fread.
    }
}

u32 Huffman_GetNbBytesFileCompress(void)
{
  u32 c = 0;

  fseek(gpFile, -4, SEEK_END); // -4 car on travail avec u32 soit 4 octets.
  fread(&c, sizeof(c), 1, gpFile); // Curseur est en position EOF après lécture.
  return c;
}

void Huffman_DisplayDataFileCompress(void)
{
  printf("\n-------------------------------------------------------------------------\n");
  printf("--------------------------------DATA DU DICO-----------------------------\n");
  printf("-------------------------------------------------------------------------\n\n");  
  for(u32 i = 0; i < gnNbStructDicoUsed; i++)
  {
    printf("gDicoUsed[%d].nUsed = %d ; gDicoUsed[%d].nIdx = %c ; gDicoUsed[%d].nNbBits = %d ; gDicoUsed[%d].nCodeCara = %d\n",  
    i, gDicoUsed[i].nUsed, i, gDicoUsed[i].nIdx, i, gDicoUsed[i].nNbBits, i, gDicoUsed[i].nCodeCara);
  }

  printf("\n");
  for(u32 i = 0; i < gnNbBytesUsed; i++)
    printf("gpTxtCompress[%d] = %d\n", i, gpTxtCompress[i]);
}

void Huffman_GetDicoFileCompressed(void)
{
  for(u32 i = 0; i < gnNbStructDicoUsed; i++)
  {
    fread(&gDicoUsed[i], sizeof(gDicoUsed[i]), 1, gpFile);
  }
}

void Huffman_GetDataFileCompress(void)
{
  gpFile = fopen(FILENAME_Compressed, FLAG_FILENAME_rb);  
  Huffman_GetDicoFileCompressed();
  Huffman_GetTxtCompress();
  fclose(gpFile);
}

void Huffman_FreeDicoUsed(void)
{
  free(gDicoUsed);
}

void Huffman_InitDicoUsed(void)
{
  gDicoUsed = malloc(gnNbStructDicoUsed * sizeof(struct SDico));
}

char *Huffman_MyItoA(u32 nInt)
{
  static char pTmp[BUFSIZ] = "";
  char pCara[] = "0";
  u32 nLen = 0;
  char cTmp = 0;

  do
  {
    pCara[0] = (char)((nInt % 10) + '0');
    strcat(pTmp, pCara);
    nLen++;
  }while((nInt /= 10) > 0);

  for (u32 i = 0; i < nLen/2 ; i++)
  {
    cTmp = pTmp[i];
    pTmp[i] = pTmp[(nLen - 1) - i];
    pTmp[(nLen - 1) - i ] = cTmp;
  }
  
  return pTmp;
}

void Huffman_SetDicoFileCompressed(void)
{ 
  gpFile = fopen(FILENAME_Compressed, FLAG_FILENAME_wb);
  
  for(u32 i = 0; i < MAX_Slot; i++)
  {
    if(gDico[i].nUsed)
    {
      fwrite(&gDico[i], sizeof(gDico[i]), 1, gpFile);
      gnNbStructDicoUsed++;
    }
  }
  fclose(gpFile);
}

void Huffman_DisplayCaraFileCompression(void)
{
  gpFile = fopen(FILENAME_Compressed, FLAG_FILENAME_rb);
  s32 c = 0;
  printf("\n-----------------------------------------------------------------------\n");
  printf("---------------------------DATA DU DICTIONNAIRE COMPRESSE---------------\n");
  printf("-----------------------------------------------------------------------\n\n");  
  do
  {
    c = fgetc(gpFile);
    printf("c = %d -- c = %c\n", c, c);
  }while(c != EOF);
  fclose(gpFile);
}

void Huffman_CompressionFileTxt(void)
{
  gpFile = fopen(FILENAME_Compressed, FLAG_FILENAME_abPlus);
  u8 cBuffer = 0; // Buffer pour placer les codes caractères.
  s32 nBits = BITS; // Nb de bits libre dans cBuffer.
  u32 nDicoNbBits = 0, nCodeCara = 0;
  Bool nVal = True; // Si nBits < 0 on ne rentre pas dans le if.
  u32 nNbBytesUsed = 0; // Nombre d'octet utilisé pour la compression du fichier.

  for(u32 i = 0; pFileTxt[i] != '\0'; i++)
  {
    nDicoNbBits = gDico[pFileTxt[i]].nNbBits;
    gnNbBitsTotal += nDicoNbBits;
    nCodeCara = gDico[pFileTxt[i]].nCodeCara;
    nBits -= nDicoNbBits;

    while(nBits < 0) // Si le code caractère ne peut pas rentré en entier dans cBuffer.
    {
      nBits = abs(nBits);
      cBuffer |= (nCodeCara >> nBits);  

      fwrite(&cBuffer, sizeof(cBuffer), 1, gpFile);
      nNbBytesUsed++;
      cBuffer = 0; 
      nBits = BITS - nBits;

      if(nBits > 0)
      {
        cBuffer |= (nCodeCara << nBits); // Met la valeur restante dans l'octet suivant.
      }
      nVal = False;
    }

    if(nVal)
    {
      cBuffer |= (nCodeCara << nBits);
      if(nBits == 0 || pFileTxt[i+1] == '\0')
      {
        fwrite(&cBuffer, sizeof(cBuffer), 1, gpFile);
        nNbBytesUsed++;
        nBits = BITS;
        cBuffer = 0;
      }
    }
    nVal = True;
  }

  fwrite(&nNbBytesUsed, sizeof(nNbBytesUsed), 1, gpFile);
  fclose(gpFile);
}

void Huffman_FileMalloc(void)
{

}

void Huffman_RecoveryFile(void)
{
  s32 c = 0;

  gpFile = fopen(FILENAME, FLAG_FILENAME_rb);
  for(u32 i = 0; (c = fgetc(gpFile)) != EOF; i++)
  {
    if(i < FILE_Lg)
      Huffman_FileMalloc();
    pFileTxt[i] = c;
    gnNbCaraInFile++;
  }

  fclose(gpFile);
}



void Huffman_InitListCara(void)
{
  for(u32 i = 0; i < FILE_Lg; i++)
  {
    pFileTxt[i] = '\0';
  }
}

void Huffman_DisplayDico(void)
{
  printf("\n-----------------------------------------------------------------------\n");
  printf("---------------------------DATA DU DICTIONNAIRE------------------------\n");
  printf("-----------------------------------------------------------------------\n\n");  
  for(u32 i = 0; i < 256; i++)
  {
    printf("nCodeCara[%c] = %d -- .nNbBits = %d -- .nIdx = %d\n", i, gDico[i].nCodeCara, 
            gDico[i].nNbBits, gDico[i].nIdx);
  }
}

void Huffman_DicoInit(void)
{
  for(u32 i = 0; i < MAX_Slot; i++)
  {
    gDico[i].nUsed = 0;
    gDico[i].nIdx = -1;
    gDico[i].nNbBits = -1; // -1 car on par de la racine ce qui oblige à faire un tour de plus.
    gDico[i].nCodeCara = 0;
  }
}

void Huffman_SetOneSlotInBuffer(struct SHuffman *pHuffman, u32 nIdx, u32 nIdx2)
{
  pHuffman[nIdx].nIdx_Lft = gHuffman[nIdx2].nIdx_Lft;
  pHuffman[nIdx].nIdx_Right = gHuffman[nIdx2].nIdx_Right;
  pHuffman[nIdx].nUsed = gHuffman[nIdx2].nUsed ;
  pHuffman[nIdx].nCara = gHuffman[nIdx2].nCara;
  pHuffman[nIdx].nOcc = gHuffman[nIdx2].nOcc;
  pHuffman[nIdx].nCodeCara = gHuffman[nIdx2].nCodeCara;
  pHuffman[nIdx].bSheet = gHuffman[nIdx2].bSheet;
  pHuffman[nIdx].bNodeDoneL = gHuffman[nIdx2].bNodeDoneL;
  pHuffman[nIdx].bNodeDoneR = gHuffman[nIdx2].bNodeDoneR;
}

void Huffman_HuffmanInitOneSlot(u32 nIdx)
{
  gHuffman[nIdx].nIdx_Lft = -1;
  gHuffman[nIdx].nIdx_Right = -1;  
  gHuffman[nIdx].nUsed = False;
  gHuffman[nIdx].nCara = 0;
  gHuffman[nIdx].nOcc = (u32)SLOT_Occ_Void;
  gHuffman[nIdx].nCodeCara = -1;
  gHuffman[nIdx].bSheet = False;
  gHuffman[nIdx].bNodeDoneL = False;
  gHuffman[nIdx].bNodeDoneR = False;
}

void Huffman_SetSlotsBufferInHuffman(struct SHuffman *pHuffman, u32 nIdx)
{
  for(u32 i = 0; pHuffman[i].nUsed != 0; i++, nIdx++)
  {
    gHuffman[nIdx].nIdx_Lft = pHuffman[i].nIdx_Lft ;
    gHuffman[nIdx].nIdx_Right = pHuffman[i].nIdx_Right;    
    gHuffman[nIdx].nUsed = pHuffman[i].nUsed ;
    gHuffman[nIdx].nCara = pHuffman[i].nCara;
    gHuffman[nIdx].nOcc = pHuffman[i].nOcc;
    gHuffman[nIdx].nCodeCara = pHuffman[i].nCodeCara;
    gHuffman[nIdx].bSheet = pHuffman[i].bSheet;
    gHuffman[nIdx].bNodeDoneL = pHuffman[i].bNodeDoneL;
    gHuffman[nIdx].bNodeDoneR = pHuffman[i].bNodeDoneR;
  }
}

u32 Huffman_HeighTree(u32 len)
{
  return log2(len);
}

void Huffman_CreateDico(void)
{
  u32 nLgArrayHuffman = Huffman_SizeTabSlotsUsed(gHuffman);
  u32 nIdxTmp = nLgArrayHuffman-1;
  u32 nNbBits = 0;
  // Pour récupéré les index des noeuds.
  u32 pIdxNode[nLgArrayHuffman]; 
  // nVal_Left et right autorise l'entré dans le bloc pour intégré au dico code cara + nb de bits.
  u32 nVal_Left = False, nVal_Right = True;
  u32 nCodeCara = 0;

  if(nLgArrayHuffman == 1)
  {
    gDico[(u32)gHuffman[nIdxTmp].nCara].nUsed = 1;
    gDico[(u32)gHuffman[nIdxTmp].nCara].nIdx = gHuffman[nIdxTmp].nCara;      
    gDico[(u32)gHuffman[nIdxTmp].nCara].nNbBits = nNbBits+1;
    gDico[(u32)gHuffman[nIdxTmp].nCara].nCodeCara = nCodeCara;     
    return;
  }

  for(u32 i = 0; i < nLgArrayHuffman; i++)
  {
    pIdxNode[i] = -1;
  }

  while(1)
  {
    // Left son.
    while(gHuffman[nIdxTmp].bSheet != True)
    {
      // Si on a déjà fait le fils gauche.
      if(gHuffman[nIdxTmp].bNodeDoneL == True)
      {
        nVal_Left = True;
        break;
      }

      pIdxNode[nNbBits] = nIdxTmp; 
      nCodeCara <<= 1;
      nNbBits++; 
      
      gHuffman[nIdxTmp].bNodeDoneL = True;
      nIdxTmp = gHuffman[nIdxTmp].nIdx_Lft;
    }

    // Ajout dans le dico code cara + nb de bits.
    if(nVal_Left == False) 
    {
      gDico[(u32)gHuffman[nIdxTmp].nCara].nUsed = 1;      
      gDico[(u32)gHuffman[nIdxTmp].nCara].nIdx = gHuffman[nIdxTmp].nCara;      
      gDico[(u32)gHuffman[nIdxTmp].nCara].nNbBits = nNbBits;
      gDico[(u32)gHuffman[nIdxTmp].nCara].nCodeCara = nCodeCara;    
      // On supprime le bit de la feuille gauche. 
      nCodeCara >>= 1; 
      // On enlève 1 ce qui permettra de récupéré le bonne index.
      nNbBits--; 
      nVal_Left = False;
    }

    // Right son.  
    nIdxTmp = pIdxNode[nNbBits];
    while(gHuffman[nIdxTmp].bSheet != True)
    {
      nCodeCara = (nCodeCara << 1) + 1;
      nNbBits++;

      gHuffman[nIdxTmp].bNodeDoneR = True;
      nIdxTmp = gHuffman[nIdxTmp].nIdx_Right;

      // Si ce n'est pas une feuille.
      if(gHuffman[nIdxTmp].bSheet == False)
      {
        nVal_Right = False;
        nVal_Left = False;
        break;
      }      
    }

    // Ajout dans le dico code cara + nb de bits.
    if(nVal_Right)
    {
      gDico[(u32)gHuffman[nIdxTmp].nCara].nUsed = 1;      
      gDico[(u32)gHuffman[nIdxTmp].nCara].nIdx = gHuffman[nIdxTmp].nCara;      
      gDico[(u32)gHuffman[nIdxTmp].nCara].nNbBits = nNbBits;
      gDico[(u32)gHuffman[nIdxTmp].nCara].nCodeCara = nCodeCara;
      do
      {
        nCodeCara >>= 1; 
        nNbBits--; 
        nIdxTmp = pIdxNode[nNbBits];
        if(gHuffman[nIdxTmp].bNodeDoneL && gHuffman[nIdxTmp].bNodeDoneR)
          pIdxNode[nNbBits] = -1;
        else
        {
          break;
        }
      }while(1);     
    }
    else
    {
      nVal_Right = True;
    }

    if(pIdxNode[0] == -1)
      break;
  }
}

void Huffman_AddOcc(void)
{
  u32 nSumOcc = 0; // Somme des occurences.
  u32 nLenTabUsedPaire = 0; // Taille paire du tableau utilisé.
  u32 nLastSizePaireTabUsed = Huffman_SizeTabSlotsUsed(NULL);
  u32 nIdx = 0; // Index pour ajout de la somme des occurences dans la suite du tableau.
  u32 nIdx2 = 0;
  u32 nVal = 0;
  // Compte le nb de tours de boucle sert à calculer la taille du tableau utilisé.
  u32 nCount = 0; 
  
  struct SHuffman pHuffman[MAX_Slot] = {0}; // Buffer.
  
  Huffman_HuffmanInit(pHuffman);

  do
  {
    // Taille du tableau : impaire ? Soustrait 1. On travail sur la longueur du tableau utilisé paire.
    if(nLastSizePaireTabUsed % 2) 
    {
      nLastSizePaireTabUsed -= 1;
      // On prend le slot impaire et on le met dans le buffer à l'index 0.
      Huffman_SetOneSlotInBuffer(pHuffman, 0, nLastSizePaireTabUsed);
      Huffman_HuffmanInitOneSlot(nLastSizePaireTabUsed); // Supprime le slot impaire.
      nIdx2 = 1;
      nCount = 1; // A 1 pour mettre la taille exacte du tab utilisé.
    }
    
    nIdx = nLastSizePaireTabUsed;
    for(s32 i = nLenTabUsedPaire; i < nLastSizePaireTabUsed; i+=2)
    {
      nSumOcc = gHuffman[i].nOcc + gHuffman[i+1].nOcc;
      pHuffman[nIdx2].nOcc = nSumOcc; 
      pHuffman[nIdx2].nIdx_Right = i;
      pHuffman[nIdx2].nIdx_Lft = i + 1;
      pHuffman[nIdx2].nUsed = 1;
      nIdx2++;
      nCount++;
  
    }

    Huffman_SlotToVoid(pHuffman); 
    Huffman_Qsort(pHuffman);
    Huffman_SetSlotsBufferInHuffman(pHuffman, nIdx); 
    
    #ifdef DEBUG
      Huffman_Check_StructHuffman(NULL);
    #endif

    nVal = Huffman_SizeTabSlotsUsed(pHuffman);
    Huffman_HuffmanInit(pHuffman); 

    nLenTabUsedPaire = 0;
    nLenTabUsedPaire += nLastSizePaireTabUsed;
    nLastSizePaireTabUsed += nCount;
    nIdx2 = 0;
    nCount = 0;
  }while(nVal != 1);
}

void Huffman_AddValueBinarySlot(struct SHuffman *pHuffman)
{
  u32 i = 0;
  struct SHuffman *pTmp = NULL;

  if(pHuffman == NULL)
    pTmp = gHuffman;
  else
  {
    pTmp = pHuffman;
  }
  
  for(i = 0; pTmp[i].nUsed != 0; i--)
  {
    if( ! (i % 2))
    {
      pTmp[i].nCodeCara = 1;
    }
    else 
    {
      pTmp[i].nCodeCara = 0;
    }
  }
  if(i % 2)
    pTmp[i-1].nCodeCara = -1;
}

u32 Huffman_SizeTabSlotsUsed(struct SHuffman *pHuffman)
{
  u32 i = 0;
  struct SHuffman *pTmp = NULL;

  if(pHuffman == NULL)
    pTmp = gHuffman;
  else
  {
    pTmp = pHuffman;
  }
  
  for(i = 0; pTmp[i].nUsed != 0; i++)
    continue;

  return i;
}

int Huffman_CmpQsortMax(const void *pEl1, const void *pEl2)
{
  return ((*(struct SHuffman *)pEl2).nOcc - (*(struct SHuffman *)pEl1).nOcc);
}

void Huffman_QsortMax(struct SHuffman *pHuffman)
{
  struct SHuffman *pTmp = NULL;

  if(pHuffman == NULL)
    pTmp = gHuffman;
  else
  {
    pTmp = pHuffman;
  }
  qsort(pTmp, MAX_Slot, sizeof(pTmp[0]), Huffman_CmpQsortMax);
}

int Huffman_CmpQsort(void const *pEl1, void const *pEl2)
{
  return ((*(struct SHuffman *)pEl1).nOcc - (*(struct SHuffman *)pEl2).nOcc);
}

void Huffman_Qsort(struct SHuffman *pHuffman)
{
  struct SHuffman *pTmp = NULL;
  if(pHuffman == NULL)
    pTmp = gHuffman;
  else
  {
    pTmp = pHuffman;
  }
  qsort(pTmp, MAX_Slot, sizeof(pTmp[0]), Huffman_CmpQsort);
}

Bool Huffman_CheckCara(u32 nCara, u32 idx)
{
  for(u32 i = 0; gHuffman[i].nUsed != False && idx != 0; i++)
  { 
    if(gHuffman[i].nCara == nCara) 
    {
      return False;
    }
  }
  return True;
}

void Huffman_SlotToVoid(struct SHuffman *pHuffman)
{
  struct SHuffman *pTmp = NULL;

  if(pHuffman == NULL)
    pTmp = gHuffman;
  else
  {
    pTmp = pHuffman;
  }  
  for(u32 i = 0; i < MAX_Slot; i++)
  {
    if(pTmp[i].nUsed == False)
      pTmp[i].nOcc = (u32)SLOT_Occ_Void;  
  }
}

void Huffman_CounterOcc(void)
{
  u32 nOcc = 0;
  u32 i = 0, j = 0, k = 0;
  while(pFileTxt[i] != '\0')
  {
    #ifdef DEBUG
      printf("pFileTxt[%d] = %c -- %d\n", i, pFileTxt[i], pFileTxt[i]);
    #endif

    if(Huffman_CheckCara(pFileTxt[i], i))
    { 
      gHuffman[k].nCara = pFileTxt[i];
      gHuffman[k].nUsed = True;
      j = i; 
      // Note, pour la prochaine fois : les caractères ne son pas connexe dans gHuffman.
      while(pFileTxt[j] != '\0') 
      {
        if(pFileTxt[j] == gHuffman[k].nCara) // Compte 1er occurence.
          nOcc++;
        j++;
      }
      gHuffman[k].nOcc = nOcc;
      nOcc = 0;
      k++;
    }
    i++;
  }
  for(i = 0; i < MAX_Slot; i++)
  {
    if(gHuffman[i].nUsed != 0)
      gHuffman[i].bSheet = True;
  }
}

void Huffman_HuffmanInit(struct SHuffman *pHuffman)
{
  struct SHuffman *pTmp = NULL;

  if(pHuffman == NULL)
    pTmp = gHuffman;
  else
  {
    pTmp = pHuffman;
  }
      
  for(u32 i = 0; i < MAX_Slot; i++) 
  {
    pTmp[i].nIdx_Lft = -1;
    pTmp[i].nIdx_Right = -1;  
    pTmp[i].nUsed = False;
    pTmp[i].nCara = 0;
    pTmp[i].nOcc = (u32)SLOT_Occ_Void;
    pTmp[i].nCodeCara = -1;
    pTmp[i].bSheet = False;
    pTmp[i].bNodeDoneL = False;
    pTmp[i].bNodeDoneR = False;
  }
}

void Huffman_Check_StructHuffman(struct SHuffman *pHuffman)
{
  struct SHuffman *pTmp = NULL;

  if(pHuffman == NULL)
    pTmp = gHuffman;
  else
  {
    pTmp = pHuffman;
  }
  printf("\n");
  printf("-----------------------------------------------------------------------\n");
  printf("------------------------TABLEAU DES STRUCT HUFFMAN---------------------\n");
  printf("-----------------------------------------------------------------------\n\n");
  for(u32 i = 0; i < MAX_Slot; i++)
    printf("Idx = %d -- Used = %d -- Cara = %c - %d -- Nb occ = %d -- cara bits = %d -- Sheet ? = %d -- nIdx_Lft = %d -- nIdx_Right = %d\n", 
           i, pTmp[i].nUsed, pTmp[i].nCara, pTmp[i].nCara, pTmp[i].nOcc, pTmp[i].nCodeCara, 
           pTmp[i].bSheet, pTmp[i].nIdx_Lft, pTmp[i].nIdx_Right);
}

