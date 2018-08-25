/* config.h.in.  Generated automatically from configure.in by autoheader.  */

/* Define if type char is unsigned and you are not using gcc.  */
// #ifndef __CHAR_UNSIGNED__
// #undef __CHAR_UNSIGNED__
// #endif

#pragma inline_recursion(on) // �C�����C���W�J�L��
#pragma inline_depth(256)

#pragma warning(disable:4996) // ���S�ł͂Ȃ��֐��̎g�p
#pragma warning(disable:4101) // ��x���g�p���Ă��Ȃ��ϐ�
#pragma warning(disable:4018) // signed unsigned �̔�r
#pragma warning(disable:4013) // int�^�̊O���֐��Ƃ݂Ȃ�

//#pragma warning(disable:4133) // �݊������Ȃ��Ăяo��

#pragma warning(disable:4554) // �V�t�g���Z�q�̗D�揇��
#pragma warning(disable:4005) // �Ē�`

#ifndef _DEBUG
#pragma warning(disable:4244) // �^�ϊ�
#pragma warning(disable:4068) // �s���ȃv���O�}
#endif

/* Define to empty if the keyword does not work.  */
// #undef const

/* Define if you don't have vprintf but do have _doprnt.  */
#undef HAVE_DOPRNT

/* Define if you have a working `mmap' system call.  */
#undef HAVE_MMAP

/* Define if you have <sys/wait.h> that is POSIX.1 compatible.  */
#undef HAVE_SYS_WAIT_H

/* Define if you have the vprintf function.  */
// #undef HAVE_VPRINTF
#define HAVE_VPRINTF

/* Define as __inline if that's what the C compiler calls it.  */
// #undef inline
#define inline __inline

/* Define to `long' if <sys/types.h> doesn't define.  */
// #undef off_t

/* Define to `int' if <sys/types.h> doesn't define.  */
// #undef pid_t

/* Define as the return type of signal handlers (int or void).  */
// #undef RETSIGTYPE
#define RETSIGTYPE int

/* Define if the setvbuf function takes the buffering type as its second
   argument and the buffer pointer as the third, as on System V
   before release 3.  */
// #undef SETVBUF_REVERSED

/* Define to `unsigned' if <sys/types.h> doesn't define.  */
// #undef size_t

/* Define if you have the ANSI C header files.  */
#undef STDC_HEADERS

/* Define if you can safely include both <sys/time.h> and <time.h>.  */
// #undef TIME_WITH_SYS_TIME

/* Define if your processor stores words with the most significant
   byte first (like Motorola and SPARC, unlike Intel and VAX).  */
#undef WORDS_BIGENDIAN

/* Define if the X Window System is missing or not being used.  */
#undef X_DISPLAY_MISSING

/* Get 64-bit file size support */
/* #undef _FILE_OFFSET_BITS */

/* Define for ftello and fseeko extension. */
/* #undef _LARGEFILE_SOURCE */

/* Define if you have the XShmCreatePixmap function.  */
#undef HAVE_XSHMCREATEPIXMAP

/* Define if you have the XmuRegisterExternalAgent function.  */
#undef HAVE_XMUREGISTEREXTERNALAGENT

/* Define if you have the getcwd function.  */
// #undef HAVE_GETCWD
#define HAVE_GETCWD

/* Define if you have the gethostbyname function.  */
// #undef HAVE_GETHOSTBYNAME
#define HAVE_GETHOSTBYNAME

/* Define if you have the getopt function.  */
#undef HAVE_GETOPT

/* Define if you have the getpagesize function.  */
#undef HAVE_GETPAGESIZE

/* Define if you have the gettimeofday function.  */
#undef HAVE_GETTIMEOFDAY

/* Define if you have the isatty function.  */
// #undef HAVE_ISATTY
#define HAVE_ISATTY

/* Define if you have the popen function.  */
// #undef HAVE_POPEN
#ifdef _MSC_VER
#define HAVE_POPEN
#define popen _popen
#define pclose _pclose
#endif

/* Define if you have the select function.  */
// #undef HAVE_SELECT
#define HAVE_SELECT

/* Define if you have the signal function.  */
// #undef HAVE_SIGNAL
#define HAVE_SIGNAL

