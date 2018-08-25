

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */
#ifdef __POCC__
#include <sys/types.h>
#endif // for off_t

#include <stdio.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif /* HAVE_STDLIB_H */

#ifndef NO_STRING_H
#include <string.h>
#else
#include <strings.h>
#endif
#include <math.h>
#ifdef __W32__
#include <windows.h>
#endif /* __W32__ */
#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#endif /* HAVE_PTHREAD_H */
#ifdef HAVE_PTHREADS_H
#include <pthreads.h>
#endif /* HAVE_PTHREADS_H */
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H */

#include "timidity.h"
#include "common.h"
#include "optcode.h"
#include "instrum.h"
#include "playmidi.h"
#include "controls.h"

#include "thread.h"
#ifdef __W32__
#include "w32g.h"
#endif /* __W32__ */



/*
win�ȊO��unix/linux/mac���ł�posix

Win�ł̃X���b�h�\���e�X�g
test1 ����beginthread ���ɒx��
test2 ����ResumeThread/SuspendThread ���X���b�h�Œx�� Sleep���[�v�ɂ��s�v�ȋ��肪��CPU���׍���
test3 �����ƃX���b�h�񂵂��ςȂ� ���x�� ���삪������ �{�c
test4 �Đ����܂킵���ςȂ���莞�Ԍ�X���b�h�I�� CPU�X���b�h���̂Ƃ��x���Ȃ� (�R���g���[���p�X���b�h�ǉ��̕� CPU�X���b�h�������s�X���b�h�������Ȃ�
test5 Event�ŃX���b�h���R���g���[�� ���X���b�h�ő��� Sleep���[�v�ɂ��s�v�ȋ��肪�Ȃ��̂�CPU���׏��Ȃ� �X���b�h�I���C�x���g�ҋ@��WaitForMulti���x���悤�ȁE�E�H
test6 test5�̃X���b�h�I���C�x���g�ҋ@��_mm_pause()���[�v�ɂ������� test5��葬�� ���C���X���b�h�̕��ׂ͑�����
test7 test6+load_balance �ׂ�����������job���󂢂Ă�X���b�h�Ɋ��蓖�� �X���b�h�Ԃ̏����ʂ̍������炷 mutex�g�p test6��葬��
test8 test7��mutex��CRITICAL_SECTION�ɕύX  test7��葬��
test9 test8+reload ��x���s�����X���b�h���蓖�Ă��L�^�� ��莞�ԓ����Ȃ��Ŏ��s test7���炢�ɒx���Ȃ��� �{�c
*/


int compute_thread_num = 0;
int compute_thread_ready = 0;

void go_compute_thread(thread_func_t fnc, int num);
void terminate_compute_thread(void);
void begin_compute_thread(void);
void reset_compute_thread(void);
static void do_compute_null(int thread_num){}
static thread_func_t do_compute_func = do_compute_null;
static int compute_thread_job = 0, compute_thread_job_cnt = 0;

#if MULTI_THREAD_COMPUTE2
static thread_func_t mtcs_func0 = do_compute_null;
static thread_func_t mtcs_func1 = do_compute_null;
static int mtcs_job_num0 = 0, mtcs_job_cnt0 = 0;
static int mtcs_job_num1 = 0, mtcs_job_cnt1 = 0;
static int8 mtcs_job_flg0[MTC2_JOB_MAX] = {0};
static int8 mtcs_job_flg1[MTC2_JOB_MAX] = {0};
#endif

#if defined(MULTI_THREAD_COMPUTE) && defined(__W32__)

static int thread_exit = 0;
static HANDLE hComputeThread[MAX_THREADS - 1];
VOLATILE DWORD ComputeThreadID[MAX_THREADS - 1];
static DWORD ComputeThreadPriority = THREAD_PRIORITY_NORMAL;
static HANDLE hEventTcv[MAX_THREADS - 1];
static ALIGN uint8 thread_finish_all[MAX_THREADS]; // byte*16=64bit*2=128bit
static ALIGN uint8 thread_finish[MAX_THREADS]; // byte*16=64bit*2=128bit
CRITICAL_SECTION critThread;

