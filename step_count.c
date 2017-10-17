
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>


#include <opencv/cv.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


#define NONE		 "\033[m" 
#define RED 		 "\033[0;32;31m" 
#define LIGHT_RED	 "\033[1;31m" 
#define GREEN		 "\033[0;32;32m" 
#define LIGHT_GREEN  "\033[1;32m" 
#define BLUE		 "\033[0;32;34m" 
#define LIGHT_BLUE	 "\033[1;34m" 
#define DARY_GRAY	 "\033[1;30m" 
#define CYAN		 "\033[0;36m" 
#define LIGHT_CYAN	 "\033[1;36m" 
#define PURPLE		 "\033[0;35m" 
#define LIGHT_PURPLE "\033[1;35m" 
#define BROWN		 "\033[0;33m" 
#define YELLOW		 "\033[1;33m" 
#define LIGHT_GRAY	 "\033[0;37m" 
#define WHITE		 "\033[1;37m"


#if 1

#define dXDebugMinVal

#ifdef dXDebugMinVal
#define DEBUG_PRINTF_MIN(x) printf x
#define DEBUG_PRINTF_MAX(x) 
#else 
#define DEBUG_PRINTF_MIN(x) 
#define DEBUG_PRINTF_MAX(x) printf x
#endif

#define dXStartWalkStep	10

#define dXUseCalSleep

typedef void (*SleepCallback)(unsigned int tInputStartTime, int tInputMode);

typedef struct{
	int value;
	int index;
}StepValue;

enum{
	dXMode_Static=0,
	dXMode_Walk,
};

enum{
	dXSleep_NotDetec=0,
	dXSleep_WakeUp,
	dXSleep_LightSleep,
	dXSleep_DeepSleep,
};

typedef struct{
/*	int up;
	int down;
	int prevValue;
	int falseVlue;
	StepValue pMinValue[5];
	int minIndex;
	StepValue pMaxValue[5];
	int maxIndex;*/

	unsigned int startTime;
	int staticCount;
	int prevCommitMode;
	int sleepMode;
	int totalValue;
	int totalCount;

	int startWalkIndex;
}SleepParam;
SleepParam *pXSleepParam=NULL;


typedef struct {
	int up;
	int down;
	int prevValue;
	int falseVlue;
	StepValue pMinValue[7];
	int minIndex;
	StepValue pMaxValue[7];
	int maxIndex;

	int stepMode;

	int lastStepIndex;
	int startStep;
	int stepCount;
	int totalValue;
	int totalCount;
	int maxValueCnt;
}StepParam;


StepParam *pXStepParam=NULL;
SleepCallback FCSleepCallback=NULL;

static int pXTestData[1024]={0};
static int tXTestCnt=0;

static int pXMaxTestData[1024]={0};
static int tXMaxTestCnt=0;

void MySleepCallback(unsigned int tInputStartTime, int tInputMode)
{
	printf("damon ===> sleep call back : %d %d \n", tInputStartTime, tInputMode);
	pXSleepParam->prevCommitMode=tInputMode;
}

void draw_image(int *pInputData, int tInputLength, char *pInputWindowName)
{
	int i=0;
	int tTempMaxVal=0;
	int *pTempOutData=(int *)malloc(sizeof(int)*tInputLength);
	for(i=0; i<tInputLength; i++)
	{
		pTempOutData[i]=pInputData[i];

		if(tTempMaxVal<pInputData[i])
			tTempMaxVal=pInputData[i];
	}
printf("damon ===> max val : %d \n", tTempMaxVal);

	IplImage *pTempRgbImg=cvCreateImage(cvSize(tInputLength, 1000), IPL_DEPTH_8U, 3);
	memset(pTempRgbImg->imageData, 0, pTempRgbImg->widthStep*pTempRgbImg->height);

	unsigned char *pTempImgData=pTempRgbImg->imageData;
	int m,n, c=0;
	int r, g, b, tTempColor=0;
	CvPoint tTempPrevPoint, tTempCurPoint;
	tTempPrevPoint.x=0;
	tTempPrevPoint.y=1000-pTempOutData[0];
	for(m=1; m<pTempRgbImg->width; m ++)
	{
	#if 0
	//		  int tTempY=pTempOutData[m];
		tTempCurPoint.x=m;
		tTempCurPoint.y=1000-pTempOutData[m];
	//		cvLine(pTempRgbImg, tTempPrevPoint, tTempCurPoint, CV_RGB(255, 0, 0), 1, 8);
		if(m>2719 && m<2746)
			cvLine(pTempRgbImg, tTempPrevPoint, tTempCurPoint, CV_RGB(0, 255, 255), 1, 8, 0);
		else if(m>=2775 && m<2889)
			cvLine(pTempRgbImg, tTempPrevPoint, tTempCurPoint, CV_RGB(0, 255, 0), 1, 8, 0);
		else if(m>=2889 && m<2966)
			cvLine(pTempRgbImg, tTempPrevPoint, tTempCurPoint, CV_RGB(0, 0, 255), 1, 8, 0);
		else if(m>=2705 && m<2719)
			cvLine(pTempRgbImg, tTempPrevPoint, tTempCurPoint, CV_RGB(255, 255, 0), 1, 8, 0);
	//	else if(m>=935 && m<971 /*tTempSampleCnt+260*/)
	//		cvLine(pTempRgbImg, tTempPrevPoint, tTempCurPoint, CV_RGB(0, 255, 255), 1, 8, 0);	
		else
			cvLine(pTempRgbImg, tTempPrevPoint, tTempCurPoint, CV_RGB(255, 0, 0), 1, 8, 0);
//		printf("damon ====> point : %d %d \n", tTempCurPoint.x, tTempCurPoint.y);
		tTempPrevPoint.x=tTempCurPoint.x;
		tTempPrevPoint.y=tTempCurPoint.y;
	#else
	//	  int tTempY=pTempOutData[m];
			tTempCurPoint.x=m;
			tTempCurPoint.y=1000-pTempOutData[m];
	//		cvLine(pTempRgbImg, tTempPrevPoint, tTempCurPoint, CV_RGB(255, 0, 0), 1, 8);

	#ifdef dXDebugMinVal
		if(c<tXTestCnt-1 && m>=pXTestData[c])
	#else
		if(c<tXMaxTestCnt-1 && m>=pXMaxTestData[c])
	#endif
		{
		
	//	printf("damon ===> save cnt : %d %d %d %d %d \n", c, tTempSaveMinCnt, m, pTempSaveMinIndex[c], pTempSaveMinIndex[c+1]);

		//	r=abs(rand()%255);
		//	g=abs(rand()%255);
		//	b=abs(rand()%255);

			if(tTempColor==0)
			{
				r=255; g=255; b=0;
			}else if(tTempColor==1)
			{
				r=0; 	g=255; b=0;
			}else 
			{
				r=0;	g=0;	b=255;
			}
			tTempColor = (tTempColor+1)%3;

		#ifdef dXDebugMinVal
			for(i=pXTestData[c]; i<pXTestData[c+1]; i++)
		#else
			for(i=pXMaxTestData[c]; i<pXMaxTestData[c+1]; i++)
		#endif
			{
				tTempCurPoint.x=m;
				tTempCurPoint.y=1000-pTempOutData[m];
				cvLine(pTempRgbImg, tTempPrevPoint, tTempCurPoint, CV_RGB(r, g, b), 1, 8, 0);
	
				m++;
				
				tTempPrevPoint.x=tTempCurPoint.x;
				tTempPrevPoint.y=tTempCurPoint.y;
			}
			
			c++;
		#ifdef dXDebugMinVal
			m=pXTestData[c]-1;
		#else
			m=pXMaxTestData[c]-1;
		#endif
	
		//	printf("damon ===> index : %d \n", m);
		}else
		{
			cvLine(pTempRgbImg, tTempPrevPoint, tTempCurPoint, CV_RGB(255, 0, 0), 1, 8, 0);
	
			
			tTempPrevPoint.x=tTempCurPoint.x;
			tTempPrevPoint.y=tTempCurPoint.y;
		}
	#endif
	}

	cvShowImage("滤波后图形", pTempRgbImg);
	
}