/* Define if you have the sleep function.  */
#undef HAVE_SLEEP

/* Define if you have the snprintf function.  */
// #undef HAVE_SNPRINTF
#ifdef _MSC_VER
#define HAVE_SNPRINTF
#define snprintf _snprintf
#endif

/* Define if you have the socket function.  */
// #undef HAVE_SOCKET
#define HAVE_SOCKET

/* Define if you have the strdup function.  */
// #undef HAVE_STRDUP
#define HAVE_STRDUP

/* Define if you have the strerror function.  */
// #undef HAVE_STRERROR
#define HAVE_STRERROR

/* Define if you have the strncasecmp function.  */
// #undef HAVE_STRNCASECMP
#define HAVE_STRNCASECMP

/* Define if you have the strstr function.  */
// #undef HAVE_STRSTR
#define HAVE_STRSTR

/* Define if you have the usleep function.  */
#undef HAVE_USLEEP

/* Define if you have the vsnprintf function.  */
// #undef HAVE_VSNPRINTF
#ifdef _MSC_VER
#define HAVE_VSNPRINTF
//#define vsnprintf _vsnprintf
#endif

/* Define if you have the <X11/Xmu/ExtAgent.h> header file.  */
#undef HAVE_X11_XMU_EXTAGENT_H

/* Define if you have the <X11/extensions/XShm.h> header file.  */
#undef HAVE_X11_EXTENSIONS_XSHM_H

/* Define to 1 if you enable Sherry WRD */
#define ENABLE_SHERRY 1

/* Define to 1 if you need thread support */
#define ENABLE_THREAD 1

/* Define if you have the <curses.h> header file.  */
#undef HAVE_CURSES_H

/* Define if you have the <dirent.h> header file.  */
// #undef HAVE_DIRENT_H
#ifdef _MSC_VER
#undef HAVE_DIRENT_H
#else
#define HAVE_DIRENT_H
#endif

/* Define if you have the <dlfcn.h> header file.  */
#undef HAVE_DLFCN_H

/* Define if you have the <errno.h> header file.  */
// #undef HAVE_ERRNO_H
#define HAVE_ERRNO_H

/* Define if you have the <fcntl.h> header file.  */
// #undef HAVE_FCNTL_H
#define HAVE_FCNTL_H

/* Define if you have the <getopt.h> header file.  */
#undef HAVE_GETOPT_H

/* Define if you have the <glob.h> header file.  */
#undef HAVE_GLOB_H

/* Define if you have the <limits.h> header file.  */
// #undef HAVE_LIMITS_H
#define HAVE_LIMITS_H

/* Define if you have the <machine/endian.h> header file.  */
#undef HAVE_MACHINE_ENDIAN_H

/* Define if you have the <malloc.h> header file.  */
#define HAVE_MALLOC_H

/* Define if you have the <ncurses.h> header file.  */
#undef HAVE_NCURSES_H

/* Define if you have the <ncurses/curses.h> header file.  */
#undef HAVE_NCURSES_CURSES_H

/* Define if you have the <ndir.h> header file.  */
#undef HAVE_NDIR_H

/* Define if you have the <slang.h> header file.  */
#undef HAVE_SLANG_H

/* Define if you have the <slang/slang.h> header file.  */
#undef HAVE_SLANG_SLANG_H

/* Define if you have the <strings.h> header file.  */
#undef HAVE_STRINGS_H

/* Define if you have the <stropts.h> header file.  */
#undef HAVE_STROPTS_H

/* Define if you have the <sun/audioio.h> header file.  */
#undef HAVE_SUN_AUDIOIO_H

/* Define if you have the <sys/audioio.h> header file.  */
#undef HAVE_SYS_AUDIOIO_H

/* Define if you have the <sys/dir.h> header file.  */
#undef HAVE_SYS_DIR_H

/* Define if you have the <sys/ioctl.h> header file.  */
#undef HAVE_SYS_IOCTL_H

/* Define if you have the <sys/ipc.h> header file.  */
#undef HAVE_SYS_IPC_H

/* Define if you have the <sys/ndir.h> header file.  */
#undef HAVE_SYS_NDIR_H

