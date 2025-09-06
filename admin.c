#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <limits.h>
#include "mylib.h"

#define MEMBER_ID_BASE 100001

static void addMember(MemberList* ml);
static void updateMember(MemberList* ml);
static void del_member(MemberList* ml);
void printMemberInfo(const Member* m);
void member_view(MemberList* ml);
static void find_VR(void);
static void update_password(Admin* admin);

//管理员登录认证
Admin* admin_login(Admin* admin, Admin* admin_in)
{
	if(!admin|| !admin_in) return 0; 

	FILE* fp = fopen(ADMIN_FILE_INFO, "rb");
	
	if(fp == NULL)
	{
		perror("用户名或密码错误！");
		return 0;
	}
	//获取管理员个数
	fseek(fp, 0, SEEK_END);
	size_t size = ftell(fp) / sizeof(Admin);
	rewind(fp);
	
	//从文件中读取管理员
	if(size > 0 && fread(admin, sizeof(Admin), size, fp) == size)
	{
		int i;
		for(i = 0; i < size; i++)
		{
			if(strcmp(admin[i].username, admin_in->username) == 0 && strcmp(admin[i].password, admin_in->password)==0)
				return admin_in;
		}
	}
	fclose(fp);
	return NULL;	
}


//管理员菜单
void admin_print_menu()
{
	system("clear");
	printf("\n———-———管理员页面———————\n");

	printf("\n操作说明如下：\n");
	printf("1. 新会员注册\n");
	printf("2. 会员注销\n");
	printf("3. 修改会员信息\n");
	printf("4. 查看会员信息\n");
	printf("5. 查看会员出入记录\n");
	printf("6. 修改密码\n");
	printf("7. 新增管理员\n");
	printf("8. 注销登录\n");
	printf("0. 退出系统\n");

	printf("\n————————————————————————\n");
}

//管理员主页面
int admin_menu(Admin* admin, MemberList* ml)                               
{
	int choice, c;
	do
	{   
		admin_print_menu();
		printf("\n:>");
		scanf("%d", &choice);
		while((c = getchar()) != '\n' && c != EOF );

		switch(choice)
		{   
			case 1:
				//添加会员
				addMember(ml);
				break;
			case 2:
				//注销会员
				del_member(ml);
				break;
			case 3:
				//修改会员信息
				updateMember(ml);
				break;
			case 4:
				//查看会员信息
				member_view(ml);
				break;
			case 5:
				//查看会员出入信息
				find_VR();
				break;
			case 6:
				//修改密码
				update_password(admin);
				break;
			case 7:
				//新增管理员
				break;
			case 8:
				//注销登录
				return 0;
			case 0:
				//退出系统
				return 1;
			default:
				printf("无此功能选项，请输入已有功能选项！\n");
		}
	}while(1);
}

//生成用户ID
static unsigned int get_new_mid(void)
{
	Member m;
	unsigned int mid;

	FILE* fp = fopen(MEMBER_INFO_FILE, "rb");

	if(fp == NULL)
	{
		return MEMBER_ID_BASE;
	}

	fseek(fp, -sizeof(m), SEEK_END);

	if(fread(&m, sizeof(m), 1, fp) == 1)
	{
		mid = m.mid + 1;
	}
	else
	{
		mid = MEMBER_ID_BASE;
	}

	fclose(fp);

	return mid;
}