#if 0
void process_sleep(int tInputData, int tInputIdx)
{

	if(pXSleepParam==NULL)
	{
		pXSleepParam=(SleepParam *)malloc(sizeof(SleepParam));
		memset(pXSleepParam, 0, sizeof(SleepParam));
		pXSleepParam->sleepMode=dXSleep_NotDetec;
	}
	
	if(FCSleepCallback==NULL)
	{
		FCSleepCallback=MySleepCallback;
	}

	if(tInputIdx==0)
	{
		pXSleepParam->prevValue=tInputData;
		return ;
	}

//printf("damon ===> sleep data : %d %d \n", tInputIdx, tInputData);

	pXSleepParam->totalValue += tInputData;
	pXSleepParam->totalCount++;
	int tTempAvgVal=pXSleepParam->totalValue/pXSleepParam->totalCount;
	if(tInputData>pXSleepParam->prevValue)
	{
		if(pXSleepParam->up==0 && pXSleepParam->down==0)
		{
			pXSleepParam->pMinValue[pXSleepParam->minIndex].index=tInputIdx-1;
			pXSleepParam->pMinValue[pXSleepParam->minIndex].value=pXSleepParam->prevValue;
			pXSleepParam->minIndex++;
			pXSleepParam->up=1;
		}else if(pXSleepParam->up==1)
		{
			if(pXSleepParam->falseVlue==1)
			{
				if(tInputData>pXSleepParam->pMaxValue[pXSleepParam->maxIndex-1].value)
				{
					pXSleepParam->falseVlue=0;
					pXSleepParam->maxIndex--;
				}else if((tInputIdx-pXSleepParam->pMaxValue[pXSleepParam->maxIndex-1].index>8) && (tInputIdx-tTempAvgVal>10))
				{
					pXSleepParam->falseVlue=0;
					pXSleepParam->maxIndex--;
				}
			}
		}else if(pXSleepParam->down==1)
		{
			if(pXSleepParam->falseVlue==0)
			{
				pXSleepParam->falseVlue=1;

if(pXSleepParam->minIndex<0)
	printf("damon ===> min idx error : %d \n", pXSleepParam->minIndex);
				if(pXSleepParam->minIndex>0)
				{
					// no move 
				printf("damon ===> sleep mode = %d - %d %d \n", pXSleepParam->sleepMode, tInputIdx, pXSleepParam->pMinValue[pXSleepParam->minIndex-1].index);
				sleep(1);
					if(tInputIdx-pXSleepParam->pMinValue[pXSleepParam->minIndex-1].index>200)
					{
						if(pXSleepParam->sleepMode==dXSleep_WakeUp)
						{
							if(pXSleepParam->staticCount==0)
							{
								pXSleepParam->staticCount=tInputIdx;									
								pXSleepParam->startTime=tInputIdx;
							}else if(tInputIdx-pXSleepParam->staticCount>200)
							{
								pXSleepParam->sleepMode=dXSleep_DeepSleep;
								pXSleepParam->startTime=pXSleepParam->staticCount;
							}
						}else if(pXSleepParam->sleepMode==dXSleep_DeepSleep)
						{
							if(pXSleepParam->prevCommitMode!=dXSleep_DeepSleep && tInputIdx-pXSleepParam->startTime>400)
							{
								// cmomit data
								if(FCSleepCallback!=NULL)
									FCSleepCallback(pXSleepParam->startTime, dXSleep_DeepSleep);
						
							}
						}else if(pXSleepParam->sleepMode==dXSleep_LightSleep)
						{
							pXSleepParam->sleepMode=dXSleep_DeepSleep;
							pXSleepParam->startTime=tInputIdx;
						}

						pXSleepParam->minIndex=0;
					}
				}


				if(pXSleepParam->minIndex<5)
				{
					pXSleepParam->pMinValue[pXSleepParam->minIndex].index=tInputIdx-1;
					pXSleepParam->pMinValue[pXSleepParam->minIndex].value=pXSleepParam->prevValue;
					pXSleepParam->minIndex++;
				}
			}else if((tInputIdx-pXSleepParam->pMinValue[pXSleepParam->minIndex-1].index>5) 
				|| (tInputData-pXSleepParam->pMinValue[pXSleepParam->minIndex-1].value>15))
			{
				pXSleepParam->down=0;
				pXSleepParam->up=1;
				pXSleepParam->falseVlue=0;

//printf("damon ===>  min idx = %d , max idx = %d \n", pXSleepParam->minIndex, pXSleepParam->maxIndex);

				int tTempAvg=pXSleepParam->totalValue/pXSleepParam->totalCount;
				if(pXSleepParam->minIndex>=5 && pXSleepParam->maxIndex>0)
				{
					if(pXStepParam->stepMode==dXMode_Walk)
					{
						if(pXSleepParam->sleepMode==dXSleep_LightSleep || pXSleepParam->sleepMode==dXSleep_DeepSleep)
						{
							pXSleepParam->sleepMode=dXSleep_WakeUp;
							pXSleepParam->startTime=tInputIdx;

							if(FCSleepCallback)
								FCSleepCallback(pXSleepParam->startTime, dXSleep_WakeUp);
						}

						pXSleepParam->staticCount=0;
					}else
					{
					#if 0
						int tTempMinAvgVal=(pXSleepParam->pMinValue[0].value+pXSleepParam->pMinValue[1].value+
							pXSleepParam->pMinValue[2].value+pXSleepParam->pMinValue[3].value)/4;
					printf("damon ===> %d %d %d \n", pXSleepParam->pMaxValue[pXSleepParam->maxIndex-1].value, tTempMinAvgVal, tTempAvg);
					sleep(1);
						if((pXSleepParam->pMaxValue[pXSleepParam->maxIndex-1].value-tTempMinAvgVal<10)
							&& (pXSleepParam->pMaxValue[pXSleepParam->maxIndex-1].value-tTempAvg<10))
						{
							if(pXSleepParam->sleepMode==dXSleep_WakeUp)
							{
								if(pXSleepParam->staticCount==0)
								{
									pXSleepParam->staticCount=tInputIdx;									
									pXSleepParam->startTime=tInputIdx;
								}else if(tInputIdx-pXSleepParam->staticCount>200)
								{
									pXSleepParam->sleepMode=dXSleep_DeepSleep;
									pXSleepParam->startTime=pXSleepParam->staticCount;
								}
							}else if(pXSleepParam->sleepMode==dXSleep_DeepSleep)
							{
								if(pXSleepParam->prevCommitMode!=dXSleep_DeepSleep && tInputIdx-pXSleepParam->startTime>400)
								{
									// cmomit data
									if(FCSleepCallback!=NULL)
										FCSleepCallback(pXSleepParam->startTime, dXSleep_DeepSleep);

								}
							}else if(pXSleepParam->sleepMode==dXSleep_LightSleep)
							{
								pXSleepParam->sleepMode=dXSleep_DeepSleep;
								pXSleepParam->startTime=tInputIdx;
							}

						}else
					#endif
						{
							if(pXSleepParam->sleepMode==dXSleep_NotDetec)
							{
								if(pXSleepParam->startTime==0)
								{
									pXSleepParam->startTime=tInputIdx;
								}else if(tInputIdx-pXSleepParam->startTime>200)
								{
									pXSleepParam->sleepMode=dXSleep_WakeUp;
								}
							}else if(pXSleepParam->sleepMode==dXSleep_DeepSleep)
							{

								if(pXSleepParam->prevCommitMode==dXSleep_DeepSleep)
								{
									pXSleepParam->sleepMode=dXSleep_LightSleep;
									pXSleepParam->startTime=tInputIdx;
									if(FCSleepCallback)
										FCSleepCallback(pXSleepParam->startTime, dXSleep_LightSleep);										
									
								}else if(pXSleepParam->prevCommitMode!=dXSleep_LightSleep)
								{
									pXSleepParam->sleepMode=dXSleep_WakeUp;
									pXSleepParam->startTime=tInputIdx;
								}else if(pXSleepParam->prevCommitMode==dXSleep_LightSleep)
								{
									pXSleepParam->sleepMode=dXSleep_LightSleep;
									pXSleepParam->startTime=tInputIdx;
								}
							
							}

							pXSleepParam->staticCount=0;
						}
					}
					

					int i=0;
					for(i=0; i<5; i++)
					{
						if(i==0)
						{
							pXSleepParam->pMinValue[i].index=pXSleepParam->pMinValue[4].index;
							pXSleepParam->pMinValue[i].value=pXSleepParam->pMinValue[4].value;
						}else
						{
							pXSleepParam->pMinValue[i].index=0;
							pXSleepParam->pMinValue[i].value=0;
						}
					}
					pXSleepParam->minIndex -= 4;
				}
			}
		}
	}else
	{
		if(pXSleepParam->up==0 && pXSleepParam->down==0)
		{
			pXSleepParam->pMaxValue[pXSleepParam->maxIndex].index=tInputIdx-1;
			pXSleepParam->pMaxValue[pXSleepParam->maxIndex].value=tInputData;
			pXSleepParam->maxIndex++;
			pXSleepParam->down=1;
		}else if(pXSleepParam->down==1)
		{
			if(pXSleepParam->falseVlue==1)
			{
				if(tInputIdx<pXSleepParam->pMinValue[pXSleepParam->minIndex-1].value)
				{				
					pXSleepParam->falseVlue=0;
					pXSleepParam->minIndex--;
				}else if((tInputIdx-pXSleepParam->pMinValue[pXSleepParam->minIndex-1].index>8) && (tTempAvgVal-tInputData>10))
				{					
					pXSleepParam->falseVlue=0;
					pXSleepParam->minIndex--;
				}
			}
		}else if(pXSleepParam->up==1)
		{
			if(pXSleepParam->falseVlue==0)
			{
				pXSleepParam->falseVlue=1;

	if(pXSleepParam->maxIndex<0)
		printf("damon ===> max idx error : %d \n", pXSleepParam->maxIndex);

				if(pXSleepParam->maxIndex<5)
				{
					pXSleepParam->pMaxValue[pXSleepParam->maxIndex].index=tInputIdx-1;
					pXSleepParam->pMaxValue[pXSleepParam->maxIndex].value=pXSleepParam->prevValue;
					pXSleepParam->maxIndex++;
				}
			}else if((tInputIdx-pXSleepParam->pMaxValue[pXSleepParam->maxIndex-1].index>5) 
							|| (tInputData-pXSleepParam->pMaxValue[pXSleepParam->maxIndex-1].value>15))
			{
				pXSleepParam->up=0;
				pXSleepParam->down=1;
				pXSleepParam->falseVlue=0;

				if(pXSleepParam->maxIndex>=5)
				{
					int i=0;
					for(i=0; i<3; i++)
					{
						if(i+3<5)
						{
							pXSleepParam->pMaxValue[i].index=pXSleepParam->pMaxValue[i+3].index;
							pXSleepParam->pMaxValue[i].value=pXSleepParam->pMaxValue[+3].value;
						}else
						{
							pXSleepParam->pMaxValue[i].index=0;
							pXSleepParam->pMaxValue[i].value=0;
						}
					}
					pXSleepParam->maxIndex -= 3;
				}
			}
		}
	}

	pXSleepParam->prevValue=tInputData;

}
#endif


