/* function.c - include some functions of fcs.c*/
/* Author: Jia-Ming Chang (jmchang@cs.nthu.edu.tw) */
/* Version Creation Date: 2/1/2001 */
/* 1.3 2/28/2001*/
/*
Weight; //the weight of edge
 int startVertex;
 int endVertex;
*/ 

int rank[maxNodeNum];
int p[maxNodeNum];

void Exchange(EDGE A[maxVertexNum],int p,int r)
{
 EDGE mid;
 
 mid.Weight = A[p].Weight;
 mid.startVertex = A[p].startVertex;
 mid.endVertex = A[p].endVertex;
 
 A[p].Weight = A[r].Weight;
 A[p].startVertex = A[r].startVertex;
 A[p].endVertex = A[r].endVertex;
 
 //A[p] = A[r];
 A[r].Weight = mid.Weight;
 A[r].startVertex = mid.startVertex;
 A[r].endVertex = mid.endVertex;
 //A[r] = mid;
}

int Partition(EDGE A[maxVertexNum],int p,int r)
{
 float x;
 int i,j;
 
 x = A[p].Weight;
 i = p - 1;
 j = r + 1;
 
 while(true){
  do{
   j = j - 1;
  }while(A[j].Weight > x);
  do{
   i = i + 1;
  }while(A[i].Weight < x);
  if (i < j)
   Exchange(A,i,j);
  else
  {
   return j;    
   break;
  } 
 }// End while	
}

void Quicksort(EDGE A[maxVertexNum],int p,int r)
{
 int q;
 	
 if (p < r){
  q = Partition(A,p,r);
  Quicksort(A,p,q);
  Quicksort(A,q+1,r);
 }
}

void Make_Set(int x)
{
 p[x] = x;
 rank[x] = 0;
}

int Find_Set(int x)
{
 if (x != p[x])
  p[x] = Find_Set(p[x]);
 return p[x]; 
}

void Link(int x,int y)
{
 if (rank[x] > rank[y])
  p[y] = x;
 else 
 {
  p[x] = y; 
  if (rank[x] = rank[y])
   rank[y] = rank[y] + 1;
 }//End else
}

void Union(int x,int y)
{
 Link(Find_Set(x),Find_Set(y));	
}

void Push(STACKNODEPTR *topPtr, int info)
{
 STACKNODEPTR newPtr;
 
 newPtr = (STACKNODEPTR) malloc(sizeof(STACKNODEPTR));
 if (newPtr != NULL)
 {
  newPtr->data = info;
  newPtr->nextPtr = *topPtr;
  *topPtr = newPtr;
 } 	
 else
  printf("No memory available\n");
  
// free(newPtr);
}

int Pop(STACKNODEPTR *topPtr)
{
 STACKNODEPTR tempPtr;	
 int popValue;
 
 tempPtr = *topPtr;
 popValue = (*topPtr)->data;
 *topPtr = (*topPtr)->nextPtr;
 free(tempPtr);
 return popValue;
}

int Top(STACKNODEPTR *topPtr)
{
 int topValue;
 
 topValue = (*topPtr)->data;
 return topValue;
}