//新增会员
static void addMember(MemberList* ml)
{
	if(!ml || ml->size >= MAX_MEMBERS)
	{
		fprintf(stderr, "无效列表或会员容量已满");
		return;
	}

	Member* m = &ml->ms[ml->size];

	system("clear");
	printf("\n---------会员注册---------\n");
	printf("\n请按照如下提示输入会员信息：\n");
	printf("卡号：");
	scanf("%10s", m->card_id);

	printf("姓名：");
	scanf("%30s", m->name);

	int sex;
	printf("性别(0-女， 1-男，2-保密)：");
	scanf("%d", &sex);
	if(sex >=0 && sex <=2)
	{
		m->gender = (Gender)sex;
	}else{
		printf("无效性别选择，默认为保密\n");
		m->gender = secure;
	}

	printf("生日（YYYY-MM-DD）：");
	scanf("%10s", m->birthday);

	printf("手机号：");
	scanf("%11s", m->phone);

	m->reg_time = time(NULL); //获取当前时间

	//会员类型
	int type;
	printf("会员类型(0-年卡, 1-季卡, 2-月卡, 3-次卡)：");

	scanf("%d", &type);
	switch(type)
	{
		case 0: m->member_type = ANNUAL;
			break;
		case 1: m->member_type = QUARTERLY;
			break;
		case 2: m->member_type = MONTHLY;
			break;
		case 3: m->member_type = CIKA;
			break;
		default: printf("无此卡型");
	}

	//有效期
	switch(m->member_type)
	{
		case ANNUAL:
			m->expiry_date = m->reg_time + 365*24*3600;
			break;
		case QUARTERLY: 
			m->expiry_date = m->reg_time + 120*24*3600;
			break;
		case MONTHLY:
			m->expiry_date = m->reg_time + 30*24*3600;
			break;
		default:
			break;
	}
	if(m->member_type == CIKA) 
	{
		printf("购买次数：");
		scanf("%d", &m->times);
	}
	m->state = 0;

	m->mid = get_new_mid();

	FILE* fp = fopen(MEMBER_INFO_FILE, "ab");

	if(fp == NULL)
	{
		perror("会员注册失败");
		return;
	}

	fwrite(m, sizeof(Member), 1, fp);

	fclose(fp);

	printf("该会员注册成功！！！");

	ml->size++;

}

//打印会员信息
void printMemberInfo(const Member* m)
{
	if(!m) return;

	system("clear");
	printf("\n====== 会员详细信息 ======\n");
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

	// 会员状态
	printf("状态: %s\n", m->state ? "已注销" : "正常");
	printf("===========================\n");


	printf("按任意键继续......\n");
	getchar();
}

//条件查询会员信息
Member* findMember(MemberList* ml, const char* k)
{
	if (!ml || !k || ml->size == 0) return NULL;

	for (int i = 0; i < ml->size; i++) {
		Member* m = &ml->ms[i];

		// 将会员ID转换为字符串进行比较
		char id_str[20];
		snprintf(id_str, sizeof(id_str), "%u", m->mid);

		// 检查三种匹配方式
		if (strcmp(m->card_id, k) == 0 || 
				strcmp(m->phone, k) == 0 || 
				strcmp(id_str, k) == 0) 
		{
			return m;
		}
	}
	return NULL;
}

//打印所有会员信息
static void member_all_print(MemberList* ml)
{
	int i;
	printf("=========所有会员信息=========\n\n");
	printf("%-10s\t%s\t%s\t%s\t%-11s\t%-11s\t注册时间\t%16s\t%-16s\t%20s\t会员状态\n", "卡号", "会员ID", "姓名", "性别","生日", "手机号","会员类型", "有效期","剩余次数");
	
	const char* genders[] = {"女", "男", "保密"};
	char reg_time_str[20], expiry_str[20];
	struct tm* tm_reg, *tm_expiry;
        const char* types[] = {"年卡", "季卡", "月卡", "次卡"};


	for(i = 0; i < ml->size; i++)
	{
		printf("%s\t", ml->ms[i].card_id);
		printf("%u\t", ml->ms[i].mid);
		printf("%s\t", ml->ms[i].name);
		
		//性别
        	printf("%s\t", genders[ml->ms[i].gender]);
		

		//生日
		printf("%s\t", ml->ms[i].birthday);
		printf("%s\t", ml->ms[i].phone);
		
		tm_reg = localtime(&(ml->ms[i].reg_time));
		strftime(reg_time_str, sizeof(reg_time_str), "%Y-%m-%d %H:%M", tm_reg);

        	tm_expiry = localtime(&(ml->ms[i].expiry_date));
        	strftime(expiry_str, sizeof(expiry_str), "%Y-%m-%d %H:%M", tm_expiry);

		//注册时间
        	printf("%s\t", reg_time_str);

        	// 会员类型显示
        	printf("%s\t", types[ml->ms[i].member_type]);

        	if(ml->ms[i].member_type != CIKA) {
                	printf("%s\t", expiry_str);
			printf("%-12s\t","-");
        	} else {
			printf("%-19s\t","-");
                	printf("%-12d\t", ml->ms[i].times);
        	}

        	// 会员状态
        	printf("%s\n", ml->ms[i].state ? "已注销" : "正常");

	}
	printf("============================================\n");
}

