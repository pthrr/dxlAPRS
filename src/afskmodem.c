/*
 * dxlAPRS toolchain
 *
 * Copyright (C) Christian Rabler <oe5dxl@oevsv.at>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */


#define X2C_int32
#define X2C_index32
#ifndef X2C_H_
#include "X2C.h"
#endif
#define afskmodem_C_
#ifndef ptty_H_
#include "ptty.h"
#endif
#ifndef soundctl_H_
#include "soundctl.h"
#endif
#ifndef udp_H_
#include "udp.h"
#endif
#include <fcntl.h>
#ifndef osi_H_
#include "osi.h"
#endif
#ifndef Lib_H_
#include "Lib.h"
#endif
#ifndef InOut_H_
#include "InOut.h"
#endif
#include <stdio.h>
#ifndef aprsstr_H_
#include "aprsstr.h"
#endif
#ifndef RealMath_H_
#include "RealMath.h"
#endif
#ifndef symlink_H_
#include "symlink.h"
#endif
#ifndef mlib_H_
#include "mlib.h"
#endif
#ifndef Select_H_
#include "Select.h"
#endif
#ifndef Storage_H_
#include "Storage.h"
#endif
#ifndef TimeConv_H_
#include "TimeConv.h"
#endif
#include <unistd.h>
#ifndef afskmodemptt_H_
#include "afskmodemptt.h"
#endif
#include <signal.h>

/* (a)fsk -  kiss/axudp stereo soundcard afsk/fsk multimodem by OE5DXL */
/*FROM setsighandler IMPORT SIGTERM, SIGINT, setsignalproc, SIGPROC; */
#define afskmodem_MAXLEN 339

#define afskmodem_MAXMODEMS 8

#define afskmodem_TXBUFLEN 60
/* default kiss buffers */

#define afskmodem_ADCBUFLEN 4096

#define afskmodem_BAUDSAMP 65536

#define afskmodem_PLLSHIFT 4096

#define afskmodem_DEFTIMEOUT 60
/* then unsent frames are discarded */

#define afskmodem_AFIRLEN 32
/*32, 64*/

#define afskmodem_AOVERSAMP 16
/*16*/

#define afskmodem_DFIRLEN 64

#define afskmodem_DOVERSAMP 16

#define afskmodem_TFIRLEN 64
/* 2^ */
/*32/5*/

#define afskmodem_SHIFTBITS 6

#define afskmodem_TFIRFINE 128

#define afskmodem_MAXVOL 25000.0
/* max dac / overswing */

static unsigned long afskmodem_POLYNOM = 0x8408UL;

static unsigned long afskmodem_CRCINIT = 0xFFFFUL;

static unsigned long afskmodem_CRCRESULT = 0x9F0BUL;

#define afskmodem_MINFLEN 16

#define afskmodem_STUFFLEN 5

#define afskmodem_FEND 192

#define afskmodem_FESC 219

#define afskmodem_TFEND 220

#define afskmodem_TFESC 221

struct KISSBUF;

typedef struct KISSBUF * pKISSNEXT;


struct KISSBUF {
   pKISSNEXT next;
   char port;
   unsigned long time0;
   unsigned long len;
   char data[341]; /* +2 byte crc */
};

typedef float TXFIRTAB[128][64];

typedef char FILENAME[1024];

enum MONITOR {afskmodem_off, afskmodem_noinfo, afskmodem_normal,
                afskmodem_passall};


enum CHANNELS {afskmodem_LEFT, afskmodem_RIGHT};


typedef float DFIRTAB[1024];

typedef float DFIR[64];

typedef float AFIRTAB[512];

struct MPAR;


struct MPAR {
   unsigned char ch;
   long baudfine;
   long pllshift;
   float left;
   float tcnt;
   float freq;
   float afskmidfreq;
   float clamp;
   float squelchdcd;
   float noise;
   float afshift;
   float afmid;
   float txvolum;
   float afskhighpass;
   unsigned char monitor;
   float sqmed[2];
   unsigned long dfin;
   DFIRTAB dfirtab;
   DFIR dfir;
   AFIRTAB afirtab;
   char afsk;
   char cbit;
   char oldd;
   char plld;
   char scramb;
   char data1;
   unsigned long rxstuffc;
   unsigned long rxbyte;
   unsigned long rxbitc;
   unsigned long rxp;
   unsigned long demodbaud;
   unsigned long txbaud;
   unsigned long port16;
   unsigned long dcdclockm;
   unsigned long bertc;
   unsigned long berterr;
   unsigned long txdel;
   unsigned long txtail;
   unsigned long rxcrc;
   unsigned long scrambler;
   char rxbuf[339];
   pKISSNEXT txbufin;
   pKISSNEXT txbufout;
   unsigned long timeout;
   unsigned long configbaud;
   unsigned long configtxdel;
   unsigned long configtxtail;
   unsigned long txdelpattern;
   unsigned long configafskshift;
   unsigned long configafskmid;
   unsigned long confignyquist;
   long configequalizer;
   long leveldcd;
   long bert;
   char configured;
   char kissignore;
   long udpsocket;
   unsigned long udpport;
   unsigned long udpbind;
   unsigned long udpip;
   char checkip;
};

enum TXSTATE {afskmodem_receiv, afskmodem_slotwait, afskmodem_sendtxdel,
                afskmodem_senddata, afskmodem_sendtxtail};


enum DUPLEX {afskmodem_simplex, afskmodem_shiftdigi, afskmodem_fullduplex};


struct CHAN;


struct CHAN {
   unsigned long gmqtime;
   unsigned long gmcnt; /* gm900 quiet tx time after ptt */
   unsigned long dcdclock;
   unsigned long pttsoundbufs;
   unsigned long dds;
   unsigned long txbaudgen;
   unsigned long addrandom;
   unsigned long tbyte;
   unsigned long tbytec;
   long txstuffc;
   long tbitc;
   X2C_ADDRESS hptt;
   char pttstate;
   char tnrzi;
   unsigned char state;
   unsigned char duplex;
   float persist;
   float hipasscap;
   unsigned long tscramb;
   long adcmax;
   long actmodem;
   unsigned long configpersist;
   float afir[32];
};

static long pipefd;

static long soundfd;

static long debfd;

static char esc;

static char abortonsounderr;

static unsigned long getst;

static unsigned long i;

static unsigned long afin;

static unsigned long soundbufs;

static unsigned long maxsoundbufs;

static unsigned long adcrate;

static unsigned long adcbuflen;

static unsigned long adcbytes;

static unsigned long fragmentsize;

static unsigned long extraaudiodelay;

static unsigned long clock0;

static unsigned long systime;

static unsigned char maxchannels;

static struct CHAN chan[2];

static pKISSNEXT pTxFree;

static pKISSNEXT pGetKiss;

static TXFIRTAB TFIR;

static struct MPAR modpar[8];

static float SIN[32768];

static unsigned char CRCL[256];

static unsigned char CRCH[256];

static FILENAME soundfn;


static void Error(char text[], unsigned long text_len)
{
   X2C_PCOPY((void **)&text,text_len);
   InOut_WriteString(text, text_len);
   osi_WrStrLn(" error abort", 13ul);
   X2C_ABORT();
   X2C_PFREE(text);
} /* end Error() */

/*
PROCEDURE SetStatLine(fd:INTEGER; rts, dtr:BOOLEAN):BOOLEAN;
VAR arg : ARRAY[0..255] OF CARDINAL;
    r:INTEGER;
BEGIN
  arg[0]:=TIOCM_RTS;
  IF rts THEN r:=ioctl(fd, TIOCMBIS, ADR(arg)) ELSE r:=ioctl(fd, TIOCMBIC,
                ADR(arg)) END;
  IF r>=0 THEN
    arg[0]:=TIOCM_DTR;
    IF dtr THEN r:=ioctl(fd, TIOCMBIS, ADR(arg)) ELSE r:=ioctl(fd, TIOCMBIC,
                ADR(arg)) END;
  END;
  RETURN r>=0
END SetStatLine;
*/

static void Hamming(float f[], unsigned long f_len)
{
   unsigned long i0;
   unsigned long tmp;
   tmp = f_len-1;
   i0 = 0UL;
   if (i0<=tmp) for (;; i0++) {
      f[i0] = f[i0]*(0.54f+0.46f*RealMath_cos(3.141592f*(X2C_DIVR((float)i0,
                (float)(1UL+(f_len-1))))));
      if (i0==tmp) break;
   } /* end for */
} /* end Hamming() */


static void initdfir(DFIRTAB dfirtab, unsigned long fg)
{
   unsigned long f;
   unsigned long i0;
   float t[512];
   float e;
   float f1;
   unsigned long tmp;
   for (i0 = 0UL; i0<=511UL; i0++) {
      t[i0] = 0.5f;
   } /* end for */
   f1 = X2C_DIVR((float)(fg*64UL),(float)adcrate);
   tmp = (unsigned long)X2C_TRUNCC(f1,0UL,X2C_max_longcard)+1UL;
   f = 1UL;
   if (f<=tmp) for (;; f++) {
      e = 1.0f;
      if (f==(unsigned long)X2C_TRUNCC(f1,0UL,X2C_max_longcard)+1UL) {
         e = f1-(float)(unsigned long)X2C_TRUNCC(f1,0UL,X2C_max_longcard);
      }
      for (i0 = 0UL; i0<=511UL; i0++) {
         t[i0] = t[i0]+e*RealMath_cos(X2C_DIVR(3.141592f*(float)(i0*f),
                512.0f));
      } /* end for */
      if (f==tmp) break;
   } /* end for */
   /*
     FOR f:=1 TO fg*DFIRLEN DIV adcrate DO
       FOR i:=0 TO HIGH(t) DO t[i]:=t[i]+cos(pi*FLOAT(i*f)/FLOAT(HIGH(t)+1))
                END;
     END;
   */
   Hamming(t, 512ul);
   for (i0 = 0UL; i0<=511UL; i0++) {
      t[i0] = t[i0]*(0.54f+0.46f*RealMath_cos(3.141592f*(X2C_DIVR((float)i0,
                512.0f))));
   } /* end for */
   for (i0 = 0UL; i0<=511UL; i0++) {
      dfirtab[511UL+i0] = t[i0];
      dfirtab[511UL-i0] = t[i0];
   } /* end for */
/*
IO.WrLn;
FOR i:=0 TO HIGH(dfirtab) DO IO.WrFixed(dfirtab[i], 2,8) END;
IO.WrLn;
*/
/*
fd:=FIO.Create("/tmp/td.raw");
FOR i:=0 TO HIGH(dfirtab) DO n:=VAL(INTEGER, dfirtab[i]*2000.0);
                FIO.WrBin(fd,n,2) END;
FIO.Close(fd);
*/
} /* end initdfir() */