void ProcessWaveStep(int tInputMinStepPrevIdx, int tInputMinStepCurIdx)
{
#ifdef dXUseCalSleep
		int tTempSubIdx=tInputMinStepCurIdx-tInputMinStepPrevIdx;
//printf("damon ===> process step : %d %d %d \n", tInputMinStepPrevIdx, tInputMinStepCurIdx, tTempSubIdx);
		if(pXSleepParam->startWalkIndex>0 && tInputMinStepPrevIdx==pXSleepParam->startWalkIndex)
		{
			if(pXSleepParam->sleepMode==dXSleep_LightSleep || pXSleepParam->sleepMode==dXSleep_DeepSleep)
			{
				pXSleepParam->sleepMode=dXSleep_WakeUp;
				pXSleepParam->startTime=pXStepParam->pMinValue[0].index;
			
				if(FCSleepCallback)
					FCSleepCallback(pXSleepParam->startTime, dXSleep_WakeUp);

				return ;
			}
		}

		if(tTempSubIdx>500)
		{
			if(pXSleepParam->sleepMode==dXSleep_WakeUp)
			{
			/*	if(pXSleepParam->staticCount==0)
				{
					pXSleepParam->staticCount=i;									
					pXSleepParam->startTime=i;
				}else if(i-pXSleepParam->staticCount>200)
				{
					pXSleepParam->sleepMode=dXSleep_DeepSleep;
					pXSleepParam->startTime=pXSleepParam->staticCount;
				}*/
				
				pXSleepParam->sleepMode=dXSleep_DeepSleep;
				pXSleepParam->startTime=tInputMinStepPrevIdx;
	
				if(FCSleepCallback!=NULL)
					FCSleepCallback(pXSleepParam->startTime, dXSleep_DeepSleep);
			}else if(pXSleepParam->sleepMode==dXSleep_DeepSleep)
			{
			/*	if(pXSleepParam->prevCommitMode!=dXSleep_DeepSleep)
				{
					// cmomit data
					if(FCSleepCallback!=NULL)
						FCSleepCallback(pXSleepParam->startTime, dXSleep_DeepSleep);
			
				}*/
			}else if(pXSleepParam->sleepMode==dXSleep_LightSleep)
			{
				pXSleepParam->sleepMode=dXSleep_DeepSleep;
				pXSleepParam->startTime=tInputMinStepPrevIdx;

				if(FCSleepCallback!=NULL)
					FCSleepCallback(pXSleepParam->startTime, dXSleep_DeepSleep);
			}
	
		//	pXSleepParam->minIndex=0;
		}else
		{
			if(pXSleepParam->sleepMode==dXSleep_NotDetec)
			{
				pXSleepParam->startTime=tInputMinStepPrevIdx;
				pXSleepParam->sleepMode=dXSleep_WakeUp;
			}else if(pXSleepParam->sleepMode==dXSleep_WakeUp)
			{
				
			}else if(pXSleepParam->sleepMode==dXSleep_LightSleep)
			{
				
			}else if(pXSleepParam->sleepMode==dXSleep_DeepSleep)
			{
				pXSleepParam->sleepMode=dXSleep_LightSleep;
				pXSleepParam->startTime=tInputMinStepPrevIdx;

				if(FCSleepCallback!=NULL)
					FCSleepCallback(pXSleepParam->startTime, dXSleep_LightSleep);				
			}
		}
#endif
}

