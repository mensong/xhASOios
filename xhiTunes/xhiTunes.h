#ifndef BRANK_API
#define BRANK_API extern "C" _declspec(dllimport)
#endif

BRANK_API BOOL InitializeModule(LPVOID lpBuffer);			    //加载iTunes模块接口
BRANK_API void UnInitializeModule(void);					    //释放iTunes模块
BRANK_API BOOL GetMachineGUID(LPVOID lpGsaServices,LPVOID lpBuffer);				    //生成机器guid值
BRANK_API BOOL GetEncryptKbsyncValue(IN LPVOID lpGsaServices,LPVOID lpBuffer,char* lpDsid,LONGLONG dsid,bool bAuthLogin);	//得到kbsync值,登录成功后会得到dsid的值
BRANK_API LPVOID CreateGsaServicesObject();                                      //得到用于计算gsavervices的全局对象
BRANK_API VOID ReleaseGsaServicesObject(LPVOID lpGsaServices);
BRANK_API BOOL generateX_JS_SP_TOKEN_Value(LPVOID lpGsaServices,LPVOID lpSrcData,int dataLen,LPVOID lpOutData);//生成JS-SP-TOKE的值

BRANK_API BOOL generateSPXAppleActionSignature(IN LPVOID lpGsaServices,
											   IN LPVOID lpDsid,    //输入参数disd
											   IN LPVOID lpSaleId,  //输入参数appId
											   IN LPVOID lpTimeStamp,//时间戳
											   IN LPVOID lpXAppleStoreFront,//字体
											   IN LPVOID lpSignData, //输入参数 signature的值
											   IN int	signLength,  //输入signature的值长度
											   OUT LPVOID lpOutSignature,//输出参数actionsigature的值
											   OUT int& outSignLength,//输出参数的长度
											   IN BOOL bHighVersion);

BRANK_API BOOL generateCheckinacksarValue(IN LPVOID lpGsaServices,
										  IN LPVOID lpRentalbagResponse,
										  int bagResLen,
										  IN LPVOID lpCheckinsar,
										  int checksarLen,
										  OUT LPVOID lpCheckacksar);

BRANK_API BOOL generateRbsyncValue(IN LPVOID lpGsaServices,OUT LPVOID lpRbsyncValue);

BRANK_API BOOL generateClientData(IN LPVOID lpGsaServices,
								  IN LPVOID lpXappleMdData,//服务器下来的的x-apple-amd-data值
								  IN int xAppleMdDataLen,  //服务器下来的x-apple-amd-data值长度
								  IN LPVOID lpDsid,        //dsid的指针
								  OUT LPVOID lpClientData, //输出参数,clientData的值
								  OUT int& nvContext);     //输出参数,上下文的指针

BRANK_API BOOL generateCpimData(IN LPVOID lpGsaServices,
								IN LPVOID lpSpimData,      //服务器下来的spim的值
								IN int spimLen,            //spim的值长度
								OUT LPVOID lpCpimData,     //输出参数:cpim的值
								OUT int& nvContext);       //输出参数,上下文的指针

BRANK_API BOOL SetFinishProvisionData(IN LPVOID lpGsaServices,
									  IN LPVOID lpTKTransportKeyData,//TK,TransportKey的值,其中之一
									  IN int tkTransportKeyLen,      //TK,TransportKey的值长度其中之一
									  IN LPVOID lpPtmSettingInfoData, //ptm，SettingInfo的值其中一
									  IN int ptmSettingInfoLen,       //ptm，SettingInfo的长度其中一
									  IN int nvContext,               //上下文的指针
									  IN BOOL bGSAProvision);         //当值输入参数为SettingInfo和transportKey的值时，为FALSE

BRANK_API BOOL generateA2kValue(IN LPVOID lpGsaServices,OUT LPVOID lpA2kValue,OUT int& a2kDataLen);//输出参数,a2k的值和长度

BRANK_API BOOL generateM1Value(IN LPVOID lpGsaServices,
							   IN LPVOID lpAppleId,   //apple Id:XXXXX@XXXX.com
						       IN LPVOID lpPassword,//密码
							   IN int dw_i_data,    //i 的值
							   IN LPVOID lp_s_data, //s 的值
							   IN int s_data_Len,   // s 的长度
							   IN LPVOID lp_B_data, // B 的值
							   IN int B_data_Len,   // B 的长度
							   OUT LPVOID lpM1,     // 输出参数,M1的值
							   OUT int& m1DataLen); //输出参数M1的长度

BRANK_API BOOL setSrpReponseData(IN LPVOID lpGsaServices,
								 IN LPVOID lpSpd, //spd 的值
								 IN int spdLen, //spd 的长度
								 IN LPVOID lpM2,//M2 的值
								 IN int M2Len,  //M2 的长度
								 IN LPVOID lpNp,//np 的值
								 IN int npLen); //np 的长度

BRANK_API BOOL generateSrpPETPassword(IN LPVOID lpGsaServices,OUT LPVOID lpPetPassword,OUT int& petPasswordLen);//输出参数PET密码和长度