static void initafir(AFIRTAB atab, unsigned long F0, unsigned long F1,
                float eq)
{
   unsigned long f;
   unsigned long i0;
   float t[256];
   float f10;
   float f00;
   float e;
   unsigned long tmp;
   f00 = X2C_DIVR((float)(F0*32UL),(float)adcrate);
   f10 = X2C_DIVR((float)(F1*32UL),(float)adcrate);
   for (i0 = 0UL; i0<=255UL; i0++) {
      t[i0] = 0.0f;
   } /* end for */
   tmp = (unsigned long)X2C_TRUNCC(f10,0UL,X2C_max_longcard)+1UL;
   f = (unsigned long)X2C_TRUNCC(f00,0UL,X2C_max_longcard);
   if (f<=tmp) for (;; f++) {
      e = 1.0f+eq*((X2C_DIVR((float)f,X2C_DIVR((float)((F0+F1)*32UL),
                (float)adcrate)))*2.0f-1.0f);
      /*
          e:=1.0 + eq*(FLOAT(f)/FLOAT((F0+F1)*AFIRLEN DIV adcrate)*2.0-1.0);
      */
      if (e<0.0f) e = 0.0f;
      if (f==(unsigned long)X2C_TRUNCC(f00,0UL,X2C_max_longcard)) {
         e = e*(1.0f-(f00-(float)(unsigned long)X2C_TRUNCC(f00,0UL,
                X2C_max_longcard)));
      }
      if (f==(unsigned long)X2C_TRUNCC(f10,0UL,X2C_max_longcard)+1UL) {
         e = e*(f10-(float)(unsigned long)X2C_TRUNCC(f10,0UL,
                X2C_max_longcard));
      }
      /*
      IF eq<>0 THEN IO.WrFixed(e,2,2);IO.WrLn; END;
      */
      if (f==0UL) {
         for (i0 = 0UL; i0<=255UL; i0++) {
            t[i0] = t[i0]+e*0.5f;
         } /* end for */
      }
      else {
         for (i0 = 0UL; i0<=255UL; i0++) {
            t[i0] = t[i0]+e*RealMath_cos(X2C_DIVR(3.141592f*(float)(i0*f),
                256.0f));
         } /* end for */
      }
      if (f==tmp) break;
   } /* end for */
   Hamming(t, 256ul);
   for (i0 = 0UL; i0<=255UL; i0++) {
      atab[255UL+i0] = t[i0];
      atab[255UL-i0] = t[i0];
   } /* end for */
   if (F0>0UL) {
      /* make dc level zero */
      e = 0.0f;
      for (i0 = 0UL; i0<=511UL; i0++) {
         e = e+atab[i0];
      } /* end for */
      e = X2C_DIVR(e,512.0f);
      for (i0 = 0UL; i0<=511UL; i0++) {
         atab[i0] = atab[i0]-e;
      } /* end for */
   }
/*
IO.WrLn;
FOR i:=0 TO HIGH(atab) DO IO.WrFixed(atab[i], 2,8) END;
IO.WrLn;
*/
/*
IF eq<>0.0 THEN
debfd:=FIO.Create("/tmp/ta.raw");
FOR i:=0 TO HIGH(atab) DO f:=VAL(INTEGER, atab[i]*1000.0);
                FIO.WrBin(debfd,f,2) END;
FIO.Close(debfd);
END;
*/
} /* end initafir() */


static void initTFIR(void)
{
   unsigned long b;
   unsigned long f;
   unsigned long j;
   unsigned long i0;
   float sym[384];
   float sum[128];
   float s[774];
   float max0;
   float k;
   float fr;
   for (i0 = 0UL; i0<=383UL; i0++) {
      sym[i0] = 0.5f;
   } /* end for */
   fr = 2.88f;
   for (f = 1UL; f<=3UL; f++) {
      if (f==(unsigned long)X2C_TRUNCC(fr,0UL,X2C_max_longcard)+1UL) {
         k = fr-(float)(unsigned long)X2C_TRUNCC(fr,0UL,X2C_max_longcard);
      }
      else k = 1.0f;
      for (i0 = 0UL; i0<=383UL; i0++) {
         sym[i0] = sym[i0]+k*RealMath_cos(X2C_DIVR(3.141592f*(float)(i0*f),
                384.0f));
      } /* end for */
   } /* end for */
   Hamming(sym, 384ul);
   for (i0 = 0UL; i0<=773UL; i0++) {
      s[i0] = 0.0f;
   } /* end for */
   max0 = 0.0f;
   for (i0 = 0UL; i0<=383UL; i0++) {
      s[383UL+i0+3UL] = sym[i0];
      s[(383UL-i0)+3UL] = sym[i0];
      if ((float)fabs(sym[i0])>max0) max0 = (float)fabs(sym[i0]);
   } /* end for */
   max0 = X2C_DIVR(2.0f,max0);
   for (j = 0UL; j<=63UL; j++) {
      for (i0 = 0UL; i0<=127UL; i0++) {
         sum[i0] = 0.0f;
      } /* end for */
      for (b = 0UL; b<=5UL; b++) {
         for (i0 = 0UL; i0<=127UL; i0++) {
            sum[i0] = sum[i0]+s[i0+b*128UL]*((float)(unsigned long)X2C_IN(b,
                32,(unsigned long)j)-0.5f);
         } /* end for */
      } /* end for */
      for (i0 = 0UL; i0<=127UL; i0++) {
         TFIR[i0][j] = sum[i0]*max0;
      } /* end for */
   } /* end for */
} /* end initTFIR() */


static void SetMixer(char mixfn[], unsigned long mixfn_len,
                unsigned long chan0, unsigned long left,
                unsigned long right)
{
   long fd;
   X2C_PCOPY((void **)&mixfn,mixfn_len);
   fd = open(mixfn, 2L);
   if (fd>=0L) {
      if (chan0==255UL) chan0 = recnum();
      setmixer(fd, chan0, (right<<8)+left);
   }
   else {
      InOut_WriteString(mixfn, mixfn_len);
      Error(" open", 6ul);
   }
   X2C_PFREE(mixfn);
} /* end SetMixer() */


static void OpenSound(void)
{
   long s;
   long i0;
   soundfd = open(soundfn, 2L);
   if (soundfd>=0L) {
      i0 = samplesize(soundfd, 16UL); /* 8, 16 */
      i0 = channels(soundfd, (unsigned long)maxchannels+1UL); /* 1, 2  */
      i0 = setfragment(soundfd, fragmentsize); /* 2^bufsize * 65536*bufs*/
      if (i0) {
         InOut_WriteString("sound setfragment returns ", 27ul);
         InOut_WriteInt(i0, 1UL);
         osi_WrLn();
      }
      i0 = sampelrate(soundfd, adcrate); /* 8000..48000 */
      s = (long)getsampelrate(soundfd);
      if (s!=(long)adcrate) {
         InOut_WriteString("sound device returns ", 22ul);
         InOut_WriteInt(s, 1UL);
         osi_WrStrLn("Hz!", 4ul);
      }
   }
   else if (abortonsounderr) {
      /*
            IF s>=0 THEN Error("") END;
      */
      InOut_WriteString(soundfn, 1024ul);
      Error(" open", 6ul);
   }
} /* end OpenSound() */


static void ttypar(char fn[], unsigned long fn_len)
{
   struct termios term;
   long fd;
   long res;
   X2C_PCOPY((void **)&fn,fn_len);
   fd = open(fn, 2048L);
   if (fd>=0L) {
      res = tcgetattr(fd, &term);
      /*
          term.c_lflag:=CAST(CARDINAL, CAST(BITSET,
                term.c_lflag) - CAST(BITSET, ECHO));
      */
      term.c_lflag = 0UL;
      res = tcsetattr(fd, 0L, &term);
   }
   /*
       res:=tcsetattr (fd, 0, term);
   */
   osi_Close(fd);
   X2C_PFREE(fn);
} /* end ttypar() */


static long Opentty(char linkname[], unsigned long linkname_len)
{
   long fd;
   char ptsname[4096];
   long Opentty_ret;
   X2C_PCOPY((void **)&linkname,linkname_len);
   fd = open("/dev/ptmx", 2050L);
   if (fd<0L) Error("/dev/ptmx open", 15ul);
   if (getptsname(fd, (char *)ptsname, 4096UL)) Error("no ttyname", 11ul);
   /*
     IO.WrStr(ptsname); IO.WrLn;
   */
   if (grantpts(fd)) Error("ptty grant", 11ul);
   if (unlockpts(fd)) Error("ptty unlock", 12ul);
   ttypar(ptsname, 4096ul);
   /*make link*/
   remove(linkname);
   if (symblink((char *)ptsname, (char *)linkname)) {
      InOut_WriteString("cannot create link <", 21ul);
      InOut_WriteString(linkname, linkname_len);
      osi_WrStrLn(">, starting without kiss interface", 35ul);
      osi_Close(fd);
      fd = -1L;
   }
   Opentty_ret = fd;
   X2C_PFREE(linkname);
   return Opentty_ret;
} /* end Opentty() */


static void Makekissbufs(unsigned long n)
{
   pKISSNEXT pt;
   pTxFree = 0;
   pGetKiss = 0;
   i = 0UL;
   for (;;) {
      if (i>=n) break;
      Storage_ALLOCATE((X2C_ADDRESS *) &pt, sizeof(struct KISSBUF));
      ++i;
      if (pt==0) break;
      pt->next = pTxFree;
      pTxFree = pt;
   }
} /* end Makekissbufs() */


static void StoBuf(long m, pKISSNEXT p)
{
   struct MPAR * anonym;
   { /* with */
      struct MPAR * anonym = &modpar[m];
      if (anonym->configured && pTxFree) {
         p->next = 0;
         if (anonym->txbufin==0) anonym->txbufin = p;
         else anonym->txbufout->next = p;
         anonym->txbufout = p;
      }
      else {
         p->next = 0; /* discard frame, out of memory */
         pTxFree = p;
      }
   }
} /* end StoBuf() */


static void ExtractWord(char w[], unsigned long w_len, char s[],
                unsigned long s_len)
{
   unsigned long j;
   unsigned long i0;
   w[0UL] = 0;
   i0 = 0UL;
   while ((i0<=s_len-1 && s[i0]) && s[i0]!=':') {
      if (i0<=w_len-1) w[i0] = s[i0];
      ++i0;
   }
   if (i0<=w_len-1) w[i0] = 0;
   j = 0UL;
   if (i0<=s_len-1 && s[i0]) {
      ++i0;
      while (i0<=s_len-1 && s[i0]) {
         s[j] = s[i0];
         ++i0;
         ++j;
      }
   }
   s[j] = 0;
} /* end ExtractWord() */


static void Config(void)
{
   long i0;
   unsigned char c;
   struct CHAN * anonym;
   struct MPAR * anonym0;
   for (c = afskmodem_LEFT;; c++) {
      { /* with */
         struct CHAN * anonym = &chan[c];
         anonym->persist = (float)
                ((anonym->configpersist*adcrate*((unsigned long)
                maxchannels+1UL))/adcbuflen)*0.001f; /* ms */
      }
      if (c==afskmodem_RIGHT) break;
   } /* end for */
   for (i0 = 0L; i0<=7L; i0++) {
      { /* with */
         struct MPAR * anonym0 = &modpar[i0];
         anonym0->txbaud = (anonym0->configbaud*65536UL)/adcrate;
         anonym0->demodbaud = anonym0->txbaud*2UL;
         anonym0->txdel = (anonym0->configbaud*anonym0->configtxdel)/8000UL;
         anonym0->txtail = 2UL+2UL*(unsigned long)
                anonym0->scramb+(anonym0->configbaud*anonym0->configtxtail)
                /8000UL;
         anonym0->afskmidfreq = X2C_DIVR((float)anonym0->configafskmid*2.0f,
                (float)adcrate);
         anonym0->afshift = X2C_DIVR(0.5f*(float)
                (anonym0->configafskshift*32768UL),(float)adcrate);
         anonym0->afmid = X2C_DIVR((float)(anonym0->configafskmid*32768UL),
                (float)adcrate);
         if (anonym0->afsk) {
            initafir(anonym0->afirtab,
                (anonym0->configafskmid-anonym0->configafskshift/2UL)
                -anonym0->configbaud/4UL,
                anonym0->configafskmid+anonym0->configafskshift/2UL+anonym0->configbaud/4UL,
                 X2C_DIVR((float)anonym0->configequalizer,100.0f));
            initdfir(anonym0->dfirtab,
                (anonym0->configbaud*anonym0->confignyquist)/100UL);
         }
         else {
            initafir(anonym0->afirtab, 0UL,
                (anonym0->configbaud*anonym0->confignyquist)/100UL,
                X2C_DIVR((float)anonym0->configequalizer,100.0f));
         }
         anonym0->baudfine = 0L;
         anonym0->left = 0.0f;
         anonym0->tcnt = 0.0f;
         anonym0->freq = 0.0f;
         anonym0->dfin = 0UL;
         anonym0->cbit = 0;
         anonym0->rxp = 0UL;
         anonym0->rxbitc = 0UL;
         anonym0->rxcrc = 0xFFFFUL;
      }
   } /* end for */
} /* end Config() */