void set_compute_thread_priority(DWORD var)
{
	ComputeThreadPriority = var;
}

#if (USE_X86_EXT_INTRIN >= 3)
#define THREAD_WAIT_MAIN _mm_pause(); // SSE2
#define THREAD_WAIT_SUB Sleep(0);
#else
#define THREAD_WAIT_MAIN Sleep(0);
#define THREAD_WAIT_SUB Sleep(0);
// ���̃X���b�h�ɏ�����n��(�ŏ�����) ���ꂪ�Ȃ��ƕ��ב���
// OS�f�t�H���g�ł�1~15�̎w�莞 15.6ms 
#endif


static void compute_thread_core(int thread_num)
{	
#if MULTI_THREAD_COMPUTE2
	for(;;){
		int job_num, job_nums0 = 0, job_nums1 = 0;
		EnterCriticalSection(&critThread); // single thread ~
		job_num = (compute_thread_job_cnt++);
		LeaveCriticalSection(&critThread); // ~ single thread
		if(job_num < compute_thread_job)
			do_compute_func(job_num);
		if(mtcs_job_num0){
			EnterCriticalSection(&critThread); // single thread ~
			job_nums0 = (mtcs_job_cnt0++);
			LeaveCriticalSection(&critThread); // ~ single thread
			if(job_nums0 < mtcs_job_num0){
				mtcs_func0(job_nums0);
				mtcs_job_flg0[job_nums0] = 0;
			}
		}
		if(mtcs_job_num1){
			EnterCriticalSection(&critThread); // single thread ~
			job_nums1 = (mtcs_job_cnt1++);
			LeaveCriticalSection(&critThread); // ~ single thread
			if(job_nums1 < mtcs_job_num1){
				mtcs_func1(job_nums1);
				mtcs_job_flg1[job_nums1] = 0;
			}
		}
		if(job_num >= compute_thread_job && job_nums0 >= mtcs_job_num0 && job_nums1 >= mtcs_job_num1)
			break;
	}
#elif 1 // load_balance // �󂢂Ă�X���b�h�ɃW���u���蓖��
	for(;;){
		int job_num;
		EnterCriticalSection(&critThread); // single thread ~
		job_num = (compute_thread_job_cnt++);
		LeaveCriticalSection(&critThread); // ~ single thread
		if(job_num >= compute_thread_job) break;
		do_compute_func(job_num);
	}
#else // normal // �X���b�h�ɋϓ��ɃW���u���蓖��
	int i;
	for (i = thread_num; i < compute_thread_job; i += compute_thread_ready)
		do_compute_func(i);
#endif
}

static void WINAPI ComputeThread(void *arglist)
{
	const int thread_num = (int)arglist;
	
	for(;;){		
		WaitForSingleObject(hEventTcv[thread_num], INFINITE); // �X���b�h�J�n�C�x���g�ҋ@
		if(thread_exit) break;		
		compute_thread_core(thread_num + 1); // 1~15
		ResetEvent(hEventTcv[thread_num]); // �X���b�h�J�n�C�x���g���Z�b�g
		thread_finish[thread_num] = 1; // �X���b�h�I���t���O�Z�b�g
	}
	crt_endthread();
}

#ifdef MULTI_THREAD_COMPUTE2
static inline void compute_thread_sub_wait(int8 *ptr)
{
// (MTC2_JOB_MAX == 16)
	// byte*8��64bit�P�ʂŔ�r 
	uint64 *ptr1 = (uint64 *)ptr;
	while(ptr1[0] || ptr1[1])
		THREAD_WAIT_MAIN
}

void go_compute_thread_sub0(thread_func_t fnc, int num)
{
	int i;
	if(!compute_thread_job)
		return; // error
	if(mtcs_job_num0)
		return; // error
	if(!fnc || num < 1)
		return; // error
	for(i = 0; i < num; i++)
		mtcs_job_flg0[i] = 1;
	mtcs_func0 = fnc;
	mtcs_job_cnt0 = 0;
	mtcs_job_num0 = num; // start flag
	compute_thread_core(0);
	compute_thread_sub_wait(mtcs_job_flg0);
	mtcs_job_num0 = 0; // end flag
	mtcs_func0 = do_compute_null;
}

