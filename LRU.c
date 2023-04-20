#include "ssu_PR_simulator.h"


int min_t(pageFrameLRU *head);

int lru(char *filename,int stream[], int framenum, int stringnum, int data, char *resultfile)
{
	FILE *fp_output;
	if((fp_output=fopen(resultfile,"a"))==NULL){
		printf("file open error for %s\n", resultfile);
		return -1;
	}
	printf("\n----------LRU----------\n");
	fprintf(fp_output,"\n----------LRU----------\n");

	int string[stringnum]; //프레임 배열
	int change=0, pageFault=0, hit=0; //바뀐횟수 페이지폴트 횟수
	int num = 0;
	pageFrameLRU *head=(pageFrameLRU *)malloc(sizeof(pageFrameLRU));
	memset(head, 0, sizeof(pageFrameLRU));
	memset(string, 0, stringnum*sizeof(int));
	if(data == 1){
		int i;
		printf("Stream :");
		fprintf(fp_output,"Stream :");
		for(i=0;i<stringnum;i++){
			printf("%d ",stream[i]);
			fprintf(fp_output,"%d ",stream[i]);
			string[i]=stream[i];
		}
		printf("\n");
		fprintf(fp_output,"\n");
	}
	else{
		char buf[BUFSIZE];
		char *argv[stringnum];
		int i, length, argc;
		FILE *fp;
		if((fp=fopen(filename, "r"))==NULL){
			printf("file open error for %s \n", filename);
			return -1;
		}
		while(fgets(buf, sizeof(buf), fp) != NULL){
			argc=tokenize(buf, argv);
			stringnum=argc;
			for(i=0; i<argc;i++){
				string[num++]=atoi(argv[i]);
			}
		}
		fclose(fp);
	}

	for(num=0; num<framenum;num++){
		pageFrameLRU_append(head, num, -1, -1); //프레임 개수만큼 생성 -1로 초기화, 초기t도 -1로 초기화
	}

	for(num=0; num<stringnum;num++){//num을 이용해 t값을 업데이트 한다. 그러므로 t가 가장 작은 수가 사용된지 가장 오래된 페이지다.
		int frame=0, page=0;
		printf("%d\t|\t", string[num]);
		fprintf(fp_output,"%d\t|\t", string[num]);

		if((frame=pageFrameLRU_find(head,-1))>-1){//빈공간이 있을때, 페이지 폴트
			if(pageFrameLRU_find(head, string[num])>-1){//이미 있을때
				hit++;
				pageFrameLRU_change(head, string[num], string[num], num); //t값만 업데이트 해 최근 사용되었음을 알려줌
				printf("hit\n\t\t");
				fprintf(fp_output,"hit\n\t\t");
				pageFrameLRU_print(head, fp_output);
				printf("\n");
				fprintf(fp_output,"\n");
				continue;
			}
			pageFault++;
			printf("page fault\n\t\t");
			fprintf(fp_output,"page fault\n\t\t");

			pageFrameLRU_change(head, -1, string[num], num); // t값으로 최근 사용됨을 알려	

		}
		else if((frame=pageFrameLRU_find(head, string[num]))>-1)//이미 들어있을 때 페이지 폴트 아닐때 t값만 업데이트 해줘 최근 사용되었음을 알려줌
		{
			printf("hit\n\t\t");
			fprintf(fp_output,"hit\n\t\t");
			pageFrameLRU_change(head, string[num], string[num], num);
			hit++;
		}
		else if((frame=pageFrameLRU_find(head, string[num]))==-1)//꽉차서 교체 and page fault
		{
			pageFault++;
			change++;

			if((page=min_t(head))==-2){
				printf("predict error\n");
				return -1;
			}
			printf("page fault, 교체 page: %d -> %d\n\t\t", page, string[num]);
			fprintf(fp_output,"page fault, 교체 page: %d -> %d\n\t\t", page, string[num]);

			pageFrameLRU_change(head, page, string[num], num); // t의 최소값페이지를 찾아 교체하면서 t값을 업데이트 해 최근에 사용되었음을 알려줌
		}
		pageFrameLRU_print(head,fp_output);
		printf("\n");
		fprintf(fp_output,"\n");
	}	
	
	printf("total page fault: %d\nhit rate: %.4f\npage fault rate: %.4f\n",pageFault, (double)hit/stringnum,(double)pageFault/stringnum);
	fprintf(fp_output,"total page fault: %d\nhit rate: %.4f\npage fault rate: %.4f\n",pageFault, (double)hit/stringnum,(double)pageFault/stringnum);
	fclose(fp_output);
	return 1;
}

int min_t(pageFrameLRU * head)//바꿀 페이지 프레임  번호 알려줌
{
	int min=INT_MAX;
	int change=-1;
	pageFrameLRU *cur;
	if(head->next == NULL)
		return -2;
	else{
		cur = head->next;
		while(cur != NULL){
			if(cur->t < min){
				min = cur->t;  //t 의 값이 최소일 때 즉 사용된지 가장 오래된 페이지의 프레임을 저장한다.
				change = cur->page;
			}
			cur = cur->next;
		}
	}
	return change; //가장 사용된지 오래된 페이지프레임번호를 리턴한다.
}
	

