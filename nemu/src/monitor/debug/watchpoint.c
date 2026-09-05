#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
	int i;
	for(i = 0; i < NR_WP; i ++) {
		wp_pool[i].NO = i;
		wp_pool[i].next = &wp_pool[i + 1];
	}
	wp_pool[NR_WP - 1].next = NULL;

	head = NULL;
	free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
WP* new_wp(){
	if(free_==NULL) assert(0);
	WP* wp=free_;
	free_=free_->next;
	wp->next=head;
	head=wp;
	return wp;
}

void free_wp(WP* wp){
	WP* prev=head;
	if(wp==head) head=head->next;
	else{
		while(prev!=NULL&&prev->next!=wp){
			prev=prev->next;
		}
		if(prev==NULL){
			printf("watchpoint %d not found\n",wp->NO);
			return;
		}
		prev->next=wp->next;
	}
	wp->next=free_;
	free_=wp;
}

int wp_is_changed(){
	WP* wp=head;
	bool success;
	while(wp!=NULL){
		uint32_t val_=expr(wp->expression,&success);
		if(val_!=wp->val){
			wp->val=val_;
			return wp->NO;
		}
		wp=wp->next;
	}
	return -1;
}


