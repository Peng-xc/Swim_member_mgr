#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "mylib.h"

#define VenueRecord_FILE_INFO "data/VenueRecord_info"

void member_out(const Member* m);
void member_entry(Member* m);
void member_exit(Member *m);


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

		// 提示用户选择操作
		printf("\n请选择操作：\n");
		printf("1. 入场登记\n");
		printf("2. 离场登记\n");
		printf("0. 返回\n");

		int choice;
		scanf("%d", &choice);
		getchar();

		switch(choice) {
			case 1:
				member_entry(ml->ms + i);
				break;
			case 2:
				member_exit(ml->ms + i);
				break;
			case 0:
				break;
			default:
				printf("无效选择\n");
		}
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
void member_entry(Member *m)
{
	time_t now = time(NULL);
	struct tm *now_tm = localtime(&now);
	int already_entered = 0;

	// 检查是否已入场
	FILE *fp = fopen(VenueRecord_FILE_INFO, "rb");
	if (fp) {
		VenueRecord vr;
		while (fread(&vr, sizeof(VenueRecord), 1, fp) == 1) {
			if (vr.mid == m->mid && vr.exit_time == 0) {
				struct tm *entry_tm = localtime(&vr.entry_time);
				if (entry_tm->tm_year == now_tm->tm_year &&
						entry_tm->tm_mon == now_tm->tm_mon &&
						entry_tm->tm_mday == now_tm->tm_mday) {
					already_entered = 1;
					break;
				}
			}
		}
		fclose(fp);
	}

	if (already_entered) {
		printf("今日已入场，无需重复登记！\n");
		return;
	}

	// 创建新入场记录
	VenueRecord new_record = {
		.mid = m->mid,
		.entry_time = now,
		.exit_time = 0  // 0表示尚未离场
	};

	// 追加写入文件
	fp = fopen(VenueRecord_FILE_INFO, "ab");
	if (!fp) {
		perror("登记入场失败");
		return;
	}

	fwrite(&new_record, sizeof(VenueRecord), 1, fp);
	fclose(fp);
	printf("入场登记成功！\n");
}


// 离场记录
void member_exit(Member *m)
{
	FILE *fp = fopen(VenueRecord_FILE_INFO, "r+b");
	if (!fp) {
		perror("打开场地记录失败");
		return;
	}

	VenueRecord vr;
	long last_unexited_pos = -1;

	// 查找最近未离场记录
	while (fread(&vr, sizeof(VenueRecord), 1, fp) == 1) {
		if (vr.mid == m->mid && vr.exit_time == 0) {
			last_unexited_pos = ftell(fp) - sizeof(VenueRecord);
		}
	}

	// 更新离场时间
	if (last_unexited_pos != -1) {
		fseek(fp, last_unexited_pos, SEEK_SET);
		vr.exit_time = time(NULL);
		fwrite(&vr, sizeof(VenueRecord), 1, fp);
		printf("离场登记成功！\n");
	} else {
		printf("无未离场记录！\n");
	}

	fclose(fp);
}
