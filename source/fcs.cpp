/* FCS.c - Finding Compact Set (include function.c)*/
/* input distance matrix */
/* Author: Jia-Ming Chang (jmchang@cs.nthu.edu.tw) */
/* Version Creation Date: 2/1/2001 */
/* 1.4 2/28/2001*/
/* Implemaint Paper */
/*  Title: A note on finding compact sets in graphs represented by an adjacency list */
/*  Author: Sung Kwon Kim */
/*  Information Processing Letters 57 (1996) 335-338 */
/* Reference List */
/*  [1]T.H. Corman, C.E. eiserson and R.L. Rivest, Introduction to Algorithms,ch.22 24,1994  */
/* distance matrix format */
/*
  4
   4 5 5
     7 3 
       9
*/

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <vector>
#include <stack>
#include <algorithm>
#include <iostream>

using namespace std;

#define maxVertexNum 10000
#define maxEdgeNum 49995000 //=(10000*9999)/2
#define maxNodeNum 20000
#define true 1

#define max(x,y)  ((x) >= (y) ? (x) : (y))
enum LCACheckTypes { UNDO=0, DO=1 };
enum CSType { NO=0, YES=1 };

struct edge {
 float Weight; //the weight of edge
 int startVertex;
 int endVertex;
 int LCA; // the lowest commer acestor of the two end vertixs of the edge
};

struct TcanNode {
 int index;
 float maxCross; //maximal cross edge weight of this internal node 
 float maxInside; //maximal inside edge weight of this internal node 
 float minBorder; //minimal border edge weight of this internal node 
 int parent; 
 enum CSType CS; //0: this internal node represent sets isn't compact set 
 struct TcanNode *leftPtr; 
 struct TcanNode *rightPtr;
};

struct TMarkNode {
 struct TMarkNode *childPtr[maxVertexNum]; 
 int index; 
 int lma; 
 int childNum; //count # of child,start from 1
};

struct stackNode {
 int data;
 struct stackNode *nextPtr;
};

typedef struct stackNode STACKNODE; 
typedef struct edge EDGE;
typedef struct TcanNode TCANNODE;
typedef struct TMarkNode TMARKNODE;
typedef TCANNODE *TCANNODEPTR;
typedef STACKNODE *STACKNODEPTR;
typedef TMARKNODE *TMARKNODEPTR;

vector<EDGE> edgeArr; //array start from #1
vector<string> index2name;
stack<int> nodeStack;
TCANNODE TcanNodeArr[maxNodeNum]; //array start from #1
TMARKNODE TMarkNodeArr[maxNodeNum]; //array start from #1

int vn,en;	//vertex number,edge number
int interNum; // # of inter node 
enum LCACheckTypes LCACheckArr[maxNodeNum];
int ancestor[maxNodeNum];
STACKNODEPTR stackPtr = NULL;

void readDM(char *dis_f);
void initial();
void Sort_Edge();
void Make_Node();
void Merge(int, int ,float);
int Find_Node();
void MST_Kruskal();
string ListCompactSet(TCANNODEPTR x);
void signEdgeLCA(int ,int);
void LCA(TCANNODEPTR);
void TcanCrossEdge();
void maxInsideEdge(TCANNODEPTR );
void Verify_Tcan();
int Descendant();
void TMarkNode();
void LMA(); 
void TMark();
void PrintCompactSetTree(char *cs_f);
void postTMark();

#include "function.c"

int main(int argc, char **argv)
{
 int temp; 
 if (argc<3)
    { printf("please use command: a.out DistanceFile CompactSetFile\n"); exit(0);}

 readDM(argv[1]);  // read Distance Matrix File  
 temp = en;
 Sort_Edge(); // sorting distance edge
 en = temp;
 MST_Kruskal(); // MST-Kruskal's algorithm to make canidate tree
 interNum--;
 TcanCrossEdge();
 Verify_Tcan();// Verify all candidate sets
 ListCompactSet(&(TcanNodeArr[interNum]));
 PrintCompactSetTree(argv[2]); // List all compact set of G 

 return 0;
}