/* Define if you have the <sys/param.h> header file.  */
#undef HAVE_SYS_PARAM_H

/* Define if you have the <sys/shm.h> header file.  */
#undef HAVE_SYS_SHM_H

/* Define if you have the <sys/soundcard.h> header file.  */
#undef HAVE_SYS_SOUNDCARD_H

/* Define if you have the <sys/time.h> header file.  */
#undef HAVE_SYS_TIME_H

/* Define if you have the <sys/types.h> header file.  */
// #undef HAVE_SYS_TYPES_H
#define HAVE_SYS_TYPES_H

/* Define if you have the <unistd.h> header file.  */
#undef HAVE_UNISTD_H

/* Define if you have the ICE library (-lICE).  */
#undef HAVE_LIBICE

/* Define if you have the X11 library (-lX11).  */
#undef HAVE_LIBX11

/* Define if you have the asound library (-lasound).  */
#undef HAVE_LIBASOUND

/* Define if you have the m library (-lm).  */
#define HAVE_LIBM

/* Define if you have the socket library (-lsocket).  */
#define HAVE_LIBSOCKET

/* Define if you have the vorbis library (-lvorbis).  */
#define HAVE_LIBVORBIS

/* Define if you have the vorbis library (-lgogo).  */
#define HAVE_LIBGOGO

/* Define if you have the <mmsystem.h> header file for Win32. */
#define HAVE_MMSYSTEM_H

/* Define to 1 if you need spectrogram vewer. */
#define SUPPORT_SOUNDSPEC 1

/* place to install modules */
#define PKGLIBDIR "."

/* In VDS Macro AAA=BBB is not available. */
#define __W32__
#define TIMID_VERSION	"current"
#define DEFAULT_PATH	".\\"
#define AU_W32 1
#define AU_WASAPI 1
#define AU_WDMKS 1
#define AU_VORBIS 1
#define AU_VORBIS_DLL 1
#define VORBIS_DLL_INCLUDE_VORBISENC 1 /* ver 1.3.3 */
//#define VORBIS_DLL_INCLUDE_VORBISFILE 1 /* ver 1.3.3 */
#define AU_GOGO 1
#define AU_GOGO_DLL 1
#define AU_LAME 1
#define AU_FLAC 1
#define AU_FLAC_DLL 1
#define AU_OGGFLAC 1
//#define AU_OGGFLAC_DLL
#define AU_OPUS 1
#define AU_OPUS_DLL 1
#define AU_SPEEX 1
//#define AU_AO 1
//#define AU_ASIO 1
#define AU_PORTAUDIO 1
#define AU_PORTAUDIO_DLL 1
#define PORTAUDIO_V19 1
#define PORTAUDIO_V19_6 1  /* ver 19.6 */
#define VST_LOADER_ENABLE 1
#define VSTWRAP_EXT 1
#define AU_NPIPE 1
#define AU_WRITE_MIDI 1
#define AU_LIST 1
#define AU_MODMIDI 1
#define AU_VOLUME_CALC 1
#define AU_BENCHMARK 1
#define FLAC__NO_DLL 1
#define SMFCONV 1
//#undef AU_WRITE_MIDI
//#undef AU_LIST
//#undef AU_MODMIDI
//#undef AU_VOLUME_CALC

#define WINSOCK 1 
#define __W32READDIR__ 1
#define URL_DIR_CACHE_ENABLE
#define ANOTHER_MAIN 1
#define __W32G__	/* for Win32 GUI */
#define SUPPORT_SOCKET

#undef WINVER
#define WINVER 0x0410

#ifndef _WIN32_WINNT
#define _WIN32_WINNT WINVER
#endif

/* Windows 2000 or later */
#if WINVER >= 0x0500
#define HAVE_GETADDRINFO 1
#endif

#ifndef NDEBUG
#define TIMIDITY_LEAK_CHECK 1
#endif

#ifndef KBTIM
#include <winsock2.h>
#endif
//#include <Ws2tcpip.h>
//#include <Wspiapi.h>
#include <windows.h>
#include <string.h>
#include <time.h>
#include <io.h>
#include <direct.h>