BRANK_API BOOL generateSrpHBToken(IN LPVOID lpGsaServices,OUT LPVOID lpHBToken,OUT int& hbTokenLen); //HBToken的值和长度

BRANK_API BOOL generateSrpAdsid(IN LPVOID lpGsaServices,OUT LPVOID lpAdsid,OUT int& adsidLen);// adsid的值和长度

BRANK_API BOOL generateXAppleHBToken(IN LPVOID lpGsaServices,OUT LPVOID lpXAppleHBToken,OUT int& hbTokenLen); //X-Apple-HB-Token的值和长度

BRANK_API void releaseGsaAllocate(IN LPVOID lpGsaServices); //释放GsaServices2内存

BRANK_API BOOL generateMidOtpValue(IN LPVOID lpGsaServices,IN LPVOID lpMidValue,IN LPVOID lpOtpValue);

BRANK_API BOOL generateX_Apple_AMD_Value(IN LPVOID lpGsaServices,char* lpDsid,char* lpXAppleAMDM,char* lpXAppleAMD);//输出参数amd-m和amd的值

BRANK_API BOOL generateX_Apple_I_MD_Value(IN LPVOID lpGsaServices,OUT LPVOID lpXAppleIMDM,OUT LPVOID lpXAppleIMD);//输出参数I_md-m和I_md的值

BRANK_API BOOL generateX_Apple_MD_Value(IN LPVOID lpGsaServices,IN LPVOID lpDsid,OUT LPVOID lpXAppleMDM,OUT LPVOID lpXAppleMD);//输出参数md-m和md的值

BRANK_API BOOL generateSbsyncValue(IN LPVOID lpGsaServices,
								   IN LPVOID lpDsid,          //dsid 的指针
								   IN LPVOID lpXApple_amd_m,  //amd-m的值
								   IN int amd_m_Len,          //amd-m的长度
								   OUT LPVOID lpSbsyncValue,  //输出参数sbsync的值
								   OUT int& sbsyncLen);       //输出参数sbsync的长度

BRANK_API void setKeyBagData(IN LPVOID lpGsaServices,IN LPVOID lpKeyBagData,IN int keyBagDataLen);

BRANK_API void setKeyBagDataEx(IN LPVOID lpGsaServices,IN LONGLONG dsid,IN LPVOID lpKeyBagData,IN int keyBagDataLen);

//登录时得到SignsapSetupBuffer的值，登录时调用的接口
BRANK_API int GetSignsapSetupBuffer(IN LPVOID lpGsaServices,
									LPVOID lpSignSap,//输入参数:signature的值
									int signSize,    //输入参数:大小
									LPVOID lpBuffer, //输出参数，存储得到的签名值
									int& OutDataLength,//输出参数，大小
									BOOL bSet,//输入参数
									LPVOID lpUserXmlInfo,//输入参数
									int userXmlSize);//输入参数

BRANK_API int  GetiTunesFunAddr_1(IN LPVOID lpGsaServices);
BRANK_API int  GetiTunesFunAddr_2(IN LPVOID lpGsaServices,char* lpUdid);

//得到准备注册之前的SignsapSetupBuffer值,注册时候的调用接口
BRANK_API int GetRegisterSignsapSetupBuffer(IN LPVOID lpGsaServices,
											LPVOID lpBuffer,//输入参数，第一次从服务器得到的sign-sap-setup-cert字符串传进这个参数
											int signSize,   //字符串长度
										    LPVOID lpXAppleActionSignature,//输出参数，算法生成的输出sign-sap-setup-buffer值
											int& outSignLen); //输出参数，签名数据的长度
									 
//得到注册时引导包真正的X-apple-signature的值，只有这个值正确了，注册才能跳转到继续页面往下进行注册
BRANK_API int GetRegisterSignupWizardXappleSignature(IN LPVOID lpGsaServices,
													 IN LPVOID lpBuffer,//输入参数,第二次交换从服务器得到的sign-sap-set-buffer字符串传进这个参数
													 IN int inLength,   //输入参数，sign-sap-set-buffer长度
													 OUT LPVOID sXappleSignature,//输出X-Apple-signature的值
													 OUT int& signDataLen);//X-Apple-signature的值的长度

//设置从网页来的数字签名X-Apple-ActionSignature的值
BRANK_API int SetXappleActionSignatureForPage(IN LPVOID lpGsaServices,LPVOID lpXAppleActionSignature,int signLen);


//得到Boundary的值
BRANK_API char* GetBoundaryValue();

//同意页面发送需要的xAppleActionSignature的值
BRANK_API int GenerateAgreeWebPageXAppleActionSignature(IN LPVOID lpGsaServices,
														IN LPVOID lpAgreeWebPageUUID,//输入参数,同意页面的uuid
													    IN LPVOID lpUdid,              //输入参数,udid的值
													    IN LPVOID lpAppleId,           //输入参数，注册时输入的邮箱账号
													    OUT LPVOID lpXAppleSignature,  //输出参数,生成的XAppleActionSignature
													    OUT int& signDataLen);         //输出参数,XAppleActionSignature的长度