void go_compute_thread_sub1(thread_func_t fnc, int num)
{
	int i;
	if(!compute_thread_job)
		return; // error
	if(mtcs_job_num1)
		return; // error
	if(!fnc || num < 1)
		return; // error
	for(i = 0; i < num; i++)
		mtcs_job_flg1[i] = 1;
	mtcs_func1 = fnc;
	mtcs_job_cnt1 = 0;
	mtcs_job_num1 = num; // start flag
	compute_thread_core(0);
	compute_thread_sub_wait(mtcs_job_flg1);
	mtcs_job_num1 = 0; // end flag
	mtcs_func1 = do_compute_null;
}
#endif // MULTI_THREAD_COMPUTE2

static inline void compute_thread_wait(void)
{	
// (MAX_THREADS == 16)
#if (USE_X86_EXT_INTRIN >= 6)
	// byte*8��128bit�P�ʂŔ�r
	__m128i vec = _mm_load_si128((__m128i *)thread_finish_all);
	while(!_mm_testc_si128(_mm_load_si128((__m128i *)thread_finish), vec)) // SSE4.1 !(finish == finish_all)
		THREAD_WAIT_MAIN
#else
	// byte*8��64bit�P�ʂŔ�r 
	uint64 *ptr1 = (uint64 *)&thread_finish, *ptr2 = (uint64 *)&thread_finish_all;
	while(ptr1[0] != ptr2[0] || ptr1[1] != ptr2[1])
		THREAD_WAIT_MAIN
#endif
}

void go_compute_thread(thread_func_t fnc, int num)
{
	const int thread = compute_thread_ready - 1;
	int i;

	do_compute_func = fnc;
	compute_thread_job = num;
	compute_thread_job_cnt = 0;
#ifdef MULTI_THREAD_COMPUTE2
	mtcs_func0 = do_compute_null;
	mtcs_job_num0 = 0;
	mtcs_job_cnt0 = 0;
	mtcs_func1 = do_compute_null;
	mtcs_job_num1 = 0;
	mtcs_job_cnt1 = 0;
	memset(mtcs_job_flg0, 0, sizeof(mtcs_job_flg0));
	memset(mtcs_job_flg1, 0, sizeof(mtcs_job_flg1));
#endif // MULTI_THREAD_COMPUTE2
#if (USE_X86_EXT_INTRIN >= 3) && (MAX_THREADS == 16)
	_mm_store_si128((__m128i *)thread_finish, _mm_setzero_si128());  // �X���b�h�I���t���O���Z�b�g
	for(i = 0; i < thread; i++)
		SetEvent(hEventTcv[i]); // �X���b�h�J�n�C�x���g�Z�b�g (�ĊJ)
#else
	for(i = 0; i < thread; i++){
		thread_finish[i] = 0;  // �X���b�h�I���t���O���Z�b�g
		SetEvent(hEventTcv[i]); // �X���b�h�J�n�C�x���g�Z�b�g (�ĊJ)
	}
#endif
	compute_thread_core(0);
	compute_thread_wait(); // �S�X���b�h�I���ҋ@
}

static void init_compute_thread_param(void)
{
	static int thread_init = 0;
	int i;

	if(thread_init)
		return;
	for(i = 0; i < (MAX_THREADS - 1); i++){
		hComputeThread[i] = NULL;
	}	
	memset(thread_finish, 0, MAX_THREADS);
	memset(thread_finish_all, 0, MAX_THREADS);	
	InitializeCriticalSection(&critThread);	
	compute_thread_ready = 0;
	thread_init = 1; // set init flag
}

static int check_compute_thread(void)
{	
	int i, flg = 0;

	for(i = 0; i < (MAX_THREADS - 1); i++){
		if(hComputeThread[i] != NULL)
			flg++;
	}
	if(flg){
		terminate_compute_thread();
		compute_thread_ready = 0;
		ctl->cmsg(CMSG_ERROR, VERB_NORMAL, "ERROR ComputeThread : already thread exsist.");
		return 1;	
	}
	return 0;	
}