/*
PROCEDURE GetIp(h:ARRAY OF CHAR; VAR ip:IPNUM; VAR dp, lp:UDPPORT;
                VAR fd:INTEGER; VAR check:BOOLEAN):INTEGER;
VAR i, p, n:CARDINAL;
    ok:BOOLEAN;
BEGIN
  p:=0;
  h[HIGH(h)]:=0C;
  ip:=0;
  n:=0;
  FOR i:=0 TO 5 DO
    n:=0;
    ok:=FALSE;
    WHILE (h[p]>="0") & (h[p]<="9") DO
      ok:=TRUE;
      n:=n*10+ORD(h[p])-ORD("0");
      INC(p);
    END;
    IF NOT ok THEN RETURN -1 END;
    IF i<3 THEN
      IF (h[p]<>".") OR (n>255) THEN RETURN -1 END;
      ip:=ip*256+n;
    ELSIF i=3 THEN
      ip:=ip*256+n;
      IF (h[p]<>":")OR (n>255) THEN RETURN -1 END;
    ELSIF i=4 THEN
      check:=h[p]<>"/";
      IF (h[p]<>":") & (h[p]<>"/") OR (n>65535) THEN RETURN -1 END;
      dp:=n;
    ELSIF n>65535 THEN RETURN -1 END;
    lp:=n;
    INC(p);
  END;  

  fd:=udp.openudp();
  IF (fd<0) OR (udp.bindudp(fd, lp)<0) (*OR (udp.udpnonblock(fd)<0)*) THEN RETURN -1 END;
  RETURN 0

END GetIp;
*/

static long GetIp(char h[], unsigned long h_len, unsigned long * ip,
                unsigned long * dp, unsigned long * lp, long * fd,
                char * check)
{
   long GetIp_ret;
   X2C_PCOPY((void **)&h,h_len);
   if (aprsstr_GetIp2(h, h_len, ip, dp, lp, check)<0L) {
      GetIp_ret = -1L;
      goto label;
   }
   *fd = openudp();
   if (*fd<0L || bindudp(*fd, *lp)<0L) {
      /*OR (udp.udpnonblock(fd)<0)*/
      GetIp_ret = -1L;
      goto label;
   }
   GetIp_ret = 0L;
   label:;
   X2C_PFREE(h);
   return GetIp_ret;
} /* end GetIp() */


static void bertstart(void)
{
   pKISSNEXT p;
   unsigned long i0;
   struct MPAR * anonym;
   for (i0 = 0UL; i0<=7UL; i0++) {
      { /* with */
         struct MPAR * anonym = &modpar[i0];
         if (anonym->bert>0L && pTxFree) {
            p = pTxFree;
            pTxFree = pTxFree->next;
            p->port = (char)i0;
            p->len = 0UL;
            p->time0 = X2C_max_longcard;
            StoBuf((long)i0, p);
         }
      }
   } /* end for */
} /* end bertstart() */


