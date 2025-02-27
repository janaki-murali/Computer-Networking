#include <stdio.h>
#include <string.h>

int main() {
 int nr, srcr, i, j, k, w, v, min;
 int costmatrix[100][100], dist[100], last[100];
 int flag[100];
 printf("\n Enter the number of routers: ");
 scanf("%d", &nr);
 printf("\n Enter the distance matrix values: \n");
 for(i=0;i<nr;i++){
  for(j=0;j<nr;j++){
   scanf("%d",&costmatrix[i][j]);
   if(costmatrix[i][j]<0)
    costmatrix[i][j]=1000;
  }
 }

 printf("\nEnter the source number: ");
 scanf("%d", &srcr);
 for(v=0;v<nr;v++){
  flag[v]=0;
  last[v]=srcr;
  dist[v]=costmatrix[srcr][v];
 }
 flag[srcr]=1;
 for(i=0;i<nr;i++){
  min=1000;
  for(w=0;w<nr;w++){
   if(!flag[w])
    if(dist[w]<min){
     v=w;
     min=dist[w];
    }
  }
  flag[v]=1;
  for(w=0;w<nr;w++){
   if(!flag[w])
    if(min+costmatrix[v][w]<dist[w]){
     dist[w]=min+costmatrix[v][w];
     last[w]=v;
    }
  }
 }
 for(i=0;i<nr;i++){
  printf("\n%d -> %d : Path Taken: %d", srcr,i, last[i]);
  w=i;
  printf("\nShortest path cost: %d", dist[i]);
 }
 printf("\n");
}
