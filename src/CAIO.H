/**
 * @file Caio.h
 * @brief CONTEC AIO (Analog I/O) board API header file
 *
 * Defines constants and macros for CONTEC analog I/O hardware interface.
 * AIO用ヘッダファイル
 */
//==============================================================================================

/**
 * @defgroup ExternalControlSignals External control signals
 * @brief Constants for external trigger and clock signals
 * @{
 */
#define AIO_AIF_CLOCK 0 /**< Analog input external clock */
#define AIO_AIF_START 1 /**< Analog input external start trigger */
#define AIO_AIF_STOP 2  /**< Analog input external stop trigger */
#define AIO_AOF_CLOCK 3 /**< Analog output external clock */
#define AIO_AOF_START 4 /**< Analog output external start trigger */
#define AIO_AOF_STOP 5  /**< Analog output external stop trigger */
/** @} */

/**
 * @defgroup IORange Input/Output range constants
 * @brief Voltage and current range definitions for A/D and D/A conversion
 * @{
 */
#define PM10 0       /**< ±10V */
#define PM5 1        /**< ±5V */
#define PM25 2       /**< ±2.5V */
#define PM125 3      /**< ±1.25V */
#define PM1 4        /**< ±1V */
#define PM0625 5     /**< ±0.625V */
#define PM05 6       /**< ±0.5V */
#define PM03125 7    /**< ±0.3125V */
#define PM025 8      /**< ±0.25V */
#define PM0125 9     /**< ±0.125V */
#define PM01 10      /**< ±0.1V */
#define PM005 11     /**< ±0.05V */
#define PM0025 12    /**< ±0.025V */
#define PM00125 13   /**< ±0.0125V */
#define P10 50       /**< 0～10V */
#define P5 51        /**< 0～5V */
#define P4095 52     /**< 0～4.095V */
#define P25 53       /**< 0～2.5V */
#define P125 54      /**< 0～1.25V */
#define P1 55        /**< 0～1V */
#define P05 56       /**< 0～0.5V */
#define P025 57      /**< 0～0.25V */
#define P01 58       /**< 0～0.1V */
#define P005 59      /**< 0～0.05V */
#define P0025 60     /**< 0～0.025V */
#define P00125 61    /**< 0～0.0125V */
#define P20MA 100    /**< 0～20mA */
#define P4TO20MA 101 /**< 4～20mA */
#define P1TO5 150    /**< 1～5V */
/** @} */

/**
 * @defgroup AIEvent Analog input events
 * @brief Event flags for analog input operations
 * @{
 */
#define AIE_START 0x00000002    /**< AD conversion start condition met event */
#define AIE_RPTEND 0x00000010   /**< Repeat end event */
#define AIE_END 0x00000020      /**< Device operation end event */
#define AIE_DATA_NUM 0x00000080 /**< Specified sampling count stored event */
#define AIE_DATA_TSF 0x00000100 /**< Specified transfer count event */
#define AIE_OFERR 0x00010000    /**< Overflow event */
#define AIE_SCERR 0x00020000    /**< Sampling clock error event */
#define AIE_ADERR 0x00040000    /**< AD conversion error event */
/** @} */

/**
 * @defgroup AOEvent Analog output events
 * @brief Event flags for analog output operations
 * @{
 */
#define AOE_START 0x00000002    /**< DA conversion start condition met event */
#define AOE_RPTEND 0x00000010   /**< Repeat end event */
#define AOE_END 0x00000020      /**< Device operation end event */
#define AOE_DATA_NUM 0x00000080 /**< Specified sampling count output event */
#define AOE_DATA_TSF 0x00000100 /**< Specified transfer count event */
#define AOE_SCERR 0x00020000    /**< Sampling clock error event */
#define AOE_DAERR 0x00040000    /**< DA conversion error event */
/** @} */

/**
 * @defgroup CNTEvent Counter events
 * @brief Event flags for counter operations
 * @{
 */
#define CNTE_DATA_NUM 0x00000010 /**< Compare count match event */
#define CNTE_ORERR 0x00010000    /**< Count overrun event */
#define CNTE_ERR 0x00020000      /**< Counter operation error */
/** @} */