void terminate_compute_thread(void)
{
	int i;
	DWORD status;
	
	thread_exit = 1;
	compute_thread_job = 0; // �f�b�h���b�N�΍�
#ifdef MULTI_THREAD_COMPUTE2
	mtcs_job_num0 = 0; // �f�b�h���b�N�΍�
	memset(mtcs_job_flg0, 0, sizeof(mtcs_job_flg0));
	mtcs_job_num1 = 0; // �f�b�h���b�N�΍�
	memset(mtcs_job_flg1, 0, sizeof(mtcs_job_flg1));
#endif // MULTI_THREAD_COMPUTE2
	for(i = 0; i < (MAX_THREADS - 1); i++){
		if(hComputeThread[i] == NULL)
			continue;		
		SetEvent(hEventTcv[i]);
		switch(WaitForSingleObject(hComputeThread[i], 10)) {
		case WAIT_OBJECT_0:
			break;
		case WAIT_TIMEOUT:
			status = WaitForSingleObject(hComputeThread[i], 1000);
			if(status == WAIT_TIMEOUT)
				TerminateThread(hComputeThread[i], 0);
			break;
		default:
			TerminateThread(hComputeThread[i], 0);
			break;
		}
		CloseHandle(hComputeThread[i]);
		hComputeThread[i] = NULL;
	}	
	for(i = 0; i < (MAX_THREADS - 1); i++){
		if(hEventTcv[i] != NULL){
			CloseHandle(hEventTcv[i]);
			hEventTcv[i] = NULL;
		}
	}
	memset(thread_finish, 0, MAX_THREADS);
	memset(thread_finish_all, 0, MAX_THREADS);	
	DeleteCriticalSection(&critThread);	
	compute_thread_ready = 0;
	thread_exit = 0;
	uninit_compute_data_midi_thread();
}

// load ini, load config , init_playmidi()... �̌�ɂ���
void begin_compute_thread(void)
{
	int i, pnum, error = 0;			
	SYSTEM_INFO sys_info;

	GetSystemInfo(&sys_info);
	pnum = sys_info.dwNumberOfProcessors;
	ctl->cmsg(CMSG_INFO, VERB_NORMAL, "NumberOfProcessors : %d", pnum);

	init_compute_thread_param(); // init thread param	
	if(check_compute_thread()) // check thread exist
		return;	
	if(compute_thread_num < 2){ // check thread num
		compute_thread_num = 0;
		ctl->cmsg(CMSG_INFO, VERB_NORMAL, "SetComputeThread : OFF");
	}else{
		if(compute_thread_num > MAX_THREADS){ // check thread num
			compute_thread_num = MAX_THREADS;
			ctl->cmsg(CMSG_INFO, VERB_NORMAL, "ERROR ComputeThread : Limit MAX_THREADS:%d", MAX_THREADS);
		}
		ctl->cmsg(CMSG_INFO, VERB_NORMAL, "SetComputeThread : %d", compute_thread_num);
	}
	if(compute_thread_num == 0) // check multi thread
		return;		
	// beginthread after CreateEvent
	InitializeCriticalSection(&critThread);	
	for(i = 0; i < (compute_thread_num - 1); i++){	
		hEventTcv[i] = CreateEvent(NULL,FALSE,FALSE,NULL); // reset manual
		thread_finish_all[i] = 1; // 1byte full bit
		if(hEventTcv[i] == NULL){
			++error;
			ctl->cmsg(CMSG_INFO, VERB_NORMAL, "ERROR ComputeThread: CreateEvent(%d) error.", i);
			break;
		}
		hComputeThread[i] = crt_beginthreadex(
				NULL, 
				0,
				(LPTHREAD_START_ROUTINE)ComputeThread, 
				(LPVOID)i, /* void *arglist = thread_num */
				0, /* initflag = NULL or CREATE_SUSPENDED */
				&(ComputeThreadID[i])
				);
		if(hComputeThread[i] == NULL){
			++error;
			ctl->cmsg(CMSG_INFO, VERB_NORMAL, "ERROR ComputeThread : beginthread(%d) error.", i);
			break;
		}
	}	
	if(error){
		terminate_compute_thread();
		compute_thread_ready = 0;
		ctl->cmsg(CMSG_INFO, VERB_NORMAL, "SetComputeThread : OFF");
		return;
	}
	compute_thread_ready = compute_thread_num;
	switch(ComputeThreadPriority){
	case THREAD_PRIORITY_LOWEST:
	case THREAD_PRIORITY_BELOW_NORMAL:
	case THREAD_PRIORITY_NORMAL:
	case THREAD_PRIORITY_ABOVE_NORMAL:
	case THREAD_PRIORITY_HIGHEST:
	case THREAD_PRIORITY_TIME_CRITICAL:
		for(i = 0; i < (compute_thread_ready - 1); i++)
			if(!SetThreadPriority(hComputeThread[i], ComputeThreadPriority)){
				ctl->cmsg(CMSG_INFO, VERB_NORMAL, "ERROR ComputeThread : Invalid priority");
			}
		break;
	default:
		ctl->cmsg(CMSG_INFO, VERB_NORMAL, "ERROR ComputeThread : Invalid priority");
		break;
	}	
	init_compute_data_midi_thread();
    return;
}