void cal_step(int *pInputData, int tInputLength)
{
	if(pXStepParam==NULL)
	{
		pXStepParam=(StepParam *)malloc(sizeof(StepParam));
		memset(pXStepParam, 0, sizeof(StepParam));
	}

printf("damon ====> init val : %d %d %d \n", pXStepParam->minIndex, pXStepParam->maxIndex, pXStepParam->falseVlue);
#ifdef dXUseCalSleep
	if(pXSleepParam==NULL)
	{
		pXSleepParam=(SleepParam *)malloc(sizeof(SleepParam));
		memset(pXSleepParam, 0, sizeof(SleepParam));
		pXSleepParam->sleepMode=dXSleep_NotDetec;
	}

	if(FCSleepCallback==NULL)
	{
		FCSleepCallback=MySleepCallback;
	}
#endif


	int i=0;
	int tTempCurVal=0;
	int tTempAvgVal=0;
	int tTempHighHzVal=0;


//	for(i=0; i<tInputLength; i++)
//	{
//		process_sleep(pInputData[i], i);
//	}

#if 1
	for(i=0; i<tInputLength; i++)
	{
	//	process_sleep(pInputData[i], i);
		
		if(i==0)
		{
			pXStepParam->prevValue=pInputData[0];
			continue;
		}
	
		tTempCurVal=pInputData[i];
		pXStepParam->totalValue += tTempCurVal;
		pXStepParam->totalCount++;
		tTempAvgVal=pXStepParam->totalValue/pXStepParam->totalCount;


	if(i>1839 && i<1888)
		printf(CYAN"damon ===> test val : %d %d %d %d %d - %d %d \n"NONE, i, tTempCurVal, pXStepParam->prevValue, pXStepParam->stepMode, pXStepParam->falseVlue, pXStepParam->up, pXStepParam->down);

		
		if(tTempCurVal>pXStepParam->prevValue)	// up
		{

	//	if(i>723 && i<766)
	//		printf(CYAN"damon ===> test val : %d %d %d %d - %d %d \n"NONE, i, tTempCurVal, pXStepParam->prevValue, pXStepParam->falseVlue, pXStepParam->up, pXStepParam->down);
		
		
			if(pXStepParam->up==0 && pXStepParam->down==0)
			{
				pXStepParam->pMinValue[pXStepParam->minIndex].value=pXStepParam->prevValue;
				pXStepParam->pMinValue[pXStepParam->minIndex].index=i-1;
				pXStepParam->minIndex++;
				pXStepParam->up=1;
			}else if(pXStepParam->up==1)
			{			
				if(pXStepParam->falseVlue==1)
				{
					if(tTempCurVal>pXStepParam->pMaxValue[pXStepParam->maxIndex-1].value)
					{
					DEBUG_PRINTF_MAX((BLUE"damon ===> delete max val 00 : %d %d \n"NONE, pXStepParam->pMaxValue[pXStepParam->maxIndex-1].index, 
						pXStepParam->pMaxValue[pXStepParam->maxIndex-1].value));

						pXStepParam->falseVlue=0;
						pXStepParam->maxIndex--;
					}else if((i-pXStepParam->pMaxValue[pXStepParam->maxIndex-1].index>8) && (pXStepParam->pMaxValue[pXStepParam->maxIndex-1].index-tTempCurVal<20)/*(tTempCurVal-tTempAvgVal>10)*/)
					{
					DEBUG_PRINTF_MAX((BLUE"damon ===> delete max val 11 : %d %d \n"NONE, pXStepParam->pMaxValue[pXStepParam->maxIndex-1].index, 
						pXStepParam->pMaxValue[pXStepParam->maxIndex-1].value));

						pXStepParam->falseVlue=0;
						pXStepParam->maxIndex--;
					}
				}
			}else if(pXStepParam->down==1)
			{
				if(pXStepParam->falseVlue==0)
				{
					pXStepParam->falseVlue=1;

					if(pXStepParam->minIndex>0)
					{
					if(i==2169)
						DEBUG_PRINTF_MIN(("damon ===> min : %d %d  (%d, %d) (%d, %d) \n", i, pXStepParam->minIndex,
							pXStepParam->pMinValue[pXStepParam->minIndex-1].index, 
							pXStepParam->pMinValue[pXStepParam->minIndex-1].value,
							pXStepParam->pMinValue[pXStepParam->minIndex-2].index, 
							pXStepParam->pMinValue[pXStepParam->minIndex-2].value));

						if((pXStepParam->stepMode==dXMode_Static) && (i-pXStepParam->pMinValue[pXStepParam->minIndex-1].index>120))
						{
							if(pXSleepParam->sleepMode!=dXSleep_NotDetec)
							{
								int j=0;
								for(j=0; j<pXStepParam->minIndex-1; j++)
								{
								DEBUG_PRINTF_MIN(("damon  ===> delete val : %d  %d \n", pXStepParam->minIndex, pXStepParam->pMinValue[j].index));
									if(tXTestCnt<1024)
										pXTestData[tXTestCnt++]=pXStepParam->pMinValue[j].index;
							
								#ifdef dXUseCalSleep
									if(tXTestCnt>=2)
									{
										ProcessWaveStep(pXTestData[tXTestCnt-2], pXTestData[tXTestCnt-1]);
									}
								#endif
								}
							}
							
							pXStepParam->minIndex=0;

							pXStepParam->startStep=0;
						}
						else	if((i-pXStepParam->pMinValue[pXStepParam->minIndex-1].index>200) 
							|| (pXStepParam->lastStepIndex>0 && i-pXStepParam->lastStepIndex>300))
						{
						
							DEBUG_PRINTF_MIN((CYAN"damon ===> enter static mode : %d %d  %d \n"NONE, i, pXStepParam->lastStepIndex, pXStepParam->pMinValue[pXStepParam->minIndex-1].index));
							if(pXStepParam->stepMode==dXMode_Walk)
							{
								int j=0;
								for(j=0; j<pXStepParam->minIndex-1; j++)
								{
									if(tXTestCnt<1024)
										pXTestData[tXTestCnt++]=pXStepParam->pMinValue[j].index;

								#ifdef dXUseCalSleep
									if(tXTestCnt>=2)
									{
										ProcessWaveStep(pXTestData[tXTestCnt-2], pXTestData[tXTestCnt-1]);
									}
								#endif
								}
								
								pXStepParam->stepCount += pXStepParam->minIndex;
							}
							pXStepParam->minIndex=0;
							pXStepParam->stepMode=dXMode_Static;
							pXStepParam->startStep=0;
							pXStepParam->lastStepIndex=0;

							pXStepParam->maxValueCnt=0;
						}else if(pXStepParam->minIndex>=2)
						{
				//		if(i==936)
				//			printf("damon ===> test 00000 \n");
				
							if(i-pXStepParam->pMinValue[pXStepParam->minIndex-1].index<10)
							{
								if(pXStepParam->stepMode==dXMode_Static)
									pXStepParam->minIndex=0;
								else if(pXStepParam->pMinValue[pXStepParam->minIndex-1].value-pXStepParam->prevValue>10)
								{
								DEBUG_PRINTF_MIN(("damon ===> 000 delete min val : %d %d - %d %d %d \n", pXStepParam->pMinValue[pXStepParam->minIndex-1].index, pXStepParam->pMinValue[pXStepParam->minIndex-1].value, 
									pXStepParam->pMinValue[pXStepParam->minIndex-1].value, i, pXStepParam->pMinValue[pXStepParam->minIndex-1].index));
								
									pXStepParam->minIndex--;
								}
							}else if(pXStepParam->pMinValue[pXStepParam->minIndex-1].value-pXStepParam->pMinValue[pXStepParam->minIndex-2].value>100)
							{
								if((pXStepParam->pMinValue[pXStepParam->minIndex-1].index-pXStepParam->pMinValue[pXStepParam->minIndex-2].index<35))
								{
								DEBUG_PRINTF_MIN(("damon ===> 111 delete min val : %d %d - %d \n", pXStepParam->pMinValue[pXStepParam->minIndex-1].index, pXStepParam->pMinValue[pXStepParam->minIndex-1].value, pXStepParam->pMinValue[pXStepParam->minIndex-1].value));
																
									pXStepParam->pMinValue[pXStepParam->minIndex-1].value=0;
									pXStepParam->pMinValue[pXStepParam->minIndex-1].index=0;
									pXStepParam->minIndex--;
								}
							}else if(pXStepParam->pMinValue[pXStepParam->minIndex-2].value-pXStepParam->pMinValue[pXStepParam->minIndex-1].value>100)
							{
								if((pXStepParam->pMinValue[pXStepParam->minIndex-1].index-pXStepParam->pMinValue[pXStepParam->minIndex-2].index<35))
								{
								DEBUG_PRINTF_MIN(("damon ===> 222 delete min val : %d %d - %d \n", pXStepParam->pMinValue[pXStepParam->minIndex-2].index, pXStepParam->pMinValue[pXStepParam->minIndex-2].value, pXStepParam->pMinValue[pXStepParam->minIndex-2].value));
								
									pXStepParam->pMinValue[pXStepParam->minIndex-2].value=pXStepParam->pMinValue[pXStepParam->minIndex-1].value;
									pXStepParam->pMinValue[pXStepParam->minIndex-2].index=pXStepParam->pMinValue[pXStepParam->minIndex-1].index;
									pXStepParam->minIndex--;								
								}
							}else if(pXStepParam->maxIndex>1)
							{
								int tTempSubVal=pXStepParam->pMaxValue[pXStepParam->maxIndex-1].value-pXStepParam->pMinValue[pXStepParam->minIndex-2].value;
								int tTempSumVal=pXStepParam->pMaxValue[pXStepParam->maxIndex-1].value+pXStepParam->pMinValue[pXStepParam->minIndex-2].value;

							//	if(pXStepParam->pMinValue[pXStepParam->minIndex-1].value>tTempSumVal/2)
								if(pXStepParam->pMaxValue[pXStepParam->maxIndex-1].value-pXStepParam->pMinValue[pXStepParam->minIndex-1].value<tTempSubVal*2/5)
								{
								DEBUG_PRINTF_MIN(("damon ===> 333 delete min val : %d %d - %d %d \n", pXStepParam->pMinValue[pXStepParam->minIndex-1].index, pXStepParam->pMinValue[pXStepParam->minIndex-1].value, pXStepParam->pMaxValue[pXStepParam->maxIndex-1].value, tTempSubVal));
									pXStepParam->minIndex--;
								}/*else if((tTempSubVal>170 && pXStepParam->pMaxValue[pXStepParam->maxIndex-1].value-pXStepParam->pMinValue[pXStepParam->minIndex-1].value<150) && 
									(pXStepParam->pMaxValue[pXStepParam->maxIndex-2].value-pXStepParam->pMinValue[pXStepParam->minIndex-1].value<150))
								{
										printf("damon ===> delete min val 66 : %d %d %d - %d %d \n", tTempSubVal, pXStepParam->pMinValue[pXStepParam->minIndex-1].index, 
											pXStepParam->pMinValue[pXStepParam->minIndex-1].value, pXStepParam->pMaxValue[pXStepParam->maxIndex-2].value, pXStepParam->pMaxValue[pXStepParam->maxIndex-1].value);
								
									pXStepParam->minIndex--;
								}*/else
								{
								DEBUG_PRINTF_MIN(("damon ===> value %d - %d  : (%d, %d)  (%d , %d) \n",
									pXStepParam->minIndex, pXStepParam->maxIndex,
									pXStepParam->pMinValue[pXStepParam->minIndex-1].index, 
									pXStepParam->pMinValue[pXStepParam->minIndex-1].value,
									pXStepParam->pMaxValue[pXStepParam->maxIndex-1].index,
									pXStepParam->pMaxValue[pXStepParam->maxIndex-1].value));
								

									if(pXStepParam->maxIndex>=2)
									{
										int tTempHTotal=0, tTempHAvgVal=0;
										int j=0;										
										for(j=0; j<pXStepParam->maxIndex-1; j++)
										{
											tTempHTotal += pXStepParam->pMaxValue[j].value;
										}

										tTempHAvgVal=tTempHTotal/(pXStepParam->maxIndex-1);
										if(tTempHAvgVal- pXStepParam->pMinValue[pXStepParam->minIndex-1].value<18)
										{
									DEBUG_PRINTF_MIN(("damon ===> 888 delete min val : %d %d - %d %d \n", pXStepParam->pMinValue[pXStepParam->minIndex-1].index, pXStepParam->pMinValue[pXStepParam->minIndex-1].value, tTempHAvgVal, pXStepParam->pMinValue[pXStepParam->minIndex-1].value));
											if(pXStepParam->stepMode==dXMode_Static && pXStepParam->startStep<=0 && pXStepParam->minIndex==2)
											{
												pXStepParam->pMinValue[0].index=pXStepParam->pMinValue[1].index;
												pXStepParam->pMinValue[0].value=pXStepParam->pMinValue[1].value;
												pXStepParam->minIndex=1;
											}else
											{
												pXStepParam->minIndex--;
											}
										}else if(pXStepParam->minIndex>=2)
										{
											if(pXStepParam->pMinValue[pXStepParam->minIndex-2].index>pXStepParam->pMaxValue[pXStepParam->maxIndex-1].index)
											{
											DEBUG_PRINTF_MIN(("damon ===> 999 delete min val : %d %d - %d %d \n", pXStepParam->pMinValue[pXStepParam->minIndex-1].index, pXStepParam->pMinValue[pXStepParam->minIndex-1].value,
												pXStepParam->pMinValue[pXStepParam->minIndex-2].index, pXStepParam->pMaxValue[pXStepParam->maxIndex-1].index));
											
												pXStepParam->minIndex--;
											}
										}
									}else if(pXStepParam->minIndex>=3 && pXStepParam->maxIndex==0)
									{									
									DEBUG_PRINTF_MIN(("damon ===> 444 delete min val : %d %d \n", pXStepParam->pMinValue[pXStepParam->minIndex-1].index, pXStepParam->pMinValue[pXStepParam->minIndex-1].value));
										pXStepParam->minIndex--;
									}
								}
							}
						}

					//	if(i==936)
					//		printf("damon ===> test idx = %d \n", pXStepParam->minIndex);
					}


				/*	if(pXStepParam->minIndex>0)
					{
						printf("damon ===> value %d  : ", pXStepParam->minIndex);
						int j=0;
						for(j=0; j<pXStepParam->minIndex; j++)
						{
							printf(" (%d, %d) ", pXStepParam->pMinValue[j].index, pXStepParam->pMinValue[j].value);
						}
						printf("\n");
					}*/

					if(i>1839 && i<1888)
					{
						printf(CYAN"damon ===> add min val : %d %d - %d %d \n"NONE, i, pXStepParam->prevValue, pXStepParam->minIndex, pXStepParam->pMinValue[pXStepParam->minIndex-1].value);
					}

				if(pXStepParam->pMinValue[pXStepParam->minIndex-1].index==0)
					printf(CYAN"damon ++++++++> error data : %d\n"NONE, pXStepParam->minIndex);

			//	printf("damon ===> %d %d \n", pXStepParam->pMinValue[pXStepParam->minIndex-1].value, pXStepParam->prevValue);
					pXStepParam->pMinValue[pXStepParam->minIndex].value=pXStepParam->prevValue;
					pXStepParam->pMinValue[pXStepParam->minIndex].index=i-1;
					pXStepParam->minIndex++;
				
					pXStepParam->totalValue=0;
					pXStepParam->totalCount=0;
					
				}else if(i-pXStepParam->pMinValue[pXStepParam->minIndex-1].index>5)
				{

					pXStepParam->down=0;
					pXStepParam->up=1;
					pXStepParam->falseVlue=0;
				

					if(tTempAvgVal-pXStepParam->pMinValue[pXStepParam->minIndex-1].value<5)
					{
					//	if(pXStepParam->pMinValue[pXStepParam->minIndex-1].index>2065 && 
					//		pXStepParam->pMinValue[pXStepParam->minIndex-1].index<2142)

						if(pXStepParam->stepMode==dXMode_Static && pXStepParam->startStep<=0 && pXStepParam->minIndex==2)
						{
						DEBUG_PRINTF_MIN(("damon  ====> error data \n"));
							pXStepParam->pMinValue[0].index=pXStepParam->pMinValue[1].index;
							pXStepParam->pMinValue[0].value=pXStepParam->pMinValue[1].value;
							pXStepParam->minIndex=1;
						}else
						{						
						DEBUG_PRINTF_MIN(("damon ===> delete min val 22 : %d %d \n", pXStepParam->pMinValue[pXStepParam->minIndex-1].index, 
							pXStepParam->pMinValue[pXStepParam->minIndex-1].value));
						
							pXStepParam->minIndex--;
						}
					}else if(pXStepParam->minIndex>=2 /*&& pXStepParam->maxIndex>0*/)
					{
						int tTempSubVal=pXStepParam->pMinValue[pXStepParam->minIndex-1].value-pXStepParam->pMinValue[pXStepParam->minIndex-2].value;
						if(tTempSubVal<0)
							tTempSubVal=-1*tTempSubVal;

						if(tTempSubVal<20 && (pXStepParam->pMinValue[pXStepParam->minIndex-1].index-pXStepParam->pMinValue[pXStepParam->minIndex-2].index)>15)
						{
							
						}else
						if(pXStepParam->maxIndex>0)
						{
							int tTempTwoAvgVal=(pXStepParam->pMaxValue[pXStepParam->maxIndex-1].value+pXStepParam->pMinValue[pXStepParam->minIndex-2].value)/2;
							int tTempSubMaxVal=pXStepParam->pMaxValue[pXStepParam->maxIndex-1].value-pXStepParam->pMinValue[pXStepParam->minIndex-2].value;
							int tTempTwoSubIdx=pXStepParam->pMaxValue[pXStepParam->maxIndex-1].index-pXStepParam->pMinValue[pXStepParam->minIndex-2].index;
						
							if(pXStepParam->pMinValue[pXStepParam->minIndex-2].index>pXStepParam->pMaxValue[pXStepParam->maxIndex-1].index)
							{
							DEBUG_PRINTF_MIN(("damon ===> delete min val 123 :  %d %d - %d %d \n", pXStepParam->pMinValue[pXStepParam->minIndex-1].index, pXStepParam->pMinValue[pXStepParam->minIndex-1].value, pXStepParam->pMinValue[pXStepParam->minIndex-2].index, pXStepParam->pMaxValue[pXStepParam->maxIndex-1].index));
							
								pXStepParam->minIndex--;
							}
							else if(tTempSubMaxVal>=10 && pXStepParam->pMaxValue[pXStepParam->maxIndex-1].value-pXStepParam->pMinValue[pXStepParam->minIndex-1].value<(tTempSubMaxVal*2/5))
							{
								if(pXStepParam->pMaxValue[pXStepParam->maxIndex-1].value-pXStepParam->pMinValue[pXStepParam->minIndex-1].value>=20 && 
									tTempTwoSubIdx>=20 && tTempTwoSubIdx<60)
								{
								
								}else
								{
							DEBUG_PRINTF_MIN(("damon ===> delete min val 456 :  %d %d - %d %d %d \n", pXStepParam->pMinValue[pXStepParam->minIndex-1].index, pXStepParam->pMinValue[pXStepParam->minIndex-1].value, pXStepParam->pMaxValue[pXStepParam->maxIndex-1].index, pXStepParam->pMaxValue[pXStepParam->maxIndex-1].value, tTempTwoSubIdx));
									pXStepParam->minIndex--;
								}
							}
							else if(tTempSubVal>150)
							{
							DEBUG_PRINTF_MIN(("damon ===> delete min val 789 :  %d %d - %d \n", pXStepParam->pMinValue[pXStepParam->minIndex-1].index, pXStepParam->pMinValue[pXStepParam->minIndex-1].value, tTempSubVal));
								pXStepParam->minIndex--;
							}
						}else
						{
							pXStepParam->minIndex--;
						}
					}

			/*	#ifdef dXUseCalSleep
					if(pXStepParam->minIndex>3)
					{
						if(pXSleepParam->sleepMode==dXSleep_NotDetec)
						{
							if(pXSleepParam->startTime==0)
							{
								pXSleepParam->startTime=pXStepParam->pMinValue[pXStepParam->minIndex-2].index;
							}else if(i-pXSleepParam->startTime>200)
							{
								pXSleepParam->sleepMode=dXSleep_WakeUp;
							}
						}else if(pXSleepParam->sleepMode==dXSleep_DeepSleep)
						{

							if(pXSleepParam->prevCommitMode==dXSleep_DeepSleep)
							{
								pXSleepParam->sleepMode=dXSleep_LightSleep;
								pXSleepParam->startTime=pXStepParam->pMinValue[pXStepParam->minIndex-2].index;
								if(FCSleepCallback)
									FCSleepCallback(pXSleepParam->startTime, dXSleep_LightSleep);										
								
							}else if(pXSleepParam->prevCommitMode!=dXSleep_LightSleep)
							{
								pXSleepParam->sleepMode=dXSleep_WakeUp;
								pXSleepParam->startTime=pXStepParam->pMinValue[pXStepParam->minIndex-2].index;
							}else if(pXSleepParam->prevCommitMode==dXSleep_LightSleep)
							{
								pXSleepParam->sleepMode=dXSleep_LightSleep;
								pXSleepParam->startTime=pXStepParam->pMinValue[pXStepParam->minIndex-2].index;
							}
						
						}

						pXSleepParam->staticCount=0;
					}
				#endif	*/


			//	printf(CYAN"damon ===> step mode = %d \n"NONE, pXStepParam->stepMode);
					if(pXStepParam->stepMode==dXMode_Static)
					{
						if(pXStepParam->minIndex>=7)
						{
							// cal step count 

							int tTempHTotalVal=0, tTempHAvgVal=0;
							int tTempLTotalVal=0, tTempLAvgVal=0, tTempLMaxVal=0, tTempLMinVal=0;
							int j=0;

							if(pXStepParam->maxIndex>0)
							{
								for(j=0; j<pXStepParam->maxIndex; j++)
									tTempHTotalVal += pXStepParam->pMaxValue[j].value;
								tTempHAvgVal=tTempHTotalVal/pXStepParam->maxIndex;
							}

							for(j=0; j<pXStepParam->minIndex; j++)
							{
								if(tTempLMaxVal<pXStepParam->pMinValue[j].value)
									tTempLMaxVal=pXStepParam->pMinValue[j].value;

								if(tTempLMinVal==0 || tTempLMinVal>pXStepParam->pMinValue[j].value)
									tTempLMinVal=pXStepParam->pMinValue[j].value;

								tTempLTotalVal += pXStepParam->pMinValue[j].value;
							}
							tTempLAvgVal=tTempLTotalVal/pXStepParam->minIndex;
						printf("damon ====> start walk  maxAvg = %d , minAvg=%d \n", tTempHAvgVal, tTempLAvgVal);

							
							pXStepParam->startStep += 5;
							if(pXStepParam->startStep>=dXStartWalkStep)
							{
						printf("damon ===> start walk : %d %d %d \n", pXStepParam->maxIndex, pXStepParam->maxValueCnt, pXStepParam->minIndex);
						
								if( ((pXStepParam->maxIndex>=5)|| ((pXStepParam->maxValueCnt>=5) && (pXStepParam->maxIndex>=2))) && (tTempHAvgVal-tTempLAvgVal>=30))
								{
								printf(CYAN"damon ===> enter walk step \n"NONE);
									
									pXStepParam->stepCount += pXStepParam->startStep;
									pXStepParam->stepMode=dXMode_Walk;
									pXStepParam->startStep=0;

									pXStepParam->lastStepIndex=pXStepParam->pMinValue[4].index;


								#ifdef dXUseCalSleep
									if(pXSleepParam)
									{
									/*	if(pXSleepParam->sleepMode==dXSleep_LightSleep || pXSleepParam->sleepMode==dXSleep_DeepSleep)
										{
											pXSleepParam->sleepMode=dXSleep_WakeUp;
											pXSleepParam->startTime=pXStepParam->pMinValue[0].index;
									
											if(FCSleepCallback)
												FCSleepCallback(pXSleepParam->startTime, dXSleep_WakeUp);
										}
									
										pXSleepParam->staticCount=0;*/

										pXSleepParam->startWalkIndex=pXStepParam->pMinValue[0].index;
									}
								#endif
								}else
								{
									pXStepParam->startStep -= 5;
								}
							}

						/*	if(pXStepParam->stepMode==dXMode_Walk)
							{
								printf("damon ===> start walk max data %d %d :  (%d, %d)  (%d, %d)  (%d, %d)  (%d, %d)  (%d, %d)  (%d, %d)  (%d, %d) \n", 
									pXStepParam->maxIndex, pXStepParam->maxValueCnt,
									pXStepParam->pMaxValue[0].index, pXStepParam->pMaxValue[0].value,
									pXStepParam->pMaxValue[1].index, pXStepParam->pMaxValue[1].value,
									pXStepParam->pMaxValue[2].index, pXStepParam->pMaxValue[2].value,
									pXStepParam->pMaxValue[3].index, pXStepParam->pMaxValue[3].value,
									pXStepParam->pMaxValue[4].index, pXStepParam->pMaxValue[4].value,
									pXStepParam->pMaxValue[5].index, pXStepParam->pMaxValue[5].value,
									pXStepParam->pMaxValue[6].index, pXStepParam->pMaxValue[6].value);
							}*/


							DEBUG_PRINTF_MIN((LIGHT_RED"damon ===> min data : (%d, %d)  (%d, %d)  (%d, %d)  (%d, %d)  (%d, %d) (%d, %d) (%d, %d) \n"NONE, 
								pXStepParam->pMinValue[0].index, pXStepParam->pMinValue[0].value,
								pXStepParam->pMinValue[1].index, pXStepParam->pMinValue[1].value,
								pXStepParam->pMinValue[2].index, pXStepParam->pMinValue[2].value,
								pXStepParam->pMinValue[3].index, pXStepParam->pMinValue[3].value,
								pXStepParam->pMinValue[4].index, pXStepParam->pMinValue[4].value,
								pXStepParam->pMinValue[5].index, pXStepParam->pMinValue[5].value,
								pXStepParam->pMinValue[6].index, pXStepParam->pMinValue[6].value));

							if(tTempHAvgVal-tTempLAvgVal>30)
							{
								int j=0;
								for(j=0; j<5; j ++)
								{
								// test
								if(tXTestCnt<1024)
									pXTestData[tXTestCnt++]=pXStepParam->pMinValue[j].index;

								#ifdef dXUseCalSleep
									if(tXTestCnt>=2)
									{
										ProcessWaveStep(pXTestData[tXTestCnt-2], pXTestData[tXTestCnt-1]);
									}
								#endif
								
									if(j+5<7)
									{
										pXStepParam->pMinValue[j].value=pXStepParam->pMinValue[j+5].value;
										pXStepParam->pMinValue[j].index=pXStepParam->pMinValue[j+5].index;
									}else
									{
										pXStepParam->pMinValue[j].value=0;
										pXStepParam->pMinValue[j].index=0;
									}
								}
							
								pXStepParam->minIndex -= 5;
							}else
							{
								pXStepParam->minIndex=0;
								pXStepParam->startStep=0;
							}
						}

					}else if(pXStepParam->stepMode==dXMode_Walk)
					{
						if(pXStepParam->minIndex>=5)
						{
							// cal step count 
						
							pXStepParam->stepCount += 3;
							pXStepParam->lastStepIndex=pXStepParam->pMinValue[2].index;

							DEBUG_PRINTF_MIN((LIGHT_RED"damon ===> min data : (%d, %d)   (%d, %d)   (%d, %d) (%d, %d) (%d, %d)\n"NONE,
								pXStepParam->pMinValue[0].index, pXStepParam->pMinValue[0].value,
								pXStepParam->pMinValue[1].index, pXStepParam->pMinValue[1].value,
								pXStepParam->pMinValue[2].index, pXStepParam->pMinValue[2].value,
								pXStepParam->pMinValue[3].index, pXStepParam->pMinValue[3].value,
								pXStepParam->pMinValue[4].index, pXStepParam->pMinValue[4].value));

							
							DEBUG_PRINTF_MIN(("damon ===> max data : (%d, %d)   (%d, %d)   (%d, %d) \n",
								pXStepParam->pMaxValue[0].index, pXStepParam->pMaxValue[0].value,
								pXStepParam->pMaxValue[1].index, pXStepParam->pMaxValue[1].value,
								pXStepParam->pMaxValue[2].index, pXStepParam->pMaxValue[2].value));
							

							int j=0;
							for(j=0; j<3; j ++)
							{
							// test
							if(tXTestCnt<1024)
								pXTestData[tXTestCnt++]=pXStepParam->pMinValue[j].index;
							
							#ifdef dXUseCalSleep
								if(tXTestCnt>=2)
								{
									ProcessWaveStep(pXTestData[tXTestCnt-2], pXTestData[tXTestCnt-1]);
								}
							#endif
							
								if(j+3<5)
								{
									pXStepParam->pMinValue[j].value=pXStepParam->pMinValue[j+3].value;
									pXStepParam->pMinValue[j].index=pXStepParam->pMinValue[j+3].index;
								}else
								{
									pXStepParam->pMinValue[j].value=0;
									pXStepParam->pMinValue[j].index=0;
								}
							}
						
							pXStepParam->minIndex -= 3;
						}
							
					}
					
				}

			}
		}else
		{
			if(pXStepParam->up==0 && pXStepParam->down==0)
			{
				pXStepParam->down=1;
				pXStepParam->pMaxValue[pXStepParam->maxIndex].value=pXStepParam->prevValue;
				pXStepParam->pMaxValue[pXStepParam->maxIndex].index=i-1;
				pXStepParam->maxIndex++;
			}else if(pXStepParam->down==1)
			{
			
				if(pXStepParam->falseVlue==1)
				{
				
					if(tTempCurVal<pXStepParam->pMinValue[pXStepParam->minIndex-1].value)
					{
					
				//	if(pXStepParam->pMinValue[pXStepParam->minIndex-1].index>1033 && 
				//		pXStepParam->pMinValue[pXStepParam->minIndex-1].index<1119)
						DEBUG_PRINTF_MIN(("damon ===> delete min val 00 : %d %d \n", pXStepParam->pMinValue[pXStepParam->minIndex-1].index, 
							pXStepParam->pMinValue[pXStepParam->minIndex-1].value));
					
						pXStepParam->falseVlue=0;
						pXStepParam->minIndex--;
					}else if((i-pXStepParam->pMinValue[pXStepParam->minIndex-1].index>8) && (tTempCurVal-pXStepParam->pMinValue[pXStepParam->minIndex-1].index>20)/*(tTempAvgVal-tTempCurVal>10)*/)
					{
			//		if(pXStepParam->pMinValue[pXStepParam->minIndex-1].index>1033 && 
			//			pXStepParam->pMinValue[pXStepParam->minIndex-1].index<1119)
						DEBUG_PRINTF_MIN(("damon ===> delete min val 11 : %d %d - %d %d %d \n", pXStepParam->pMinValue[pXStepParam->minIndex-1].index, 
							pXStepParam->pMinValue[pXStepParam->minIndex-1].value, i, tTempAvgVal, tTempCurVal));
					
						pXStepParam->falseVlue=0;
						pXStepParam->minIndex--;
					}

					
					if(pXStepParam->stepMode==dXMode_Walk && pXStepParam->falseVlue==1)
					{
						if((tTempCurVal==pXStepParam->prevValue) && (i-pXStepParam->pMinValue[pXStepParam->minIndex-1].index>=6) 
							&& (i-pXStepParam->pMinValue[pXStepParam->minIndex-1].index<=25) 
							&& (tTempCurVal-pXStepParam->pMinValue[pXStepParam->minIndex-1].value)>=20)
						{
						printf(RED"damon ===> max add val : %d %d - %d %d %d \n"NONE, pXStepParam->maxIndex, pXStepParam->minIndex, pXStepParam->minIndex, pXStepParam->pMinValue[pXStepParam->minIndex-1].index, pXStepParam->pMinValue[pXStepParam->minIndex-1].value);
							if(pXStepParam->maxIndex<7)
							{
								pXStepParam->pMaxValue[pXStepParam->maxIndex].index=i;
								pXStepParam->pMaxValue[pXStepParam->maxIndex].value=tTempCurVal;
								pXStepParam->maxIndex++;

								pXStepParam->down=0;
								pXStepParam->up=1;

								pXStepParam->falseVlue=1;

								
								if(pXStepParam->minIndex>=5)
								{
									// cal step count 
								
									pXStepParam->stepCount += 3;
									pXStepParam->lastStepIndex=pXStepParam->pMinValue[2].index;
								
									DEBUG_PRINTF_MIN((LIGHT_RED"damon ===> min data : (%d, %d)	 (%d, %d)	(%d, %d) (%d, %d) (%d, %d)\n"NONE,
										pXStepParam->pMinValue[0].index, pXStepParam->pMinValue[0].value,
										pXStepParam->pMinValue[1].index, pXStepParam->pMinValue[1].value,
										pXStepParam->pMinValue[2].index, pXStepParam->pMinValue[2].value,
										pXStepParam->pMinValue[3].index, pXStepParam->pMinValue[3].value,
										pXStepParam->pMinValue[4].index, pXStepParam->pMinValue[4].value));
								
									
									DEBUG_PRINTF_MIN(("damon ===> max data : (%d, %d)	(%d, %d)   (%d, %d) \n",
										pXStepParam->pMaxValue[0].index, pXStepParam->pMaxValue[0].value,
										pXStepParam->pMaxValue[1].index, pXStepParam->pMaxValue[1].value,
										pXStepParam->pMaxValue[2].index, pXStepParam->pMaxValue[2].value));
									
								
									int j=0;
									for(j=0; j<3; j ++)
									{
									// test
									if(tXTestCnt<1024)
										pXTestData[tXTestCnt++]=pXStepParam->pMinValue[j].index;
									
								#ifdef dXUseCalSleep
										if(tXTestCnt>=2)
										{
											ProcessWaveStep(pXTestData[tXTestCnt-2], pXTestData[tXTestCnt-1]);
										}
								#endif
									
										if(j+3<5)
										{
											pXStepParam->pMinValue[j].value=pXStepParam->pMinValue[j+3].value;
											pXStepParam->pMinValue[j].index=pXStepParam->pMinValue[j+3].index;
										}else
										{
											pXStepParam->pMinValue[j].value=0;
											pXStepParam->pMinValue[j].index=0;
										}
									}
								
									pXStepParam->minIndex -= 3;
								}
							}
						}
					}
				}
			}else if(pXStepParam->up==1)
			{
				if(pXStepParam->falseVlue==0)
				{
					pXStepParam->falseVlue=1;


					if(pXStepParam->maxIndex>0)
					{
						if(i-pXStepParam->pMaxValue[pXStepParam->maxIndex-1].index>150)
						{
						DEBUG_PRINTF_MAX((BLUE"damon ===> delete max val 22 : %d %d \n"NONE, i, pXStepParam->pMaxValue[pXStepParam->maxIndex-1].index));
							pXStepParam->maxIndex=0;
						}else if(pXStepParam->maxIndex>=2)
						{
							if(pXStepParam->pMaxValue[pXStepParam->maxIndex-1].value-pXStepParam->pMaxValue[pXStepParam->maxIndex-2].value>100)
							{
							DEBUG_PRINTF_MAX((BLUE"damon ===> delete max val 33 : %d %d - %d %d \n"NONE, pXStepParam->pMaxValue[pXStepParam->maxIndex-2].index, pXStepParam->pMaxValue[pXStepParam->maxIndex-2].value, 
								pXStepParam->pMaxValue[pXStepParam->maxIndex-1].value, pXStepParam->pMaxValue[pXStepParam->maxIndex-2].value));
							
								pXStepParam->pMaxValue[pXStepParam->maxIndex-2].value=pXStepParam->pMaxValue[pXStepParam->maxIndex-1].value;
								pXStepParam->pMaxValue[pXStepParam->maxIndex-2].index=pXStepParam->pMaxValue[pXStepParam->maxIndex-1].index;
								pXStepParam->maxIndex--;
							}else if(pXStepParam->pMaxValue[pXStepParam->maxIndex-2].value-pXStepParam->pMaxValue[pXStepParam->maxIndex-1].value>100)
							{
							DEBUG_PRINTF_MAX((BLUE"damon ===> delete max val 44 : %d %d - %d %d \n"NONE, pXStepParam->pMaxValue[pXStepParam->maxIndex-1].index, pXStepParam->pMaxValue[pXStepParam->maxIndex-1].value, 
								pXStepParam->pMaxValue[pXStepParam->maxIndex-1].value, pXStepParam->pMaxValue[pXStepParam->maxIndex-2].value));
							
								pXStepParam->maxIndex--;
							}else if(pXStepParam->pMaxValue[pXStepParam->maxIndex-1].index-pXStepParam->pMaxValue[pXStepParam->maxIndex-2].index<10)
							{
								if(pXStepParam->pMaxValue[pXStepParam->maxIndex-1].value-pXStepParam->pMaxValue[pXStepParam->maxIndex-2].value>60)
								{					
								DEBUG_PRINTF_MAX((BLUE"damon ===> delete max val 55 : %d %d - %d %d \n"NONE, pXStepParam->pMaxValue[pXStepParam->maxIndex-2].index, pXStepParam->pMaxValue[pXStepParam->maxIndex-2].value,
									pXStepParam->pMaxValue[pXStepParam->maxIndex-1].index, pXStepParam->pMaxValue[pXStepParam->maxIndex-2].index));
								
									pXStepParam->pMaxValue[pXStepParam->maxIndex-2].value=pXStepParam->pMaxValue[pXStepParam->maxIndex-1].value;
									pXStepParam->pMaxValue[pXStepParam->maxIndex-2].index=pXStepParam->pMaxValue[pXStepParam->maxIndex-1].index;
									pXStepParam->maxIndex--;
								}else
								{
									DEBUG_PRINTF_MAX((BLUE"damon ===> delete max val 66 : %d %d - %d %d \n"NONE, pXStepParam->pMaxValue[pXStepParam->maxIndex-1].index, pXStepParam->pMaxValue[pXStepParam->maxIndex-1].value,
										pXStepParam->pMaxValue[pXStepParam->maxIndex-1].index, pXStepParam->pMaxValue[pXStepParam->maxIndex-2].index));
									
									pXStepParam->maxIndex--;										
								}
							}
						}
					}

			if(i-1>2526 && i-1<2566)
				printf("damon ===> add max val : %d %d \n", i-1, pXStepParam->prevValue);

					pXStepParam->pMaxValue[pXStepParam->maxIndex].index=i-1;
					pXStepParam->pMaxValue[pXStepParam->maxIndex].value=pXStepParam->prevValue;
					pXStepParam->maxIndex++;

					pXStepParam->totalCount=0;
					pXStepParam->totalValue=0;
				}else if(i-pXStepParam->pMaxValue[pXStepParam->maxIndex-1].index>5)
				{
					pXStepParam->down=1;
					pXStepParam->up=0;
					pXStepParam->falseVlue=0;

					if((pXStepParam->pMaxValue[pXStepParam->maxIndex-1].index-pXStepParam->pMinValue[pXStepParam->minIndex-1].index>8)
						&& (pXStepParam->pMaxValue[pXStepParam->maxIndex-1].value-pXStepParam->pMinValue[pXStepParam->minIndex-1].value>10))
					{

					}else
					if(pXStepParam->pMaxValue[pXStepParam->maxIndex-1].value-tTempAvgVal<5)
					{
				DEBUG_PRINTF_MAX((BLUE"damon ===> delete max val 77 :  %d %d - %d %d \n"NONE, pXStepParam->pMaxValue[pXStepParam->maxIndex-1].index, pXStepParam->pMaxValue[pXStepParam->maxIndex-1].value,
					pXStepParam->pMaxValue[pXStepParam->maxIndex-1].value, tTempAvgVal));
				
						pXStepParam->maxIndex--;
					}else 
					{
						if(pXStepParam->maxIndex>=2 && pXStepParam->minIndex>0)
						{
							if(pXStepParam->pMinValue[pXStepParam->minIndex-1].index<pXStepParam->pMaxValue[pXStepParam->maxIndex-2].index)
							{
								DEBUG_PRINTF_MAX((BLUE"damon ===> delete max val 88 :  %d %d \n"NONE, pXStepParam->pMaxValue[pXStepParam->maxIndex-1].index, pXStepParam->pMaxValue[pXStepParam->maxIndex-1].value));

								pXStepParam->maxIndex--;
							}
						}
					}

					if(pXStepParam->maxIndex>=7)
					{
				DEBUG_PRINTF_MAX((LIGHT_RED"damon ===> max data : (%d, %d)  (%d, %d)  (%d, %d)  (%d, %d)  (%d, %d) (%d, %d) (%d, %d) \n"NONE, 
					pXStepParam->pMaxValue[0].index, pXStepParam->pMaxValue[0].value,
					pXStepParam->pMaxValue[1].index, pXStepParam->pMaxValue[1].value,
					pXStepParam->pMaxValue[2].index, pXStepParam->pMaxValue[2].value,
					pXStepParam->pMaxValue[3].index, pXStepParam->pMaxValue[3].value,
					pXStepParam->pMaxValue[4].index, pXStepParam->pMaxValue[4].value,
					pXStepParam->pMaxValue[5].index, pXStepParam->pMaxValue[5].value,
					pXStepParam->pMaxValue[6].index, pXStepParam->pMaxValue[6].value));

					
						int j=0;
						for(j=0; j<5; j++)
						{
						if(tXMaxTestCnt<1024)
							pXMaxTestData[tXMaxTestCnt++]=pXStepParam->pMaxValue[j].index;
						
							if(j+5<7)
							{
								pXStepParam->pMaxValue[j].value=pXStepParam->pMaxValue[j+5].value;
								pXStepParam->pMaxValue[j].index=pXStepParam->pMaxValue[j+5].index;
							}else 
							{
								pXStepParam->pMaxValue[j].value=0;
								pXStepParam->pMaxValue[j].index=0;
							}
						}

						pXStepParam->maxIndex -= 5;

						pXStepParam->maxValueCnt += 5;
					}
				}
			}
		}

		pXStepParam->prevValue=tTempCurVal;
	}
#endif

	printf("damon ====> step count : %d %d %d \n", pXStepParam->stepCount, tXTestCnt, tXMaxTestCnt);
}
#endif


