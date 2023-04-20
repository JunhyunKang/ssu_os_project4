#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <limits.h>
#include <ctype.h>
#include <dirent.h>
#include <math.h>

#define BUFSIZE 10000
#define PAGE_STREAM 30
#define MIN_PAGE_FRAME 3
#define MAX_PAGE_FRAME 10
#define MIN_PAGE_STRING 500

typedef struct pageFrame{
	int frame;
	int page;
	struct pageFrame *next;
} pageFrame;

typedef struct pageFrameLRU{
	int frame;
	int page;
	int t; //time 정보는 아니고, 반복문 돌면서 들어온 번째 수 지정 제일 작은 값을 교체할 것
	struct pageFrameLRU *next;
} pageFrameLRU;
// 위에 폹트 등 정보를 저장할지는 나중에 생각해보고
typedef struct info{
	int frame;
	int page;
	struct info *next;
} info;

int tokenize(char *input, char *argv[]);

void pageFrame_append(pageFrame *head, int frame, int page);
int pageFrame_find(pageFrame *head, int page);
int pageFrame_find_frame(pageFrame *head, int frame);
void pageFrame_change(pageFrame *head, int curPage, int newPage);
//void pageString_append(pageString *head, pageFrame *pageFrameList, int string);
int pageFrame_print(pageFrame *head, FILE *fp);
int pageFrame_print_esc(pageFrame *head, FILE *fp, int fr[], int fd[]);

//use in LRU
void pageFrameLRU_append(pageFrameLRU *head, int frame, int page, int t);
void pageFrameLRU_change(pageFrameLRU *head, int curpage, int newpage, int t);
int pageFrameLRU_find(pageFrameLRU *head, int page);
int pageFrameLRU_find_frame(pageFrameLRU *head, int frame);
int pageFrameLRU_print(pageFrameLRU *head, FILE *fp);
//optimal
int optimal(char *filename,int stream[], int framenum, int stringnum, int data, char *resultfile);
//fifo
int fifo(char *filename,int stream[], int framenum, int stringnum, int data, char *resultfile);
//lifo
int lifo(char *filename,int stream[], int framenum, int stringnum, int data, char *resultfile);
//lru
int lru(char *filename,int stream[], int framenum, int stringnum, int data, char *resultfile);
//lfu
int lfu(char *filename,int stream[], int framenum, int stringnum, int data, char *resultfile);
//sc
int sc(char *filename,int stream[], int framenum, int stringnum, int data, char *resultfile);
//esc
int esc(char *filename,int stream[], int framenum, int stringnum, int data, char *resultfile);