#ifdef TIMIDITY_LEAK_CHECK
#define _CRTDBG_MAP_ALLOC 1
#include <crtdbg.h>
#ifdef __cplusplus
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif
#define CRTDEBUGFLAGS (_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF)
#else
#define CRTDEBUGFLAGS (0)
#endif

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2


/*
  for Visual Studio Project Option 
  LIB: mmsystem.lib comdlg32.lib
  MACRO: _MT, _WINDOWS
  Multithread library
  */
#define NAME_MAX (MAX_PATH)
#define NLS "\r\n"

#ifdef TWSYNG32
#define DEFAULT_VOICES               64 /* 64 voices */
#define DEFAULT_AUDIO_BUFFER_BITS     9 /* 512sample 11ms (44.1kHz) */
#define DEFAULT_AUDIO_BUFFER_NUM     32 /* 370ms over */
#define DEFAULT_COMPUTE_BUFFER_BITS   6 /* ratio 1.45ms (44.1kHz) */
#define TWSYNG32INI 1 // use twsyng32.ini or timpp32g.ini ??
#define USE_TWSYN_BRIDGE 1
#undef ANOTHER_MAIN
//#undef __W32G__	/* for Win32 GUI */
#undef IA_W32GUI	/* for Win32 GUI */
#undef AU_VORBIS
//#undef AU_VORBIS_DLL
#undef AU_GOGO
#undef AU_GOGO_DLL
#undef AU_LAME
//#undef AU_FLAC
//#undef AU_FLAC_DLL
//#undef AU_OGGFLAC
//#undef AU_OGGFLAC_DLL
/* #undef AU_OPUS */
/* #undef AU_OPUS_DLL */
#undef AU_SPEEX
/* #undef AU_AO */
//#undef AU_PORTAUDIO
//#undef AU_PORTAUDIO_DLL
#undef AU_NPIPE
#undef AU_WRITE_MIDI
//#undef AU_LIST
#undef AU_MODMIDI
#undef AU_VOLUME_CALC
#undef AU_BENCHMARK
#undef SMFCONV
/* Define if you have the vorbis library (-lvorbis).  */
//#undef HAVE_LIBVORBIS
/* Define if you have the vorbis library (-lgogo).  */
#undef HAVE_LIBGOGOL
#endif

/* Win32 Synthesizer Service */
#ifdef TWSYNSRV
#define DEFAULT_VOICES               64 /* 64 voices */
#define DEFAULT_AUDIO_BUFFER_BITS     9 /* 512sample 11ms (44.1kHz) */
#define DEFAULT_AUDIO_BUFFER_NUM     32 /* 370ms over */
#define DEFAULT_COMPUTE_BUFFER_BITS   6 /* ratio 1.45ms (44.1kHz) */
#undef USE_TWSYN_BRIDGE
/* #undef AU_NPIPE */
#undef AU_VORBIS
//#undef AU_VORBIS_DLL
#undef AU_GOGO
#undef AU_GOGO_DLL
/* #undef AU_PORTAUDIO */
/* #undef AU_PORTAUDIO_DLL */
#undef AU_LAME
#undef AU_FLAC
#undef AU_FLAC_DLL
#undef AU_OGGFLAC
#undef AU_OGGFLAC_DLL
#undef AU_OPUS
#undef AU_OPUS_DLL
#undef AU_SPEEX
/* #undef AU_AO */
#undef AU_WRITE_MIDI
#undef AU_LIST
#undef AU_MODMIDI
#undef AU_VOLUME_CALC
#undef AU_BENCHMARK
/* #undef VST_LOADER_ENABLE */
#undef SMFCONV
#undef SUPPORT_SOCKET
#undef SUPPORT_SOUNDSPEC
#undef __W32G__	/* for Win32 GUI */
#endif


