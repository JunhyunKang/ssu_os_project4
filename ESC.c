#include "ssu_PR_simulator.h"

int esc(char *filename,int stream[], int framenum, int stringnum, int data, char *resultfile)
{
	FILE *fp_output;
	if((fp_output=fopen(resultfile,"a"))==NULL){
		printf("file open error for %s\n", resultfile);
		return -1;
	}
	printf("\n----------Enhanced Second Chance----------\n");
	fprintf(fp_output,"\n----------Enhanced Second Chance----------\n");

	int string[stringnum]; //프레임 배열
	int pageFault=0, hit=0; //바뀐횟수 페이지폴트 횟수
	int num = 0, pointer=0; //pointer는 현재 제일 처음로 바뀔 프레임을 나타낸다. 
	int reference[PAGE_STREAM];//1이면 0으로 바꾸며 기회를 주고, 0이면 바뀐다.
	int dirtybit[stringnum];
	int fr[framenum], fd[framenum];
	int count=0;

	pageFrame *head=(pageFrame *)malloc(sizeof(pageFrame));
	memset(head, 0, sizeof(pageFrame));
	memset(string, 0, stringnum*sizeof(int));
	memset(reference, 0, PAGE_STREAM*sizeof(int));
	memset(dirtybit, 0, stringnum*sizeof(int));
	memset(fr, 0, framenum*sizeof(int));
	memset(fd, 0, framenum*sizeof(int));


	int r;
	srand(time(NULL));
	for(r=0;r<stringnum;r++){//d 비트만무작위로 0, 1 초기화 한다. r비트는 0으로 초기화. 
		dirtybit[r]=(rand()%(2));
	}


	if(data == 1){
		int i;
		printf("Stream: ");
		fprintf(fp_output,"Stream: ");
		for(i=0;i<stringnum;i++){
			printf("%d(r:%d,d:%d) ",stream[i], 0, dirtybit[i]);
			fprintf(fp_output,"%d(r:%d,d:%d) ",stream[i], 0, dirtybit[i]);
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
				printf("%d(r:%d,d:%d) ",string[i], 0, dirtybit[i]);
				fprintf(fp_output,"%d(r:%d,d:%d) ",string[i], 0, dirtybit[i]);
			}
		}
		printf("\n");
		fprintf(fp_output,"\n");

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
				reference[string[num]-1]=1; // access 되었으므로 1로 레퍼런스 비트르 교환
				hit++;
				printf("hit\n\t\t");
				fprintf(fp_output,"hit\n\t\t");
				

				fr[f]=1;//r bit 1
				fd[f]=dirtybit[num];//프레임에 나타나는 더티 비트에 페이지 스트링의 더티비트저장
					
				

				pageFrame_print_esc(head, fp_output, fr, fd);
				printf("\n");
				fprintf(fp_output,"\n");
				continue;
			}
			reference[string[num]-1]=1; //access 되었으므로 레퍼런스 비트는 1한다.
			pageFault++;
			
			fr[frame]=1;  //프레임에 나타는 레퍼런스비트에 1을저장 
			fd[frame]=dirtybit[num];//프레임에 나타나는 더티 비트에 페이지 스트링의 더티비트저장
		//	count++;
		//	count%=framenum;

			pointer++;
			pointer%=framenum;//enhanced second chance를 위한 시작 위치를 알려주는 포인터 값을 하나 증가시키고 모듈러 연산을 해줘 원형으로 순환하게 함.
			printf("page fault\n\t\t");
			fprintf(fp_output,"page fault\n\t\t");
			pageFrame_change(head, -1, string[num]); 	
		}
		else if((frame=pageFrame_find(head, string[num]))>-1)//이미 들어있을 때 페이지 폴트 아닐때
		{
			reference[string[num]-1]=1; //access 되었으므로 1로 레퍼런스 비트를 교환
			printf("hit\n\t\t");
			fprintf(fp_output,"hit\n\t\t");
			
			fr[frame]=1;  //프레임에 나타는 레퍼런스비트에 1을저장 
			fd[frame]=dirtybit[num];//프레임에 나타나는 더티 비트에 페이지 스트링의 더티비트저장

			hit++;
		}
		else if((frame=pageFrame_find(head, string[num]))==-1)//꽉차서 교체 and page fault
		{
			pageFault++;
			int i=0, j, temp;
			int rbitcount=0;

			temp = pointer;

			for(i=0;i<framenum;i++){
				if(fr[i]==1)
					rbitcount++;
			}
			if(rbitcount==framenum){  //r비트 모두 1되면 0으로 초기화
				for(i=0;i<framenum;i++){
					fr[i]=0;
				}
			}
			rbitcount=0;

			temp = pointer; //포인터 값부터 00차증로  돌고, 업으면  01 돈다


			for(i=temp;i<framenum;i=(i+1)%framenum){
				if(fr[i]==0&&fd[i]==0){
					pointer=i;
					break;
				}
				if(fr[i]==0&&fd[i]==1&&rbitcount>=framenum){ //처음 순환에는 00을 찾고 그 이후에 01을 찾는다. esc우선순위
					pointer=i;
					break;
				}
				
				rbitcount++;
			}
			

			page= pageFrame_find_frame(head, pointer);//프레임 가장 위 부터 오래된 것이므로 모듈러 연산을 통해 가장 오랜된 값을 찾아옴		
			reference[string[num]-1]=1; //올라오는 페이지의 레퍼런스 비트는 1
			fr[pointer]=1; //올라오는 r비트 1
			fd[pointer]=dirtybit[num];//해당 더티비트로 프레임의 더티비트에 알려줌
			printf("page fault, 교체 page: %d -> %d\n\t\t", page, string[num]);
			fprintf(fp_output,"page fault, 교체 page: %d -> %d\n\t\t", page, string[num]);
			pageFrame_change(head, page, string[num]); //해당 페이지를 교체해준다

			pointer++;//enhanced second chance를 위한 시작 위치를 알려주는 포인터 값을 하나 증가시키고 모듈러 연산을 해줘 원형으로 순환하게 함.
			pointer%=framenum;
		}

		pageFrame_print_esc(head, fp_output, fr, fd);
		printf("\n");
		fprintf(fp_output,"\n");
	}	
	
	printf("total page fault: %d\nhit rate: %.4f\npage fault rate: %.4f\n",pageFault, (double)hit/stringnum,(double)pageFault/stringnum);
	fprintf(fp_output,"total page fault: %d\nhit rate: %.4f\npage fault rate: %.4f\n",pageFault, (double)hit/stringnum,(double)pageFault/stringnum);

	fclose(fp_output);
	return 1;
}