/**
 * @defgroup TMEvent Timer events
 * @brief Event flags for timer operations
 * @{
 */
#define TME_INT 0x00000001 /**< Interval event */
/** @} */

/**
 * @defgroup AIStatus Analog input status flags
 * @brief Status flags for analog input device state
 * @{
 */
#define AIS_BUSY 0x00000001      /**< Device operating */
#define AIS_START_TRG 0x00000002 /**< Waiting for start trigger */
#define AIS_DATA_NUM 0x00000010  /**< Specified sampling count stored */
#define AIS_OFERR 0x00010000     /**< Overflow */
/** @} (Additional status flags follow in file) */
#define AIS_SCERR 0x00020000 // サンプリングクロックエラー
#define AIS_AIERR 0x00040000 // AD変換エラー

//----------------------------------------------------------------------------------------------
//	アナログ出力ステータス
//----------------------------------------------------------------------------------------------
#define AOS_BUSY 0x00000001      // デバイス動作中
#define AOS_START_TRG 0x00000002 // 開始トリガ待ち
#define AOS_DATA_NUM 0x00000010  // 指定サンプリング回数出力
#define AOS_SCERR 0x00020000     // サンプリングクロックエラー
#define AOS_AOERR 0x00040000     // DA変換エラー

//----------------------------------------------------------------------------------------------
//	カウンタステータス
//----------------------------------------------------------------------------------------------
#define CNTS_BUSY 0x00000001     // カウンタ動作中
#define CNTS_DATA_NUM 0x00000010 // 比較カウント一致
#define CNTS_ORERR 0x00010000    // オーバーラン
#define CNTS_ERR 0x00020000      // カウンタ動作エラー

//----------------------------------------------------------------------------------------------
//	アナログ入力メッセージ
//----------------------------------------------------------------------------------------------
#define AIOM_AIE_START 0x1000    // AD変換開始条件成立イベント
#define AIOM_AIE_RPTEND 0x1001   // リピート終了イベント
#define AIOM_AIE_END 0x1002      // デバイス動作終了イベント
#define AIOM_AIE_DATA_NUM 0x1003 // 指定サンプリング回数格納イベント
#define AIOM_AIE_DATA_TSF 0x1007 // 指定転送数毎イベント
#define AIOM_AIE_OFERR 0x1004    // オーバーフローイベント
#define AIOM_AIE_SCERR 0x1005    // サンプリングクロックエラーイベント
#define AIOM_AIE_ADERR 0x1006    // AD変換エラーイベント

//----------------------------------------------------------------------------------------------
//	アナログ出力メッセージ
//----------------------------------------------------------------------------------------------
#define AIOM_AOE_START 0x1020    // DA変換開始条件成立イベント
#define AIOM_AOE_RPTEND 0x1021   // リピート終了イベント
#define AIOM_AOE_END 0x1022      // デバイス動作終了イベント
#define AIOM_AOE_DATA_NUM 0x1023 // 指定サンプリング回数出力イベント
#define AIOM_AOE_DATA_TSF 0x1027 // 指定転送数毎イベント
#define AIOM_AOE_SCERR 0x1025    // サンプリングクロックエラーイベント
#define AIOM_AOE_DAERR 0x1026    // DA変換エラーイベント

//----------------------------------------------------------------------------------------------
//	カウンタメッセージ
//----------------------------------------------------------------------------------------------
#define AIOM_CNTE_DATA_NUM 0x1042 // 比較カウント一致イベント
#define AIOM_CNTE_ORERR 0x1043    // カウントオーバーランイベント
#define AIOM_CNTE_ERR 0x1044      // カウント動作エラーイベント

//----------------------------------------------------------------------------------------------
//	タイマメッセージ
//----------------------------------------------------------------------------------------------
#define AIOM_TME_INT 0x1060 // インターバル成立イベント