void  readDM(char *dis_f)
{
 int i,j,temp;
 float weight;
 char name[80];
 EDGE tmp_edge = {0,0,0,0};  
 FILE *disPtr;
 
 if ((disPtr=fopen(dis_f,"r"))==NULL){
   printf("cannot open distance file");exit(0);
 }
 edgeArr.push_back(tmp_edge);
 fscanf(disPtr,"%d",&vn);
 for(i=1;i<=vn;i++)
 {
    fscanf (disPtr, "%s", name);
    index2name.push_back(name);
    for(j=1;j<=vn;j++)
    {
      fscanf(disPtr,"%f",&weight);
      if(j > i)
      {
//  	printf("i=%d j=%d weight=%5.2f\n",i,j,weight);
	tmp_edge.Weight = weight;
	tmp_edge.startVertex = i; 
	tmp_edge.endVertex = j;
	edgeArr.push_back(tmp_edge);
      }
    }
 } 
 fclose(disPtr); 
 en = (vn*(vn-1))/2;
 
 cout << "[SUMMARY]" << endl
      << "# of veterx = " << vn << endl
      << "# of edges  = " << (int)edgeArr.size() << " " << en << endl; 
}

/*
void initial()
{
  int i;
  for (i=1;i<=2*en;i++)
  { 
   TcanNodeArr[i].maxCross = INT_MIN;
   TcanNodeArr[i].maxInside = INT_MIN;
   TcanNodeArr[i].minBorder = INT_MIN;
  }
}
*/

bool cmp(const EDGE& a, const EDGE& b)
{
  return (a.Weight < b.Weight);
}

void Sort_Edge()
{  
  sort(edgeArr.begin(), edgeArr.end(), cmp);
}

void Make_Node(int x)
{
 TcanNodeArr[x].index = x;
 TcanNodeArr[x].parent = x;
 if (x <= vn) //initial leaf node.CS is a compact set
  TcanNodeArr[x].CS = YES;
}

void Merge(int x,int y,float weight)
{
  Make_Node(interNum);
//  printf("merge x %d y %d Parent %d weight %5.2f\n",x,y,interNum,weight);
  TcanNodeArr[interNum].leftPtr = &(TcanNodeArr[x]);
  TcanNodeArr[interNum].leftPtr->minBorder = weight;
  TcanNodeArr[interNum].rightPtr = &(TcanNodeArr[y]);
  TcanNodeArr[interNum].rightPtr->minBorder = weight;
  TcanNodeArr[x].parent = interNum;
  TcanNodeArr[y].parent = interNum;
  interNum++;
}

int Find_Node(int x)
{
 if (TcanNodeArr[x].parent != x)
  return Find_Node(TcanNodeArr[x].parent);
 else
  return TcanNodeArr[x].parent;
}

void MST_Kruskal() //specify the own disjoint set data struct and operation
{ 
 int i,u,v,right,left;
 
 interNum = vn + 1;  //  A <- Null
 for (i=1;i<=vn;i++)
  Make_Node(i);
 for (i=1;i<=en;i++)
 {
    u = edgeArr[i].startVertex;
    v = edgeArr[i].endVertex;
    right = Find_Node(u);
    left  = Find_Node(v);
    if (right != left) //  A <- A U {(u,v)}
      Merge(right,left,edgeArr[i].Weight);  //printf("u...%d v...%d\n",u,v);  
 }//End for
}

string ListCompactSet(TCANNODEPTR x)
{
 string list_str;
 
 //only consider non-leaf node
 if (x->index > vn )
 {
  if (x->leftPtr != NULL)
   list_str += ListCompactSet(x->leftPtr);
  if (x->rightPtr != NULL) 
   list_str += ListCompactSet(x->rightPtr); //printf("index...%d\n",x->index); 
  if((x->CS == YES) && (x->index != interNum))
    cout << "compact set = " << list_str << endl;

  return list_str;
 }
 else
 {
  return (index2name[x->index-1] + ",");
 }
}

void signEdgeLCA(int u,int i)
{
 if (edgeArr[i].startVertex == u)
 {
  if (LCACheckArr[edgeArr[i].endVertex] == DO)
   edgeArr[i].LCA = ancestor[Find_Set(edgeArr[i].endVertex)];
 }
 else if (edgeArr[i].endVertex == u)
 {
  if (LCACheckArr[edgeArr[i].startVertex] == DO)
   edgeArr[i].LCA = ancestor[Find_Set(edgeArr[i].startVertex)];
 }
}

void LCA(TCANNODEPTR x) //LCA : Tarjan's off-line least-common-ancestors : Ref[1],P.460
{ 
 int i,u;
 
 u = x->index;
 Make_Set(u);
 ancestor[Find_Set(u)] = u;
 if (x->rightPtr != NULL)
 {
  LCA(x->rightPtr);
  Union(u,x->rightPtr->index);
  ancestor[Find_Set(u)] = u;
 }
 if (x->leftPtr != NULL)
 {
  LCA(x->leftPtr);
  Union(u,x->leftPtr->index);
  ancestor[Find_Set(u)] = u;
 } 
 LCACheckArr[u] = DO; //sign "Do" after walk
 if (u < en)
 { 
  for (i=1;i<=en;i++)
   signEdgeLCA(u,i);
 }  
}