//查询会员信息功能
void member_view(MemberList* ml)
{
	member_all_print(ml);

	if (!ml || ml->size == 0) {
		printf("会员列表为空！\n");
		return;
	}
	printf("是否查询某个会员信息（y/n）:");
	if(getchar() == 'y')
	{
		char k[50] = {0};
		printf("请输入卡号/会员ID/手机号: ");
		if (scanf("%49s", k) != 1) {
			printf("输入无效！\n");
			return;
		}
		getchar();

		Member* found = findMember(ml, k);
		found ? printMemberInfo(found) : printf("无此会员信息！\n");
	}
}

//修改会员信息
static void updateMember(MemberList* ml)
{
	if (!ml || ml->size == 0) {
		printf("会员列表为空！\n");
		return;
	}

	char k[50] = {0};
	printf("请输入卡号/会员ID/手机号: ");
	if (scanf("%49s", k) != 1) {
		printf("输入无效！\n");
		return;
	}
	getchar();

	Member* found = findMember(ml, k);
	if (!found) {
		printf("无此会员信息！\n");
		return;
	}

	int choice;
	do {
		system("clear");
		printf("\n====== 修改会员信息 ======\n");
		printf("当前会员: %s (ID: %u)\n", found->name, found->mid);
		printf("1. 修改卡号\n");
		printf("2. 修改姓名\n");
		printf("3. 修改性别\n");
		printf("4. 修改生日\n");
		printf("5. 修改手机号\n");
		printf("6. 修改会员类型\n");
		printf("0. 返回上级菜单\n");
		printf("===========================\n");
		printf("请选择: ");

		if (scanf("%d", &choice) != 1) {
			while(getchar() != '\n'); // 清除无效输入
			continue;
		}
		getchar(); // 清除换行符

		switch(choice) {
			case 1:
				printf("新卡号: ");
				scanf("%10s", found->card_id);
				break;

			case 2:
				printf("新姓名: ");
				scanf("%30s", found->name);
				break;

			case 3:
				printf("性别(0-女,1-男,2-保密): ");
				int sex;
				if (scanf("%d", &sex) == 1 && sex >=0 && sex <=2) 
					found->gender = (Gender)sex;
				else
					printf("无效输入！\n");
				break;

			case 4:
				printf("生日(YYYY-MM-DD): ");
				scanf("%10s", found->birthday);
				break;

			case 5:
				printf("新手机号: ");
				scanf("%14s", found->phone);
				break;

			case 6:
				printf("会员类型(0-年卡,1-季卡,2-月卡,3-次卡): ");
				int type;
				if (scanf("%d", &type) == 1 && type >=0 && type <=3) 
				{
					found->member_type = (MemberType)type;

					// 处理有效期
					if (found->member_type != CIKA) 
					{
						time_t now = time(NULL);
						switch(found->member_type) 
						{
							case ANNUAL: 
								found->expiry_date = now + 365 * 24 * 3600; 
								break;

							case QUARTERLY: 
								found->expiry_date = now + 120 * 24 * 3600; 
								break;

							case MONTHLY: 
								found->expiry_date = now + 30 * 24 * 3600; 
								break;

							default: 
								break;
						}
						found->times = 0; // 非次卡重置次数
					} 
					else 
					{
						printf("购买次数: ");
						scanf("%d", &found->times);
						found->expiry_date = 0; // 次卡无有效期
					}
				} 
				else 
					printf("无效类型！\n");
				break;

			case 0:
				break;

			default:
				printf("无效选项！\n");
		}

		// 保存修改到文件
		FILE* fp = fopen(MEMBER_INFO_FILE, "r+b");
		if (fp) 
		{
			fseek(fp, 0, SEEK_SET);
			for (int i = 0; i < ml->size; i++) 
			{
				if (fwrite(&ml->ms[i], sizeof(Member), 1, fp) != 1) 
				{
					perror("写入文件失败");
					break;
				}
			}
			fclose(fp);
			printf("修改已保存！\n");
		} 
		else 
		{
			perror("无法打开会员文件");
		}

	} while (choice != 0);
}