//----------------------------------------------------------------------------------------------
//	アナログ入力添付データ
//----------------------------------------------------------------------------------------------
#define AIAT_AI 0x00000001   // アナログ入力付属情報
#define AIAT_AO0 0x00000100  // アナログ出力データ
#define AIAT_DIO0 0x00010000 // デジタル入出力データ
#define AIAT_CNT0 0x01000000 // カウンタチャネル０データ
#define AIAT_CNT1 0x02000000 // カウンタチャネル１データ

//----------------------------------------------------------------------------------------------
//	カウンタ動作モード
//----------------------------------------------------------------------------------------------
#define CNT_LOADPRESET 0x0000001 // プリセットカウント値のロード
#define CNT_LOADCOMP 0x0000002   // 比較カウント値のロード

//----------------------------------------------------------------------------------------------
//	イベントコントローラ接続先信号
//----------------------------------------------------------------------------------------------
#define AIOECU_DEST_AI_CLK 4       // アナログ入力サンプリングクロック
#define AIOECU_DEST_AI_START 0     // アナログ入力変換開始信号
#define AIOECU_DEST_AI_STOP 2      // アナログ入力変換停止信号
#define AIOECU_DEST_AO_CLK 36      // アナログ出力サンプリングクロック
#define AIOECU_DEST_AO_START 32    // アナログ出力変換開始信号
#define AIOECU_DEST_AO_STOP 34     // アナログ出力変換停止信号
#define AIOECU_DEST_CNT0_UPCLK 134 // カウンタ０アップクロック信号
#define AIOECU_DEST_CNT1_UPCLK 135 // カウンタ１アップクロック信号
#define AIOECU_DEST_CNT0_START 128 // カウンタ０、タイマ０動作開始信号
#define AIOECU_DEST_CNT1_START 129 // カウンタ１、タイマ１動作開始信号
#define AIOECU_DEST_CNT0_STOP 130  // カウンタ０、タイマ０動作停止信号
#define AIOECU_DEST_CNT1_STOP 131  // カウンタ１、タイマ１動作停止信号
#define AIOECU_DEST_MASTER1 104    // 同期バスマスタ信号１
#define AIOECU_DEST_MASTER2 105    // 同期バスマスタ信号２
#define AIOECU_DEST_MASTER3 106    // 同期バスマスタ信号３

//----------------------------------------------------------------------------------------------
//	イベントコントローラ接続元信号
//----------------------------------------------------------------------------------------------
#define AIOECU_SRC_OPEN -1          // 未接続
#define AIOECU_SRC_AI_CLK 4         // アナログ入力内部クロック信号
#define AIOECU_SRC_AI_EXTCLK 146    // アナログ入力外部クロック信号
#define AIOECU_SRC_AI_TRGSTART 144  // アナログ入力外部トリガ開始信号
#define AIOECU_SRC_AI_LVSTART 28    // アナログ入力レベルトリガ開始信号
#define AIOECU_SRC_AI_STOP 17       // アナログ入力変換回数終了信号（遅延なし）
#define AIOECU_SRC_AI_STOP_DELAY 18 // アナログ入力変換回数終了信号（遅延あり）
#define AIOECU_SRC_AI_LVSTOP 29     // アナログ入力レベルトリガ停止信号
#define AIOECU_SRC_AI_TRGSTOP 145   // アナログ入力外部トリガ停止信号
#define AIOECU_SRC_AO_CLK 66        // アナログ出力内部クロック信号
#define AIOECU_SRC_AO_EXTCLK 149    // アナログ出力外部クロック信号
#define AIOECU_SRC_AO_TRGSTART 147  // アナログ出力外部トリガ開始信号
#define AIOECU_SRC_AO_STOP_FIFO 352 // アナログ出力指定回数出力終了信号（FIFO使用）
#define AIOECU_SRC_AO_STOP_RING 80  // アナログ出力指定回数出力終了信号（RING使用）
#define AIOECU_SRC_AO_TRGSTOP 148   // アナログ出力外部トリガ停止信号
#define AIOECU_SRC_CNT0_UPCLK 150   // カウンタ０アップクロック信号
#define AIOECU_SRC_CNT1_UPCLK 152   // カウンタ１アップクロック信号
#define AIOECU_SRC_CNT0_CMP 288     // カウンタ０比較カウント一致
#define AIOECU_SRC_CNT1_CMP 289     // カウンタ１比較カウント一致
#define AIOECU_SRC_SLAVE1 136       // 同期バススレーブ信号１
#define AIOECU_SRC_SLAVE2 137       // 同期バススレーブ信号２
#define AIOECU_SRC_SLAVE3 138       // 同期バススレーブ信号３
#define AIOECU_SRC_START 384        // Ai, Ao, Cnt, Tmソフトウェア開始信号
#define AIOECU_SRC_STOP 385         // Ai, Ao, Cnt, Tmソフトウェア停止信号

