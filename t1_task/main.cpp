/*
Вы были у нас на собеседовании.
Предлагаю выполнить следующее тестовое задание на параллелизацию фрагмента кода при помощи OpenMP.

Требуется написать ф-ю void f2(double * pdArr), делающую то же самое что и ф-я f1 но работающая
 в параллельном режиме, естественно чтобы она работала существенно быстрее чем первая на многопроцессорной машине.
Качество и быстрота выполнения задания приветствуются.
 */

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <memory.h>
#include <math.h>
#include <omp.h>
#include <iostream>
#include <vector>
#include <time.h>

static const int iLoopLen = 10000000;
static const int iFactor = 100;
static const int iArrLen = 100000;

void print_time_delta(DWORD tcStart, DWORD tcEnd) {
    const double dTime = (tcEnd-tcStart) * .001;
    printf("%g s\n",dTime);
}


void f1(double* pdArr) {
    for(int i=0; i<iLoopLen;++i) {
        const int index = i%iArrLen;
        pdArr[index]+= fabs(i*.001-20.) * 5. + 3.;
    }
}

void f2(double* pdArr)
{
    #pragma omp parallel num_threads(10)
    {
        int j = 0;
        #pragma omp for private(j)
        for(int i = 0; i < iArrLen; ++i) {
            for(j = 0; j < iLoopLen/iArrLen; ++j) {
                pdArr[i] += fabs((iArrLen*j + i)*.001-20.) * 5. + 3.;
            }
        }
    }
    if (iLoopLen%iArrLen != 0) {
        #pragma omp parallel num_threads(10)
        {
            #pragma omp for
            for(int i = 0; i < iLoopLen%iArrLen; ++i) {
                pdArr[i] += fabs(((iLoopLen/iArrLen)*iArrLen + i)*.001-20.) * 5. + 3.;
            }
        }
    }
}




int _tmain(int argc, _TCHAR* argv[]) {
    double dArr[iArrLen];

    printf("*** Test0, check f2: ");
    {
        double* dArr1 = new double[iArrLen];
        double* dArr2 = new double[iArrLen];

        f1(dArr1);
        f2(dArr2);
        for(int i = 0; i < iArrLen; ++i) {
            if (dArr1[i] != dArr2[i]) {
                std::cout<<"error in f2"<<std::endl;
            }
        }
        std::cout<<"f2 is correct"<<std::endl;
        
        delete[] dArr1;
        delete[] dArr2;
    }

    printf("*** Test1: 1 thread ");
    {
        const DWORD tcStart = GetTickCount();
        for(int i=0; i<iFactor; ++i) {
            f1(dArr);
        }
        const DWORD tcEnd = GetTickCount();
        print_time_delta(tcStart,tcEnd);
    }

    printf("*** Test2: 10 threads (critical) ");
    {
        const DWORD tcStart = GetTickCount();
        for(int i=0; i<iFactor; ++i) {
            f2(dArr);
        }
        const DWORD tcEnd = GetTickCount();
        print_time_delta(tcStart,tcEnd);
    }
}
