#include "ssu_PR_simulator.h"
#include <stdio_ext.h>
//초기 페이지 값은 프레임 수 만큼 모두 -1 로 채울 것
void pageFrame_append(pageFrame *head,int frame, int page)
{
	pageFrame *pageFrame_cur;
	pageFrame *newFrame = (pageFrame *)malloc(sizeof(pageFrame));
	memset(newFrame, 0, sizeof(pageFrame));
	newFrame->page = page;
	newFrame->frame = frame;
	newFrame->next = NULL;
	if(head->next == NULL){

		head->next = newFrame;
	}
	else{
		pageFrame_cur = head->next;
		while(pageFrame_cur->next != NULL)
			pageFrame_cur = pageFrame_cur->next;
		
		pageFrame_cur->next = newFrame;

	}
}

//-1로 되어있는 곳, 또는 찾는 페이지 존재 등 에 사용
int pageFrame_find(pageFrame *head, int page)
{//페이지 번호로 프레임 번호 찾기
	pageFrame *pageFrame_cur;
	if(head->next == NULL)
		return -2;//error
	else{
		pageFrame_cur = head->next;
		while(pageFrame_cur!= NULL){
			if(pageFrame_cur->page==page)
				return pageFrame_cur->frame;
			pageFrame_cur = pageFrame_cur->next;
		}
	}
	return -1; //can't find

}
int pageFrame_find_frame(pageFrame *head, int frame)
{//프레임 번호로 페이지 찾기
	pageFrame *pageFrame_cur;
	if(head->next == NULL)
		return -2;//error
	else{
		pageFrame_cur = head->next;
		while(pageFrame_cur!= NULL){

			if(pageFrame_cur->frame==frame)
				return pageFrame_cur->page;
			pageFrame_cur = pageFrame_cur->next;
		}
	}
	return -1; //can't find

}
int pageFrame_print(pageFrame *head, FILE *fp)
{
	pageFrame *pageFrame_cur;
	if(head->next == NULL)
		return -1;//error
	else{
		pageFrame_cur = head->next;
		while(pageFrame_cur != NULL){
			if(pageFrame_cur->page<0){
				printf("\t");
				fprintf(fp, "\t");
			}
			else{
				printf("%d\t", pageFrame_cur->page);
				fprintf(fp,"%d\t", pageFrame_cur->page);
			}
			pageFrame_cur = pageFrame_cur->next;
		}
		
	}
	return 1;
}
int pageFrame_print_esc(pageFrame *head, FILE *fp, int fr[], int fd[])
{
	int i=0;
	pageFrame *pageFrame_cur;
	if(head->next == NULL)
		return -1;//error
	else{
		pageFrame_cur = head->next;
		while(pageFrame_cur != NULL){
			if(pageFrame_cur->page<0){
				printf("\t");
				fprintf(fp, "\t");
			}
			else{
				printf("%d(r:%d,d:%d)\t", pageFrame_cur->page, fr[i],fd[i]);
				fprintf(fp,"%d(r:%d,d:%d)\t", pageFrame_cur->page, fr[i], fd[i]);
				i++;
			}
			pageFrame_cur = pageFrame_cur->next;
		}
		
	}
	return 1;
}
void pageFrame_change(pageFrame *head, int curpage, int newpage)
{
	pageFrame *pageFrame_cur;
	if(head->next == NULL)
		return;
	else{
		pageFrame_cur = head->next;
		while(pageFrame_cur != NULL){
			if(pageFrame_cur->page==curpage){
				pageFrame_cur->page = newpage;
				return;
			}
			pageFrame_cur = pageFrame_cur->next;
		}
	}
	return;

}

//LRU에 사용되는 pageFrame_append함수 초기 t는 모두 -1로 설정한다.
void pageFrameLRU_append(pageFrameLRU *head,int frame, int page, int t)
{
	pageFrameLRU *pageFrame_cur;
	pageFrameLRU *newFrame = (pageFrameLRU *)malloc(sizeof(pageFrameLRU));
	memset(newFrame, 0, sizeof(pageFrameLRU));
	newFrame->page = page;
	newFrame->frame = frame;
	newFrame->t=t;
	newFrame->next = NULL;

	if(head->next == NULL){

		head->next = newFrame;
	}
	else{
		pageFrame_cur = head->next;
		while(pageFrame_cur->next != NULL)
			pageFrame_cur = pageFrame_cur->next;
		
		pageFrame_cur->next = newFrame;
	}
}
//LRU에 사용되는 체인지 함수
void pageFrameLRU_change(pageFrameLRU *head, int curpage, int newpage, int t)
{
	pageFrameLRU *pageFrame_cur;
	if(head->next == NULL)
		return;
	else{
		pageFrame_cur = head->next;
		while(pageFrame_cur != NULL){
			if(pageFrame_cur->page==curpage){
				pageFrame_cur->page = newpage;
				pageFrame_cur->t=t;
				return;
			}
			pageFrame_cur = pageFrame_cur->next;
		}
	}
	return;

}

//-1로 되어있는 곳, 또는 찾는 페이지 존재 등 에 사용
int pageFrameLRU_find(pageFrameLRU *head, int page)
{
	pageFrameLRU *pageFrame_cur;
	if(head->next == NULL)
		return -2;//error
	else{
		pageFrame_cur = head->next;
		while(pageFrame_cur!= NULL){
			if(pageFrame_cur->page==page)
				return pageFrame_cur->frame;
			pageFrame_cur = pageFrame_cur->next;
		}
	}
	return -1; //can't find

}
int pageFrameLRU_find_frame(pageFrameLRU *head, int frame)
{
	pageFrameLRU *pageFrame_cur;
	if(head->next == NULL)
		return -2;//error
	else{
		pageFrame_cur = head->next;
		while(pageFrame_cur!= NULL){
			if(pageFrame_cur->frame==frame)
				return pageFrame_cur->page;
			pageFrame_cur = pageFrame_cur->next;
		}
	}
	return -1; //can't find

}
int pageFrameLRU_print(pageFrameLRU *head, FILE *fp)
{
	pageFrameLRU *pageFrame_cur;
	if(head->next == NULL)
		return -1;//error
	else{
		pageFrame_cur = head->next;
		while(pageFrame_cur != NULL){
			if(pageFrame_cur->page<0){
				printf("\t");
				fprintf(fp, "\t");
			}
			else{
				printf("%d\t", pageFrame_cur->page);
				fprintf(fp,"%d\t", pageFrame_cur->page);
			}
			pageFrame_cur = pageFrame_cur->next;
		}
		
	}
	return 1;
}