#ifdef WINDRV
#define DEFAULT_VOICES               64 /* 64 voices */
#define DEFAULT_AUDIO_BUFFER_BITS     9 /* 512sample 11ms (44.1kHz) */
#define DEFAULT_AUDIO_BUFFER_NUM     32 /* 370ms over */
#define DEFAULT_COMPUTE_BUFFER_BITS   6 /* ratio 1.45ms (44.1kHz) */
#define TIMDRVINI 1 // use timdrv.ini or twsyng32.ini or timpp32g.ini ??
#define IA_WINSYN
#undef USE_TWSYN_BRIDGE
//#undef AU_WASAPI
#undef AU_VORBIS
//#undef AU_VORBIS_DLL
#undef AU_GOGO
#undef AU_GOGO_DLL
#undef AU_LAME
#undef AU_FLAC
#undef AU_FLAC_DLL
#undef AU_OGGFLAC
#undef AU_OGGFLAC_DLL
#undef AU_OPUS
#undef AU_OPUS_DLL
#undef AU_SPEEX
#undef AU_AO
#define AU_PORTAUDIO 1
#define AU_PORTAUDIO_DLL 1
//#undef AU_PORTAUDIO
//#undef AU_PORTAUDIO_DLL
#undef AU_NPIPE
#undef AU_WRITE_MIDI
#undef AU_LIST
#undef AU_MODMIDI
#undef AU_VOLUME_CALC
#undef AU_BENCHMARK
#undef SMFCONV

/* Define if you have the vorbis library (-lvorbis).  */
//#undef HAVE_LIBVORBIS

/* Define if you have the vorbis library (-lgogo).  */
//#undef HAVE_LIBGOGOL

#undef __W32G__	/* for Win32 GUI */
#undef SUPPORT_SOUNDSPEC
#endif


/* Win32GUI Standalone */
#if defined(__W32G__) && !TWSYNG32 && !KBTIM
#define FORCE_TIME_PERIOD 1
#endif


/* Win32 Console */
#ifdef TIM_CUI
#undef ANOTHER_MAIN
#undef __W32G__	/* for Win32 GUI */
#undef IA_W32G_SYN	/* for Win32 GUI */
//#undef AU_VORBIS
//#undef AU_VORBIS_DLL
//#undef AU_GOGO
//#undef AU_GOGO_DLL
//#undef AU_LAME
//#undef AU_FLAC
//#undef AU_FLAC_DLL
//#undef AU_OGGFLAC
//#undef AU_OGGFLAC_DLL
//#undef AU_SPEEX
//#undef AU_AO
//#undef AU_PORTAUDIO
//#undef AU_PORTAUDIO_DLL
//#undef AU_NPIPE
#undef AU_WRITE_MIDI
#define AU_LIST 1
#define AU_MODMIDI 1
#define AU_VOLUME_CALC 1
//#undef AU_VOLUME_CALC
//#undef AU_BENCHMARK
//#undef SMFCONV
#undef SUPPORT_SOCKET
//#undef SUPPORT_SOUNDSPEC
#define FORCE_TIME_PERIOD 1
#endif


/* x64 */
#ifdef _WIN64
#define _AMD64_ 1
//#undef SUPPORT_SOUNDSPEC
//#undef AU_VORBIS
//#undef AU_VORBIS_DLL
//#undef AU_GOGO
//#undef AU_GOGO_DLL
//#undef AU_LAME
//#undef AU_FLAC
//#undef AU_FLAC_DLL
//#undef AU_OGGFLAC
//#undef AU_OGGFLAC_DLL
//#undef AU_OPUS
//#undef AU_OPUS_DLL
//#undef AU_SPEEX
//#undef AU_AO
//#undef AU_PORTAUDIO
//#undef AU_PORTAUDIO_DLL
//#undef AU_NPIPE
//#undef AU_WRITE_MIDI
//#undef AU_LIST
//#undef AU_MODMIDI
//#undef AU_VOLUME_CALC
#undef SMFCONV

#define DWL_MSGRESULT (DWLP_MSGRESULT)
#define DWL_DLGPROC (DWLP_DLGPROC)
#define DWL_USER (DWLP_USER)
#endif

