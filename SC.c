#include "ssu_PR_simulator.h"

int sc(char *filename,int stream[], int framenum, int stringnum, int data, char *resultfile)
{
	FILE *fp_output;
	if((fp_output=fopen(resultfile,"a"))==NULL){
		printf("file open error for %s\n", resultfile);
		return -1;
	}
	printf("\n----------Second Chance----------\n");
	fprintf(fp_output,"\n----------Second Chance----------\n");

	int string[stringnum]; //프레임 배열
	int pageFault=0, hit=0; //바뀐횟수 페이지폴트 횟수
	int num = 0, pointer=0; //pointer는 현재 제일 처음로 바뀔 프레임을 나타낸다. 
	int reference[framenum];//1이면 0으로 바꾸며 기회를 주고, 0이면 바뀐다.
	pageFrame *head=(pageFrame *)malloc(sizeof(pageFrame));
	memset(head, 0, sizeof(pageFrame));
	memset(string, 0, stringnum*sizeof(int));
	memset(reference, 0, framenum*sizeof(int));
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
		pageFrame_append(head, num, -1); //프레임 개수만큼 생성 -1로 초기화
	}

	for(num=0; num<stringnum;num++){
		int frame=0, page=0;
		int f;
		
		printf("%d\t|\t", string[num]);
		fprintf(fp_output,"%d\t|\t", string[num]);

		if((frame=pageFrame_find(head,-1))>-1){//빈공간이 있을때, 페이지 폴트
			if((f=pageFrame_find(head, string[num]))>-1){//이미 있을때 f에 프레임 번호 저장
				reference[f]=1; // access 되었으므로 1로 레퍼런스 비트르 교환
				hit++;
				printf("hit\n\t\t");
				fprintf(fp_output,"hit\n\t\t");
				pageFrame_print(head, fp_output);
				printf("\n");
				fprintf(fp_output,"\n");
				continue;
			}
			reference[frame]=0; //처음 올라온 것의 레퍼런스 비트는 0으로 초기화한다.
			pageFault++;
			pointer++;
			pointer%=framenum;//second chance를 위한 시작 위치를 알려주는 포인터 값을 하나 증가시키고 모듈러 연산을 해줘 원형으로 순환하게 함.
			printf("page fault\n\t\t");
			fprintf(fp_output,"page fault\n\t\t");
			pageFrame_change(head, -1, string[num]); 	
		}
		else if((frame=pageFrame_find(head, string[num]))>-1)//이미 들어있을 때 페이지 폴트 아닐때
		{
			reference[frame]=1; //access 되었으므로 1로 레퍼런스 비트를 교환
			printf("hit\n\t\t");
			fprintf(fp_output,"hit\n\t\t");
			hit++;
		}
		else if((frame=pageFrame_find(head, string[num]))==-1)//꽉차서 교체 and page fault
		{
			pageFault++;
			int i=0, j, temp;

			temp = pointer;
			while(reference[temp]==1){//레퍼런스 비트가 1인 대상들에게는 한번 더 기회가 주어진다.
				reference[temp]=0;//그대신 해당 레퍼런스 비트는 0으로 변경된다.		
				pointer++;//현재 포인터가 있는 위치부터 1씩 증가하게 찾으며 레퍼런스 비트가 0이 나올때까지 순환한다.
				pointer%=framenum;
				temp=pointer;
			}
			pointer++;//second chance를 위한 시작 위치를 알려주는 포인터 값을 하나 증가시키고 모듈러 연산을 해줘 원형으로 순환하게 함.
			pointer%=framenum;
			page= pageFrame_find_frame(head, temp);//프레임 가장 위 부터 오래된 것이므로 모듈러 연산을 통해 가장 오랜된 값을 찾아옴		
			reference[temp]=0; //처음 올라오는 페이지의 레퍼런스 비트는 0으로 초기화
			printf("page fault, 교체 page: %d -> %d\n\t\t", page, string[num]);
			fprintf(fp_output,"page fault, 교체 page: %d -> %d\n\t\t", page, string[num]);
			pageFrame_change(head, page, string[num]); //해당 페이지를 교체해준다
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