//会员注销
static void del_member(MemberList* ml)
{
	if (!ml || ml->size == 0) 
	{
                printf("会员列表为空！\n");
                return;
        }

        char k[50] = {0};
        printf("请输入卡号/会员ID/手机号: ");
        if (scanf("%49s", k) != 1) 
	{
                printf("输入无效！\n");
                return;
        }
        getchar();

        Member* found = findMember(ml, k);

	found->state = 1;

	// 保存修改到文件
	FILE* fp = fopen(MEMBER_INFO_FILE, "r+b");
	if (fp) 
	{
		fseek(fp, 0, SEEK_SET);
		for (int i = 0; i < ml->size; i++) 
		{
			if (fwrite(&ml->ms[i], sizeof(Member), 1, fp) != 1) 
			{
				perror("写入文件失败");
				break;
			}
		}
		fclose(fp);
		printf("注销成功！\n");
	} 
	else 
	{
		perror("无法打开会员文件");
	}


}


//查看会员入场离场记录
static void find_VR(void)
{
	VenueRecord vr;
	FILE* fp = fopen(VenueRecord_FILE_INFO, "rb");

	if(fp)
	{
		struct tm* tm_entry, *tm_exit; 
		char entry_str[20], exit_str[20];
		
		printf("=======会员入场和离场信息======\n");
		printf("%-20s\t%-20s\t%-20s\t\n","会员ID","入场时间", "离场时间");
		
		while(fread(&vr, sizeof(VenueRecord), 1, fp) == 1)
		{
			printf("%-20u\t", vr.mid);

			//入场时间
			tm_entry = localtime(&vr.entry_time);
			strftime(entry_str, sizeof(entry_str), "%Y-%m-%d %H:%M", tm_entry);

			printf("%-20s\t", entry_str);

			//离场时间
			tm_exit = localtime(&vr.entry_time);
                        strftime(exit_str, sizeof(exit_str), "%Y-%m-%d %H:%M", tm_exit);
		
			printf("%-20s\n", exit_str);
		}
		
		printf("=============================\n");

	}
	else
	{
		printf("无会员入场信息！\n");
	}

	fclose(fp);
	
	printf("请按任意键继续.....\n");
	getchar();
}



//修改密码
static void update_password(Admin* admin)
{
	FILE* fp = fopen(ADMIN_FILE_INFO, "r+b");
	
	if(fp == NULL)
	{
		perror("无法打开管理员文件");
		return;
	}

	Admin am;
	int8_t found;
	char p1[17], p2[17];

	while(1)
	{
		printf("新的密码：");
		fgets(p1, sizeof(p1), stdin);
		p1[strcspn(p1,"\n")] = '\0';

		printf("重新输入新的密码：");
		fgets(p2, sizeof(p2), stdin);
		p2[strcspn(p1,"\n")] = '\0';

		if(strcmp(p1, p2) == 0)
		{
			// 更新当前内存中的密码
			strcpy(admin->password, p1);

			// 更新文件中的管理员密码
			while(fread(&am, sizeof(Admin), 1, fp) == 1)
			{
				if(!strcmp(am.username, admin->username))
				{
					fseek(fp, -sizeof(Admin), SEEK_CUR);
					fwrite(admin, sizeof(Admin), 1, fp);

					found = 1;
					break;
				}
			}
			if (found) 
			{
				printf("修改密码成功！\n");
			} 
			else 
			{
				printf("未找到管理员记录！\n");
			}

			fclose(fp);
			
			printf("按回车键继续...\n");
			getchar();
			
			return;
		}
		else
		{
			perror("两次密码不匹配，请重新输入！\n");
		}

	}
}