void reset_compute_thread(void)
{
	if(compute_thread_num == compute_thread_ready)
		return;
	terminate_compute_thread();
	begin_compute_thread();
}


#elif 0 // defined(MULTI_THREAD_COMPUTE) && (defined(HAVE_PTHREAD_H) || defined(HAVE_PTHREADS_H)) && defined(HAVE_PTHREAD_CREATE)
/*
�ꉞ�����Ă͂��邪���ɒx���E�ECPU�g�p�����S�R�オ��Ȃ��E�E
cond_wait/cond_signal���x���̂ł́E�E�H
Linux�ł͍����ȃX���b�h�@�\�͂Ȃ��̂��H ���ɉ�������̂��H

�g�p����ꍇ�͊e�t�@���N�V�����̎d�l���m�F������œ���e�X�g����
�Q�l�ɂ����̂� PTHREADS-WIN32 RELEASE 2.8.0 (2006-12-22) �� POSIX���̂��̂ł͂Ȃ��̂ŁE�E
Win��p������PTHREAD�t�@���N�V�����ɒu������������

CRITICAL_SECTION��pthread_mutex_ , EVENT��pthread_cond_���ő��

pthread_create()		�ȑO����g�p���Ă�̂ł��Ԃ�OK
pthread_join()			�ȑO����g�p���Ă�̂ł��Ԃ�OK
pthread_????			�X���b�h������/�Ȃ��̏�Ԏ擾 �s�� (�Ȃ��Ă�����
pthread_mutex_init()	���������₵�� ���L�t���O�H�H
pthread_mutex_destroy()	CloseHandle()�Ɠ��� 
pthread_mutex_lock()	WaitForSingleObject()�Ɠ���
pthread_mutex_unlock()	ReleaseMutex()�Ɠ���
pthread_cond_init()		���������₵�� ���L�t���O�H�H
pthread_cond_destroy()	CloseHandle()�Ɠ��� 
pthread_cond_wait()		WaitForSingleObject()�Ɠ����H�H CPU���ԏ����̂��ǂ����H mutex_lock���K�v�炵��
pthread_cond_signal()	SetEvent()�Ɠ����H�H
pthread_????			ResetEvent()�����̂��̖����H �V�O�i���͎������Z�b�g�H 
*/

static int thread_exit = 0;
static pthread_t handle_thread[MAX_THREADS - 1];
pthread_cond_t cond_thread[MAX_THREADS - 1];
static ALIGN int8 thread_finish_all[MAX_THREADS]; // byte*16=64bit*2=128bit
static ALIGN int8 thread_finish[MAX_THREADS]; // byte*16=64bit*2=128bit
pthread_mutex_t	mutex_job;
pthread_mutex_t	mutex_thread[MAX_THREADS];

