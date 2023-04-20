#include "ssu_PR_simulator.h"
#include <stdio_ext.h>
#define MIN_SELECTION 1
#define MAX_SELECTION 3
#define STRMAX 256

int tokenize(char *input, char *argv[]);
int makeStringFile(char *filename, int stringnum, int framenum);

int main(void)
{	
	while(1){
		char *argv[10], *filename, *resultfile;
		char input[STRMAX];
		int argc;
		int i, framenum, data, stringnum;

		filename=(char *)malloc(STRMAX);
		resultfile=(char *)malloc(STRMAX);
		memset(filename, 0, STRMAX);
		memset(resultfile, 0, STRMAX);
		printf("Page Replacement 알고리즘 시뮬레이터를 선택하시요 (최대 3개) (종료: EXIT)\n(1) Optimal  (2) FIFO  (3) LIFO  (4) LRU  (5) LFU  (6) SC  (7) ESC  (8) ALL\n>> ");

		memset(input, 0, STRMAX);
		__fpurge(stdin);
		fgets(input, sizeof(input), stdin);

		input[strlen(input) - 1] = '\0';
		argc  = tokenize(input, argv);
		argv[argc] = (char *)0;
		
		if(argc<MIN_SELECTION || argc>MAX_SELECTION){
			printf("Usage error: The number of selection 1~3 or ALL or EXIT\n");
			continue;
		}
		 		
		if(!strcmp(argv[0],"EXIT"))
			break;


		printf("페이지 프레임의 개수를 입력하시오.(3~10) \n>> ");
		scanf("%d", &framenum);
		if(framenum<MIN_PAGE_FRAME || framenum>MAX_PAGE_FRAME){
			printf("ERROR: Only 3~10\n");
			continue;

		}
		printf("페이지 스트링 개수를 입력하시오. (최소 500)\n>> ");
		scanf("%d", &stringnum);
		if(stringnum<500){
			printf("ERROR: mininum is 500 strings\n");
			continue;
		}
	
		int stream[stringnum];
		memset(stream, 0, sizeof(int)*stringnum);
		printf("데이터 입력 방식을 선택하시오.\n(1) 랜덤생성, (2) 파일로 생성\n>> ");
		scanf("%d", &data);
		if(data==2){
			printf("페이지 스트링을 생성 및 저장할 파일 이름을 입력하시오.\n>> ");
			scanf("%s", filename);
			if(makeStringFile(filename, stringnum, framenum)<0)
				continue;
			printf("success!! simulator uses this file.\n");
		}
		else if(data==1){
			srand(time(NULL));
			for(i=0;i<stringnum;i++){
				stream[i]=((rand()%PAGE_STREAM)+1);
			}
		}
		if(data!=1 && data !=2){
			printf("ERROR only 1, 2\n");
			continue;
		}

		printf("결과 저장을 위한 파일이름을 입력하시오.\n>> ");
		scanf("%s", resultfile);
		if(access(resultfile, F_OK)!=-1){//미리 만든 결과 파일이 있을 경우
			printf("이미 존재하는 파일입니다. 다른 이름을 입력하세요.\n");
			continue;
		}



		if(!strcmp(argv[0], "8")){
			if(argc>1){
				printf("Only '(8) ALL'\n");
				continue;
			}
			optimal(filename,stream, framenum, stringnum, data, resultfile);
			fifo(filename,stream, framenum, stringnum, data, resultfile);
			lifo(filename,stream, framenum, stringnum, data, resultfile);
			lru(filename,stream, framenum, stringnum, data, resultfile);
			lfu(filename,stream, framenum, stringnum, data, resultfile);
			sc(filename,stream, framenum, stringnum, data, resultfile);
			esc(filename,stream, framenum, stringnum, data, resultfile);

		}
		else{
			for(i=0;i<argc;i++){
				if(!strcmp(argv[i], "1")){
					optimal(filename,stream, framenum, stringnum, data, resultfile);
				}
				else if(!strcmp(argv[i], "2")){
					fifo(filename,stream, framenum, stringnum, data, resultfile);

				}
				else if(!strcmp(argv[i], "3")){
					lifo(filename,stream, framenum, stringnum, data, resultfile);

				}
				else if(!strcmp(argv[i], "4")){
					lru(filename,stream, framenum, stringnum, data, resultfile);
				}
				else if(!strcmp(argv[i], "5")){
					lfu(filename,stream, framenum, stringnum, data, resultfile);

				}
				else if(!strcmp(argv[i], "6")){
					sc(filename, stream, framenum, stringnum, data, resultfile);
				}
				else if(!strcmp(argv[i], "7")){
					esc(filename,stream, framenum, stringnum, data, resultfile);

				}
				else{
					printf("ERROR: Only 1~7\n");
					break;
				}
			}
		}
		printf("--------------------종료-------------------\n");
	}
	
	exit(0);
}

int tokenize(char *input, char *argv[])//입력 문자열 띄어쓰기 기준으로 토크나이징
{
	char *ptr = NULL;
	int argc = 0;
	ptr = strtok(input, " ");

	while (ptr!=NULL){
		argv[argc++]=ptr;
		ptr=strtok(NULL, " ");
	}
	
	return argc;
}


//파일 생성
int makeStringFile(char *filename, int stringnum, int framenum){
	FILE *file;
	int argc;
	char *argv[1000000];
	char buf[BUFSIZE];
	if(access(filename, F_OK)!=-1){//미리 만든 스트림 파일이 있을 경우
		printf("이미 파일이 존재합니다.\n");
		file = fopen(filename, "r");
		while(fgets(buf, sizeof(buf), file) != NULL){
			argc=tokenize(buf, argv);
		}
		if(stringnum==argc){
			fclose(file);
			return 0;
		}
		printf("존재하는 파일에 있는 스트림 개수에 맞게 다시 입력해주세요.\n");
		printf("존재하는 파일의 스트림 개수는 : %d 입니다.\n", argc);
		fclose(file);
		return -1;
	}
	else{
		if((file=fopen(filename, "w+")) == NULL){
			fprintf(stderr,"fopen error for %s\n", filename);
			return -1;
		}
		int i;
		srand(time(NULL));
		for(i=0;i<stringnum;i++){
			int string = (rand()%(PAGE_STREAM))+1;
			fprintf(file,"%d ",string);
		}
	}
	fclose(file);	
	return 0;
}
		