#ifdef CFG_FOR_SF
#undef AU_WASAPI
#undef AU_VORBIS
#undef AU_VORBIS_DLL
#undef AU_GOGO
#undef AU_GOGO_DLL
#undef AU_PORTAUDIO
#undef AU_PORTAUDIO_DLL
#undef AU_LAME
#undef AU_FLAC
#undef AU_FLAC_DLL
#undef AU_OGGFLAC
#undef AU_OGGFLAC_DLL
#undef AU_OPUS
#undef AU_OPUS_DLL
#undef AU_SPEEX
#undef AU_AO
#undef AU_NPIPE
#undef AU_WRITE_MIDI
#undef AU_LIST
#undef AU_MODMIDI
#undef AU_VOLUME_CALC
#undef AU_BENCHMARK
#undef VST_LOADER_ENABLE
#undef SMFCONV
#undef SUPPORT_SOCKET
#undef SUPPORT_SOUNDSPEC
#undef __W32G__
#undef HAVE_POPEN
#endif


#ifdef WINDRV_SETUP
#define DEFAULT_VOICES               64 /* 64 voices */
#define DEFAULT_AUDIO_BUFFER_BITS     9 /* 512sample 11ms (44.1kHz) */
#define DEFAULT_AUDIO_BUFFER_NUM     32 /* 370ms over */
#define DEFAULT_COMPUTE_BUFFER_BITS   6 /* ratio 1.45ms (44.1kHz) */
#define TIMDRVINI 1 // use timdrv.ini or twsyng32.ini or timpp32g.ini ??
#define IA_WINSYN
#undef USE_TWSYN_BRIDGE
#define WINDRV 1
#define AU_W32 1
#define AU_WDMKS 1
#define AU_WASAPI 1
#undef AU_VORBIS
//#undef AU_VORBIS_DLL
#undef AU_GOGO
#undef AU_GOGO_DLL
#undef AU_LAME
#undef AU_FLAC
#undef AU_FLAC_DLL
#undef AU_OGGFLAC
#undef AU_OGGFLAC_DLL
#undef AU_OPUS
#undef AU_OPUS_DLL
#undef AU_SPEEX
#undef AU_AO
#define AU_PORTAUDIO 1
#define AU_PORTAUDIO_DLL 1
//#undef AU_PORTAUDIO
//#undef AU_PORTAUDIO_DLL
#undef AU_NPIPE
#undef AU_WRITE_MIDI
#undef AU_LIST
#undef AU_MODMIDI
#undef AU_VOLUME_CALC
#undef AU_BENCHMARK
#undef SMFCONV
#endif



#ifdef KBTIM /*added by Kobarin*/
#define IN_TIMIDITY // in_timidity(WINAMP Plugin) �Ŏg�p����@�\�ǉ� // SMF�ȊO�Ή�
#undef TIM_CUI
#undef IA_W32G_SYN	/* for Win32 GUI */
#define __W32G__
#define cmsg(X) cmsg  //50KB �قǏ������Ȃ�
//#define DEFAULT_RESAMPLATION_NUM 1 // linear
#define NO_MIDI_CACHE
#define HAVE_MKSTEMP
#undef HAVE_POPEN
#undef REDUCE_VOICE_TIME_TUNING //playmidi.c �̐錾���� #define �����폜���Ȃ��Ƌ@�\���Ȃ�
#undef AU_W32
#undef AU_WASAPI
#undef AU_WDMKS
#undef AU_VORBIS
//#undef AU_VORBIS_DLL
#undef AU_GOGO
#undef AU_GOGO_DLL
#undef AU_LAME
#undef AU_FLAC
#undef AU_FLAC_DLL
#undef AU_OGGFLAC
#undef AU_OGGFLAC_DLL
#undef AU_OPUS
#undef AU_OPUS_DLL
#undef AU_SPEEX
#undef AU_AO
#undef AU_PORTAUDIO
#undef AU_PORTAUDIO_DLL
#undef AU_NPIPE
#undef AU_WRITE_MIDI
#undef AU_LIST
#undef AU_MODMIDI
#undef AU_VOLUME_CALC
#undef AU_BENCHMARK
#undef URL_DIR_CACHE_ENABLE
#ifndef IN_TIMIDITY
#undef SMFCONV // for in_timidity
#endif
#undef SUPPORT_SOCKET
#define HAVE_SYS_STAT_H
#define S_ISDIR(mode)   (((mode)&0xF000) == 0x4000)
#define HAVE_VSNPRINTF
#define HAVE_SNPRINTF
typedef __int64 int64;
#undef HAVE_SIGNAL
#undef SUPPORT_SOUNDSPEC
#endif

