#ifndef __HUFFMAN_H__
#define __HUFFMAN_H__

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "ctypes.h"

#define MAX_Slot 256
#define FILE_Lg 1048576+1 // +1 pour le caractère de fin.

typedef enum {
  False, True
}Bool;

// Pour la création de l'arbre de Huffman.
struct SHuffman{
  u32 nUsed; // 1 slot utilisé, 0 sinon.
  u32 nOcc; // Nb d'occurences.
  u32 nIdx_Right, nIdx_Lft; // Index des slots des fils.
  u32 nCodeCara; // Valeur en bit du caractère.
  u32 nCara; // Caractère à récupérer.
  Bool bSheet; // Si c'est une feuille : true sinon false.
  // Permet de vérifier lors du parcours de l'arbre pour la création
  // du code caractère si on est déjà passé par le fils gauche ou droit.
  Bool bNodeDoneL, bNodeDoneR; 
};

// Pour la création du dictionnaire.
struct SDico {
  u32 nUsed; // 1 slot utilisé, 0 sinon.
  u32 nIdx; // Index représentant le caractère.
  u32 nNbBits; // Nb de bits par caractère.
  u32 nCodeCara; // Reçois le code caractère.
};

struct SDico gDico[MAX_Slot];
struct SHuffman gHuffman[MAX_Slot];


s32 pFileTxt[FILE_Lg]; // Pour récupéré le fichier texte.



// Insert le texte décompressé dans un fichier texte.
void Huffman_TxtUnCompressedToFileTxt(u8 *pTxt);

// Décompression du fichier texte.
void Huffman_UncompressTxt(void);

// Convertit un int en ASCII.
char *Huffman_MyItoA(u32 nInt);

// Libération de la mémoire pour le tableau pTxtCompress.
void Huffman_FreeArrayTxtCompress(void);

// Allocation mémoire pour le tableau pTxtCompress.
u32 *Huffman_InitArrayTxtCompress(u32 nMax_Slot);

// Récupération de toutes les données utilisé dans le fichier compressé (dico + texte compressé + nb octets utilisé pour la compression).
void Huffman_GetDataFileCompress(void);

// Récupération du texte compressé.
void Huffman_GetTxtCompress(void);

// Récupère l'octet (placé à la fin du fichier compressé) représentant le nb d'octet du fichier compresser.
u32 Huffman_GetNbBytesFileCompress(void);

// Libération de la mémoire de la struct SDicoUsed.
void Huffman_FreeDicoUsed(void);

// Initialisation de la struct SDicoUsed. 
//ATTENTION : utilisation obligatoire après Huffman_SetDicoFileCompressed() à cause variable global nCountDicoUsed.
void Huffman_InitDicoUsed(void);

// Affiche le dico + texte récupéré dans le fichier.
void Huffman_DisplayDataFileCompress(void);

// Récupération dico dans le fichier compressé.
void Huffman_GetDicoFileCompressed(void);

// Insertion du dico dans le fichier qui recevra les données compressées.
void Huffman_SetDicoFileCompressed(void);

// Affiche les caractère compressé du fichier.
void Huffman_DisplayCaraFileCompression(void);

// Compression texte.Huffman_CompressionFileTxt
void Huffman_CompressionFileTxt(void);

// Extension de la taille du tableau. ?????
void Huffman_FileMalloc(void);

// Donne un caractère.
void Huffman_RecoveryFile(void);

// Met '\0' à toute la liste.
void Huffman_InitListCara(void);

// Affiche le dico. Nb de bits + code caractère.
void Huffman_DisplayDico(void);

// Initialisation du dico.
void Huffman_DicoInit(void);

//---------------------------------------------------------------------------------

// Calcul de la hauteur de l'arbre.
u32 Huffman_HeighTree(u32 len);

// Construction du code binaire d'un caractère.
void Huffman_BuildCodeBit(void);

// Place un slot tab huffman dans le buffer.
void Huffman_SetOneSlotInBuffer(struct SHuffman *pHuffman, u32 nIdx, u32 nIdx2);

// Init d'un slot.
void Huffman_HuffmanInitOneSlot(u32 nIdx);

// Placement des slots du buffer dans le tableau gHuffman.
void Huffman_SetSlotsBufferInHuffman(struct SHuffman *pHuffman, u32 nIdx);

// Affiche l'adresse du fils gauche.
void Huffman_CreateDico(void);

// Init des variables du tableau de struct.
void Huffman_HuffmanInit(struct SHuffman *pHuffman); 

// Affiche les valeurs des variables dans le tableau de struct.
void Huffman_Check_StructHuffman(struct SHuffman *pHuffman); 

// Compte le nombre d'occurence.
void Huffman_CounterOcc(void); 

// Tri le tableau de struct SHuffman.
// Ordre décroissant.
void Huffman_QsortMax(struct SHuffman *pHuffman);

// Ordre croissant.
void Huffman_Qsort(struct SHuffman *pHuffman); 

// Met les occurence des slots non utilisé à (1<<31)-1. Pour le tri. 
void Huffman_SlotToVoid(struct SHuffman *pHuffman);  

// Addition des occurences.
void Huffman_AddOcc(void); 
// Taille du tableau utilisé.
u32 Huffman_SizeTabSlotsUsed(struct SHuffman *pHuffman);

// Comparaison du qsort. Ordre décroissant.
int Huffman_CmpQsortMax(const void *pEl1, const void *pEl2); 
// Comparaison du qsort.
int Huffman_CmpQsort(const void *pEl1, const void *pEl2); 
// Check si caractère suivant différent des précédents.
Bool Huffman_CheckCara(u32 nCara, u32 idx); 

#endif