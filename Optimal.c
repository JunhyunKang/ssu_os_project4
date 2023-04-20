#include "ssu_PR_simulator.h"


int predict(int string[], int cur, int stringnum, int framenum, pageFrame *head);

int optimal(char *filename,int stream[], int framenum, int stringnum, int data, char *resultfile)
{
	FILE *fp_output;
	if((fp_output = fopen(resultfile, "a"))==NULL){
		printf("file open error for %s \n", resultfile);
		return -1;
	}
	printf("\n----------Optimal----------\n");
	fprintf(fp_output,"\n----------Optimal----------\n");

	int string[stringnum]; //프레임 배열
	int change=0, pageFault=0, hit=0; //바뀐횟수 페이지폴트 횟수
	int num = 0;
	pageFrame *head=(pageFrame *)malloc(sizeof(pageFrame));
	memset(head, 0, sizeof(pageFrame));
	memset(string, 0, stringnum*sizeof(int));
	if(data == 1){
		int i;
		printf("Stream: ");
		fprintf(fp_output,"Stream: ");

		for(i=0;i<stringnum;i++){
			printf("%d ",stream[i]);
			fprintf(fp_output,"%d ",stream[i]);

			string[i]=stream[i];
		}
		printf("\n");
		fprintf(fp_output,"\n");

	}
	if(data==2){
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
			
			for(i=0; i<argc;i++){
				string[num++]=atoi(argv[i]);
				
			}
		}
		fclose(fp);
	}
	for(num=0; num<framenum;num++){
		pageFrame_append(head, num, -1); //프레임 개수만큼 생성 -1로 초기화
	}
	for(num=0; num<stringnum;num++){
		int frame=0, page=0;
		printf("%d\t|\t", string[num]);
		fprintf(fp_output,"%d\t|\t", string[num]);//결과 저장

		if((frame=pageFrame_find(head,-1))>-1){//빈공간이 있을때, 페이지 폴트
			if(pageFrame_find(head, string[num])>-1){//이미 있을때
				hit++;
				printf("hit\n\t\t");
				fprintf(fp_output,"hit\n\t\t");
				pageFrame_print(head, fp_output);
				printf("\n");
				fprintf(fp_output,"\n");
				continue;
			}
			pageFault++;
			printf("page fault\n\t\t");
			fprintf(fp_output,"page fault\n\t\t");

			pageFrame_change(head, -1, string[num]); 	

		}
		else if((frame=pageFrame_find(head, string[num]))>-1)//이미 들어있을 때 페이지 폴트 아닐때 아무것도 안함
		{
			printf("hit\n\t\t");
			fprintf(fp_output,"hit\n\t\t");
			hit++;
		}
		else if((frame=pageFrame_find(head, string[num]))==-1)//꽉차서 교체 and page fault
		{
			pageFault++;
			change++;

			if((page=predict(string, num, stringnum, framenum, head))==0){
				printf("predict error\n");
				return -1;
			}
			printf("page fault, 교체 page: %d -> %d\n\t\t", page, string[num]);
			fprintf(fp_output,"page fault, 교체 page: %d -> %d\n\t\t", page, string[num]);

			pageFrame_change(head, page, string[num]); 
		}

		pageFrame_print(head, fp_output);
		printf("\n");
		fprintf(fp_output,"\n");


	}	
	
	printf("total page fault: %d\nhit rate: %.4f\npage fault rate: %.4f\n",pageFault, (double)hit/stringnum,(double)pageFault/stringnum);
	fprintf(fp_output,"total page fault: %d\nhit rate: %.4f\npage fault rate: %.4f\n",pageFault, (double)hit/stringnum,(double)pageFault/stringnum);
	
	fclose(fp_output);

	return 1;
}

int predict(int string[], int cur, int stringnum, int framenum, pageFrame * head)//바꿀 페이지  번호 알려줌
{
	int i,j, check;
	int change[stringnum];
	int mark[PAGE_STREAM];//어느 번호가 쓰이고 있는 지 마크해줄 배열
	int m;
	int c;
	int pred=0;
	memset(mark, 0, sizeof(int)*PAGE_STREAM);//마크 해줄 배열을 0으로 초기화

	for(i=0;i<framenum; i++){
		m=pageFrame_find_frame(head, i)-1; //현재 들어있는 페이지에 해당하는 배열요소를 -1로 해준다.
		mark[m]=-1; 
	}
	for(i=cur+1; i<stringnum; i++){
		m=string[i]-1;
		mark[m]=1; //현재 뒤에 부터 사용되는 페이지스트림정보를 갖고 해당 번호의 마커에 1을 할당한다.
		check=0;
		c=0;
		for(j=0; j<PAGE_STREAM ; j++){		
			if(mark[j]==-1){ //-1일 경우를 체크하고 이 체크가 1개이면 -1한개 남은 거고, 즉 제일 오랫동안 사용 안 될것이므로 optimal알고리즘을 구현할 수 있다.
				c=j;
				check++;
			}
		}
		if(check==1)//제일 오래 안 사용될페이지 번호를 리턴한다.
			return c+1;
	}
	for(j=0; j<PAGE_STREAM ; j++){ //만약 모두 다시 나오지 않는다면 그 중 안 나온 것중 제일 작은 번호으 페이지가 들어있는 페이지을 리턴한다.
			if(mark[j]==-1)
				return j+1;
	}

	return 0;
}
	