#if (USE_X86_EXT_INTRIN >= 3)
#define THREAD_WAIT_MAIN _mm_pause(); // SSE2
#else
#define THREAD_WAIT_MAIN usleep(0);
#endif

static void compute_thread_core(int thread_num)
{
	if(compute_thread_job <= compute_thread_ready){
		if(thread_num < compute_thread_job)
			do_compute_func(thread_num);
	}else{	
#if 1 // load_balancer	// �󂢂Ă�X���b�h�ɃW���u���蓖��
		for(;;){
			int job_num;
			pthread_mutex_lock(&mutex_job); // �~���[�e�b�N�X�擾
			job_num = (compute_thread_job_cnt++);
			pthread_mutex_unlock(&mutex_job); // �~���[�e�b�N�X���			
			if(job_num >= compute_thread_job) break;
			do_compute_func(job_num);
		}
#else // normal // �X���b�h�ɋϓ��ɃW���u���蓖��
		int i;
		for (i = thread_num; i < compute_thread_job; i += compute_thread_ready)
			do_compute_func(i);
#endif
	}
}

static void *ComputeThread(void *arglist)
{
	const int thread_num = (int)arglist;
	
	for(;;){
	//	pthread_mutex_lock(&mutex_thread[thread_num]); // �~���[�e�b�N�X�擾
		pthread_cond_wait(&cond_thread[thread_num], &mutex_thread[thread_num]); // �X���b�h�J�n�V�O�i���ҋ@ . �V�O�i����ҋ@��Ԃɖ߂�H
	//	pthread_mutex_unlock(&mutex_thread[thread_num]);
		if(thread_exit) break;		
		compute_thread_core(thread_num + 1); // 1~15
		// ���b�N�p�̃V�O�i���Ȃ��H cond_wait�͎������Z�b�g�H
		thread_finish[thread_num] = 1;
	}
	return 0;
}

static inline void compute_thread_wait(void)
{	
// (MAX_THREADS == 16)
#if (USE_X86_EXT_INTRIN >= 6)
	 // byte*8��128bit�P�ʂŔ�r
	__m128i vec = _mm_load_si128((__m128i *)thread_finish_all);
	while(!_mm_testc_si128(_mm_load_si128((__m128i *)thread_finish), vec)) // SSE4.1 !(finish == finish_all)
		THREAD_WAIT_MAIN
#else // byte*8��64bit�P�ʂŔ�r
	uint64 *ptr1 = (uint64 *)&thread_finish, *ptr2 = (uint64 *)&thread_finish_all;
	while(ptr1[0] != ptr2[0] || ptr1[1] != ptr2[1])
		THREAD_WAIT_MAIN
#endif
}

void go_compute_thread(thread_func_t fnc, int num)
{
	const int thread = compute_thread_ready - 1;
	int i;
		
	do_compute_func = fnc;
	compute_thread_job = num;
	compute_thread_job_cnt = 0;
#if (USE_X86_EXT_INTRIN >= 3) && (MAX_THREADS == 16)
	_mm_store_si128((__m128i *)thread_finish, _mm_setzero_si128());  // �X���b�h�I���t���O���Z�b�g
	for(i = 0; i < thread; i++){		
		pthread_mutex_lock(&mutex_thread[i]); // �~���[�e�b�N�X�擾
		pthread_cond_signal(&cond_thread[i]); // �X���b�h�J�n�V�O�i���Z�b�g (�ĊJ)
		pthread_mutex_unlock(&mutex_thread[i]); // �~���[�e�b�N�X���
	}
#else
	for (i = 0; i < thread; i++) {
		thread_finish[i] = 0;  // �X���b�h�I���t���O���Z�b�g		
		pthread_mutex_lock(&mutex_thread[i]); // �~���[�e�b�N�X�擾
		pthread_cond_signal(&cond_thread[i]); // �X���b�h�J�n�V�O�i���Z�b�g (�ĊJ)
		pthread_mutex_unlock(&mutex_thread[i]); // �~���[�e�b�N�X���
	}
#endif
	compute_thread_core(0); // thread 0
	compute_thread_wait(); // �S�X���b�h�I���ҋ@
}