void filter_data(int *pInputData ,int tInputLength)
{
	int i=0, j=0;
	int pTempValue[5]={0};

	// 中值滤波
	for(i=0; i<tInputLength-5; i++)
	{
		int tTempTotalVal=0;
		for(j=0; j<5; j++)
		{
			pTempValue[j]=pInputData[i+j];
			tTempTotalVal += pTempValue[j];
		}

	//	printf("damon ===> start : %d %d %d %d %d \n", pTempValue[0], pTempValue[1], pTempValue[2], pTempValue[3], pTempValue[4]);
		int m=0, n=0;
		for(m=0; m<4; m++)
		{
			for(n=m+1; n<5; n++)
			{
				if(pTempValue[m]>pTempValue[n])
				{
					int tTempSwap=pTempValue[m];
					pTempValue[m]=pTempValue[n];
					pTempValue[n]=tTempSwap;
				}
			}	
		}

	
	//	printf("damon ===> end : %d %d %d %d %d \n", pTempValue[0], pTempValue[1], pTempValue[2], pTempValue[3], pTempValue[4]);
		pInputData[i]= pTempValue[2];
	//	sleep(1);

	}

//	draw_image(pInputData, tInputLength, "filter data img");
}


int main(int argc, char *argv[])
{
	if(argc<2)
	{
		printf("damon ===> usage : ./a.out gsensor.dat \n");
		return 0;
	}

	srand(time(NULL));

	int tTempGSensorFd=open(argv[1], O_RDONLY);
	if(tTempGSensorFd<0)
	{
		printf("damon ===> open file [%s] failed !\n", argv[2]);
		return 0;
	}

	int tTempGSensorFileLen=lseek(tTempGSensorFd, 0, SEEK_END);
	char *pTempGSensorBuf=malloc(tTempGSensorFileLen+1);
	memset(pTempGSensorBuf, 0, tTempGSensorFileLen+1);

	lseek(tTempGSensorFd, 0, SEEK_SET);

	if(tTempGSensorFileLen!=(read(tTempGSensorFd, pTempGSensorBuf, tTempGSensorFileLen)))
	{
		printf("damon ===> read file [%s] failed !\n", argv[1]);
		close(tTempGSensorFd);
		return 0;
	}

	
	// gsensor 数据
	char * pTempStr=strrchr(pTempGSensorBuf, ',');
	if(pTempStr==NULL)
	{
		close(tTempGSensorFd);
		
		return 0;
	}
	int tTempSubIndex=pTempStr-pTempGSensorBuf;
	pTempStr++;
	
	unsigned int i=0;
	char pTempNum[10]={0};
	while(pTempStr)
	{
		if(*pTempStr>='0' && *pTempStr<='9')		
		{
			pTempNum[i++]=*pTempStr;
		}else
		{
			break;
		}
		pTempStr++;
	}
	int tTempGSensorDataNum=atoi(pTempNum);
	int *pTempGSensorData=malloc(tTempGSensorDataNum*sizeof(int));
	pTempGSensorBuf[tTempSubIndex]=0;
	memset(pTempNum, 0, sizeof(pTempNum));
	int j=0, k=0;
	for(i=0; i<tTempSubIndex; i++)
	{
		if(pTempGSensorBuf[i]>='0' && pTempGSensorBuf[i]<='9')
		{
			pTempNum[j++]=pTempGSensorBuf[i];
		}else
		{
			if(j>0)
			{
				if(k<tTempGSensorDataNum)
					pTempGSensorData[k++]=atoi(pTempNum);
				else
					printf("damon ===> data err: %d %d \n", k, tTempGSensorDataNum);
			}
			j=0;
			memset(pTempNum, 0, sizeof(pTempNum));
		}
	}

	printf("damon ===> read data : %d %d \n", k, tTempGSensorDataNum);

#if 1
//	draw_image(pTempGSensorData, tTempGSensorDataNum, "src gsensor img");
	filter_data(pTempGSensorData, tTempGSensorDataNum);
	cal_step(pTempGSensorData, tTempGSensorDataNum);
	draw_image(pTempGSensorData, tTempGSensorDataNum, "src gsensor img");
	printf("damon ====> step count : %d \n", pXStepParam->stepCount);
	
#else

//memset(pXStepParam, 0, sizeof(StepParam));
for(i=0; i<tTempGSensorDataNum;  i++)
	StepCalculation(pTempGSensorData[i], i, i);
//	testFc(pTempGSensorData[i], i);
printf("damon ====> step count : %d \n", GetStepCount());
#endif

//draw_image(pXTest, tXTest, "src gsensor img");

/*for(i=1; i<tXTestCnt; i++)
{
	printf("damon ===> (%d  %d) - %d \n", pXTestData[i-1], pXTestData[i], pXTestData[i]-pXTestData[i-1]);
}*/

	cvWaitKey(0);
	while(1)
	{
		sleep(1);
	}

	return 0;
}