#ifdef KBTIM_SETUP
#define IN_TIMIDITY // in_timidity(WINAMP Plugin) �Ŏg�p����@�\�ǉ� // SMF�ȊO�Ή�
#undef TIM_CUI
#undef IA_W32G_SYN	/* for Win32 GUI */
#define __W32G__
//#define cmsg(X) cmsg  //50KB �قǏ������Ȃ�
//#define DEFAULT_RESAMPLATION_NUM 1 // linear
#define NO_MIDI_CACHE
#define HAVE_MKSTEMP
#undef HAVE_POPEN
#undef REDUCE_VOICE_TIME_TUNING //playmidi.c �̐錾���� #define �����폜���Ȃ��Ƌ@�\���Ȃ�
#undef AU_W32
#undef AU_WASAPI
#undef AU_WDMKS
#undef AU_VORBIS
//#undef AU_VORBIS_DLL
#undef AU_GOGO
#undef AU_GOGO_DLL
#undef AU_LAME
#undef AU_FLAC
#undef AU_FLAC_DLL
#undef AU_OGGFLAC
#undef AU_OGGFLAC_DLL
#undef AU_OPUS
#undef AU_OPUS_DLL
#undef AU_SPEEX
#undef AU_AO
#define AU_PORTAUDIO 1
#define AU_PORTAUDIO_DLL 1
#undef AU_PORTAUDIO
#undef AU_PORTAUDIO_DLL
#undef AU_NPIPE
#undef AU_WRITE_MIDI
#undef AU_LIST
#undef AU_MODMIDI
#undef AU_VOLUME_CALC
#undef AU_BENCHMARK
#undef SUPPORT_SOUNDSPEC
#undef URL_DIR_CACHE_ENABLE
#ifndef IN_TIMIDITY
#undef SMFCONV // for in_timidity
#endif
#undef SUPPORT_SOCKET
#define HAVE_SYS_STAT_H
#define S_ISDIR(mode)   (((mode)&0xF000) == 0x4000)
#define HAVE_VSNPRINTF
#define HAVE_SNPRINTF
typedef __int64 int64;
#undef HAVE_SIGNAL
//#undef SUPPORT_SOUNDSPEC

#endif



#ifdef TIMVSTI
#ifdef WIN32
#define WINVSTI 1
#endif
#endif

#ifdef WINVSTI
#define DEFAULT_VOICES               64 /* 64 voices */
#define DEFAULT_AUDIO_BUFFER_BITS     9 /* 512sample 11ms (44.1kHz) */
#define DEFAULT_AUDIO_BUFFER_NUM     32 /* 370ms over */
#define DEFAULT_COMPUTE_BUFFER_BITS   6 /* ratio 1.45ms (44.1kHz) */
#undef TIM_CUI
#undef IA_WINSYN
#undef HAVE_POPEN
#undef REDUCE_VOICE_TIME_TUNING //playmidi.c �̐錾���� #define �����폜���Ȃ��Ƌ@�\���Ȃ�
#undef AU_BUFFER 1
#undef AU_W32
#undef AU_WASAPI
#undef AU_WDMKS
#undef AU_VORBIS
#undef AU_VORBIS_DLL
#undef AU_GOGO
#undef AU_GOGO_DLL
#undef AU_LAME
#undef AU_FLAC
#undef AU_FLAC_DLL
#undef AU_OGGFLAC
#undef AU_OGGFLAC_DLL
#undef AU_OPUS
#undef AU_OPUS_DLL
#undef AU_SPEEX
#undef AU_AO
#undef AU_PORTAUDIO 1
#undef AU_PORTAUDIO_DLL 1
#undef AU_NPIPE
#undef AU_WRITE_MIDI
#undef AU_LIST
#undef AU_MODMIDI
#undef AU_VOLUME_CALC
#undef AU_BENCHMARK
#undef SMFCONV
/* Define if you have the vorbis library (-lvorbis).  */
#undef HAVE_LIBVORBIS
/* Define if you have the vorbis library (-lgogo).  */
#undef HAVE_LIBGOGOL
#undef __W32G__	/* for Win32 GUI */
#undef SUPPORT_SOUNDSPEC
#endif
