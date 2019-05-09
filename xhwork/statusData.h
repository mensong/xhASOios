#ifndef __STATUSDATA_H
#define __STATUSDATA_H
#define    MAX_ALLOC_SIZE          1024*8
#define	   ALLOCATE_SIZE		   1024*4
#define	   SIGNATURE_SIZE	       1024*2
#define	   EXECTUEFAILED           -6  //执行异常
#define    SPIM_NULL               -5  //SPIM值为空
#define	   OBJECT_NULL			   -4  //对象为空
#define	   MODULELOAD_FAILED	   -3  //模块加载失败
#define	   GSASERVEVICEOBJERROR    -2  //创建GSA对象失败
#define    DETECTDATA_FAILED       -1  //重定向失败
#define    STATUS_FAILED            0  //失败---->可重新做一次
#define    STATUS_SUCCESS           1  //成功---->可重新做一次
#define	   RE_LOGIN                 2  //重新登录
#define	   ACCOUNT_DISABLED		    3  //账号禁用
#define	   SP_AUTH_ERROR		    4  //需AMD权限
#define	   PASSWORD_ERROR		    5  //密码错误
#define	   SP_MD_AUTH_ERROR         6  //需要MD值
#define	   AUNKNOWN_ERROR           7  //未知错误
#define	   AMD_BUYPRODUCT_ERROR     8  //AMD值错误
#define	   GSA_FAILED				9  //GSA失败
#define	   LOGIN_FAILED				0xA//登录失败-->可重新做一次
#define	   DOWNLOADFAILED			0xB//下载失败-->可重新做一次
#define	   SEARCHFAILED				0xC//搜索失败-->可重新做一次
#define	   DOWNLOADSUCCESS			0xD//下载成功
#define	   SEARCHSUCCESS			0xE//搜索成功
#define	   COMMENTSUCCESS			0xF//评论成功
#define	   COMMENTFAILED			0x10//评论失败-->可重新做一次
#define    ACTIVEAPPFAILED			0x11//激活失败-->可重新做一次
#define	   EXECUTE_TIME_OUT			0x12//执行超时-->可重新做一次
#define	   LOGIN_SUCCESS			0x13//登录成功
#define    BUY_SUCCESS              0x14//购买成功
#define    BUY_FAILED               0x15//购买失败-->可重新做一次
#define    ACCOUNT_LOCKED           0x18 //账号被锁
#define	   ACCOUNT_FAULT			0x19 //账号信息不全
#define    NEED_SET_KEYBAG          0x20 //需要设置keybag的值
#define    CHANGE_CONDITION        0xBDE //messageCode:3038 条款和条件已经更改，需要同意确认
#define    BUY_LOGIN_VERIFY        0x818 //messageCode: 2072 购买时需要登录验证
#define    SUBMIT_PAID_BUY_VERIFY  0xC07 //messageCode:3079  提交付费购买验证
#define	   AGREE_BUY_VERIFY		   0x7E8 //messageCode:2024  继续购买需要验证
#define	   UNKNOWN_ERROR		   0x138A//failureTypecode:5002
#define	   UNKNOWN_DEVICE          0x3F0 //messagecode:1008 无法验证您的设备或电脑，请联系技术支持寻求帮助
#define	   NEED_ALOWAYS_PWD        0x4b5 //判断购买是否一致需要密码
#define    SAVE_PWD_FOR_FREE_APP   0x4b6 //为免费项目保存密码
#define	   ACCOUNT_STOP		       0x7D4 //messagecode:2004 账号停用
#define	   PRICE_MISMATCH          0x7E3 //messagecode:2019 价格不匹配，不能购买此项目
#define    CAPTCHA_REQUIRED        0x7E4 //messagecode:2020  (自定义代码)需要验证码


#define    PROXY_NETWORK           0x4

#endif