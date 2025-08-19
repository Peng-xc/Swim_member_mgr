#ifndef MYLIB_H
#define MYLIB_H

#include <time.h>

#define MAX_MEMBERS 100
#define MAX_ADMINS 10
#define NAME_LEN 31
#define PHONE_LEN 15
#define ID_LEN 11
#define ADMIN_FILE_INFO "data/admin_in.bat"
#define MEMBER_INFO_FILE "data/member_in.bat"

typedef enum
{
	female,	   //女
	male,	   //男	
	secure     //保密
}Gender;

typedef enum {
	ANNUAL,    // 年卡
	QUARTERLY, // 季卡
	MONTHLY,   // 月卡
	CIKA	   // 次卡
} MemberType;

// 会员数据结构
typedef struct {
	char card_id[ID_LEN];       // 卡号
	unsigned int mid;       // 用户ID
	char name[NAME_LEN];        // 姓名
	Gender gender;              // 性别
	char birthday[11];          // 生日（YYYY-MM-DD）
	char phone[PHONE_LEN];      // 手机号
	time_t reg_time;            // 注册时间
	MemberType member_type;     // 会员类型
	time_t expiry_date;         // 有效期
	int times;		    //次数
	int state;		    //会员注销情况 1 注销 ，0 未注销
} Member;
// 场馆记录结构
typedef struct {
	char card_id[ID_LEN];       // 卡号
	time_t entry_time;          // 入场时间
	time_t exit_time;            // 离场时间（0表示尚未离场）
} VenueRecord;

// 管理员结构
typedef struct {
	char username[50];
	char password[17];
} Admin;

//会员表
typedef struct{
	Member* ms;
	size_t size;
} MemberList;
#endif