static void Parms(void)
{
   char err;
   FILENAME pipefn;
   FILENAME mixfn;
   FILENAME h1;
   FILENAME h;
   unsigned long right;
   unsigned long left;
   unsigned long cnum;
   unsigned long kissbufs;
   long inum;
   long modem;
   long channel;
   unsigned char c;
   struct CHAN * anonym;
   struct MPAR * anonym0;
   struct MPAR * anonym1;
   err = 0;
   abortonsounderr = 0;
   adcrate = 16000UL;
   adcbytes = 2UL;
   adcbuflen = 256UL;
   fragmentsize = 9UL;
   maxchannels = afskmodem_LEFT;
   extraaudiodelay = 1UL;
   debfd = -1L;
   for (cnum = 0UL; cnum<=32767UL; cnum++) {
      SIN[cnum] = RealMath_cos(X2C_DIVR((float)cnum*2.0f*3.141592f,
                32768.0f));
   } /* end for */
   for (c = afskmodem_LEFT;; c++) {
      { /* with */
         struct CHAN * anonym = &chan[c];
         anonym->state = afskmodem_receiv;
         anonym->pttsoundbufs = 0UL;
         anonym->dds = 0UL;
         anonym->duplex = afskmodem_simplex;
         anonym->persist = 800.0f;
         anonym->actmodem = -1L;
         anonym->hptt = 0;
         anonym->configpersist = 800UL;
         anonym->tscramb = 0x1UL; /* for bert */
         anonym->gmqtime = 0UL;
      }
      if (c==afskmodem_RIGHT) break;
   } /* end for */
   for (cnum = 0UL; cnum<=7UL; cnum++) {
      { /* with */
         struct MPAR * anonym0 = &modpar[cnum];
         anonym0->configured = cnum==0UL;
         anonym0->configbaud = 1200UL;
         anonym0->configtxdel = 300UL;
         anonym0->configtxtail = 20UL;
         anonym0->configafskshift = 1000UL;
         anonym0->configafskmid = 1700UL;
         anonym0->confignyquist = 75UL;
         anonym0->configequalizer = 0L;
         anonym0->kissignore = 0;
         anonym0->leveldcd = 10L;
         anonym0->pllshift = 4096L;
         anonym0->ch = afskmodem_LEFT;
         anonym0->afsk = 1;
         anonym0->monitor = afskmodem_normal;
         anonym0->scramb = 0;
         anonym0->clamp = 0.996f;
         anonym0->port16 = cnum*16UL;
         anonym0->squelchdcd = 0.14f;
         anonym0->txbufin = 0;
         anonym0->bert = 0L;
         anonym0->txdelpattern = 126UL;
         anonym0->txvolum = 25000.0f;
         anonym0->afskhighpass = 0.0f;
         anonym0->timeout = 60UL;
         anonym0->udpsocket = -1L;
      }
   } /* end for */
   kissbufs = 60UL;
   channel = -1L;
   modem = -1L;
   X2C_COPY("/dev/dsp",9ul,soundfn,1024u);
   pipefn[0U] = 0;
   X2C_COPY("/dev/mixer",11ul,mixfn,1024u);
   maxsoundbufs = 10UL;
   for (;;) {
      Lib_NextArg(h, 1024ul);
      if (h[0U]==0) break;
      if ((h[0U]=='-' && h[1U]) && h[2U]==0) {
         if (h[1U]=='a') {
            if (modem>=0L) modpar[modem].afsk = 0;
            else if (channel<=0L) abortonsounderr = 1;
            else Error("need modem number -M before -a", 31ul);
         }
         else if (h[1U]=='b') {
            Lib_NextArg(h, 1024ul);
            if (modem>=0L) {
               if (!aprsstr_StrToCard(h, 1024ul, &cnum)) err = 1;
               if (cnum>=adcrate) {
                  Error("sampelrate -f too low for this baud", 36ul);
               }
               modpar[modem].configbaud = cnum;
            }
            else {
               if (!aprsstr_StrToCard(h, 1024ul, &cnum)) err = 1;
               maxsoundbufs = cnum;
            }
         }
         else if (h[1U]=='B') {
            Lib_NextArg(h, 1024ul);
            if (!aprsstr_StrToInt(h, 1024ul, &inum)) {
               err = 1;
            }
            if (modem>=0L) modpar[modem].bert = inum*8000L;
            else Error("need modem number -M before -B", 31ul);
         }
         else if (h[1U]=='C') {
            Lib_NextArg(h, 1024ul);
            if (!aprsstr_StrToCard(h, 1024ul, &cnum)) err = 1;
            if (cnum>1UL) Error("channel 0 to 1", 15ul);
            channel = (long)cnum;
         }
         else if (h[1U]=='c') {
            Lib_NextArg(h, 1024ul);
            if (!aprsstr_StrToCard(h, 1024ul, &cnum)) err = 1;
            if (modem>=0L) {
               if (cnum>(unsigned long)maxchannels) {
                  Error("channel too high", 17ul);
               }
               modpar[modem].ch = (unsigned char)cnum;
            }
            else {
               if (cnum<1UL || cnum>2UL) Error("maxchannels 1..2", 17ul);
               maxchannels = (unsigned char)(cnum-1UL);
            }
         }
         else if (h[1U]=='D') {
            Lib_NextArg(h1, 1024ul);
            debfd = creat(h1, 420L);
         }
         else if (h[1U]=='d') {
            Lib_NextArg(h, 1024ul);
            if (!aprsstr_StrToCard(h, 1024ul, &cnum)) err = 1;
            if (modem>=0L) {
               modpar[modem].squelchdcd = X2C_DIVR((float)cnum,400.0f);
            }
            else Error("need modem number -M before -d", 31ul);
         }
         else if (h[1U]=='e') {
            Lib_NextArg(h, 1024ul);
            if (modem>=0L) {
               if (!aprsstr_StrToInt(h, 1024ul, &inum)) err = 1;
               if (labs(inum)>999L) Error("equalizer -999..999", 20ul);
               modpar[modem].configequalizer = inum;
            }
            else {
               if (channel>=0L) {
                  Error("need modem number -M before -e", 31ul);
               }
               if (!aprsstr_StrToCard(h, 1024ul, &extraaudiodelay)) err = 1;
            }
         }
         else if (h[1U]=='f') {
            Lib_NextArg(h, 1024ul);
            if (!aprsstr_StrToCard(h, 1024ul, &cnum)) err = 1;
            if (modem>=0L) modpar[modem].configafskmid = cnum;
            else if (channel>=0L) {
               if (cnum>2UL) Error("duplex 0..2", 12ul);
               chan[channel].duplex = (unsigned char)cnum;
            }
            else {
               if (cnum<8000UL || cnum>96000UL) {
                  Error("sampelrate 8000..96000", 23ul);
               }
               adcrate = cnum;
            }
         }
         else if (h[1U]=='g') {
            if (modem>=0L) modpar[modem].scramb = 1;
            else if (channel>=0L) {
               Lib_NextArg(h, 1024ul);
               if (!aprsstr_StrToCard(h, 1024ul, &cnum) || cnum>999UL) {
                  Error("-g <ms>", 8ul);
               }
               chan[channel].gmqtime = (cnum*adcrate)/1000UL;
            }
            else Error("need modem number -M before -g", 31ul);
         }
         else if (h[1U]=='H') {
            Lib_NextArg(h, 1024ul);
            if (!aprsstr_StrToCard(h, 1024ul, &cnum)) err = 1;
            if (modem>=0L) {
               modpar[modem].afskhighpass = (float)cnum*0.01f;
            }
            else Error("need modem number -M before -H", 31ul);
         }
         else if (h[1U]=='i') {
            if (modem>=0L) modpar[modem].kissignore = 1;
            else Lib_NextArg(pipefn, 1024ul);
         }
         else if (h[1U]=='k') {
            Lib_NextArg(h, 1024ul);
            if (!aprsstr_StrToCard(h, 1024ul, &cnum)) err = 1;
            if (modem>=0L) {
               if (cnum>7UL) Error("kissports 0..7", 15ul);
               modpar[modem].port16 = cnum*16UL;
            }
            else kissbufs = cnum;
         }
         else if (h[1U]=='l') {
            Lib_NextArg(h, 1024ul);
            if (!aprsstr_StrToCard(h, 1024ul, &cnum)) err = 1;
            if (cnum>=16UL && cnum<=4096UL) adcbuflen = cnum;
            else Error("sound buffer out of range", 26ul);
         }
         else if (h[1U]=='M') {
            Lib_NextArg(h, 1024ul);
            if (!aprsstr_StrToCard(h, 1024ul, &cnum)) err = 1;
            if (cnum>7UL) Error(">maxmodems", 11ul);
            modem = (long)cnum;
            modpar[modem].configured = 1;
         }
         else if (h[1U]=='m') {
            Lib_NextArg(h, 1024ul);
            if (modem<0L && channel<0L) {
               while (h[0U]) {
                  ExtractWord(h1, 1024ul, h, 1024ul);
                  if (h1[0U]=='/') X2C_COPY(h1,1024u,mixfn,1024u);
                  else {
                     if (!aprsstr_StrToCard(h1, 1024ul, &cnum)) err = 1;
                     ExtractWord(h1, 1024ul, h, 1024ul);
                     if (!aprsstr_StrToCard(h1, 1024ul, &left)) err = 1;
                     ExtractWord(h1, 1024ul, h, 1024ul);
                     if (!aprsstr_StrToCard(h1, 1024ul, &right)) err = 1;
                     if (!err) SetMixer(mixfn, 1024ul, cnum, left, right);
                  }
               }
            }
            else {
               if (!aprsstr_StrToCard(h, 1024ul, &cnum)) err = 1;
               if (cnum>3UL) Error("monitor 0..3", 13ul);
               if (modem>=0L) modpar[modem].monitor = (unsigned char)cnum;
               else Error("need modem number -M before -m", 31ul);
            }
         }
         else if (h[1U]=='n') {
            Lib_NextArg(h, 1024ul);
            if (!aprsstr_StrToCard(h, 1024ul, &cnum)) err = 1;
            if (modem>=0L) modpar[modem].confignyquist = cnum;
            else Error("need modem number -M before -n", 31ul);
         }
         else if (h[1U]=='o') Lib_NextArg(soundfn, 1024ul);
         else if (h[1U]=='p') {
            if (modem>=0L) {
               Lib_NextArg(h, 1024ul);
               if (!aprsstr_StrToCard(h, 1024ul, &cnum)) err = 1;
               modpar[modem].pllshift = (long)(cnum*256UL);
            }
            else if (channel>=0L) {
               Lib_NextArg(h1, 1024ul);
               Lib_NextArg(h, 1024ul);
               if (!aprsstr_StrToInt(h, 1024ul, &inum)) err = 1;
               inum = labs(inum)+1L;
               if (h[0U]=='-') inum = -inum;
               chan[channel].hptt = pttinit((char *)h1, inum);
            }
         }
         else if (h[1U]=='q') {
            if (modem>=0L) {
               Lib_NextArg(h, 1024ul);
               if (!aprsstr_StrToCard(h, 1024ul, &cnum)) err = 1;
               modpar[modem].leveldcd = (long)cnum;
            }
            else Error("need modem number -M before -q", 31ul);
         }
         else if (h[1U]=='r') {
            Lib_NextArg(h, 1024ul);
            if (!aprsstr_StrToCard(h, 1024ul, &cnum)) err = 1;
            if (channel>=0L) {
               if (!aprsstr_StrToCard(h, 1024ul,
                &chan[channel].configpersist)) err = 1;
            }
            else Error("need channel number -C before -r", 33ul);
         }
         else if (h[1U]=='s') {
            Lib_NextArg(h, 1024ul);
            if (!aprsstr_StrToCard(h, 1024ul, &cnum)) err = 1;
            if (modem>=0L) modpar[modem].configafskshift = cnum;
            else fragmentsize = cnum;
         }
         else if (h[1U]=='t') {
            if (modem>=0L) {
               Lib_NextArg(h, 1024ul);
               if (!aprsstr_StrToCard(h, 1024ul, &cnum)) err = 1;
               modpar[modem].configtxdel = cnum;
            }
         }
         else if (h[1U]=='u') {
            if (channel>=0L) pttSetclaim(chan[channel].hptt, 1L);
         }
         else if (h[1U]=='U') {
            if (modem>=0L) {
               Lib_NextArg(h, 1024ul);
               { /* with */
                  struct MPAR * anonym1 = &modpar[modem];
                  if (GetIp(h, 1024ul, &anonym1->udpip, &anonym1->udpport,
                &anonym1->udpbind, &anonym1->udpsocket,
                &anonym1->checkip)<0L) Error("cannot open udp socket", 23ul);
               }
            }
            else Error("need modem number -M before -U", 31ul);
         }
         else if (h[1U]=='v') {
            if (modem>=0L) {
               Lib_NextArg(h, 1024ul);
               if (!aprsstr_StrToCard(h, 1024ul, &cnum)) err = 1;
               if (cnum>100UL) cnum = 100UL;
               modpar[modem].txvolum = (float)cnum*250.0f;
            }
            else Error("need modem number -M before -w", 31ul);
         }
         else if (h[1U]=='T') {
            if (modem>=0L) {
               Lib_NextArg(h, 1024ul);
               if (!aprsstr_StrToCard(h, 1024ul, &cnum)) err = 1;
               modpar[modem].timeout = cnum;
            }
            else Error("need modem number -M before -T", 31ul);
         }
         else if (h[1U]=='w') {
            if (modem>=0L) {
               Lib_NextArg(h, 1024ul);
               if (!aprsstr_StrToCard(h, 1024ul, &cnum)) err = 1;
               modpar[modem].txdelpattern = cnum;
            }
            else Error("need modem number -M before -w", 31ul);
         }
         else if (h[1U]=='x') {
            if (modem>=0L) {
               Lib_NextArg(h, 1024ul);
               if (!aprsstr_StrToCard(h, 1024ul, &cnum)) err = 1;
               modpar[modem].configtxtail = cnum;
            }
            else Error("need modem number -M before -x", 31ul);
         }
         else if (h[1U]=='z') {
            Lib_NextArg(h, 1024ul);
            if (!aprsstr_StrToCard(h, 1024ul, &cnum)) {
               err = 1;
            }
            if (modem>=0L) {
               modpar[modem].clamp = X2C_DIVR((float)cnum,1000.0f);
            }
            else Error("need modem number -M before -z", 31ul);
         }
         else {
            if (h[1U]=='h') {
               osi_WrStrLn("first for all modems", 21ul);
               osi_WrStrLn(" -a             abort on sounddevice error else r\
etry to open", 62ul);
               osi_WrStrLn(" -b <num>       tx dacbuffers (10) (more to avoid\
 underruns)", 61ul);
               osi_WrStrLn(" -c <num>       maxchannels (1) (1=mono, 2=stereo\
)", 51ul);
               osi_WrStrLn(" -D <filename>  (debug) write raw soundcard input\
 data to file or pipe", 71ul);
               osi_WrStrLn(" -e <num>       extra audio delay ptt hold time (\
1) in soundframes", 67ul);
               osi_WrStrLn(" -f <num>       adcrate (16000) (8000..96000)",
                46ul);
               osi_WrStrLn(" -h             help", 21ul);
               osi_WrStrLn(" -i <filename>  kiss pipename (/dev/kiss/soundmod\
em)", 53ul);
               osi_WrStrLn(" -k <num>       tx kiss bufs (60)", 34ul);
               osi_WrStrLn(" -l <num>       adcbuflen (256) more: lower syste\
m load but slower reaction", 76ul);
               osi_WrStrLn(" -m [<mixername>:]<channel>:<left>:<right> (0..25\
5) ossmixer (/dev/mixer)", 74ul);
               osi_WrStrLn(" -o <filename>  oss devicename (/dev/dsp)",
                42ul);
               osi_WrStrLn(" -s <num>       fragment size in 2^n (9)", 41ul);
               osi_WrLn();
               osi_WrStrLn("repeat for each channel -C ... :", 33ul);
               osi_WrStrLn("  -C <num>              (0..1) channel parameters\
 follow (repeat for each channel)", 83ul);
               pttHelp((char *)h1, 1024UL);
               osi_WrStrLn(h1, 1024ul);
               osi_WrStrLn("  -f <num>              fullduplex  (0) (0=halfdu\
plex, 1=master fullduplex, 2=all fullduplex)", 94ul);
               osi_WrStrLn("  -g <ms>               GM900 audio quiet time af\
ter ptt on (0)", 64ul);
               osi_WrStrLn("  -r <num>              max random wait time afte\
r dcd before start tx (ms) (800)", 82ul);
               osi_WrLn();
               osi_WrStrLn("repeat for each modem -M ... :", 31ul);
               osi_WrStrLn("   -M <num> (0..7) modem parameters follow (repea\
t for each modem)", 67ul);
               osi_WrStrLn("   -a       afsk off, fsk on (on)", 34ul);
               osi_WrStrLn("   -b <num> baud (1200) (1..32000)", 35ul);
               osi_WrStrLn("   -B <kbyte> send BERT, (negative bytes receive \
only)", 55ul);
               osi_WrStrLn("   -c <num> use stereo channel 0=left (or mono), \
1=right", 57ul);
               osi_WrStrLn("   -d <num> dcdlevel (56) (0..100)", 35ul);
               osi_WrStrLn("   -e <num> demod equalizer (0) 100=6db/oct highp\
ass   (-999..999)", 67ul);
               osi_WrStrLn("   -f <num> afsk mid frequency, tx and rx (hz) (1\
700)", 54ul);
               osi_WrStrLn("   -g       g3ruh scrambler on (off)", 37ul);
               osi_WrStrLn("   -H <num> afsk tx highpass (0) (0..100)",
                42ul);
               osi_WrStrLn("   -i       ignore modem parameters from kiss (of\
f)", 52ul);
               osi_WrStrLn("   -k <num> received data send to this kiss port \
(0) (0..7)", 60ul);
               /*
                       WrStrLn("#  -l <num> filterlow (hz)");
               */
               osi_WrStrLn("   -m <num> monitor (2) (0=off, 1=header, 2=all, \
3=passall)", 60ul);
               osi_WrStrLn("   -n <num> nyquist filter baseband -6db point in\
 % of baudrate (65) (0..100)", 78ul);
               osi_WrStrLn("   -p <num> receive clock pll aquisition speed (1\
6) (num/256 of bit time)", 74ul);
               osi_WrStrLn("   -q <num> quiet adc level to save cpu or avoid \
reciption of channel crosstalk (0)", 84ul);
               /*
                       WrStrLn("#  -r       rzi -1 1");
               */
               osi_WrStrLn("   -s <num> afsk shift tx (for rx bandfilter widt\
h) (1000) (hz)", 64ul);
               osi_WrStrLn("   -t <num> txdelay (300) (ms)", 31ul);
               osi_WrStrLn("   -T <seconds> timeout for tx buffered frame (60\
) (s)", 55ul);
               /*
                       WrStrLn("#  -u       filterup  (hz)");
               */
               osi_WrStrLn("   -U <[x.x.x.x]:destport:listenport> use axudp i\
nstead of kiss /listenport check ip", 85ul);
               osi_WrStrLn("   -v <num> tx loudness (100)", 30ul);
               osi_WrStrLn("   -w <num> txdelay pattern before 1 flag (126) (\
0..255)", 57ul);
               osi_WrStrLn("   -x <num> txtail (20) (ms), ptt hold for fulldu\
plex", 54ul);
               osi_WrStrLn("   -z <num> fsk rx baseband dc regeneration clamp\
 speed (996) (0=off, 1=fast, 999=slow)", 88ul);
               osi_WrStrLn("example: ./afskmodem -f 22050 -c 2 -C 0 -p /dev/t\
tyS0 0 -M 0 -c 0 -b 1200 -M 1 -c 1 -b 9600 -a -g -U 127.0.0.1:6001:1093",
                121ul);
               X2C_ABORT();
            }
            err = 1;
         }
      }
      else {
         /*
               h[0]:=0C;
         */
         err = 1;
      }
      if (err) break;
   }
   if (err) {
      InOut_WriteString(">", 2ul);
      InOut_WriteString(h, 1024ul);
      osi_WrStrLn("< use -h", 9ul);
      X2C_ABORT();
   }
   Config();
   OpenSound();
   pipefd = -1L;
   if (pipefn[0U]) pipefd = Opentty(pipefn, 1024ul);
   Makekissbufs(kissbufs);
   bertstart();
} /* end Parms() */