//----------------------------------------------------------------------------------------------
//	関数プロトタイプ
//----------------------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C"
{
#endif
    // 共通関数
    long WINAPI AioInit(char *DeviceName, short *Id);
    long WINAPI AioExit(short Id);
    long WINAPI AioResetDevice(short Id);
    long WINAPI AioGetErrorString(long ErrorCode, char *ErrorString);
    long WINAPI AioQueryDeviceName(short Index, char *DeviceName, char *Device);
    long WINAPI AioGetDeviceType(char *Device, short *DeviceType);
    long WINAPI AioSetControlFilter(short Id, short Signal, float Value);
    long WINAPI AioGetControlFilter(short Id, short Signal, float *Value);

    // アナログ入力関数
    long WINAPI AioSingleAi(short Id, short AiChannel, long *AiData);
    long WINAPI AioSingleAiEx(short Id, short AiChannel, float *AiData);
    long WINAPI AioMultiAi(short Id, short AiChannels, long *AiData);
    long WINAPI AioMultiAiEx(short Id, short AiChannels, float *AiData);
    long WINAPI AioGetAiResolution(short Id, short *AiResolution);
    long WINAPI AioSetAiInputMethod(short Id, short AiInputMethod);
    long WINAPI AioGetAiInputMethod(short Id, short *AiInputMethod);
    long WINAPI AioGetAiMaxChannels(short Id, short *AiMaxChannels);
    long WINAPI AioSetAiChannels(short Id, short AiChannels);
    long WINAPI AioGetAiChannels(short Id, short *AiChannels);
    long WINAPI AioSetAiChannelSequence(short Id, short AiSequence, short AiChannel);
    long WINAPI AioGetAiChannelSequence(short Id, short AiSequence, short *AiChannel);
    long WINAPI AioSetAiRange(short Id, short AiChannel, short AiRange);
    long WINAPI AioSetAiRangeAll(short Id, short AiRange);
    long WINAPI AioGetAiRange(short Id, short AiChannel, short *AiRange);
    long WINAPI AioSetAiTransferMode(short Id, short AiTransferMode);
    long WINAPI AioGetAiTransferMode(short Id, short *AiTransferMode);
    long WINAPI AioSetAiTransferData(short Id, long DataNumber, long *Buffer);
    long WINAPI AioSetAiAttachedData(short Id, long AttachedData);
    long WINAPI AioGetAiSamplingDataSize(short Id, short *DataSize);
    long WINAPI AioSetAiMemoryType(short Id, short AiMemoryType);
    long WINAPI AioGetAiMemoryType(short Id, short *AiMemoryType);
    long WINAPI AioSetAiRepeatTimes(short Id, long AiRepeatTimes);
    long WINAPI AioGetAiRepeatTimes(short Id, long *AiRepeatTimes);
    long WINAPI AioSetAiClockType(short Id, short AiClockType);
    long WINAPI AioGetAiClockType(short Id, short *AiClockType);
    long WINAPI AioSetAiSamplingClock(short Id, float AiSamplingClock);
    long WINAPI AioGetAiSamplingClock(short Id, float *AiSamplingClock);
    long WINAPI AioSetAiScanClock(short Id, float AiScanClock);
    long WINAPI AioGetAiScanClock(short Id, float *AiScanClock);
    long WINAPI AioSetAiStartTrigger(short Id, short AiStartTrigger);
    long WINAPI AioGetAiStartTrigger(short Id, short *AiStartTrigger);
    long WINAPI AioSetAiStartLevel(short Id, short AiChannel, long AiStartLevel, short AiDirection);
    long WINAPI AioSetAiStartLevelEx(short Id, short AiChannel, float AiStartLevel, short AiDirection);
    long WINAPI AioGetAiStartLevel(short Id, short AiChannel, long *AiStartLevel, short *AiDirection);
    long WINAPI AioGetAiStartLevelEx(short Id, short AiChannel, float *AiStartLevel, short *AiDirection);
    long WINAPI AioSetAiStartInRange(short Id, short AiChannel, long Level1, long Level2, long StateTimes);
    long WINAPI AioGetAiStartInRange(short Id, short AiChannel, long *Level1, long *Level2, long *StateTimes);
    long WINAPI AioSetAiStartOutRange(short Id, short AiChannel, long Level1, long Level2, long StateTimes);
    long WINAPI AioGetAiStartOutRange(short Id, short AiChannel, long *Level1, long *Level2, long *StateTimes);
    long WINAPI AioSetAiStopTrigger(short Id, short AiStopTrigger);
    long WINAPI AioGetAiStopTrigger(short Id, short *AiStopTrigger);
    long WINAPI AioSetAiStopTimes(short Id, long AiStopTimes);
    long WINAPI AioGetAiStopTimes(short Id, long *AiStopTimes);
    long WINAPI AioSetAiStopLevel(short Id, short AiChannel, long AiStopLevel, short AiDirection);
    long WINAPI AioSetAiStopLevelEx(short Id, short AiChannel, float AiStopLevel, short AiDirection);
    long WINAPI AioGetAiStopLevel(short Id, short AiChannel, long *AiStopLevel, short *AiDirection);
    long WINAPI AioGetAiStopLevelEx(short Id, short AiChannel, float *AiStopLevel, short *AiDirection);
    long WINAPI AioSetAiStopInRange(short Id, short AiChannel, long Level1, long Level2, long StateTimes);
    long WINAPI AioGetAiStopInRange(short Id, short AiChannel, long *Level1, long *Level2, long *StateTimes);
    long WINAPI AioSetAiStopOutRange(short Id, short AiChannel, long Level1, long Level2, long StateTimes);
    long WINAPI AioGetAiStopOutRange(short Id, short AiChannel, long *Level1, long *Level2, long *StateTimes);
    long WINAPI AioSetAiStopDelayTimes(short Id, long AiStopDelayTimes);
    long WINAPI AioGetAiStopDelayTimes(short Id, long *AiStopDelayTimes);
    long WINAPI AioSetAiEvent(short Id, HWND hWnd, long AiEvent);
    long WINAPI AioGetAiEvent(short Id, HWND *hWnd, long *AiEvent);
    long WINAPI AioSetAiCallBackProc(short Id,
                                     long(_stdcall *pProc)(short Id, short AiEvent, WPARAM wParam, LPARAM lParam,
                                                           void *Param),
                                     long AiEvent, void *Param);
    long WINAPI AioSetAiEventSamplingTimes(short Id, long AiSamplingTimes);
    long WINAPI AioGetAiEventSamplingTimes(short Id, long *AiSamplingTimes);
    long WINAPI AioSetAiEventTransferTimes(short Id, long AiTransferTimes);
    long WINAPI AioGetAiEventTransferTimes(short Id, long *AiTransferTimes);
    long WINAPI AioStartAi(short Id);
    long WINAPI AioStopAi(short Id);
    long WINAPI AioGetAiStatus(short Id, long *AiStatus);
    long WINAPI AioGetAiSamplingCount(short Id, long *AiSamplingCount);
    long WINAPI AioGetAiStopTriggerCount(short Id, long *AiStopTriggerCount);
    long WINAPI AioGetAiTransferCount(short Id, long *AiTransferCount);
    long WINAPI AioGetAiTransferLap(short Id, long *Lap);
    long WINAPI AioGetAiStopTriggerTransferCount(short Id, long *Count);
    long WINAPI AioGetAiRepeatCount(short Id, long *AiRepeatCount);
    long WINAPI AioGetAiSamplingData(short Id, long *AiSamplingTimes, long *AiData);
    long WINAPI AioGetAiSamplingDataEx(short Id, long *AiSamplingTimes, float *AiData);
    long WINAPI AioResetAiStatus(short Id);
    long WINAPI AioResetAiMemory(short Id);

    // アナログ出力関数
    long WINAPI AioSingleAo(short Id, short AoChannel, long AoData);
    long WINAPI AioSingleAoEx(short Id, short AoChannel, float AoData);
    long WINAPI AioMultiAo(short Id, short AoChannels, long *AoData);
    long WINAPI AioMultiAoEx(short Id, short AoChannels, float *AoData);
    long WINAPI AioGetAoResolution(short Id, short *AoResolution);
    long WINAPI AioSetAoChannels(short Id, short AoChannels);
    long WINAPI AioGetAoChannels(short Id, short *AoChannels);
    long WINAPI AioGetAoMaxChannels(short Id, short *AoMaxChannels);
    long WINAPI AioSetAoRange(short Id, short AoChannel, short AoRange);
    long WINAPI AioSetAoRangeAll(short Id, short AoRange);
    long WINAPI AioGetAoRange(short Id, short AoChannel, short *AoRange);
    long WINAPI AioSetAoTransferMode(short Id, short AoTransferMode);
    long WINAPI AioGetAoTransferMode(short Id, short *AoTransferMode);
    long WINAPI AioSetAoTransferData(short Id, long DataNumber, long *Buffer);
    long WINAPI AioGetAoSamplingDataSize(short Id, short *DataSize);
    long WINAPI AioSetAoMemoryType(short Id, short AoMemoryType);
    long WINAPI AioGetAoMemoryType(short Id, short *AoMemoryType);
    long WINAPI AioSetAoRepeatTimes(short Id, long AoRepeatTimes);
    long WINAPI AioGetAoRepeatTimes(short Id, long *AoRepeatTimes);
    long WINAPI AioSetAoClockType(short Id, short AoClockType);
    long WINAPI AioGetAoClockType(short Id, short *AoClockType);
    long WINAPI AioSetAoSamplingClock(short Id, float AoSamplingClock);
    long WINAPI AioGetAoSamplingClock(short Id, float *AoSamplingClock);
    long WINAPI AioSetAoSamplingData(short Id, long AoSamplingTimes, long *AoData);
    long WINAPI AioSetAoSamplingDataEx(short Id, long AoSamplingTimes, float *AoData);
    long WINAPI AioGetAoSamplingTimes(short Id, long *AoSamplingTimes);
    long WINAPI AioSetAoStartTrigger(short Id, short AoStartTrigger);
    long WINAPI AioGetAoStartTrigger(short Id, short *AoStartTrigger);
    long WINAPI AioSetAoStopTrigger(short Id, short AoStopTrigger);
    long WINAPI AioGetAoStopTrigger(short Id, short *AoStopTrigger);
    long WINAPI AioSetAoEvent(short Id, HWND hWnd, long AoEvent);
    long WINAPI AioGetAoEvent(short Id, HWND *hWnd, long *AoEvent);
    long WINAPI AioSetAoCallBackProc(short Id,
                                     long(_stdcall *pProc)(short Id, short AiEvent, WPARAM wParam, LPARAM lParam,
                                                           void *Param),
                                     long AoEvent, void *Param);
    long WINAPI AioSetAoEventSamplingTimes(short Id, long AoSamplingTimes);
    long WINAPI AioGetAoEventSamplingTimes(short Id, long *AoSamplingTimes);
    long WINAPI AioSetAoEventTransferTimes(short Id, long AoTransferTimes);
    long WINAPI AioGetAoEventTransferTimes(short Id, long *AoTransferTimes);
    long WINAPI AioStartAo(short Id);
    long WINAPI AioStopAo(short Id);
    long WINAPI AioEnableAo(short Id, short AoChannel);
    long WINAPI AioDisableAo(short Id, short AoChannel);
    long WINAPI AioGetAoStatus(short Id, long *AoStatus);
    long WINAPI AioGetAoSamplingCount(short Id, long *AoSamplingCount);
    long WINAPI AioGetAoTransferCount(short Id, long *AoTransferCount);
    long WINAPI AioGetAoTransferLap(short Id, long *Lap);
    long WINAPI AioGetAoRepeatCount(short Id, long *AoRepeatCount);
    long WINAPI AioResetAoStatus(short Id);
    long WINAPI AioResetAoMemory(short Id);

    // デジタル入出力関数
    long WINAPI AioSetDiFilter(short Id, short Bit, float Value);
    long WINAPI AioGetDiFilter(short Id, short Bit, float *Value);
    long WINAPI AioInputDiBit(short Id, short DiBit, short *DiData);
    long WINAPI AioOutputDoBit(short Id, short DoBit, short DoData);
    long WINAPI AioInputDiByte(short Id, short DiPort, short *DiData);
    long WINAPI AioOutputDoByte(short Id, short DoPort, short DoData);

    // カウンタ関数
    long WINAPI AioGetCntMaxChannels(short Id, short *CntMaxChannels);
    long WINAPI AioSetCntComparisonMode(short Id, short CntChannel, short CntMode);
    long WINAPI AioGetCntComparisonMode(short Id, short CntChannel, short *CntMode);
    long WINAPI AioSetCntPresetReg(short Id, short CntChannel, long PresetNumber, long *PresetData, short Flag);
    long WINAPI AioSetCntComparisonReg(short Id, short CntChannel, long ComparisonNumber, long *ComparisonData,
                                       short Flag);
    long WINAPI AioSetCntInputSignal(short Id, short CntChannel, short CntInputSignal);
    long WINAPI AioGetCntInputSignal(short Id, short CntChannel, short *CntInputSignal);
    long WINAPI AioSetCntEvent(short Id, short CntChannel, HWND hWnd, long CntEvent);
    long WINAPI AioGetCntEvent(short Id, short CntChannel, HWND *hWnd, long *CntEvent);
    long WINAPI AioSetCntCallBackProc(short Id, short CntChannel,
                                      long(_stdcall *pProc)(short Id, short CntEvent, WPARAM wParam, LPARAM lParam,
                                                            void *Param),
                                      long CntEvent, void *Param);
    long WINAPI AioSetCntFilter(short Id, short CntChannel, short Signal, float Value);
    long WINAPI AioGetCntFilter(short Id, short CntChannel, short Signal, float *Value);
    long WINAPI AioStartCnt(short Id, short CntChannel);
    long WINAPI AioStopCnt(short Id, short CntChannel);
    long WINAPI AioPresetCnt(short Id, short CntChannel, long PresetData);
    long WINAPI AioGetCntStatus(short Id, short CntChannel, long *CntStatus);
    long WINAPI AioGetCntCount(short Id, short CntChannel, long *Count);
    long WINAPI AioResetCntStatus(short Id, short CntChannel, long CntStatus);

    // タイマ関数
    long WINAPI AioSetTmEvent(short Id, short TimerId, HWND hWnd, long TmEvent);
    long WINAPI AioGetTmEvent(short Id, short TimerId, HWND *hWnd, long *TmEvent);
    long WINAPI AioSetTmCallBackProc(short Id, short TimerId,
                                     long(_stdcall *pProc)(short Id, short TmEvent, WPARAM wParam, LPARAM lParam,
                                                           void *Param),
                                     long TmEvent, void *Param);
    long WINAPI AioStartTmTimer(short Id, short TimerId, float Interval);
    long WINAPI AioStopTmTimer(short Id, short TimerId);
    long WINAPI AioStartTmCount(short Id, short TimerId);
    long WINAPI AioStopTmCount(short Id, short TimerId);
    long WINAPI AioLapTmCount(short Id, short TimerId, long *Lap);
    long WINAPI AioResetTmCount(short Id, short TimerId);
    long WINAPI AioTmWait(short Id, short TimerId, long Wait);

    // イベントコントローラ
    long WINAPI AioSetEcuSignal(short Id, short Destination, short Source);
    long WINAPI AioGetEcuSignal(short Id, short Destination, short *Source);

#ifdef __cplusplus
};
#endif