static void init_compute_thread_param(void)
{
	static int thread_init = 0;
	int i;

	if (thread_init)
		return;
	//for (i = 0; i < (MAX_THREADS - 1); i++){
	//	// �n���h�����Ȃɂ��̏�����
	//}
	memset(thread_finish, 0, MAX_THREADS);
	memset(thread_finish_all, 0, MAX_THREADS);	
	compute_thread_ready = 0;
	thread_init = 1; // set init flag
}

static int check_compute_thread(void)
{
	int i, flg = 0;
	
	// �����ŃX���b�h���c���Ă邱�Ƃ͂Ȃ��͂�
	//for (i = 0; i < (MAX_THREADS - 1); i++) {
	//	if () // �X���b�h������̏��� �n���h�����Ȃɂ��̃`�F�b�N�H
	//		flg++;
	//}
	//if(flg){
	//	terminate_compute_thread();
	//	compute_thread_ready = 0;
	//	ctl->cmsg(CMSG_ERROR, VERB_NORMAL, "ERROR ComputeThread : already thread exist.");
	//	return 1;	
	//}
	return 0;
}

void terminate_compute_thread(void)
{
	int i;
		
	thread_exit = 1;
	compute_thread_job = 0; // �f�b�h���b�N�΍�
	for(i = 0; i < (MAX_THREADS - 1); i++){
		//if() // �X���b�h���Ȃ��̏���
		//	continue;	
		pthread_mutex_lock(&mutex_thread[i]);
		pthread_cond_signal(&cond_thread[i]); // �X���b�h�J�n�V�O�i���Z�b�g (�ĊJ)
		pthread_mutex_unlock(&mutex_thread[i]);
		pthread_join(handle_thread[i], NULL);
		handle_thread[i] = NULL;
	}	
	for(i = 0; i < (MAX_THREADS - 1); i++){
		pthread_mutex_destroy(&mutex_thread[i]);
		pthread_cond_destroy(&cond_thread[i]);
	}	
	pthread_mutex_destroy(&mutex_job);
	memset(thread_finish, 0, MAX_THREADS);
	memset(thread_finish_all, 0, MAX_THREADS);	
	compute_thread_ready = 0;
	thread_exit = 0;
	uninit_compute_data_midi_thread();
}

// load ini, load config , init_playmidi()... �̌�ɂ���
void begin_compute_thread(void)
{
	int i, error = 0;

	init_compute_thread_param(); // init thread param
	if (check_compute_thread()) // check thread exist
		return;
	if (compute_thread_num > MAX_THREADS) // check thread num
		compute_thread_num = MAX_THREADS;
	else if (compute_thread_num < 2) // check thread num
		compute_thread_num = 0;
	if (compute_thread_num == 0) // check multi thread
		return;
	if(pthread_mutex_init(&mutex_job, NULL)){
		ctl->cmsg(CMSG_ERROR, VERB_NORMAL, "ERROR ComputeThread : pthread_mutex_init mutex_job error.");
		error++;
	}else for(i = 0; i < (compute_thread_num - 1); i++){	
		if(pthread_mutex_init(&mutex_thread[i], NULL)){
			ctl->cmsg(CMSG_ERROR, VERB_NORMAL, "ERROR ComputeThread : pthread_mutex_init %d error.", i);
			error++;
			break;
		}
		if(pthread_cond_init(&cond_thread[i], NULL)){ // attr�͋��L�ݒ�H 
			ctl->cmsg(CMSG_ERROR, VERB_NORMAL, "ERROR ComputeThread : pthread_cond_init %d error.", i);
			error++;
			break;
		}
		thread_finish_all[i] = 1;
		if(pthread_create(&handle_thread[i], NULL, (void*)ComputeThread, (void*)i)){
			ctl->cmsg(CMSG_ERROR, VERB_NORMAL, "ERROR ComputeThread : pthread_create %d error.", i);
			error++;
			break;
		}
	}		
	if(error){
		terminate_compute_thread();
		compute_thread_ready = 0;
		ctl->cmsg(CMSG_INFO, VERB_NORMAL, "SetComputeThread : OFF");
		return;
	}
	compute_thread_ready = compute_thread_num;
	init_compute_data_midi_thread();
    return;
}

