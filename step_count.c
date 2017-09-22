
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


#define dXStartWalkStep	10

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
	int up;
	int down;
	int prevValue;
	int falseVlue;
	StepValue pMinValue[5];
	int minIndex;
	StepValue pMaxValue[5];
	int maxIndex;
	
	unsigned int startTime;
	int sleepMode;
	int totalValue;
	int totalCount;
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
}StepParam;


StepParam *pXStepParam=NULL;
SleepCallback FCSleepCallback=NULL;

static int pXTestData[1024]={0};
static int tXTestCnt=0;

void MySleepCallback(unsigned int tInputStartTime, int tInputMode)
{
	printf("damon ===> sleep call back : %d %d \n", tInputStartTime, tInputMode);
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
	int r, g, b;
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
		if(m>1282 && m<1370)
			cvLine(pTempRgbImg, tTempPrevPoint, tTempCurPoint, CV_RGB(0, 255, 255), 1, 8, 0);
		else if(m>=1370 && m<1413 /*tTempSampleCnt+260*/)
			cvLine(pTempRgbImg, tTempPrevPoint, tTempCurPoint, CV_RGB(0, 255, 0), 1, 8, 0);
		else if(m>=1413 && m<1466 /*tTempSampleCnt+260*/)
			cvLine(pTempRgbImg, tTempPrevPoint, tTempCurPoint, CV_RGB(0, 0, 255), 1, 8, 0);
		else if(m>=1466 && m<1507 /*tTempSampleCnt+260*/)
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
	
		if(c<tXTestCnt-1 && m>=pXTestData[c])
		{
		
	//	printf("damon ===> save cnt : %d %d %d %d %d \n", c, tTempSaveMinCnt, m, pTempSaveMinIndex[c], pTempSaveMinIndex[c+1]);
			r=abs(rand()%255);
			g=abs(rand()%255);
			b=abs(rand()%255);
		
			for(i=pXTestData[c]; i<pXTestData[c+1]; i++)
			{
				tTempCurPoint.x=m;
				tTempCurPoint.y=1000-pTempOutData[m];
				cvLine(pTempRgbImg, tTempPrevPoint, tTempCurPoint, CV_RGB(r, g, b), 1, 8, 0);
	
				m++;
				
				tTempPrevPoint.x=tTempCurPoint.x;
				tTempPrevPoint.y=tTempCurPoint.y;
			}
			
			c++;
			m=pXTestData[c]-1;
	
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

void filter_data(int *pInputData ,int tInputLength)
{
	int i=0, j=0;
	int pTempValue[5]={0};

	// 中值滤波
	for(i=0; i<tInputLength-5; i++)
	{
		for(j=0; j<5; j++)
		{
			pTempValue[j]=pInputData[i+j];
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
		pInputData[i]=pTempValue[2];
	//	sleep(1);

	}

//	draw_image(pInputData, tInputLength, "filter data img");
}


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

	pXSleepParam->totalValue += tInputData;
	pXSleepParam->totalCount++;
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
			
		}else if(pXSleepParam->down==1)
		{
			if(pXSleepParam->falseVlue==0)
			{
				pXSleepParam->falseVlue=1;

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

				int tTempAvg=pXSleepParam->totalValue/pXSleepParam->totalCount;
				if(pXSleepParam->minIndex>=5 && pXSleepParam->maxIndex>0)
				{
					int tTempMinAvgVal=(pXSleepParam->pMinValue[0].value+pXSleepParam->pMinValue[1].value+
						pXSleepParam->pMinValue[2].value+pXSleepParam->pMinValue[3].value)/4;

					if((pXSleepParam->pMaxValue[pXSleepParam->maxIndex-1].value-tTempMinAvgVal<10)
						&& (pXSleepParam->pMaxValue[pXSleepParam->maxIndex-1].value-tTempAvg<10))
					{
						if(pXSleepParam->sleepMode==dXSleep_WakeUp)
						{
							pXSleepParam->sleepMode=dXSleep_DeepSleep;
							pXSleepParam->startTime=tInputIdx;
						}else if(pXSleepParam->sleepMode==dXSleep_DeepSleep)
						{
							if(tInputIdx-pXSleepParam->startTime>100)
							{
								// cmomit data
								if(FCSleepCallback!=NULL)
								{
									FCSleepCallback(pXSleepParam->startTime, dXSleep_DeepSleep);
								}
							}
						}else if(pXSleepParam->sleepMode==dXSleep_LightSleep)
						{
							
						}

					}else
					{
						if(pXSleepParam->sleepMode==dXSleep_NotDetec)
						{
							pXSleepParam->sleepMode=dXSleep_WakeUp;
						}else if(pXSleepParam->sleepMode==dXSleep_DeepSleep)
						{
							
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

		}else if(pXSleepParam->up==1)
		{
			if(pXSleepParam->falseVlue==0)
			{
				pXSleepParam->falseVlue=1;

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

	pXSleepParam->prevValue=tInputData;

}

void cal_step(int *pInputData, int tInputLength)
{
	if(pXStepParam==NULL)
	{
		pXStepParam=(StepParam *)malloc(sizeof(StepParam));
		memset(pXStepParam, 0, sizeof(StepParam));
	}

printf("damon ====> init val : %d %d %d \n", pXStepParam->minIndex, pXStepParam->maxIndex, pXStepParam->falseVlue);


	int i=0;
	int tTempCurVal=0;
	int tTempAvgVal=0;
	int tTempHighHzVal=0;
	for(i=0; i<tInputLength; i++)
	{
		if(i==0)
		{
			pXStepParam->prevValue=pInputData[0];
			continue;
		}
	
		tTempCurVal=pInputData[i];
		pXStepParam->totalValue += tTempCurVal;
		pXStepParam->totalCount++;
		tTempAvgVal=pXStepParam->totalValue/pXStepParam->totalCount;
		
		if(tTempCurVal>pXStepParam->prevValue)	// up
		{
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
				//	printf("damon ===> delete max val 00 : %d %d \n", pXStepParam->pMaxValue[pXStepParam->maxIndex-1].index, 
				//		pXStepParam->pMaxValue[pXStepParam->maxIndex-1].value);

						pXStepParam->falseVlue=0;
						pXStepParam->maxIndex--;
					}else if((i-pXStepParam->pMaxValue[pXStepParam->maxIndex-1].index>8) && (tTempCurVal-tTempAvgVal>10))
					{
				//	printf("damon ===> delete max val 11 : %d %d \n", pXStepParam->pMaxValue[pXStepParam->maxIndex-1].index, 
				//		pXStepParam->pMaxValue[pXStepParam->maxIndex-1].value);

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
						printf("damon ===> min : %d %d  (%d, %d) (%d, %d) \n", i, pXStepParam->minIndex,
							pXStepParam->pMinValue[pXStepParam->minIndex-1].index, 
							pXStepParam->pMinValue[pXStepParam->minIndex-1].value,
							pXStepParam->pMinValue[pXStepParam->minIndex-2].index, 
							pXStepParam->pMinValue[pXStepParam->minIndex-2].value);

						if((pXStepParam->stepMode==dXMode_Static) && (i-pXStepParam->pMinValue[pXStepParam->minIndex-1].index>120))
							pXStepParam->minIndex=0;
						else
						if((i-pXStepParam->pMinValue[pXStepParam->minIndex-1].index>200) 
							|| (pXStepParam->lastStepIndex>0 && i-pXStepParam->lastStepIndex>300))
						{
							printf("damon ===> enter static mode : %d %d  %d \n", i, pXStepParam->lastStepIndex, pXStepParam->pMinValue[pXStepParam->minIndex-1].index);							
							if(pXStepParam->stepMode==dXMode_Walk)
							{
								int j=0;
								for(j=0; j<pXStepParam->minIndex; j++)
								{
									if(tXTestCnt<1024)
										pXTestData[tXTestCnt++]=pXStepParam->pMinValue[j].index;
								}
								
								pXStepParam->stepCount += pXStepParam->minIndex;
							}
							pXStepParam->minIndex=0;
							pXStepParam->stepMode=dXMode_Static;
							pXStepParam->startStep=0;
							pXStepParam->lastStepIndex=0;
						}else if(pXStepParam->minIndex>=2)
						{
				//		if(i==936)
				//			printf("damon ===> test 00000 \n");
				
							if(i-pXStepParam->pMinValue[pXStepParam->minIndex-1].index<15)
							{
								if(pXStepParam->stepMode==dXMode_Static)
									pXStepParam->minIndex=0;
								else
									pXStepParam->minIndex--;								
							}else if(pXStepParam->pMinValue[pXStepParam->minIndex-1].value-pXStepParam->pMinValue[pXStepParam->minIndex-2].value>100)
							{
								if(pXStepParam->pMinValue[pXStepParam->minIndex-1].index-pXStepParam->pMinValue[pXStepParam->minIndex-2].index<35)
								{
																
									pXStepParam->pMinValue[pXStepParam->minIndex-1].value=0;
									pXStepParam->pMinValue[pXStepParam->minIndex-1].index=0;
									pXStepParam->minIndex--;
								}
							}else if(pXStepParam->pMinValue[pXStepParam->minIndex-2].value-pXStepParam->pMinValue[pXStepParam->minIndex-1].value>100)
							{
								if(pXStepParam->pMinValue[pXStepParam->minIndex-1].index-pXStepParam->pMinValue[pXStepParam->minIndex-2].index<35)
								{
								
									pXStepParam->pMinValue[pXStepParam->minIndex-2].value=pXStepParam->pMinValue[pXStepParam->minIndex-1].value;
									pXStepParam->pMinValue[pXStepParam->minIndex-2].index=pXStepParam->pMinValue[pXStepParam->minIndex-1].index;
									pXStepParam->minIndex--;								
								}
							}else if(pXStepParam->maxIndex>1)
							{
								int tTempSubVal=pXStepParam->pMaxValue[pXStepParam->maxIndex-1].value-pXStepParam->pMinValue[pXStepParam->minIndex-2].value;
								int tTempSumVal=pXStepParam->pMaxValue[pXStepParam->maxIndex-1].value+pXStepParam->pMinValue[pXStepParam->minIndex-2].value;

								if(pXStepParam->pMinValue[pXStepParam->minIndex-1].value>tTempSumVal/2)
								{
									pXStepParam->minIndex--;
								}else if((tTempSubVal>170 && pXStepParam->pMaxValue[pXStepParam->maxIndex-1].value-pXStepParam->pMinValue[pXStepParam->minIndex-1].value<150) && 
									(pXStepParam->pMaxValue[pXStepParam->maxIndex-2].value-pXStepParam->pMinValue[pXStepParam->minIndex-1].value<150))
								{
										printf("damon ===> delete min val 66 : %d %d %d - %d %d \n", tTempSubVal, pXStepParam->pMinValue[pXStepParam->minIndex-1].index, 
											pXStepParam->pMinValue[pXStepParam->minIndex-1].value, pXStepParam->pMaxValue[pXStepParam->maxIndex-2].value, pXStepParam->pMaxValue[pXStepParam->maxIndex-1].value);
								
									pXStepParam->minIndex--;
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
						if(pXStepParam->pMinValue[pXStepParam->minIndex-1].index>2065 && 
							pXStepParam->pMinValue[pXStepParam->minIndex-1].index<2142)
							printf("damon ===> delete min val 22 : %d %d \n", pXStepParam->pMinValue[pXStepParam->minIndex-1].index, 
								pXStepParam->pMinValue[pXStepParam->minIndex-1].value);
						
						pXStepParam->minIndex--;
					}


					if(pXStepParam->stepMode==dXMode_Static)
					{
						if(pXStepParam->minIndex>=7)
						{
							// cal step count 
						
							pXStepParam->startStep += 5;
							if(pXStepParam->startStep>=dXStartWalkStep)
							{
								pXStepParam->stepCount += pXStepParam->startStep;
								pXStepParam->stepMode=dXMode_Walk;
								pXStepParam->startStep=0;

								pXStepParam->lastStepIndex=pXStepParam->pMinValue[4].index;
							}

							printf("damon ===> min data : (%d, %d)  (%d, %d)  (%d, %d)  (%d, %d)  (%d, %d) (%d, %d) (%d, %d) \n", 
								pXStepParam->pMinValue[0].index, pXStepParam->pMinValue[0].value,
								pXStepParam->pMinValue[1].index, pXStepParam->pMinValue[1].value,
								pXStepParam->pMinValue[2].index, pXStepParam->pMinValue[2].value,
								pXStepParam->pMinValue[3].index, pXStepParam->pMinValue[3].value,
								pXStepParam->pMinValue[4].index, pXStepParam->pMinValue[4].value,
								pXStepParam->pMinValue[5].index, pXStepParam->pMinValue[5].value,
								pXStepParam->pMinValue[6].index, pXStepParam->pMinValue[6].value);
							
							int j=0;
							for(j=0; j<5; j ++)
							{
							// test
							if(tXTestCnt<1024)
								pXTestData[tXTestCnt++]=pXStepParam->pMinValue[j].index;
							
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
						}

					}else if(pXStepParam->stepMode==dXMode_Walk)
					{
						if(pXStepParam->minIndex>=5)
						{
							// cal step count 
						
							pXStepParam->stepCount += 3;
							pXStepParam->lastStepIndex=pXStepParam->pMinValue[2].index;

							printf("damon ===> min data : (%d, %d)   (%d, %d)   (%d, %d) \n",
								pXStepParam->pMinValue[0].index, pXStepParam->pMinValue[0].value,
								pXStepParam->pMinValue[1].index, pXStepParam->pMinValue[1].value,
								pXStepParam->pMinValue[2].index, pXStepParam->pMinValue[2].value);

							int j=0;
							for(j=0; j<3; j ++)
							{
							// test
							if(tXTestCnt<1024)
								pXTestData[tXTestCnt++]=pXStepParam->pMinValue[j].index;
							
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
					
					if(pXStepParam->pMinValue[pXStepParam->minIndex-1].index>2065 && 
						pXStepParam->pMinValue[pXStepParam->minIndex-1].index<2142)
						printf("damon ===> delete min val 00 : %d %d \n", pXStepParam->pMinValue[pXStepParam->minIndex-1].index, 
							pXStepParam->pMinValue[pXStepParam->minIndex-1].value);
					
						pXStepParam->falseVlue=0;
						pXStepParam->minIndex--;
					}else if((i-pXStepParam->pMinValue[pXStepParam->minIndex-1].index>8) && (tTempAvgVal-tTempCurVal>10))
					{					
					if(pXStepParam->pMinValue[pXStepParam->minIndex-1].index>2065 && 
						pXStepParam->pMinValue[pXStepParam->minIndex-1].index<2142)
						printf("damon ===> delete min val 11 : %d %d \n", pXStepParam->pMinValue[pXStepParam->minIndex-1].index, 
							pXStepParam->pMinValue[pXStepParam->minIndex-1].value);
					
						pXStepParam->falseVlue=0;
						pXStepParam->minIndex--;
					}
				}
			}else if(pXStepParam->up==1)
			{
				if(pXStepParam->falseVlue==0)
				{
					pXStepParam->falseVlue=1;

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

					if(pXStepParam->pMaxValue[pXStepParam->maxIndex-1].value-tTempAvgVal<5)
					{
						pXStepParam->maxIndex--;
					}

					if(pXStepParam->maxIndex>=7)
					{
						int j=0;
						for(j=0; j<5; j++)
						{
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
					}
				}
			}
		}

		pXStepParam->prevValue=tTempCurVal;
	}

	printf("damon ====> step count : %d %d \n", pXStepParam->stepCount, tXTestCnt);
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
	
	int i=0;
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

//	draw_image(pTempGSensorData, tTempGSensorDataNum, "src gsensor img");
	filter_data(pTempGSensorData, tTempGSensorDataNum);
	cal_step(pTempGSensorData, tTempGSensorDataNum);
	draw_image(pTempGSensorData, tTempGSensorDataNum, "src gsensor img");

	cvWaitKey(0);
	while(1)
	{
		sleep(1);
	}

	return 0;
}

