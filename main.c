#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include "mylib.h"
#include "admin.h"


void init(MemberList* ml,Admin** admin);
void destroy(MemberList* ml, Admin** admin);
void main_menu(MemberList* ml, Admin* admin);

void member_show(MemberList* ml);

int main()
{
	//初始化
	Admin* admin = NULL;
	MemberList ml;
	init(&ml, &admin);
	//加载数据
	
	//主菜单
	main_menu(&ml, admin);

	//保存数据
	
	//销毁
	destroy(&ml, &admin);
	return 0;
}

//初始化
void init(MemberList* ml,Admin** admin)
{
	ml->ms = calloc(MAX_MEMBERS, sizeof(Member));
	*admin = calloc(MAX_ADMINS, sizeof(Admin));
	if(ml->ms == NULL || *admin == NULL)
	{
		printf("calloc fail");
		return;
	}

	//加载会员信息
	FILE* mfp = fopen(MEMBER_INFO_FILE, "rb");
	if(mfp == NULL)
	{

		ml->size = 0;
		perror("会员信息初始化失败！");
		return;
	}

	fseek(mfp, 0, SEEK_END);
	size_t size = ftell(mfp) / sizeof(Member);
	rewind(mfp);
	if(fread(ml->ms, sizeof(Member), size, mfp) == size)
	{
		ml->size = size;
	}else{
		perror("加载会员文件数据失败：");
		ml->size = 0;
		fclose(mfp);
		return;
	}
	
	fclose(mfp);

	//加载管理员信息
	FILE* afp = fopen(ADMIN_FILE_INFO,"a+b");

	if(afp == NULL)
	{
		perror("初始管理员失败！！！");
		return;
	}	
	fseek(afp, 0, SEEK_END);
	if(ftell(afp) == 0)
	{
		rewind(afp);
		strcpy((*admin)[0].username, "admin");
		strcpy((*admin)[0].password, "123456");
		fwrite(*admin, sizeof(Admin), 1, afp);
		fflush(afp);
	}
	else
	{
		rewind(afp);
		fread(*admin, sizeof(Admin), 1, afp);
	}

	fclose(afp);
	

	

}
//销毁
void destroy(MemberList* ml,Admin** admin)
{
	if(ml->ms) free(ml->ms);
	if(*admin) free(*admin);

	ml->ms = NULL;
	*admin = NULL;
}

//主菜单
void main_menu(MemberList* ml, Admin* admin)
{
	int choice,c;
	Admin admin_in;
	do
	{
		system("clear");
		printf("\n======欢迎使用本系统！======\n");
		printf("\n1. 会员服务\n");
		printf("2. 管理员登录\n");
		printf("0. 退出系统\n");
		printf("\n===========================\n");
		printf("\n:>");
		scanf("%d", &choice);
		while((c = getchar()) != '\n' && c != EOF);

		switch(choice)
		{
			case 1:
				//会员服务
				member_show(ml);
				break;
			case 2:
				//管理员界面
				printf("\n——————管理员登录——————\n");
				printf("请输入用户名：");
				scanf("%s", admin_in.username);
				printf("请输入密码：");
				scanf("%16s", admin_in.password);

				if(admin_login(admin, &admin_in))
				{
					if(admin_menu(admin, ml))
					{
						return;
					}
				}
				else
				{
					perror("用户名或密码错误！！！\n");
				}
				break;
			case 0://退出系统
				return;
			default:
				printf("无此选项,请重新输入\n");
		}
	}while(choice != 0);

}

