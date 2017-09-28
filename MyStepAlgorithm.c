
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


#if 0

#define dXStartWalkStep	10

#define dXUseCalSleep

typedef void (*SleepCallback)(unsigned int tInputStartTime, int tInputMode);

typedef struct{
	int value;
	int index;
	unsigned int time;
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

	int startTime;
	int staticCount;
	int prevCommitMode;
	int sleepMode;
	int totalValue;
	int totalCount;

	unsigned int startWalkIndex;
	unsigned int startWalkTime;
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


static StepParam *pXStepParam=NULL;
static SleepCallback FCSleepCallback=NULL;

static StepValue pXSleepBuffer[10]={0};
static char tXSleepBufCnt=0;


typedef struct 
{
	int pData[5];
	int cnt;
	int index;
}StepFilterParam;
StepFilterParam *pXStepFilterParam=NULL;

int GetStepCount()
{
	if(pXStepParam==NULL)
		return 0;

	return pXStepParam->stepCount;
}

void MySleepCallback(unsigned int tInputStartTime, int tInputMode)
{
	printf("damon ===> sleep call back : %d %d \n", tInputStartTime, tInputMode);
	pXSleepParam->prevCommitMode=tInputMode;
}


void ProcessWaveStep(int tInputMinStepPrevIdx, int tInputMinStepCurIdx, unsigned int tInputTime)
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
					FCSleepCallback(pXSleepParam->startWalkTime /*pXSleepParam->startTime*/, dXSleep_WakeUp);

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
					FCSleepCallback(tInputTime /*pXSleepParam->startTime*/, dXSleep_DeepSleep);
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
					FCSleepCallback(tInputTime /*pXSleepParam->startTime*/, dXSleep_DeepSleep);
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
					FCSleepCallback(tInputTime /*pXSleepParam->startTime*/, dXSleep_LightSleep);				
			}
		}
#endif
}


int StepValueFilter(int tInputData)
{
//printf("damon ===> %d - %d %d \n", tXFilterCnt, tInputData, index);
	if(pXStepFilterParam==NULL)
	{
		pXStepFilterParam=(StepFilterParam *)malloc(sizeof(StepFilterParam));
		memset(pXStepFilterParam, 0, sizeof(StepFilterParam));
	}

	if(pXStepFilterParam->cnt<5)
	{
		pXStepFilterParam->pData[pXStepFilterParam->cnt++]=tInputData;
		return -1;
	}

	int tTempIndex=pXStepFilterParam->index%5;
	pXStepFilterParam->index=(pXStepFilterParam->index+1)%5;
	pXStepFilterParam->pData[tTempIndex]=tInputData;

	int pTempValue[5]={0};
	int m=0, n=0;

	for(m=0; m<5; m++)
		pTempValue[m]=pXStepFilterParam->pData[m];

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

	return pTempValue[2];

/*	printf("damon ===> filter data : %d %d %d %d %d \n", pXFilterBuf[0].value, pXFilterBuf[1].value, 
		pXFilterBuf[2].value, pXFilterBuf[3].value, pXFilterBuf[4].value);*/
}


