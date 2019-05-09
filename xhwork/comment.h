#ifndef _COMMENT_H
#define _COMMENT_H

BOOL AppStoreCustomerReviews(CString strUserAgent,
							 CString strCookie,
							 char* lpPasswordToken,
							 char* lpDsid,
							 char* lpSaleId,
							 TCHAR* lpX_Apple_I_md_m,
							 TCHAR* lpX_Apple_I_md,
							 TCHAR* lpProxy,
							 int netType);

BOOL AppStoreUserReviewRow(CString strUserAgent,
						   CString strCookie,
						   char* lpPasswordToken,
						   char* lpDsid,
						   char* lpSaleId,
						   TCHAR* lpX_Apple_I_md_m,
						   TCHAR* lpX_Apple_I_md,
						   TCHAR* lpProxy,
						   int netType);

BOOL AppStoreWriteUserReview(CString strUserAgent,
							 CString strCookie,
							 CString& strCommentCookie,
							 char* lpPasswordToken,
							 char* lpDsid,
							 char* lpSaleId,
							 char* lpAppExtVrsId,
							 CString& lpOutNickName,
							 TCHAR* lpX_Apple_I_md_m,
							 TCHAR* lpX_Apple_I_md,
							 TCHAR* lpProxy,
							 int netType);

BOOL AppStoreSaveUserReview(CString strUserAgent,
							CString strCookie,
							CString strCommentCookie,
							char* lpPasswordToken,
							 char* lpDsid,
							 char* lpSaleId,
							 char* lpAppExtVrsId,
							 int rating,
							 LPTSTR lpwNickname,
							 LPTSTR lpwTitle,
							 LPTSTR lpwBody,
							 char* pGUID,
							 TCHAR* lpX_Apple_I_md_m,
							 TCHAR* lpX_Apple_I_md,
							 TCHAR* lpProxy,
							 int netType);

///////下面两个函数是itunes12.1.1.4版本上评论内容的
BOOL CommentwriteUserReview(char* lpPasswordToken,
							char* lpDsid,
							char* lpSaleId,
							char* pGUID,
							TCHAR* lpProxy,
							int netType);

BOOL WriteCommentContentToServer(IN LPVOID lpPasswordToken,
									IN LPVOID lpDsid,
									IN LPVOID lpSaleId,
									IN int rating,
									IN LPTSTR lpwNickname,
									IN LPTSTR lpwTitle,
									IN LPTSTR lpwBody,
									char* pGUID,
									TCHAR* lpProxy,
									int netType);

#endif