static void AppCRC(char frame[], unsigned long frame_len, long size)
{
   unsigned char h;
   unsigned char l;
   unsigned char b;
   long i0;
   long tmp;
   l = 0U;
   h = 0U;
   tmp = size-1L;
   i0 = 0L;
   if (i0<=tmp) for (;; i0++) {
      b = (unsigned char)((unsigned char)(unsigned char)frame[i0]^l);
      l = CRCL[b]^h;
      h = CRCH[b];
      if (i0==tmp) break;
   } /* end for */
   frame[size] = (char)l;
   frame[size+1L] = (char)h;
} /* end AppCRC() */


static void Kisscmd(void)
{
   unsigned long x;
   unsigned long cmd;
   long modem;
   struct MPAR * anonym;
   cmd = (unsigned long)(unsigned char)pGetKiss->port;
   modem = (long)(cmd>>4&7UL);
   cmd = cmd&15UL;
   InOut_WriteString("p=", 3ul);
   InOut_WriteInt(modem, 1UL);
   osi_WrLn();
   { /* with */
      struct MPAR * anonym = &modpar[modem];
      if (anonym->configured && !anonym->kissignore) {
         x = (unsigned long)(unsigned char)pGetKiss->data[0U];
         if (cmd==1UL) {
            anonym->configtxdel = x*10UL;
            InOut_WriteString("txdel=", 7ul);
            InOut_WriteInt((long)anonym->configtxdel, 1UL);
            osi_WrLn();
         }
         else if (cmd==2UL) {
            chan[modpar[modem].ch].configpersist = 10UL*(255UL-x);
            InOut_WriteString("persist=", 9ul);
            InOut_WriteInt((long)chan[modpar[modem].ch].configpersist, 1UL);
            osi_WrLn();
         }
         else if (cmd==4UL) {
            /*3 SlotTime*/
            anonym->configtxtail = x*10UL;
            InOut_WriteString("txtail=", 8ul);
            InOut_WriteInt((long)anonym->configtxtail, 1UL);
            osi_WrLn();
         }
         else if (cmd==5UL) {
            if (x<=2UL) chan[modpar[modem].ch].duplex = (unsigned char)x;
            InOut_WriteString("duplex=", 8ul);
            InOut_WriteInt((long)x, 1UL);
            osi_WrLn();
         }
         Config();
      }
   }
} /* end Kisscmd() */


static void getkiss(void)
{
   char b[1024];
   long i0;
   long l;
   long m;
   long tmp;
   for (;;) {
      if (pipefd<0L) break;
      l = read(pipefd, (char *)b, 1024UL);
      if (l<=0L) break;
      tmp = l-1L;
      i0 = 0L;
      if (i0<=tmp) for (;; i0++) {
         /*allocate buffer*/
         if (pGetKiss==0) {
            if (pTxFree==0) goto loop_exit;
            pGetKiss = pTxFree;
            pTxFree = pTxFree->next;
         }
         /*allocate buffer*/
         if (b[i0]=='\300') {
            esc = 0;
            if (getst>2UL) {
               pGetKiss->len = getst-2UL;
               getst = 0UL;
               /*store frame*/
               m = (long)((unsigned long)(unsigned char)
                pGetKiss->port>>4&7UL);
               if (m<=7L) {
                  if (((unsigned long)(unsigned char)pGetKiss->port&15UL)
                ==0UL) {
                     /* data frame */
                     pGetKiss->port = (char)m;
                     AppCRC(pGetKiss->data, 341ul, (long)pGetKiss->len);
                     pGetKiss->time0 = systime+modpar[m].timeout;
                     StoBuf(m, pGetKiss);
                     pGetKiss = 0;
                  }
                  else Kisscmd();
               }
            }
            else {
               /*store frame*/
               getst = 1UL;
            }
         }
         else if (b[i0]=='\333' && getst>0UL) esc = 1;
         else {
            if (esc) {
               if (b[i0]=='\335') b[i0] = '\333';
               else if (b[i0]=='\334') b[i0] = '\300';
               esc = 0;
            }
            if (getst==1UL) {
               pGetKiss->port = b[i0];
               getst = 2UL;
            }
            else if (getst>=2UL && getst-2UL<339UL) {
               pGetKiss->data[getst-2UL] = b[i0];
               ++getst;
            }
         }
         if (i0==tmp) break;
      } /* end for */
   }
   loop_exit:;
} /* end getkiss() */


static void getudp(void)
{
   pKISSNEXT p;
   unsigned long i0;
   long ulen;
   unsigned long fromport;
   unsigned long fromip;
   char crc2;
   char crc1;
   struct MPAR * anonym;
   for (i0 = 0UL; i0<=7UL; i0++) {
      { /* with */
         struct MPAR * anonym = &modpar[i0];
         if (anonym->udpsocket>=0L && pTxFree) {
            ulen = udpreceive(anonym->udpsocket, pTxFree->data, 341L,
                &fromport, &fromip);
            if ((ulen>2L && ulen<341L)
                && (!anonym->checkip || fromip==anonym->udpip)) {
               crc1 = pTxFree->data[ulen-2L];
               crc2 = pTxFree->data[ulen-1L];
               AppCRC(pTxFree->data, 341ul, ulen-2L);
               if (crc1==pTxFree->data[ulen-2L]
                && crc2==pTxFree->data[ulen-1L]) {
                  p = pTxFree;
                  pTxFree = pTxFree->next;
                  p->port = (char)i0;
                  p->len = (unsigned long)(ulen-2L);
                  p->time0 = systime+anonym->timeout;
                  StoBuf((long)i0, p);
               }
            }
         }
      }
   } /* end for */
/* else crc error */
/* else wrong len or source ip */
} /* end getudp() */


static void sendkiss(char data[], unsigned long data_len, long len,
                unsigned long port16)
{
   char b[683];
   long i0;
   long l;
   char d;
   unsigned long po;
   struct MPAR * anonym;
   long tmp;
   /*
   FOR i:=0 TO len-1 DO IO.WrHex(ORD(data[i]),3) END; IO.WrLn; 
   */
   po = port16>>4&7UL;
   if (po<=7UL && modpar[po].udpsocket>=0L) {
      { /* with */
         struct MPAR * anonym = &modpar[po];
         i0 = udpsend(anonym->udpsocket, data, len+2L, anonym->udpport,
                anonym->udpip);
      }
   }
   else if (pipefd>=0L) {
      b[0U] = '\300';
      b[1U] = (char)port16;
      l = 2L;
      tmp = len-1L;
      i0 = 0L;
      if (i0<=tmp) for (;; i0++) {
         d = data[i0];
         if (d=='\300') {
            b[l] = '\333';
            ++l;
            b[l] = '\334';
         }
         else if (d=='\333') {
            b[l] = '\333';
            ++l;
            b[l] = '\335';
         }
         else b[l] = d;
         ++l;
         if (i0==tmp) break;
      } /* end for */
      b[l] = '\300';
      ++l;
      i0 = write(pipefd, (char *)b, (unsigned long)l);
   }
} /* end sendkiss() */


static void WrQuali(float q)
{
   if (q>0.0f) {
      q = 100.5f-q*200.0f;
      if (q<0.0f) q = 0.0f;
      InOut_WriteString(" q:", 4ul);
      InOut_WriteInt((long)(unsigned long)X2C_TRUNCC(q,0UL,X2C_max_longcard),
                 1UL);
   }
} /* end WrQuali() */


static void WrdB(long volt)
{
   if (volt>0L) {
      osi_WrFixed(RealMath_ln((float)volt)*8.685889638f-96.4f, 1L, 6UL);
      InOut_WriteString("dB", 3ul);
   }
} /* end WrdB() */


static void WCh(char c)
{
   if (c!='\015') {
      if ((unsigned char)c<' ' || (unsigned char)c>='\177') {
         InOut_WriteString(".", 2ul);
      }
      else InOut_WriteString((char *) &c, 1u/1u);
   }
} /* end WCh() */


static void ShowCall(char f[], unsigned long f_len, unsigned long pos)
{
   unsigned long e;
   unsigned long i0;
   unsigned long tmp;
   e = pos;
   tmp = pos+5UL;
   i0 = pos;
   if (i0<=tmp) for (;; i0++) {
      if (f[i0]!='@') e = i0;
      if (i0==tmp) break;
   } /* end for */
   tmp = e;
   i0 = pos;
   if (i0<=tmp) for (;; i0++) {
      WCh((char)((unsigned long)(unsigned char)f[i0]>>1));
      if (i0==tmp) break;
   } /* end for */
   if ((unsigned long)(unsigned char)f[pos+6UL]>>1&15UL) {
      InOut_WriteString("-", 2ul);
      InOut_WriteInt((long)((unsigned long)(unsigned char)f[pos+6UL]>>1&15UL)
                , 1UL);
   }
} /* end ShowCall() */

static unsigned long afskmodem_UA = 0x63UL;

static unsigned long afskmodem_DM = 0xFUL;

static unsigned long afskmodem_SABM = 0x2FUL;

static unsigned long afskmodem_DISC = 0x43UL;

static unsigned long afskmodem_FRMR = 0x87UL;

static unsigned long afskmodem_UI = 0x3UL;

static unsigned long afskmodem_RR = 0x1UL;

static unsigned long afskmodem_REJ = 0x9UL;

static unsigned long afskmodem_RNR = 0x5UL;


static void Showctl(unsigned long com, unsigned long cmd)
{
   unsigned long cm;
   char PF[4];
   char tmp;
   InOut_WriteString(" ctl ", 6ul);
   cm = (unsigned long)cmd&~0x10UL;
   if ((cm&0xFUL)==0x1UL) {
      InOut_WriteString("RR", 3ul);
      InOut_WriteString((char *)(tmp = (char)(48UL+(cmd>>5)),&tmp), 1u/1u);
   }
   else if ((cm&0xFUL)==0x5UL) {
      InOut_WriteString("RNR", 4ul);
      InOut_WriteString((char *)(tmp = (char)(48UL+(cmd>>5)),&tmp), 1u/1u);
   }
   else if ((cm&0xFUL)==0x9UL) {
      InOut_WriteString("REJ", 4ul);
      InOut_WriteString((char *)(tmp = (char)(48UL+(cmd>>5)),&tmp), 1u/1u);
   }
   else if ((cm&0x1UL)==0UL) {
      InOut_WriteString("I", 2ul);
      InOut_WriteString((char *)(tmp = (char)(48UL+(cmd>>5)),&tmp), 1u/1u);
      InOut_WriteString((char *)(tmp = (char)(48UL+(cmd>>1&7UL)),&tmp),
                1u/1u);
   }
   else if (cm==0x3UL) InOut_WriteString("UI", 3ul);
   else if (cm==0xFUL) InOut_WriteString("DM", 3ul);
   else if (cm==0x2FUL) InOut_WriteString("SABM", 5ul);
   else if (cm==0x43UL) InOut_WriteString("DISC", 5ul);
   else if (cm==0x63UL) InOut_WriteString("UA", 3ul);
   else if (cm==0x87UL) InOut_WriteString("FRMR", 5ul);
   else osi_WrHex(cmd, 1UL);
   strncpy(PF,"v^-+",4u);
   if (com==0UL || com==3UL) InOut_WriteString("v1", 3ul);
   else {
      InOut_WriteString((char *) &PF[(com&1UL)+2UL*(unsigned long)
                ((0x10UL & (unsigned long)cmd)!=0)], 1u/1u);
   }
} /* end Showctl() */


