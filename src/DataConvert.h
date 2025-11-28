//==================================================================================================
//	モジュール DataConvert																			
//																									
//	GetRangeValue, BinaryToVolt, VoltToBinaryメソッドを公開します。									
//==================================================================================================


//--------------------------------------------------------------------------------------------------
//	メソッド	:	GetRangeValue()																	
//	機能		:	AioSetAiRange, AioSetAoRangeで使用するレンジ値から、							
//					レンジの最大値と最小値を取得します。											
//	引数		:	short	RangeData	AioSetAiRange, AioSetAoRangeで定義されるレンジ値			
//					float*	Max			レンジの最大値を格納する変数のアドレス						
//					float*	Min			レンジの最小値を格納する変数のアドレス						
//	戻り値		:	0	:正常終了																	
//					-1	:RangeDataが異常、Max, MinがNULL											
//--------------------------------------------------------------------------------------------------
long GetRangeValue(short RangeData, float * Max, float * Min)
{
	if((Max == NULL) || (Min == NULL)){
		return -1;
	}
	switch(RangeData){
	case 0:		*Max = (float)10;		*Min = (float)-10;		break;
	case 1:		*Max = (float)5;		*Min = (float)-5;		break;
	case 2:		*Max = (float)2.5;		*Min = (float)-2.5;		break;
	case 3:		*Max = (float)1.25;		*Min = (float)-1.25;	break;
	case 4:		*Max = (float)1;		*Min = (float)-1;		break;
	case 5:		*Max = (float)0.625;	*Min = (float)-0.625;	break;
	case 6:		*Max = (float)0.5;		*Min = (float)-0.5;		break;
	case 7:		*Max = (float)0.3125;	*Min = (float)-0.3125;	break;
	case 8:		*Max = (float)0.25;		*Min = (float)-0.25;	break;
	case 9:		*Max = (float)0.125;	*Min = (float)-0.125;	break;
	case 10:	*Max = (float)0.1;		*Min = (float)-0.1;		break;
	case 11:	*Max = (float)0.05;		*Min = (float)-0.05;	break;
	case 12:	*Max = (float)0.025;	*Min = (float)-0.025;	break;
	case 13:	*Max = (float)0.0125;	*Min = (float)-0.0125;	break;
	case 50:	*Max = (float)10;		*Min = (float)0;		break;
	case 51:	*Max = (float)5;		*Min = (float)0;		break;
	case 52:	*Max = (float)4.095;	*Min = (float)0;		break;
	case 53:	*Max = (float)2.5;		*Min = (float)0;		break;
	case 54:	*Max = (float)1.25;		*Min = (float)0;		break;
	case 55:	*Max = (float)1;		*Min = (float)0;		break;
	case 56:	*Max = (float)0.5;		*Min = (float)0;		break;
	case 57:	*Max = (float)0.25;		*Min = (float)0;		break;
	case 58:	*Max = (float)0.1;		*Min = (float)0;		break;
	case 59:	*Max = (float)0.05;		*Min = (float)0;		break;
	case 60:	*Max = (float)0.025;	*Min = (float)0;		break;
	case 61:	*Max = (float)0.0125;	*Min = (float)0;		break;
	case 100:	*Max = (float)20;		*Min = (float)0;		break;
	case 101:	*Max = (float)20;		*Min = (float)4;		break;
	case 150:	*Max = (float)5;		*Min = (float)1;		break;
	default:	*Max = (float)0;		*Min = (float)0;		return -1;
	}
	return 0;
}


//--------------------------------------------------------------------------------------------------
//	メソッド	:	BinaryToVolt()																	
//	機能		:	バイナリデータを電圧や電流に変換します。										
//	引数		:	float	Max		レンジの最大値													
//					float	Min		レンジの最小値													
//					short	Bits	分解能(12 または 16)											
//					long	Binary	変換するバイナリデータ											
//	戻り値		:	変換されたデータをfloat型で返します。											
//--------------------------------------------------------------------------------------------------
float BinaryToVolt(float Max, float Min, short Bits, long Binary)
{
	long	Resolution;
	switch (Bits){
	case 12:
		Resolution = 4095;
		break;
	case 16:
		Resolution = 65535;
		break;
	default :
		Resolution = 4095;
		break;
	}
	return Binary * (Max - Min) / Resolution + Min;
}


//--------------------------------------------------------------------------------------------------
//	メソッド	:	VoltToBinary()																	
//	機能		:	電圧や電流をバイナリデータに変換します。										
//	引数		:	float	Max		レンジの最大値													
//					float	Min		レンジの最小値													
//					short	Bits	分解能(12 または 16)											
//					float	Volt	変換する電圧や電流データ										
//	戻り値		:	変換されたバイナリデータをlong型で返します。									
//--------------------------------------------------------------------------------------------------
long VoltToBinary(float Max, float Min, short Bits, float Volt)
{
	long	Resolution;
	if(Max == Min){
		return 0;
	}
	switch (Bits){
	case 12:
		Resolution = 4095;
		break;
	case 16:
		Resolution = 65535;
		break;
	default :
		Resolution = 4095;
		break;
	}
	return (long)(Resolution * (Volt - Min) / (Max - Min));
}