void ProcessUpData(int tInputData, int tInputIdx, unsigned int tInputTime)
{
	int tTempCurVal=tInputData;
	int tTempAvgVal=pXStepParam->totalValue/pXStepParam->totalCount;

	if(pXStepParam->up==0 && pXStepParam->down==0)
	{
		pXStepParam->pMinValue[pXStepParam->minIndex].value=pXStepParam->prevValue;
		pXStepParam->pMinValue[pXStepParam->minIndex].index=tInputIdx-1;
		pXStepParam->pMinValue[pXStepParam->minIndex].time=tInputTime;
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
			}else if((tInputIdx-pXStepParam->pMaxValue[pXStepParam->maxIndex-1].index>8) && (pXStepParam->pMaxValue[pXStepParam->maxIndex-1].index-tTempCurVal<20)/*(tTempCurVal-tTempAvgVal>10)*/)
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
			if(tInputIdx==2169)
				printf("damon ===> min : %d %d	(%d, %d) (%d, %d) \n", tInputIdx, pXStepParam->minIndex,
					pXStepParam->pMinValue[pXStepParam->minIndex-1].index, 
					pXStepParam->pMinValue[pXStepParam->minIndex-1].value,
					pXStepParam->pMinValue[pXStepParam->minIndex-2].index, 
					pXStepParam->pMinValue[pXStepParam->minIndex-2].value);
			

				if((pXStepParam->stepMode==dXMode_Static) && (tInputIdx-pXStepParam->pMinValue[pXStepParam->minIndex-1].index>120))
				{
					if(pXSleepParam->sleepMode!=dXSleep_NotDetec)
					{
						int j=0;
						for(j=0; j<pXStepParam->minIndex-1; j++)
						{
						printf("damon  ===> delete val : %d  %d \n", pXStepParam->minIndex, pXStepParam->pMinValue[j].index);
							if(tXSleepBufCnt<10)
							{
								pXSleepBuffer[tXSleepBufCnt].index=pXStepParam->pMinValue[j].index;
								pXSleepBuffer[tXSleepBufCnt].time=pXStepParam->pMinValue[j].time;
								tXSleepBufCnt++;
							}
				#ifdef dXUseCalSleep
							if(tXSleepBufCnt>=2)
							{
								ProcessWaveStep(pXSleepBuffer[tXSleepBufCnt-2].index, pXSleepBuffer[tXSleepBufCnt-1].index, pXSleepBuffer[tXSleepBufCnt-2].time);

								pXSleepBuffer[tXSleepBufCnt-2].index=pXSleepBuffer[tXSleepBufCnt-1].index;
								pXSleepBuffer[tXSleepBufCnt-2].time=pXSleepBuffer[tXSleepBufCnt-1].time;											
								tXSleepBufCnt--;
							}
				#endif
						}
					}
					
					pXStepParam->minIndex=0;
				}
				else	if((tInputIdx-pXStepParam->pMinValue[pXStepParam->minIndex-1].index>200) 
					|| (pXStepParam->lastStepIndex>0 && tInputIdx-pXStepParam->lastStepIndex>300))
				{
				
					printf("damon ===> enter static mode : %d %d  %d \n", tInputIdx, pXStepParam->lastStepIndex, pXStepParam->pMinValue[pXStepParam->minIndex-1].index);
					if(pXStepParam->stepMode==dXMode_Walk)
					{
						int j=0;
						for(j=0; j<pXStepParam->minIndex-1; j++)
						{
							if(tXSleepBufCnt<10)
							{
								pXSleepBuffer[tXSleepBufCnt].index=pXStepParam->pMinValue[j].index;
								pXSleepBuffer[tXSleepBufCnt].time=pXStepParam->pMinValue[j].time;
								tXSleepBufCnt++;
							}

				#ifdef dXUseCalSleep
							if(tXSleepBufCnt>=2)
							{
								ProcessWaveStep(pXSleepBuffer[tXSleepBufCnt-2].index, pXSleepBuffer[tXSleepBufCnt-1].index, pXSleepBuffer[tXSleepBufCnt-2].time);

								pXSleepBuffer[tXSleepBufCnt-2].index=pXSleepBuffer[tXSleepBufCnt-1].index;
								pXSleepBuffer[tXSleepBufCnt-2].time=pXSleepBuffer[tXSleepBufCnt-1].time;											
								tXSleepBufCnt--;
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
		
					if(tInputIdx-pXStepParam->pMinValue[pXStepParam->minIndex-1].index<15)
					{
					printf("damon ===> 000 delete min val : %d \n", pXStepParam->pMinValue[pXStepParam->minIndex-1].value);
						if(pXStepParam->stepMode==dXMode_Static)
							pXStepParam->minIndex=0;
						else if(pXStepParam->pMinValue[pXStepParam->minIndex-1].value-pXStepParam->prevValue>10)
							pXStepParam->minIndex--;
					}else if(pXStepParam->pMinValue[pXStepParam->minIndex-1].value-pXStepParam->pMinValue[pXStepParam->minIndex-2].value>100)
					{
						if(pXStepParam->pMinValue[pXStepParam->minIndex-1].index-pXStepParam->pMinValue[pXStepParam->minIndex-2].index<35)
						{
						printf("damon ===> 111 delete min val : %d \n", pXStepParam->pMinValue[pXStepParam->minIndex-1].value);
														
							pXStepParam->pMinValue[pXStepParam->minIndex-1].value=0;
							pXStepParam->pMinValue[pXStepParam->minIndex-1].index=0;
							pXStepParam->pMinValue[pXStepParam->minIndex-1].time=0;
							pXStepParam->minIndex--;
						}
					}else if(pXStepParam->pMinValue[pXStepParam->minIndex-2].value-pXStepParam->pMinValue[pXStepParam->minIndex-1].value>100)
					{
						if(pXStepParam->pMinValue[pXStepParam->minIndex-1].index-pXStepParam->pMinValue[pXStepParam->minIndex-2].index<35)
						{
						printf("damon ===> 222 delete min val : %d \n", pXStepParam->pMinValue[pXStepParam->minIndex-2].value);
						
							pXStepParam->pMinValue[pXStepParam->minIndex-2].value=pXStepParam->pMinValue[pXStepParam->minIndex-1].value;
							pXStepParam->pMinValue[pXStepParam->minIndex-2].index=pXStepParam->pMinValue[pXStepParam->minIndex-1].index;
							pXStepParam->pMinValue[pXStepParam->minIndex-2].time=pXStepParam->pMinValue[pXStepParam->minIndex-1].time;
							pXStepParam->minIndex--;								
						}
					}else if(pXStepParam->maxIndex>1)
					{
						int tTempSubVal=pXStepParam->pMaxValue[pXStepParam->maxIndex-1].value-pXStepParam->pMinValue[pXStepParam->minIndex-2].value;
						int tTempSumVal=pXStepParam->pMaxValue[pXStepParam->maxIndex-1].value+pXStepParam->pMinValue[pXStepParam->minIndex-2].value;


						if(pXStepParam->pMinValue[pXStepParam->minIndex-1].value>tTempSumVal/2)
						{
						printf("damon ===> 333 delete min val : %d \n", pXStepParam->pMinValue[pXStepParam->minIndex-1].value); 							
							pXStepParam->minIndex--;
						}else if((tTempSubVal>170 && pXStepParam->pMaxValue[pXStepParam->maxIndex-1].value-pXStepParam->pMinValue[pXStepParam->minIndex-1].value<150) && 
							(pXStepParam->pMaxValue[pXStepParam->maxIndex-2].value-pXStepParam->pMinValue[pXStepParam->minIndex-1].value<150))
						{
								printf("damon ===> delete min val 66 : %d %d %d - %d %d \n", tTempSubVal, pXStepParam->pMinValue[pXStepParam->minIndex-1].index, 
									pXStepParam->pMinValue[pXStepParam->minIndex-1].value, pXStepParam->pMaxValue[pXStepParam->maxIndex-2].value, pXStepParam->pMaxValue[pXStepParam->maxIndex-1].value);
						
							pXStepParam->minIndex--;
						}else
						{
					/*	printf("damon ===> value %d - %d  : (%d, %d)  (%d , %d) \n",
							pXStepParam->minIndex, pXStepParam->maxIndex,
							pXStepParam->pMinValue[pXStepParam->minIndex-1].index, 
							pXStepParam->pMinValue[pXStepParam->minIndex-1].value,
							pXStepParam->pMaxValue[pXStepParam->maxIndex-1].index,
							pXStepParam->pMaxValue[pXStepParam->maxIndex-1].value);
						*/

							if(pXStepParam->maxIndex>=2)
							{
								int tTempHTotal=0, tTempHAvgVal=0;
								int j=0;										
								for(j=0; j<pXStepParam->maxIndex-1; j++)
								{
									tTempHTotal += pXStepParam->pMaxValue[j].value;
								}

								tTempHAvgVal=tTempHTotal/(pXStepParam->maxIndex-1);
								if(tTempHAvgVal- pXStepParam->pMinValue[pXStepParam->minIndex-1].value<30)
								{
							printf("damon ===> 888 delete min val : %d %d \n", tTempHAvgVal, pXStepParam->pMinValue[pXStepParam->minIndex-1].value);
									pXStepParam->minIndex--;
								}
							}else if(pXStepParam->minIndex>=3 && pXStepParam->maxIndex==0)
							{									
							printf("damon ===> 444 delete min val : %d \n", pXStepParam->pMinValue[pXStepParam->minIndex-1].value);
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

			if(tInputIdx>1033 && tInputIdx<1119)
			{
				printf("damon ===> add min val : %d %d - %d %d \n", tInputIdx, pXStepParam->prevValue, pXStepParam->minIndex, pXStepParam->pMinValue[pXStepParam->minIndex-1].value);
			}

	//	printf("damon ===> %d %d \n", pXStepParam->pMinValue[pXStepParam->minIndex-1].value, pXStepParam->prevValue);
			pXStepParam->pMinValue[pXStepParam->minIndex].value=pXStepParam->prevValue;
			pXStepParam->pMinValue[pXStepParam->minIndex].index=tInputIdx-1;
			pXStepParam->pMinValue[pXStepParam->minIndex].time=tInputTime;
			pXStepParam->minIndex++;
		
			pXStepParam->totalValue=0;
			pXStepParam->totalCount=0;

		}else if(tInputIdx-pXStepParam->pMinValue[pXStepParam->minIndex-1].index>5)
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
				printf("damon ====> start walk	maxAvg = %d , minAvg=%d \n", tTempHAvgVal, tTempLAvgVal);

					
					pXStepParam->startStep += 5;
					if(pXStepParam->startStep>=dXStartWalkStep)
					{
				printf("damon ===> start walk : %d %d %d \n", pXStepParam->maxIndex, pXStepParam->maxValueCnt, pXStepParam->minIndex);
				
						if((pXStepParam->maxValueCnt>5) && (pXStepParam->maxIndex>=2) && (tTempHAvgVal-tTempLAvgVal>=30))
						{
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
								pXSleepParam->startWalkTime=pXStepParam->pMinValue[0].time;
							}
				#endif
						}else
						{
							pXStepParam->startStep -= 5;
						}
					}

				/*	if(pXStepParam->stepMode==dXMode_Walk)
					{
						printf("damon ===> start walk max data %d %d :	(%d, %d)  (%d, %d)	(%d, %d)  (%d, %d)	(%d, %d)  (%d, %d)	(%d, %d) \n", 
							pXStepParam->maxIndex, pXStepParam->maxValueCnt,
							pXStepParam->pMaxValue[0].index, pXStepParam->pMaxValue[0].value,
							pXStepParam->pMaxValue[1].index, pXStepParam->pMaxValue[1].value,
							pXStepParam->pMaxValue[2].index, pXStepParam->pMaxValue[2].value,
							pXStepParam->pMaxValue[3].index, pXStepParam->pMaxValue[3].value,
							pXStepParam->pMaxValue[4].index, pXStepParam->pMaxValue[4].value,
							pXStepParam->pMaxValue[5].index, pXStepParam->pMaxValue[5].value,
							pXStepParam->pMaxValue[6].index, pXStepParam->pMaxValue[6].value);
					}*/


					printf("damon ===> min data : (%d, %d)	(%d, %d)  (%d, %d)	(%d, %d)  (%d, %d) (%d, %d) (%d, %d) \n", 
						pXStepParam->pMinValue[0].index, pXStepParam->pMinValue[0].value,
						pXStepParam->pMinValue[1].index, pXStepParam->pMinValue[1].value,
						pXStepParam->pMinValue[2].index, pXStepParam->pMinValue[2].value,
						pXStepParam->pMinValue[3].index, pXStepParam->pMinValue[3].value,
						pXStepParam->pMinValue[4].index, pXStepParam->pMinValue[4].value,
						pXStepParam->pMinValue[5].index, pXStepParam->pMinValue[5].value,
						pXStepParam->pMinValue[6].index, pXStepParam->pMinValue[6].value);

					if(tTempHAvgVal-tTempLAvgVal>30)
					{
						int j=0;
						for(j=0; j<5; j ++)
						{
							// test
							if(tXSleepBufCnt<10)
							{
								pXSleepBuffer[tXSleepBufCnt].index=pXStepParam->pMinValue[j].index;
								pXSleepBuffer[tXSleepBufCnt].time=pXStepParam->pMinValue[j].time;
								tXSleepBufCnt++;
							}

				#ifdef dXUseCalSleep
							if(tXSleepBufCnt>=2)
							{
								ProcessWaveStep(pXSleepBuffer[tXSleepBufCnt-2].index, pXSleepBuffer[tXSleepBufCnt-1].index, pXSleepBuffer[tXSleepBufCnt-2].time);

								pXSleepBuffer[tXSleepBufCnt-2].index=pXSleepBuffer[tXSleepBufCnt-1].index;
								pXSleepBuffer[tXSleepBufCnt-2].time=pXSleepBuffer[tXSleepBufCnt-1].time;											
								tXSleepBufCnt--;
							}
				#endif
						
							if(j+5<7)
							{
								pXStepParam->pMinValue[j].value=pXStepParam->pMinValue[j+5].value;
								pXStepParam->pMinValue[j].index=pXStepParam->pMinValue[j+5].index;
								pXStepParam->pMinValue[j].time=pXStepParam->pMinValue[j+5].time;
							}else
							{
								pXStepParam->pMinValue[j].value=0;
								pXStepParam->pMinValue[j].index=0;
								pXStepParam->pMinValue[j].time=0;
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

					printf("damon ===> min data : (%d, %d)	 (%d, %d)	(%d, %d) \n",
						pXStepParam->pMinValue[0].index, pXStepParam->pMinValue[0].value,
						pXStepParam->pMinValue[1].index, pXStepParam->pMinValue[1].value,
						pXStepParam->pMinValue[2].index, pXStepParam->pMinValue[2].value);

					
				/*	printf("damon ===> max data : (%d, %d)	 (%d, %d)	(%d, %d) \n",
						pXStepParam->pMaxValue[0].index, pXStepParam->pMaxValue[0].value,
						pXStepParam->pMaxValue[1].index, pXStepParam->pMaxValue[1].value,
						pXStepParam->pMaxValue[2].index, pXStepParam->pMaxValue[2].value);
					*/

					int j=0;
					for(j=0; j<3; j ++)
					{
						// test
						if(tXSleepBufCnt<1024)
						{
							pXSleepBuffer[tXSleepBufCnt].index=pXStepParam->pMinValue[j].index;
							pXSleepBuffer[tXSleepBufCnt].time=pXStepParam->pMinValue[j].time;
							tXSleepBufCnt++;
						}
						
			#ifdef dXUseCalSleep
						if(tXSleepBufCnt>=2)
						{
							ProcessWaveStep(pXSleepBuffer[tXSleepBufCnt-2].index, pXSleepBuffer[tXSleepBufCnt-1].index, pXSleepBuffer[tXSleepBufCnt-2].time);

							pXSleepBuffer[tXSleepBufCnt-2].index=pXSleepBuffer[tXSleepBufCnt-1].index;
							pXSleepBuffer[tXSleepBufCnt-2].time=pXSleepBuffer[tXSleepBufCnt-1].time;											
							tXSleepBufCnt--;
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


void ProcessDownData(int tInputData, int tInputIdx, unsigned int tInputTime)
{
	int tTempCurVal=tInputData;
	int tTempAvgVal=pXStepParam->totalValue/pXStepParam->totalCount;

	if(pXStepParam->up==0 && pXStepParam->down==0)
	{
		pXStepParam->down=1;
		pXStepParam->pMaxValue[pXStepParam->maxIndex].value=pXStepParam->prevValue;
		pXStepParam->pMaxValue[pXStepParam->maxIndex].index=tInputIdx-1;
		pXStepParam->pMaxValue[pXStepParam->maxIndex].index=tInputTime;
		pXStepParam->maxIndex++;
	}else if(pXStepParam->down==1)
	{
		if(pXStepParam->falseVlue==1)
		{
			if(tTempCurVal<pXStepParam->pMinValue[pXStepParam->minIndex-1].value)
			{
			
			if(pXStepParam->pMinValue[pXStepParam->minIndex-1].index>1033 && 
				pXStepParam->pMinValue[pXStepParam->minIndex-1].index<1119)
				printf("damon ===> delete min val 00 : %d %d \n", pXStepParam->pMinValue[pXStepParam->minIndex-1].index, 
					pXStepParam->pMinValue[pXStepParam->minIndex-1].value);
			
				pXStepParam->falseVlue=0;
				pXStepParam->minIndex--;
			}else if((tInputIdx-pXStepParam->pMinValue[pXStepParam->minIndex-1].index>8) && (tTempCurVal-pXStepParam->pMinValue[pXStepParam->minIndex-1].index>20)/*(tTempAvgVal-tTempCurVal>10)*/)
			{
	//		if(pXStepParam->pMinValue[pXStepParam->minIndex-1].index>1033 && 
	//			pXStepParam->pMinValue[pXStepParam->minIndex-1].index<1119)
				printf("damon ===> delete min val 11 : %d %d - %d %d %d \n", pXStepParam->pMinValue[pXStepParam->minIndex-1].index, 
					pXStepParam->pMinValue[pXStepParam->minIndex-1].value, tInputIdx, tTempAvgVal, tTempCurVal);
			
				pXStepParam->falseVlue=0;
				pXStepParam->minIndex--;
			}
		}
	}else if(pXStepParam->up==1)
	{
		if(pXStepParam->falseVlue==0)
		{
			pXStepParam->falseVlue=1;


			if(pXStepParam->maxIndex>0)
			{
				if(tInputIdx-pXStepParam->pMaxValue[pXStepParam->maxIndex-1].index>150)
				{
					pXStepParam->maxIndex=0;
				}else if(pXStepParam->maxIndex>=2)
				{
					if(pXStepParam->pMaxValue[pXStepParam->maxIndex-1].value-pXStepParam->pMaxValue[pXStepParam->maxIndex-2].value>100)
					{
						pXStepParam->pMaxValue[pXStepParam->maxIndex-2].value=pXStepParam->pMaxValue[pXStepParam->maxIndex-1].value;
						pXStepParam->pMaxValue[pXStepParam->maxIndex-2].index=pXStepParam->pMaxValue[pXStepParam->maxIndex-1].index;
						pXStepParam->maxIndex--;
					}else if(pXStepParam->pMaxValue[pXStepParam->maxIndex-2].value-pXStepParam->pMaxValue[pXStepParam->maxIndex-1].value>100)
					{
						pXStepParam->maxIndex--;
					}else if(pXStepParam->pMaxValue[pXStepParam->maxIndex-1].index-pXStepParam->pMaxValue[pXStepParam->maxIndex-2].index<10)
					{
						if(pXStepParam->pMaxValue[pXStepParam->maxIndex-1].value-pXStepParam->pMaxValue[pXStepParam->maxIndex-2].value>60)
						{
							pXStepParam->pMaxValue[pXStepParam->maxIndex-2].value=pXStepParam->pMaxValue[pXStepParam->maxIndex-1].value;
							pXStepParam->pMaxValue[pXStepParam->maxIndex-2].index=pXStepParam->pMaxValue[pXStepParam->maxIndex-1].index;
							pXStepParam->maxIndex--;
						}else
						{
							pXStepParam->maxIndex--;										
						}
					}
				}
			}

			pXStepParam->pMaxValue[pXStepParam->maxIndex].index=tInputIdx-1;
			pXStepParam->pMaxValue[pXStepParam->maxIndex].value=pXStepParam->prevValue;
			pXStepParam->maxIndex++;

			pXStepParam->totalCount=0;
			pXStepParam->totalValue=0;
		}else if(tInputIdx-pXStepParam->pMaxValue[pXStepParam->maxIndex-1].index>5)
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
						pXStepParam->pMaxValue[j].time=pXStepParam->pMaxValue[j+5].time;
					}else 
					{
						pXStepParam->pMaxValue[j].value=0;
						pXStepParam->pMaxValue[j].index=0;
						pXStepParam->pMaxValue[j].time=0;
					}
				}

				pXStepParam->maxIndex -= 5;

				pXStepParam->maxValueCnt += 5;
			}
		}
	}
}

void StepCalculation(int tInputData, unsigned int tInputIdx, unsigned int tInputTime)
{
#if 1
	int tTempRetVal=0;
	tTempRetVal=StepValueFilter(tInputData);
	if(tTempRetVal==-1)
		return ;

	tInputData=tTempRetVal;
#endif

	if(pXStepParam==NULL)
	{
		pXStepParam=(StepParam *)malloc(sizeof(StepParam));
		memset(pXStepParam, 0, sizeof(StepParam));

		pXStepParam->prevValue=tInputData;
		return ;
	}

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


	int tTempCurVal=0;
	int tTempAvgVal=0;
	int tTempHighHzVal=0;
#if 1
//	for(i=0; i<tInputLength; i++)
	{	
		tTempCurVal=tInputData;
		pXStepParam->totalValue += tTempCurVal;
		pXStepParam->totalCount++;
		tTempAvgVal=pXStepParam->totalValue/pXStepParam->totalCount;
		
		if(tTempCurVal>pXStepParam->prevValue)	// up
		{
			ProcessUpData(tInputData, tInputIdx, tInputTime);
		}else
		{
			ProcessDownData(tInputData, tInputIdx, tInputTime);
		}

		pXStepParam->prevValue=tTempCurVal;
	}
#endif
}

#endif