static void ShowFrame(char f[], unsigned long f_len, unsigned long len,
                unsigned long port, float q, long volt, char noinfo)
{
   unsigned long i0;
   char d;
   char v;
   char tmp;
   i0 = 0UL;
   while (!((unsigned long)(unsigned char)f[i0]&1)) {
      ++i0;
      if (i0>len) return;
   }
   /* no address end mark found */
   if (i0%7UL!=6UL) return;
   /* address end not modulo 7 error */
   InOut_WriteString((char *)(tmp = (char)((port&7UL)+48UL),&tmp), 1u/1u);
   InOut_WriteString(":fm ", 5ul);
   ShowCall(f, f_len, 7UL);
   InOut_WriteString(" to ", 5ul);
   ShowCall(f, f_len, 0UL);
   i0 = 14UL;
   v = 1;
   while (i0+6UL<len && !((unsigned long)(unsigned char)f[i0-1UL]&1)) {
      if (v) {
         InOut_WriteString(" via", 5ul);
         v = 0;
      }
      InOut_WriteString(" ", 2ul);
      ShowCall(f, f_len, i0);
      if ((unsigned long)(unsigned char)f[i0+6UL]>=128UL && (((unsigned long)
                (unsigned char)f[i0+6UL]&1) || (unsigned long)(unsigned char)
                f[i0+13UL]<128UL)) InOut_WriteString("*", 2ul);
      i0 += 7UL;
   }
   /*
     IO.WrStr(" ctl "); IO.WrHex(ORD(f[i]),1);
   */
   Showctl((unsigned long)((0x80U & (unsigned char)(unsigned char)f[6UL])!=0)
                +2UL*(unsigned long)((0x80U & (unsigned char)(unsigned char)
                f[13UL])!=0), (unsigned long)(unsigned char)f[i0]);
   ++i0;
   if (i0<len) {
      InOut_WriteString(" pid ", 6ul);
      osi_WrHex((unsigned long)(unsigned char)f[i0], 1UL);
   }
   ++i0;
   WrQuali(q);
   WrdB(volt);
   /*
   IO.WrCard(bufree(), 3);
   */
   osi_WrLn();
   if (!noinfo) {
      d = 0;
      while (i0<len) {
         if (f[i0]!='\015') {
            WCh(f[i0]);
            d = 1;
         }
         else if (d) {
            osi_WrLn();
            d = 0;
         }
         ++i0;
      }
      if (d) osi_WrLn();
   }
/*
FOR i:=0 TO len-1 DO WrStr("\"); WrInt(ASH(ORD(f[i]), -6),1);
 IO.WrCard(ORD(f[i]) DIV 8 MOD 8,1);IO.WrCard(ORD(f[i]) MOD 8,1);
END;
IO.WrLn;
*/
} /* end ShowFrame() */

#define afskmodem_POLINOM 0x8408 


static void Gencrctab(void)
{
   unsigned long c;
   unsigned long crc;
   unsigned long i0;
   for (c = 0UL; c<=255UL; c++) {
      crc = 255UL-c;
      for (i0 = 0UL; i0<=7UL; i0++) {
         if ((crc&1)) {
            crc = (unsigned long)((unsigned long)(crc>>1)^0x8408UL);
         }
         else crc = crc>>1;
      } /* end for */
      CRCL[c] = (unsigned char)crc;
      CRCH[c] = (unsigned char)(255UL-(crc>>8));
   } /* end for */
} /* end Gencrctab() */


static float Fir(unsigned long in, unsigned long sub, unsigned long step,
                float fir[], unsigned long fir_len, float firtab[],
                unsigned long firtab_len)
{
   float s;
   unsigned long i0;
   s = 0.0f;
   i0 = sub;
   do {
      s = s+fir[in]*firtab[i0];
      ++in;
      if (in>fir_len-1) in = 0UL;
      i0 += step;
   } while (i0<=firtab_len-1);
   return s;
} /* end Fir() */


static float noiselevel(unsigned long m)
/* 0.0 perfect, ~0.25 noise only*/
{
   struct MPAR * anonym;
   { /* with */
      struct MPAR * anonym = &modpar[m];
      if (anonym->sqmed[1]==anonym->sqmed[0]) return 0.0f;
      else return X2C_DIVR(anonym->noise,anonym->sqmed[1]-anonym->sqmed[0]);
   }
   return 0;
} /* end noiselevel() */


static void demodbit(long m, char d)
{
   char xor;
   struct MPAR * anonym;
   { /* with */
      struct MPAR * anonym = &modpar[m];
      /*descrambler*/
      if (anonym->scramb) {
         /*
               scrambler:=CAST(BITSET, CAST(CARDINAL, scrambler)*2 + ORD(d));
                 
         */
         anonym->scrambler = X2C_LSH(anonym->scrambler,32,1);
         if (d) anonym->scrambler |= 0x1UL;
         d = ((0x1UL & anonym->scrambler)!=0)
                ==(((0x1000UL & anonym->scrambler)!=0)
                ==((0x20000UL & anonym->scrambler)!=0));
                /*result is xor bit 0 12 17*/
      }
      /*nrzi*/
      xor = d;
      d = d==anonym->data1;
      anonym->data1 = xor;
      /*
      IO.WrCard(ORD(d),1);
      */
      /*bert*/
      if (anonym->bert) {
         if (!d) ++anonym->berterr;
         ++anonym->bertc;
         if (anonym->bert<0L) {
            /* receive only */
            ++anonym->bert;
            if (anonym->bert==0L) osi_WrStrLn("---- end BERT", 14ul);
         }
         if (anonym->bertc>2000UL) {
            InOut_WriteInt((long)anonym->berterr, 4UL);
            InOut_WriteInt(m, 2UL);
            WrQuali(noiselevel((unsigned long)m));
            WrdB(chan[anonym->ch].adcmax);
            osi_WrLn();
            anonym->bertc = 0UL;
            anonym->berterr = 0UL;
         }
      }
      if (anonym->rxstuffc<5UL) {
         /*bit to byte*/
         anonym->rxbyte = anonym->rxbyte/2UL+((unsigned long)d<<7);
         ++anonym->rxbitc;
         /*crc*/
         xor = d!=((0x1UL & anonym->rxcrc)!=0); /*databit xor crcbit0*/
         /* 
               rxcrc:=CAST(BITSET, CAST(CARDINAL, rxcrc) DIV 2);
                (*shift right crc register*)
         */
         anonym->rxcrc = X2C_LSH(anonym->rxcrc,32,-1);
         if (xor) anonym->rxcrc = anonym->rxcrc^0x8408UL;
         /*byte to frame*/
         if (anonym->rxbitc>=8UL) {
            /*
            IO.WrStr(" <");IO.WrHex(rxbyte, 1);IO.WrStr("> ");
            */
            if (anonym->rxp<339UL) {
               anonym->rxbuf[anonym->rxp] = (char)anonym->rxbyte;
               ++anonym->rxp;
            }
            /*else frame too long error*/
            anonym->rxbitc = 0UL;
         }
      }
      else if (anonym->rxstuffc>5UL) {
         /*flag*/
         /*flag*/
         if ((!d && anonym->rxbitc==6UL) && anonym->rxp>=16UL) {
            /*0111111x 0 is flag else abort*/
            /*bits modulo 8 ?*/
            /*frame long enough ?*/
            if (anonym->rxcrc==0x9F0BUL) {
               sendkiss(anonym->rxbuf, 339ul, (long)(anonym->rxp-2UL),
                anonym->port16);
            }
            if (anonym->monitor==afskmodem_passall || anonym->rxcrc==0x9F0BUL && anonym->monitor)
                 {
               ShowFrame(anonym->rxbuf, 339ul, anonym->rxp-2UL,
                anonym->port16>>4, noiselevel((unsigned long)m),
                chan[anonym->ch].adcmax, anonym->monitor==afskmodem_noinfo);
            }
         }
         anonym->rxp = 0UL;
         anonym->rxbitc = 0UL;
         anonym->rxcrc = 0xFFFFUL; /*init crc register*/
      }
      if (d) {
         ++anonym->rxstuffc;
      }
      else anonym->rxstuffc = 0UL;
   }
/*destuffing*/
} /* end demodbit() */


static void demod(float u, long m)
{
   char d;
   struct MPAR * anonym;
   d = u>=0.0f;
   { /* with */
      struct MPAR * anonym = &modpar[m];
      if (anonym->cbit) {
         demodbit(m, d);
         if (d!=anonym->oldd) {
            if (d==anonym->plld) anonym->baudfine += anonym->pllshift;
            else anonym->baudfine -= anonym->pllshift;
            anonym->oldd = d;
         }
         /*squelch*/
         anonym->sqmed[d] = anonym->sqmed[d]+(u-anonym->sqmed[d])*0.05f;
         anonym->noise = anonym->noise+((float)fabs(u-anonym->sqmed[d])
                -anonym->noise)*0.05f;
      }
      else {
         /*squelch*/
         anonym->plld = d;
      }
      anonym->cbit = !anonym->cbit;
   }
} /* end demod() */

/*
PROCEDURE Afsk(m:INTEGER);
VAR right, mid, d, a, b, ff:REAL;
BEGIN
  WITH modpar[m] DO
    right:=Fir(afin, 0, AOVERSAMP, chan[ch].afir, afirtab);
(*
tii:=VAL(INTEGER, right/3.0); 
FIO.WrBin(tfd, tii, 2);
*)
    IF (left<0.0)<>(right<0.0) THEN
      d:=left/(left-right);
      a:=FLOAT(TRUNC(d*FLOAT(AOVERSAMP)+0.5));
      b:=a * (1.0/FLOAT(AOVERSAMP));
      IF (TRUNC(a)>0) & (TRUNC(a)<AOVERSAMP) THEN
        mid:=Fir(afin, AOVERSAMP-TRUNC(a), AOVERSAMP, chan[ch].afir,
                afirtab);
        IF (left<0.0)<>(mid<0.0) THEN d:=left/(left-mid)*b;
        ELSE d:=b + mid/(mid-right)*(1.0-b) END; 
      END;
(*
IO.WrStr(" left=");IO.WrFixed(left, 0,1);
IO.WrStr(" mid="); IO.WrFixed(mid, 0,1);
IO.WrStr(" right=");IO.WrFixed(right, 0,1);
IO.WrStr(" d=");IO.WrFixed(d, 2,1);
IO.WrStr(" a=");IO.WrFixed(a, 2,1);
IO.WrLn;
*)

(*
mid:=Fira(afin, AOVERSAMP DIV 2)-left;   
right:=right-left;
b:=right*0.5 - mid;
a:=(right*0.25 - mid)/b;
d:=sqrt(a*a - left/b);
IF a>=0.0 THEN d:=-d END;
d:=(d+a)*0.5;
*)
      IF tcnt+d<>0.0 THEN freq:=1.0/(tcnt+d) END;
      tcnt:=0.0-d;
    END;

    tcnt:=tcnt+1.0;
    left:=right;
    dfir[dfin]:=freq - afskmidfreq;   
    dfin:=(dfin+1) MOD DFIRLEN;
    INC(baudfine, demodbaud);
    IF baudfine>=BAUDSAMP THEN
      DEC(baudfine, BAUDSAMP);  
      ff:=Fir(dfin, DOVERSAMP - baudfine DIV (BAUDSAMP DIV DOVERSAMP),
                DOVERSAMP, dfir, dfirtab);
      demod(ff, m);  
    END;
  END;
END Afsk;
*/

