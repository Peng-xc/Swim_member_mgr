#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "mylib.h"

void member_out(const Member* m);


void member_show(MemberList* ml){
	

	char card_num[ID_LEN];

	system("clear");
	printf("会员请刷卡！\n");

	scanf("%10s", card_num);
	getchar();

	size_t i;
	for(i = 0; i < ml->size; i++)
	{
		if(strcmp(ml->ms[i].card_id, card_num) == 0) break;
	}

	if(i == ml->size) 
	{
		perror("该卡号没注册，请联系工作人员!\n");
		printf("\n按任意键继续......\n");
        	getchar();
		return;
	}

	if(ml->ms[i].state == 1)
	{
		printf("会员已注销！");
	}
	else
	{
		printf("会员信息如下：\n");
		member_out(ml->ms + i);
	}

	printf("\n按任意键继续......\n");
        getchar();


}

void member_out(const Member* m)
{
        printf("卡号: %s\n", m->card_id);
        printf("会员ID: %u\n", m->mid);
        printf("姓名: %s\n", m->name);

        // 性别显示
        const char* genders[] = {"女", "男", "保密"};
        printf("性别: %s\n", genders[m->gender]);

        printf("生日: %s\n", m->birthday);
        printf("手机号: %s\n", m->phone);

        // 时间格式化
        char reg_time_str[20], expiry_str[20];
        struct tm* tm_reg = localtime(&m->reg_time);
        strftime(reg_time_str, sizeof(reg_time_str), "%Y-%m-%d %H:%M", tm_reg);

	struct tm* tm_expiry = localtime(&m->expiry_date);
        strftime(expiry_str, sizeof(expiry_str), "%Y-%m-%d %H:%M", tm_expiry);

        printf("注册时间: %s\n", reg_time_str);

	// 会员类型显示
        const char* types[] = {"年卡", "季卡", "月卡", "次卡"};
        printf("会员类型: %s\n", types[m->member_type]);

        if(m->member_type != CIKA) {
                printf("有效期至: %s\n", expiry_str);
        } else {
                printf("剩余次数: %d\n", m->times);
        }


}

