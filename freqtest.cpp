//#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <stdio.h>
using namespace std;

int main()
{
    LARGE_INTEGER frequency;		// 周波数
	
    if (QueryPerformanceFrequency(&frequency) == false) {
        cout << "frequency error" << endl;  /* error */
    }

    LARGE_INTEGER startCounter,endCounter;	// カウンタ
	
    if (QueryPerformanceCounter(&startCounter) == false) {	// 開始カウンターを取得
        cout << "qpc error" << endl;  /* error */
    }
	
//	for(int j=0; j<5000; j++)
		for(int i=0; i<1000000; i++)
			;
    if (QueryPerformanceCounter(&endCounter) == false) {	// 終了カウンターを取得
        cout << "endCounter error" << endl;  /* error */
    }
	
	// 処理時間の計算は、
	//   （終了カウンター － 開始カウンター）／周波数 で処理時間（秒）が分かる
    printf("frequency =%lld, \nstartCounter=%lld, \nendCounter=%lld, \ntime((end - start)/freq)=%llf(sec), \nsec =%f\n",
		frequency.QuadPart,
		startCounter.QuadPart,
		endCounter.QuadPart,
		(double)(endCounter.QuadPart - startCounter.QuadPart) / (double)frequency.QuadPart,
		(double)startCounter.QuadPart/(double)frequency.QuadPart);
		
	/* 実行結果例
	この例での処理時間は0.003607秒（約3.6ミリ秒）である。
frequency =10000000,
startCounter=330541394036,
endCounter=330541430105,
time((end - start)/freq)=0.003607(sec),
sec =33054.139404
	*/
	
    getchar(); /* wait user input */
    return 0;
}