void reset_compute_thread(void)
{
	if (compute_thread_num == compute_thread_ready)
		return;
	terminate_compute_thread();
	begin_compute_thread();
}

#elif defined(MULTI_THREAD_COMPUTE) && (defined(HAVE_PTHREAD_H) || defined(HAVE_PTHREADS_H)) && defined(HAVE_PTHREAD_CREATE)
/*
�ꉞ�����Ă͂��邪�E�E�x���悤�ȁE�E�H

����pthread_create/pthread_join
load_balancer (mutex)  // CRITICAL_SECTION���Ȃ��H�̂�mutex�g�p
*/

static pthread_t handle_thread[MAX_THREADS - 1];
pthread_mutex_t	mutex_job;

static void compute_thread_core(int thread_num)
{	
	if(compute_thread_job <= compute_thread_ready){
		if(thread_num < compute_thread_job)
			do_compute_func(thread_num);
	}else{	
#if 1 // load_balancer	// �󂢂Ă�X���b�h�ɃW���u���蓖��
		for(;;){
			int job_num;
			pthread_mutex_lock(&mutex_job); // �~���[�e�b�N�X�擾
			job_num = (compute_thread_job_cnt++);
			pthread_mutex_unlock(&mutex_job); // �~���[�e�b�N�X���			
			if(job_num >= compute_thread_job) break;
			do_compute_func(job_num);
		}
#else // normal // �X���b�h�ɋϓ��ɃW���u���蓖��
		int i;
		for (i = thread_num; i < compute_thread_job; i += compute_thread_ready)
			do_compute_func(i);
#endif
	}
}

static void *ComputeThread(void *arglist)
{
	compute_thread_core((int)arglist + 1); // 1~15
	return 0;
}

void go_compute_thread(thread_func_t fnc, int num)
{
	int i;
		
	do_compute_func = fnc;
	compute_thread_job = num;
	compute_thread_job_cnt = 0;
	for (i = 0; i < (compute_thread_ready - 1); i++)
		pthread_create(&handle_thread[i], NULL, (void*)ComputeThread, (void*)i);
	compute_thread_core(0);
	for (i = 0; i < (compute_thread_ready - 1); i++)
		pthread_join(handle_thread[i], NULL);
}

static void init_compute_thread_param(void)
{
	static int thread_init = 0;
	int i;

	if (thread_init)
		return;
	compute_thread_ready = 0;
	thread_init = 1; // set init flag
}

void terminate_compute_thread(void)
{
	pthread_mutex_destroy(&mutex_job);
	compute_thread_ready = 0;
	uninit_compute_data_midi_thread();
}

// load ini, load config , init_playmidi()... �̌�ɂ���
void begin_compute_thread(void)
{
	int i, error = 0;

	init_compute_thread_param(); // init thread param
	if (compute_thread_num > MAX_THREADS) // check thread num
		compute_thread_num = MAX_THREADS;
	else if (compute_thread_num < 2) // check thread num
		compute_thread_num = 0;
	if (compute_thread_num == 0) // check multi thread
		return;	
	if(pthread_mutex_init(&mutex_job, NULL)){
		ctl->cmsg(CMSG_ERROR, VERB_NORMAL, "ERROR ComputeThread : pthread_mutex_init thread_core error.");
		error++;
	}		
	if(error){
		terminate_compute_thread();
		compute_thread_ready = 0;
		ctl->cmsg(CMSG_INFO, VERB_NORMAL, "SetComputeThread : OFF");
		return;
	}
	compute_thread_ready = compute_thread_num;
	init_compute_data_midi_thread();
    return;
}

void reset_compute_thread(void)
{
	if (compute_thread_num == compute_thread_ready)
		return;
	terminate_compute_thread();
	begin_compute_thread();
}
#endif /* MULTI_THREAD_COMPUTE */