static void Afsk(long m)
{
   float ff;
   float b;
   float a;
   float d;
   float mid;
   float right;
   struct MPAR * anonym;
   { /* with */
      struct MPAR * anonym = &modpar[m];
      right = Fir(afin, 0UL, 16UL, chan[anonym->ch].afir, 32ul,
                anonym->afirtab, 512ul);
      if (anonym->left<0.0f!=right<0.0f) {
         d = X2C_DIVR(anonym->left,anonym->left-right);
         a = (float)(unsigned long)X2C_TRUNCC(d*16.0f+0.5f,0UL,
                X2C_max_longcard);
         b = a*0.0625f;
         if ((unsigned long)X2C_TRUNCC(a,0UL,
                X2C_max_longcard)>0UL && (unsigned long)X2C_TRUNCC(a,0UL,
                X2C_max_longcard)<16UL) {
            mid = Fir(afin, 16UL-(unsigned long)X2C_TRUNCC(a,0UL,
                X2C_max_longcard), 16UL, chan[anonym->ch].afir, 32ul,
                anonym->afirtab, 512ul);
            if (anonym->left<0.0f!=mid<0.0f) {
               d = (X2C_DIVR(anonym->left,anonym->left-mid))*b;
            }
            else d = b+(X2C_DIVR(mid,mid-right))*(1.0f-b);
         }
         if (anonym->tcnt+d!=0.0f) {
            anonym->freq = X2C_DIVR(1.0f,anonym->tcnt+d);
         }
         anonym->tcnt = 0.0f-d;
      }
      anonym->tcnt = anonym->tcnt+1.0f;
      anonym->left = right;
      anonym->dfir[anonym->dfin] = anonym->freq-anonym->afskmidfreq;
      anonym->dfin = anonym->dfin+1UL&63UL;
      anonym->baudfine += (long)anonym->demodbaud;
      if (anonym->baudfine>=65536L) {
         anonym->baudfine -= 65536L;
         if (anonym->baudfine<65536L) {
            /* normal alway true */
            ff = Fir(anonym->dfin,
                (unsigned long)(16L-X2C_DIV(anonym->baudfine,4096L)), 16UL,
                anonym->dfir, 64ul, anonym->dfirtab, 1024ul);
            demod(ff, m);
         }
      }
   }
} /* end Afsk() */


static void Fsk(long m)
{
   float ff;
   long lim;
   struct MPAR * anonym;
   { /* with */
      struct MPAR * anonym = &modpar[m];
      lim = (long)anonym->demodbaud;
      for (;;) {
         if (anonym->baudfine>=65536L) {
            anonym->baudfine -= 65536L;
            ff = Fir(afin, (unsigned long)X2C_DIV(anonym->baudfine&65535L,
                4096L), 16UL, chan[anonym->ch].afir, 32ul, anonym->afirtab,
                512ul);
            demod(ff-(anonym->left+anonym->freq)*0.5f, m);
            /*clamp*/
            if (!anonym->cbit) {
               if (ff>anonym->left) {
                  anonym->left = anonym->left+(ff-anonym->left)*1.0f;
               }
               if (ff<anonym->freq) {
                  anonym->freq = anonym->freq+(ff-anonym->freq)*1.0f;
               }
               anonym->left = anonym->left*anonym->clamp;
               anonym->freq = anonym->freq*anonym->clamp;
            }
         }
         /*clamp*/
         anonym->baudfine += lim;
         lim = 0L;
         if (anonym->baudfine<131072L) break;
      }
   }
} /* end Fsk() */

/*
(*pp065*)
PROCEDURE setppout(fd:INTEGER; mask:SET32):BOOLEAN;
CONST PORTCFG=40026206H;

VAR pc:SET32;
    r:CARDINAL;
BEGIN
  r:=100;
  LOOP
    pc:=SET32{30};
    IF ioctl(fd, CAST(INTEGER, PORTCFG), ADR(pc))=0 THEN EXIT END;
    DEC(r);
    IF r=0 THEN pc:=SET32{}; EXIT END;
  END;
  pc:=pc*SET32{0..15}+SET32{31}+mask+SHIFT(mask,1);
  RETURN ioctl(fd, CAST(INTEGER, PORTCFG), ADR(pc))=0;
END setppout;

PROCEDURE ppbitset(fd:INTEGER; port:CARDINAL; on:BOOLEAN);
CONST PARWR=40026207H;
VAR res:INTEGER;
BEGIN
--IO.WrInt(port, 3); IO.WrInt(ORD(on) , 3); IO.WrStrLn("ppout");
  res:=ioctl(fd, CAST(INTEGER, PARWR), CAST(ADDRESS,SET32{port+ORD(NOT on)*8}
                ));
END ppbitset;
(*pp065*)



PROCEDURE Ptt;
VAR lptfd, b:INTEGER;
    map, mask:SET8;
    c:CHANNELS;
    ppmask:SET32;
BEGIN
--WrInt(ORD(chan[LEFT].pttstate),2); WrInt(ORD(chan[RIGHT].pttstate),1);
                WrStrLn("=ptts");
(*tty*)
  IF ttyfn[0]<>0C THEN
(*
    IF ttyfd<0 THEN ttyfd:=FIO.OpenMode(ttyfn, FIO.oRDWR+FIO.oNONBLOCK) END;
*)
    IF ttyfd<0 THEN ttyfd:=open(ttyfn, 0) END;
    IF ttyfd>=0 THEN

      IF NOT SetStatLine(ttyfd, chan[LEFT].pttstate, chan[RIGHT].pttstate)
      OR closetty THEN
        Close(ttyfd);
        ttyfd:=-1;
      END;                                      (* else keep open for alzheimer usb devices *)
    ELSE WrStrLn("tty port open error") END;
  END;
(*tty*)

  IF parporttyp=1 THEN
(*parport*)
    mask:=SET8{};
    map:=SET8{};
    FOR c:=LEFT TO RIGHT DO 
      b:=chan[c].lptpttbit;
      IF b<>0 THEN
        IF chan[c].pttstate=(b>=0) THEN INCL(map, ABS(b)-1) END;
        INCL(mask, ABS(b)-1);
      END;
    END; 
      IF mask<>SET8{} THEN
      lptfd:=open(lptfn, oRDWR);
      IF (lptfd<0) OR (lpt.ppclaim(lptfd)<0)
                THEN WrStrLn("lpt port open error");
      ELSE 
        map:=(CAST(SET8, lpt.ppdevrdata(lptfd))-mask) + map;
        lpt.ppdevdata(lptfd, CAST(CHAR, map));
      END;
      IF (lptfd>=0) THEN Close(lptfd) END;
    END;
(*parport*)

  ELSIF parporttyp=2 THEN
(*pp065*)
    lptfd:=open(lptfn, oRDWR);
    IF lptfd>=0 THEN 
      ppmask:=SET32{};
      FOR c:=LEFT TO RIGHT DO
        b:=chan[c].lptpttbit;
        IF b<>0 THEN INCL(ppmask, (ABS(b)-1)*2) END;
      END;
      IF setppout(lptfd, ppmask) THEN
        FOR c:=LEFT TO RIGHT DO
          b:=chan[c].lptpttbit;
          IF b<>0 THEN
            ppbitset(lptfd, ABS(b)-1, chan[c].pttstate=(b>=0));
          END;
        END;
      END;
      Close(lptfd);
    ELSE WrStr(lptfn); WrStrLn(" port open error") END;
  END; 
(*pp065*)


END Ptt;
*/
/* (usb) soundcard died */

static void repairsound(void)
{
   /*  pttok:=FALSE; */
   ptt(chan[afskmodem_LEFT].hptt, -1L);
   ptt(chan[afskmodem_RIGHT].hptt, -1L);
   if (abortonsounderr) Error("Sounddevice Failure", 20ul);
   osi_Close(soundfd);
   Usleep(100000UL);
   /*WrStrLn("openA"); */
   OpenSound();
} /* end repairsound() */


static void getadc(void)
{
   short buf[4096];
   long minr;
   long maxr;
   long minl;
   long maxl;
   long sl;
   long m;
   long i0;
   long l;
   unsigned char c;
   struct MPAR * anonym;
   l = read(soundfd, (char *)buf, adcbuflen*adcbytes);
   for (m = 0L; m<=7L; m++) {
      modpar[m].noise = modpar[m].noise*0.99f; /* clear dcd on silence */
   } /* end for */
   if (l<0L) {
      repairsound();
      return;
   }
   if (debfd>=0L) i0 = write(debfd, (char *)buf, (unsigned long)l);
   l = (long)((unsigned long)l/adcbytes);
   for (c = afskmodem_LEFT;; c++) {
      chan[c].adcmax = chan[c].adcmax*15L>>4;
      if (c==afskmodem_RIGHT) break;
   } /* end for */
   maxl = -32768L;
   maxr = -32768L;
   minl = 32767L;
   minr = 32767L;
   i0 = 0L;
   while (i0<l) {
      sl = (long)buf[i0];
      chan[afskmodem_LEFT].afir[afin] = (float)sl;
      if (sl>maxl) maxl = sl;
      if (sl<minl) minl = sl;
      if (maxl-minl>chan[afskmodem_LEFT].adcmax) {
         chan[afskmodem_LEFT].adcmax = maxl-minl;
      }
      if (maxchannels>afskmodem_LEFT) {
         sl = (long)buf[i0+1L];
         chan[afskmodem_RIGHT].afir[afin] = (float)sl;
         if (sl>maxr) maxr = sl;
         if (sl<minr) minr = sl;
         if (maxr-minr>chan[afskmodem_RIGHT].adcmax) {
            chan[afskmodem_RIGHT].adcmax = maxr-minr;
         }
      }
      afin = afin+1UL&31UL;
      for (m = 0L; m<=7L; m++) {
         { /* with */
            struct MPAR * anonym = &modpar[m];
            if ((anonym->configured && chan[anonym->ch]
                .adcmax>=anonym->leveldcd)
                && (!chan[anonym->ch].pttstate || chan[anonym->ch].duplex)) {
               /* save cpu and echo reception */
               if (anonym->afsk) Afsk(m);
               else Fsk(m);
               /*
               IF (m=0) & (clock MOD 64=0)
                THEN IO.WrInt(chan[modpar[m].ch].adcmax,6); IO.WrLn; END;
               */
               if (noiselevel((unsigned long)m)<anonym->squelchdcd) {
                  chan[anonym->ch].dcdclock = clock0; /* tx wise dcd */
                  anonym->dcdclockm = clock0;
                /* modem wise dcd for shift digi*/
               }
            }
            else anonym->rxp = 0UL;
         }
      } /* end for */
      i0 += (long)((unsigned long)maxchannels+1UL);
   }
} /* end getadc() */


static void txmon(pKISSNEXT pf)
{
   if (pf && modpar[(unsigned char)pf->port].monitor) {
      ShowFrame(pf->data, 341ul, pf->len,
                (unsigned long)(unsigned char)pf->port, 0.0f, 0L,
                modpar[(unsigned char)pf->port].monitor==afskmodem_noinfo);
   }
} /* end txmon() */


static float Phasemod(float * uc, float u, float hp)
{
   float ud;
   ud = u-*uc;
   *uc = *uc+ud*hp;
   return ud;
} /* end Phasemod() */


static void Free(pKISSNEXT * tb)
{
   pKISSNEXT ph;
   if (*tb) {
      ph = (*tb)->next;
      (*tb)->next = pTxFree;
      pTxFree = *tb;
      *tb = ph;
   }
} /* end Free() */


static char frames2tx(long modem)
{
   if (modpar[modem].txbufin==0) return 0;
   if (modpar[modem].txbufin->time0>systime) return 1;
   Free(&modpar[modem].txbufin); /* frame too old */
   return 0;
} /* end frames2tx() */


