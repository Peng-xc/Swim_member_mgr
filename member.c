#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "mylib.h"

#define VenueRecord_FILE_INFO "data/VenueRecord_info"

void member_out(const Member* m);
void member_entry(Member* m);


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
		member_entry(ml->ms + i);
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



//入场记录
void member_entry(Member* m)
{
	VenueRecord* vr1 = malloc(sizeof(VenueRecord));
	VenueRecord* vr2 = malloc(sizeof(VenueRecord));

	if(vr1 == NULL && vr2 == NULL)
	{
		perror("会员入场登记失败！");
		return;
	}
	vr1->mid = m->mid;
	vr1->entry_time = time(NULL);
	vr1->exit_time = 0;

	FILE* fp = fopen(VenueRecord_FILE_INFO, "a+b");

	if(fp == NULL) 
	{
		perror("会员入场登记失败!!");
		return;
	}

	fseek(fp, 0, SEEK_END);
	if(ftell(fp))
	{
		rewind(fp);
		while(fread(vr2, sizeof(VenueRecord), 1, fp) == 1)
		{
			if(vr1->mid == vr2->mid)
			{
				struct tm* tm1 = localtime(&vr1->entry_time);
				struct tm* tm2 = localtime(&vr2->entry_time);
				
				//离场或不在同一天刷卡登记一次入场
				if(vr2->exit_time != 0 || !( (tm1->tm_year == tm2->tm_year) 
							&& (tm1->tm_mon == tm2->tm_mon) 
							&& (tm1->tm_mday == tm2->tm_mday) ))
				{
					fwrite(vr1, sizeof(VenueRecord), 1, fp);
				}

			}
		}
	}
	else
	{
		rewind(fp);
 		fwrite(vr1, sizeof(VenueRecord), 1, fp);
	}

	fclose(fp);
	free(vr1);
	free(vr2);
}