void TcanCrossEdge() //compute all CrossEdge of inter vertex in Tcan
{
 int i, LCA_tmp;
 
 LCA(&(TcanNodeArr[interNum]));

 for (i=1;i<=en;i++){
  LCA_tmp = edgeArr[i].LCA;
  if (edgeArr[i].Weight > TcanNodeArr[LCA_tmp].maxCross)
   TcanNodeArr[LCA_tmp].maxCross = edgeArr[i].Weight;
 }//End for
}

void maxInsideEdge(TCANNODEPTR x)
{
  if (x->rightPtr != NULL)
  {
   maxInsideEdge(x->rightPtr);
   maxInsideEdge(x->leftPtr);
   x->maxInside = max(max(x->rightPtr->maxInside,x->leftPtr->maxInside),x->maxCross);
  }
  else
   x->maxInside = 0;
}

void Verify_Tcan()  // compute all inter vertex V in Tcan V.crossEdge
{  
  float scale = 1;
  maxInsideEdge(&(TcanNodeArr[interNum])); //computing all maxInsideEdge
  
  for (int i=vn+1; i<=interNum; i++)
  {
   if ((TcanNodeArr[i].maxInside < scale*TcanNodeArr[i].minBorder)||(i == interNum))
       TcanNodeArr[i].CS = YES; 
  }
}

int Descendant(int x, int y)
{
 int Boolean;
 if (x < y)
 {
  if (TcanNodeArr[x].parent == y)
   Boolean = 1;
  else 
   Boolean = Descendant(TcanNodeArr[x].parent,y);
 }  
 else
  Boolean = 0;
 return Boolean;
}

void TMarkNode(int x,int y) //make a TMarkNode
{
 int childNum;
 
 TMarkNodeArr[x].lma = y;
 TMarkNodeArr[x].index = x;
 TMarkNodeArr[y].index = y;
 TMarkNodeArr[y].childNum++;
 childNum = TMarkNodeArr[y].childNum;
 TMarkNodeArr[y].childPtr[childNum] = &(TMarkNodeArr[x]);
}

void LMA(int v) //LMA : lowest marked ancestor
{
 int x;
 
 if (v < vn) //v is leaf
//   Push(&stackPtr, v);
  nodeStack.push(v);
 else
 {
//   while(Descendant(Top(&stackPtr),v)) {
  while(Descendant(nodeStack.top(),v)) {    
//    x = Pop(&stackPtr);
   x = nodeStack.top();
   nodeStack.pop();
   TMarkNode( x, v);
  }
//   Push(&stackPtr, v);
   nodeStack.push(v);
 }
}

void TMark(TCANNODEPTR x) //constrcu mark vertex tree
{
 if (x->index != 0)  //except root node
 {
  if (x->leftPtr != NULL)
   TMark(x->leftPtr);
  if (x->rightPtr != NULL) 
   TMark(x->rightPtr);
  if (x->CS == YES)
   LMA(x->index);
 } 
}

void postTMark(TMARKNODEPTR x, FILE *csPtr)
{
 int i;
 
 if (x->childNum != 0){ //x is a inter node
  fprintf(csPtr, "(");
  for (i=1;i<= x->childNum;i++)
  {
   if ( x->childPtr[i] != NULL)
    postTMark(x->childPtr[i], csPtr);
   if (i != x->childNum) 
    fprintf(csPtr, ",");
  }  
  fprintf(csPtr, ")");
 } 
 else
  fprintf(csPtr, "%s",index2name[x->index-1].c_str());
}

void PrintCompactSetTree(char *cs_f)
{
 FILE *csPtr;
 
 if ((csPtr=fopen(cs_f,"w"))==NULL){
    printf("cannot open compactset file");exit(0);
 } 
 int i = interNum + 1;
//  Push(&stackPtr, i);
 nodeStack.push(i);
 TMark(&(TcanNodeArr[interNum]));
 postTMark(&(TMarkNodeArr[interNum]), csPtr); 
 fprintf(csPtr, ";\n");
// for(i = 1; i <= TMarkNodeArr[interNum].childNum; i++)
// {
// 	postTMark(TMarkNodeArr[interNum].childPtr[i]);
// 	printf(";\n");
// } 
  fclose(csPtr);  
}