static void sendmodem(void)
{
   short buf[4096];
   long i0;
   unsigned long clk;
   float samp;
   unsigned char c;
   struct CHAN * anonym;
   struct CHAN * anonym0;
   struct MPAR * anonym1;
   struct CHAN * anonym2;
   unsigned char tmp;
   long tmp0;
   if (soundbufs>0UL) --soundbufs;
   tmp = maxchannels;
   c = afskmodem_LEFT;
   if (c<=tmp) for (;; c++) {
      { /* with */
         struct CHAN * anonym = &chan[c];
         if (anonym->pttsoundbufs>0UL) --anonym->pttsoundbufs;
         if (anonym->state==afskmodem_receiv) {
            for (i0 = 0L; i0<=7L; i0++) {
               /* has any modem data? */
               /*
                         IF modpar[i].txbufin<>NIL THEN
                           IF c=modpar[i].ch THEN actmodem:=i END;  
                         END;
               */
               if (frames2tx(i0) && c==modpar[i0].ch) anonym->actmodem = i0;
            } /* end for */
            /*
                    IF (actmodem<0) OR (modpar[actmodem].txbufin=NIL)
                THEN     (* no data to send *)
            */
            if (anonym->actmodem<0L || !frames2tx(anonym->actmodem)) {
               /* no data to send */
               /*
               IF soundctl.getoutfilled(soundfd)
                <=0 THEN soundctl.pcmsync(soundfd) END;
               IF soundctl.getoutfilled(soundfd)
                >0 THEN IO.WrInt(soundctl.getoutfilled(soundfd),1); IO.WrLn;
                END;
               */
               if (anonym->pttstate && anonym->pttsoundbufs==0UL) {
                  anonym->pttstate = 0; /*; WrInt(ORD(c),1);
                WrStrLn(" pttoff");*/ /* guess all sound buffers are sent*/
                  ptt(anonym->hptt, 0L);
               }
            }
            else if (anonym->pttstate) {
               /* more data, ptt is on*/
               anonym->state = afskmodem_sendtxdel;
               anonym->tbitc = 0L;
               anonym->tbytec = 0UL;
               txmon(modpar[anonym->actmodem].txbufin);
            }
            else {
               /* data ptt off */
               anonym->tbytec = 0UL;
               anonym->state = afskmodem_slotwait;
               anonym->addrandom = 2UL+(unsigned long)X2C_TRUNCC(Lib_Random()
                *(double)anonym->persist,0UL,X2C_max_longcard);
                /* store ramdom wait */
               anonym->dcdclock = clock0; /* start txwait after we sent */
            }
         }
         if (anonym->state==afskmodem_slotwait) {
            if (anonym->duplex==afskmodem_shiftdigi) {
               clk = modpar[anonym->actmodem].dcdclockm;
                /* use dcd of this modulation */
            }
            else {
               clk = anonym->dcdclock;
                /* use dcd of latest heard modulation */
            }
            if (anonym->duplex==afskmodem_fullduplex || clock0-clk>anonym->addrandom)
                 {
               if (frames2tx(anonym->actmodem)) {
                  txmon(modpar[anonym->actmodem].txbufin);
                  chan[c].pttstate = 1; /*WrInt(ORD(c),1); WrStrLn(" ptton");
                */
                  ptt(chan[c].hptt, 1L);
                  chan[c].gmcnt = chan[c].gmqtime;
                  anonym->pttsoundbufs = soundbufs+extraaudiodelay;
                  anonym->state = afskmodem_sendtxdel;
                  anonym->tbitc = 0L;
                  anonym->tbytec = 0UL;
               }
               else anonym->state = afskmodem_receiv;
            }
         }
      }
      if (c==tmp) break;
   } /* end for */
   while (soundbufs<maxsoundbufs && (chan[afskmodem_LEFT]
                .state>=afskmodem_sendtxdel || chan[afskmodem_RIGHT]
                .state>=afskmodem_sendtxdel)) {
      tmp0 = (long)(adcbuflen-1UL);
      i0 = 0L;
      if (i0<=tmp0) for (;; i0++) {
         if (maxchannels==afskmodem_RIGHT) c = (unsigned char)(i0&1L);
         else c = afskmodem_LEFT;
         { /* with */
            struct CHAN * anonym0 = &chan[c];
            if (anonym0->state>=afskmodem_sendtxdel) {
               samp = TFIR[anonym0->txbaudgen/512UL][(unsigned long)
                anonym0->tscramb&63UL];
               if (modpar[anonym0->actmodem].afsk) {
                  anonym0->dds = anonym0->dds+(unsigned long)
                X2C_TRUNCC(samp*modpar[anonym0->actmodem]
                .afshift+modpar[anonym0->actmodem].afmid,0UL,
                X2C_max_longcard)&32767UL;
                  buf[i0] = (short)(long)
                X2C_TRUNCI(Phasemod(&anonym0->hipasscap,
                SIN[anonym0->dds]*modpar[anonym0->actmodem].txvolum,
                modpar[anonym0->actmodem].afskhighpass),X2C_min_longint,
                X2C_max_longint);
               }
               else {
                  buf[i0] = (short)(long)
                X2C_TRUNCI(samp*modpar[anonym0->actmodem].txvolum,
                X2C_min_longint,X2C_max_longint);
               }
               if (anonym0->gmcnt>0UL) {
                  buf[i0] = 0;
                  --anonym0->gmcnt;
               }
               anonym0->txbaudgen = anonym0->txbaudgen+modpar[anonym0->actmodem]
                .txbaud;
               if (anonym0->txbaudgen>=65536UL) {
                  anonym0->txbaudgen -= 65536UL;
                  /*send next bit */
                  if (anonym0->tbitc<=0L) {
                     anonym0->tbitc = 8L;
                     if (anonym0->state==afskmodem_sendtxdel) {
                        if (modpar[anonym0->actmodem].bert>0L) {
                           anonym0->tbyte = 255UL;
                           modpar[anonym0->actmodem].bert -= 8L;
                        }
                        else {
                           anonym0->tbyte = modpar[anonym0->actmodem]
                .txdelpattern;
                           ++anonym0->tbytec;
                        }
                        if (anonym0->tbytec>=modpar[anonym0->actmodem].txdel)
                 anonym0->tbyte = 126UL;
                        if (anonym0->tbytec>modpar[anonym0->actmodem].txdel) {
                           anonym0->state = afskmodem_senddata;
                           anonym0->tbytec = 0UL;
                           anonym0->txstuffc = 0L;
                        }
                     }
                     if (anonym0->state==afskmodem_sendtxtail) {
                        anonym0->tbyte = 126UL;
                        ++anonym0->tbytec;
                        /*next frame*/
                        if (modpar[anonym0->actmodem].txbufin) {
                           /* same modem */
                           anonym0->tbytec = 0UL;
                           anonym0->state = afskmodem_senddata;
                           anonym0->txstuffc = 0L;
                           txmon(modpar[anonym0->actmodem].txbufin);
                        }
                        else if (anonym0->tbytec>modpar[anonym0->actmodem]
                .txtail) {
                           anonym0->state = afskmodem_receiv;
                /* no data for this modem */
                        }
                     }
                     if (anonym0->state==afskmodem_senddata) {
                        { /* with */
                           struct MPAR * anonym1 = &modpar[anonym0->actmodem]
                ;
                           if (anonym1->txbufin==0 || anonym0->tbytec>=anonym1->txbufin->len+2UL)
                 {
                              /*frame sent*/
                              Free(&anonym1->txbufin);
                              anonym0->state = afskmodem_sendtxtail;
                              anonym0->tbyte = 126UL;
                              anonym0->tbytec = 0UL;
                           }
                           else {
                              anonym0->tbyte = (unsigned long)(unsigned char)
                anonym1->txbufin->data[anonym0->tbytec];
                              ++anonym0->tbytec;
                           }
                        }
                     }
                  }
                  /*stuff*/
                  if ((anonym0->tbyte&1)
                && anonym0->state==afskmodem_senddata) {
                     ++anonym0->txstuffc;
                     if (anonym0->txstuffc>=5L) {
                        anonym0->tbyte += anonym0->tbyte-1UL;
                        ++anonym0->tbitc;
                        anonym0->txstuffc = 0L;
                     }
                  }
                  else anonym0->txstuffc = 0L;
                  /*stuff*/
                  /* nrzi */
                  if (!(anonym0->tbyte&1)) anonym0->tnrzi = !anonym0->tnrzi;
                  /*
                              IF modpar[actmodem].scramb THEN
                                tscramb:=CAST(BITSET, CAST(CARDINAL,
                tscramb)*2+ORD(tnrzi <> ((11 IN tscramb) <> (16 IN tscramb)))
                );
                              ELSE tscramb:=CAST(BITSET, CAST(CARDINAL,
                tscramb)*2+ORD(tnrzi)) END;
                  */
                  anonym0->tscramb = X2C_LSH(anonym0->tscramb,32,1);
                  if (modpar[anonym0->actmodem].scramb) {
                     if (anonym0->tnrzi!=(((0x1000UL & anonym0->tscramb)!=0)
                !=((0x20000UL & anonym0->tscramb)!=0))) {
                        anonym0->tscramb |= 0x1UL;
                     }
                  }
                  else if (anonym0->tnrzi) anonym0->tscramb |= 0x1UL;
                  anonym0->tbyte = anonym0->tbyte>>1;
                  --anonym0->tbitc;
               }
            }
            else buf[i0] = 0;
         }
         if (i0==tmp0) break;
      } /* end for */
      /*    WrBin(soundfd, buf, adcbuflen*adcbytes); */
      i0 = write(soundfd, (char *)buf, adcbuflen*adcbytes);
      for (c = afskmodem_LEFT;; c++) {
         { /* with */
            struct CHAN * anonym2 = &chan[c];
            if (anonym2->state>=afskmodem_sendtxdel) ++anonym2->pttsoundbufs;
         }
         if (c==afskmodem_RIGHT) break;
      } /* end for */
      ++soundbufs;
   }
} /* end sendmodem() */

static void afskmodemcleanup(long);


static void afskmodemcleanup(long signum)
{
   pttDestroy(chan[afskmodem_LEFT].hptt);
   pttDestroy(chan[afskmodem_RIGHT].hptt);
   InOut_WriteString("exit ", 6ul);
   InOut_WriteInt(signum, 0UL);
   osi_WrStrLn("!", 2ul);
   X2C_HALT((unsigned long)signum);
} /* end afskmodemcleanup() */


X2C_STACK_LIMIT(100000l)
extern int main(int argc, char **argv)
{
   X2C_BEGIN(&argc,argv,1,4000000l,8000000l);
   if (sizeof(FILENAME)!=1024) X2C_ASSERT(0);
   TimeConv_BEGIN();
   Storage_BEGIN();
   Lib_BEGIN();
   RealMath_BEGIN();
   aprsstr_BEGIN();
   osi_BEGIN();
   signal(SIGTERM, afskmodemcleanup);
   signal(SIGINT, afskmodemcleanup);
   Parms();
   Gencrctab();
   initTFIR();
   /*
   tfd:=FIO.Create("/tmp/t.raw");
   */
   getst = 0UL;
   esc = 0;
   afin = 0UL;
   soundbufs = 0UL;
   for (;;) {
      getadc();
      ++clock0;
      if ((clock0&63UL)==0UL) {
         ptt(chan[afskmodem_LEFT].hptt, -1L);
                /* sync ptt to hardware somtime */
         ptt(chan[afskmodem_RIGHT].hptt, -1L);
         systime = TimeConv_time();
      }
      getkiss();
      getudp();
      sendmodem();
   }
/*
IO.WrStr("len="); IO.WrCard(txbuf[txp].len, 1); IO.WrStr("cmd=");
                IO.WrHex(ORD(txbuf[txp].cmd), 1);

FOR i:=1 TO txbuf[txp].len DO IO.WrHex(ORD(txbuf[txp].data[i-1]), 3) END;
IO.WrLn;   
*/
/*
  FIO.Close(pipefd);
*/
/*
FIO.Close(tfd);
*/
   X2C_EXIT();
   return 0;
}

X2C_MAIN_DEFINITION