		/**********************************************/
		/** SOFTWARE DEVELOPED BY MANDAR SHIVAREKAR. **/
		/**       B.E.(ELECTRICAL) 2000-2001         **/
		/** Roll No. 27           EXAM No. B2072541  **/
		/**********************************************/

#include<stdio.h>
#include<conio.h>
#include<dos.h>
#define t 100				/* 1/t = SPEED OF MOTOR[rpm], Range: 80 to 300 */
#define pport 0x0378			/* Parallel Port (8 BIT Out) Address */
#define statusport 0x0379		/* Parallel Port (5 BIT Input) Address */
#define ctrlreg 0x037a			/* Parallel Port (4 BIT Output) Address */ 

/** BIOS SETTINGS **/
/*******************/
/** PRINTER PORT BIOS TYPE :- STANDARD/NORMAL **/
/** PRINTER PORT ADDRESS   :- 0x0378 H **/

/********************************/
/* MOTOR NO. ASSIGNING LOGIC 	*/
/********************************/
/* MOTOR1 - BASE TABLE MOTOR	*/
/* MOTOR2 - MAIN SHAFT MOTOR	*/
/* MOTOR3 - MID-SHAFT MOTOR  	*/
/* MOTOR4 - GRIPPER MOTOR	*/
/********************************/

/** MOTOR SELECTION LOGIC **/
/*** PORT ADDRESS = 0x0378 H ***/
/*******************************************************************/
/*|    BIT D4     |     BIT D5     |    BIT D6     |   BIT D7     |*/
/*******************************************************************/
/*|1 = MOTOR1     |1 = MOTOR3      |1 = MOTOR2	   | NOT USED     |*/
/*|    SELECTED   |    SELECTED    |    SELECTED   |              |*/
/*|0 = MOTOR1 NOT | 0 = MOTOR3 NOT |0 = MOTOR2 NOT |	          |*/
/*|    SELECTED   |    SELECTED    |    SELECTED   |              |*/
/*******************************************************************/

/** STATUS REGISTER WORD **/
/*** PORT ADDRESS = 0x0379 H ***/
/*************************************************************/
/*|    D3     |       D4      |      D5      |      D6      |*/
/*************************************************************/
/*|EMERGENCY  |1 =MOTOR3 NOT  |1 =MOTOR2 NOT |1 =MOTOR1 NOT |*/
/*|HALT USER  |   INITIALIZED |   INITIALIZED|   INITIALIZED|*/
/*|INTERRUPT  |0 =MOTOR 3     |0 =MOTOR 2    |0 =MOTOR 1    |*/
/*|           |   INITIALIZED |   INITIALIZED|   INITIALIZED|*/
/*************************************************************/

/** STATUS PORT WORDS **/
/** DOS COMMAND : DEBUG i 379 **/
/*************************************/
/*|  E  |  L3  |  L2  |  L1  ||WORD||*/
/*************************************/
/*|(14) | (13) | (12) | (10) || i/p||*/    /* PIN NO. ON PARALLEL PORT */
/*************************************/
/*|  0  |  0   |  0   |  0   || 7F ||*/
/*|  0  |  0   |  0   |  1   ||    ||*/      /*************************/
/*|  0  |  0   |  1   |  0   ||    ||*/      /* E  = Emergency Switch */
/*|  0  |  0   |  1   |  1   ||    ||*/      /* L1 = Limit S/w 1      */
/*|  0  |  1   |  0   |  0   ||    ||*/      /* L2 = Limit S/w 2      */
/*|  0  |  1   |  0   |  1   ||    ||*/      /* L3 = Limit S/w 3      */
/*|  0  |  1   |  1   |  0   ||    ||*/      /*************************/
/*|  0  |  1   |  1   |  1   ||    ||*/      /* 0 = Switch Off        */
/*|  1  |  0   |  0   |  0   || 77 ||*/      /* 1 = Switch On         */
/*|  1  |  0   |  0   |  1   ||    ||*/      /*************************/
/*|  1  |  0   |  1   |  0   ||    ||*/
/*|  1  |  0   |  1   |  1   ||    ||*/
/*|  1  |  1   |  0   |  0   ||    ||*/
/*|  1  |  1   |  0   |  1   ||    ||*/
/*|  1  |  1   |  1   |  0   ||    ||*/
/*|  1  |  1   |  1   |  1   ||    ||*/
/*************************************/

		/**********************/
		/** Global Variables **/
		/**********************/

int m1r=0,m2r=0,m3r=0,m4r=0,a=0,steps=8,mc=1,flag=0;
int m1i=0,m2i=0,m3i=0,m4i=0;
int m1c=1,m2c=1,m3c=1,m4c=0;
int m1l=0,m2l=0,m3l=0,m4l=0;
int m1pp[10],m2pp[10],m3pp[10],m4pp[10];
char p;

/* Max. Limit For All Motors (in Pulses) */
/*****************************************/
int m1limit=-3500,m2limit=-2450,m3limit=-5000,m4limit=-3000;

/* Output Words From PC To Motors */
/**********************************/
int combf[9]={0,0x05,0x01,0x09,0x08,0x0A,0x02,0x06,0x04};
int combb[9]={0,0x04,0x06,0x02,0x0A,0x08,0x09,0x01,0x05};
int combf4[9]={0,0x0E,0x0A,0x02,0x03,0x01,0x09,0x0D,0x0F};
int combb4[9]={0,0x0F,0x0D,0x09,0x01,0x03,0x02,0x0A,0x0E};

void main(void)
{
 unsigned int m,tpp=0,*tpp1;
 char exit1,x;
 nosound();
 outportb(pport,0x00);
 outportb(ctrlreg,0x0b);
 do
 {
  clrscr();
  printf("\n\tSoftware For Controlling 4-axis Articulated Robotic Arm.");
  printf("\n\t========================================================");
  printf("\n\n Full Stepping --> 1\tHalf Stepping --> 2\n\n Select Mode Of Operation :- ");
  scanf("%d",&a);
 }
 while(a<1||a>2);
 outportb(ctrlreg,0x0B);
 if(a==1)
 {
   for(a=1;a<=4;a++)
    {
     combf[a]=combf[(2*a)-1];
     combb[a]=combb[(2*a)-1];
     combf4[a]=combf4[(2*a)-1];
     combb4[a]=combb4[(2*a)-1];
    }
   steps=4;
  }
 REPEAT:
 Home();
 Initialize();
 clrscr();
 printf("\n\tSoftware For Controlling 4-axis Articulated Robotic Arm.");
 printf("\n\t========================================================");
 printf("\n\n  Record Mode :- 1\tLoad File :- 2\n :- ");
 do
   scanf("%d",&m);
 while(m<1||m>2);
 if(m==1)
   Record();
 if(m==2)
  {
   (int)tpp1=Load();
   tpp=*tpp1;
   flag=0;
   Playback(&tpp);
   flag=1;
   Playback(&tpp);
  }
 printf("\n\n\t\tDO YOU WANT TO EXIT (Y/N) ? ");
 exit1=getche();
 if(exit1=='y'||exit1=='Y')
     End();
 else
  {
   printf("\n\n Reset All Counters ?");
	x=getche();
   switch(x)
    {
	 case 'n':
		break;
	 case 'N':
		break;
	 case 'y':
	 {
          m1i=m2i=m3i=m4i=0;
	  m1c=m2c=m3c=m4c=0;
          m1r=m2r=m3r=m4r=0;
	  m1l=m2l=m3l=m4l=0;
	  break;
	 }
     }
   goto REPEAT;
 }
}

		/*******************/
		/* HOME SUBROUTINE */
		/*******************/

Home(void)
{
 int moto,c,x=0,y=0,z=0,a,m;
 clrscr();
		/****************************************************************************/
 goto JUMP;	/****** Insert/Remove Comment To Activate/De-Activate HOME SUBROUTINE *******/
		/****************************************************************************/
 outportb(ctrlreg,0x0B);
 printf("\n\t\tINITIALIZING TO HOME POSITION.\n\n\t\tPLEASE WAIT......");
 sound(1000);
 moto=0x40;
 c=1;
 for(a=500;a>=1;a--)
  {
	outportb(pport,(combf[c]|moto));
	delay(t/2);
	c++;
	if(c>=9)
		c=1;
	m=inportb(statusport);
	if(m==0x77||m==0x57||m==0x37||m==0x67||m==0x47||m==0x27||m==0x17||m==0x07)
	{
	 Emergency();
	 break;
	}
  }
 moto=0x70;
 do
  {
   for(c=1;c<=steps;c++)
	{
	 if(moto==0)
	    break;
	 outportb(pport,(combb[c]|moto));
	 delay(t);
	 a=inportb(statusport);
	 if(a==0x5f||a==0x1f||a==0x0f||a==0x4f)
	  {
	   if(x==1)
	      goto A;
	   moto&=0x60;
	   printf("\n\n\t\tMOTOR 1 INITIALIZED !");
	   x=1;
	   goto A;
	  }
	 A:
	 if(a==0x3f||a==0x1f||a==0x0f||a==0x2f)
	    {
	     if(y==1)
		goto B;
	     moto&=0x50;
	     printf("\n\n\t\tMOTOR 2 INITIALIZED !");
	     y=1;
	     goto B;
	    }
	 B:
	 if(a==0x6f||a==0x0f||a==0x4f||a==0x2f)
	  {
	   if(z==1)
	     goto C;
	   moto&=0x30
	   printf("\n\n\t\tMOTOR 3 INITIALIZED !");
	   z=1;
	   goto C;
	  }
	 C:
	 if(a==0x77||a==0x57||a==0x37||a==0x67||a==0x47||a==0x27||a==0x17||a==0x07)
	 {
	  Emergency();
	  break;
	 }
	}
  }
 while(moto!=0x00);
 outportb(pport,0x00);
 outportb(ctrlreg,0x0B);
 sound(900);
 delay(3000);
 sound(650);
 delay(3000);
 printf("\n\n\t\tHOME INITIALIZING COMPLETE !");
 nosound();
 printf("\n\n\t\tPRESS ANY KEY TO EXIT......");
 getch();
 JUMP:
 outportb(pport,0x00);
 outportb(ctrlreg,0x0B);
 return 0;
}

		/*************************/
		/* INITIALIZE SUBROUTINE */
		/*************************/

Initialize(void)
{
 char ai,bi;
 clrscr();
 do
 {
 outportb(pport,0x00);
 clrscr();
 printf("\n\tSoftware For Controlling 4-axis Articulated Robotic Arm.");
 printf("\n\t========================================================");
 printf("\n\n\t\t\tINITIALIZATION MODE");
 printf("\n\t\t\t===================");
 printf("\nSelect Motor --> 5\tExit --> 0\n");
 printf("\nClockwise\nMotor1 --> 1\nMotor2 --> 2\nMotor3 --> 3\nMotor4 --> 4");
 printf("\n\nAnti-Clockwise\nMotor1 --> 7\nMotor2 --> 8\nMotor3 --> 9\nMotor4 --> 6");
 printf("\nEnter :- ");
 ai=getche();
 switch(ai)
  {
   case '1':			/** Motor1 Clockwise Rotation Loop **/
    {
     printf("\a\n\n\t\tMotor1 CLOCKWISE Motion Selected.\n\n:- ");
     do
      {
       bi=getch();
       if(bi=='5'||bi=='0')
	{
	 m1c--;
	 break;
	}
     if(m1l<m1limit)
	{
	 Warn();
	 break;
	}
     if(m1c>steps)
	m1c=1;
     outportb(pport,combf[m1c]|0x10);
     m1i--;
     m1c++;
     m1l--;
     delay(t);
    }
   while(1);
   break;
  }
 case '2':			/** Motor2 Clockwise Rotation Loop **/
  {
   printf("\a\n\n\t\tMotor2 CLOCKWISE Motion Selected.\n\n:- ");
   do
    {
     bi=getch();
     if(bi=='5'||bi=='0')
      {
       m2c--;
       break;
      }
     if(m2l<m2limit)
      {
       Warn();
       break;
      }
     if(m2c>steps)
	m2c=1;
     outportb(pport,combf[m2c]|0x20);
     m2i--;
     m2c++;
     m2l--;
     delay(t);
    }
   while(1);
   break;
  }
 case '3':			/** Motor3 Clockwise Rotation Loop **/
  {
   printf("\a\n\n\t\tMotor3 CLOCKWISE Motion Selected.\n\n:- ");
   do
    {
     bi=getch();
     if(bi=='5'||bi=='0')
      {
       m3c--;
       break;
      }
     if(m3l<m3limit)
      {
       Warn();
       break;
      }
     if(m3c>steps)
	m3c=1;
     outportb(pport,combf[m3c]|0x40);
     m3i--;
     m3c++;
     m3l--;
     delay(t);
    }
   while(1);
   break;
  }
 case '4':			/** Motor4 Clockwise Rotation Loop **/
  {
   printf("\a\n\n\t\tMotor4 CLOCKWISE Motion Selected.\n\n:- ");
   do
    {
     bi=getch();
     if(bi=='5'||bi=='0')
      {
       m4c--;
       break;
      }
     if(m4l<m4limit)
      {
       Warn();
       break;
      }
     if(m4c>steps)
	m4c=1;
     outportb(ctrlreg,combf4[m4c]);
     m4i--;
     m4c++;
     m4l--;
     delay(t);
    }
   while(1);
   break;
  }
 case '7':			/** Motor1 Anti-Clockwise Rotation Loop **/
  {
   printf("\a\n\n\t\tMotor1 ANTI-CLOCKWISE Motion Selected.\n\n:- ");
   do
    {
     bi=getch();
     if(bi=='5'||bi=='0')
      {
	m1c--;
	break;
      }
     a=inportb(statusport);
     if(a==0x5f||a==0x1f||a==0x0f||a==0x4f)
	{
	 Warn();
	 break;
	}
     if(m1c>steps)
     m1c=1;
     outportb(pport,combb[m1c]|0x10);
     m1i++;
     m1c++;
     m1l++;
     delay(t);
    }
   while(1);
   break;
  }
 case '8':			/** Motor2 Anti-Clockwise Rotation Loop **/
  {
   printf("\a\n\n\t\tMotor2 ANTI-CLOCKWISE Motion Selected.\n\n:- ");
   do
    {
     bi=getch();
     if(bi=='5'||bi=='0')
      {
       m2c--;
       break;
      }
     a=inportb(statusport);
     if(a==0x3f||a==0x1f||a==0x0f||a==0x2f)
      {
       Warn();
       break;
      }
     if(m2c>steps)
	m2c=1;
     outportb(pport,combb[m2c]|0x20);
     m2i++;
     m2c++;
     m2l++;
     delay(t);
    }
   while(1);
   break;
  }
 case '9':			/** Motor3 Anti-Clockwise Rotation Loop **/
  {
   printf("\a\n\n\t\tMotor3 ANTI-CLOCKWISE Motion Selected.\n\n:- ");
   do
    {
     bi=getch();
     if(bi=='5'||bi=='0')
      {
       m3c--;
       break;
      }
     a=inportb(statusport);
     if(a==0x6f||a==0x0f||a==0x4f||a==0x2f)
      {
       Warn();
       break;
      }
     if(m3c>steps)
	m3c=1;
     outportb(pport,combb[m3c]|0x40);
     m3i++;
     m3c++;
     m3l++;
     delay(t);
    }
   while(1);
   break;
  }
 case '6':			/** Motor4 Anti-Clockwise Rotation Loop **/
  {
   printf("\a\n\n\t\tMotor4 ANTI-CLOCKWISE Motion Selected.\n\n:- ");
   do
    {
     bi=getch();
     if(bi=='5'||bi=='0')
      {
       m4c--;
       break;
      }
     if(m4l<m4limit)
      {
       Warn();
       break;
      }
     if(m4c>steps)
	m4c=1;
      outportb(ctrlreg,combb4[m4c]);
      m4i++;
      m4c++;
      m4l++;
      delay(t);
     }
    while(1);
    break;
   }
   case '0':
    goto END;
  }
 outportb(pport,0x00);
 }
 while(1);
 END:
 sound(900);
 delay(3000);
 sound(650);
 delay(3000);
 nosound();
 printf("\n\n\n\t\tINITIALIZATION MODE COMPLETE");
 printf("\n\n\t\tPRESS ANY KEY TO EXIT......");
 getch();
 return 0;
}

		/*********************/
		/* RECORD SUBROUTINE */
		/*********************/

Record(void)
{
 int c=0,*tpp1,tpp=0;
 char ar,bi;
 do
 {
 outportb(pport,0x00);
 clrscr();
 printf("\n\t   Software For Controlling 4-axis Articulated Robotic Arm.");
 printf("\n\t   ========================================================");
 printf("\n\t\t\t\tRECORD MODE");
 printf("\n\t\t\t\t===========");
 printf("\n\n\nSelect Motor --> 5\tExit --> 0\tRecord --> +\n");
 printf("\nClockwise\nMotor1 --> 1\nMotor2 --> 2\nMotor3 --> 3\nMotor4 --> 4");
 printf("\n\nAnti-Clockwise\nMotor1 --> 7\nMotor2 --> 8\nMotor3 --> 9\nMotor4 --> 6");
 printf("\nEnter :- ");
 ar=getche();
 switch(ar)
 {
 case '1':			/** Motor1 Clockwise Rotation Loop **/
  {
	printf("\a\n\n\t\tMotor1 CLOCKWISE Motion Selected.\n\n:- ");
	do
	 {
	  bi=getch();
	  if(bi=='5'||bi=='0')
		{
		 m1c--;
		 break;
		}
	  if(m1l<m1limit)
		{
		 Warn();
		 break;
		}
	  if(m1c>steps)
		m1c=1;
	  outportb(pport,combf[m1c]|0x10);
	  m1r--;
	  m1c++;
	  m1l--;
	  delay(t);
	 }
	while(1);
	break;
	}
 case '2':			/** Motor2 Clockwise Rotation Loop **/
	{
	printf("\a\n\n\t\tMotor2 CLOCKWISE Motion Selected.\n\n:- ");
	do
	 {
	  bi=getch();
	  if(bi=='5'||bi=='0')
		{
		 m2c--;
		 break;
		}
	  if(m2l<m2limit)
	   {
	    Warn();
		 break;
		}
	  if(m2c>steps)
		m2c=1;
	  outportb(pport,combf[m2c]|0x20);
	  m2r--;
	  m2c++;
	  m2l--;
	  delay(t);
	 }
	while(1);
	break;
	}
 case '3':			/** Motor3 Clockwise Rotation Loop **/
	{
	printf("\a\n\n\t\tMotor3 CLOCKWISE Motion Selected.\n\n:- ");
	do
	 {
	  bi=getch();
	  if(bi=='5'||bi=='0')
		{
		 m3c--;
		 break;
		}
	  if(m3l<m3limit)
	   {
	    Warn();
		 break;
		}
	  if(m3c>steps)
		m3c=1;
	  outportb(pport,combf[m3c]|0x40);
	  m3r--;
	  m3c++;
	  m3l--;
	  delay(t);
	 }
	while(1);
	break;
	}
 case '4':			/** Motor4 Clockwise Rotation Loop **/
	{
	printf("\a\n\n\t\tMotor4 CLOCKWISE Motion Selected.\n\n:- ");
	do
	 {
	  bi=getch();
	  if(bi=='5'||bi=='0')
		{
		 m4c--;
		 break;
		}
	  if(m4l<m4limit)
	   {
	    Warn();
		 break;
		}
	  if(m4c>steps)
		m4c=1;
	  outportb(ctrlreg,combf4[m4c]);
	  m4r--;
	  m4c++;
	  m4l--;
	  delay(t);
	 }
	while(1);
	break;
	}
 case '7':			/** Motor1 Anti-Clockwise Rotation Loop **/
	{
	printf("\a\n\n\t\tMotor1 ANTI-CLOCKWISE Motion Selected.\n\n:- ");
	do
	 {
	  bi=getch();
	  if(bi=='5'||bi=='0')
		{
		 m1c--;
		 break;
		}
	  a=inportb(statusport);
	  if(a==0x5f||a==0x1f||a==0x0f||a==0x4f)
		{
		 Warn();
		 break;
		}
	  if(m1c>steps)
		m1c=1;
	  outportb(pport,combb[m1c]|0x10);
	  m1r++;
	  m1c++;
	  m1l++;
	  delay(t);
	 }
	while(1);
	break;
	}
 case '8':			/** Motor2 Anti-Clockwise Rotation Loop **/
	{
	printf("\a\n\n\t\tMotor2 ANTI-CLOCKWISE Motion Selected.\n\n:- ");
	do
	 {
	  bi=getch();
	  if(bi=='5'||bi=='0')
		{
		 m2c--;
		 break;
		}
	  a=inportb(statusport);
	  if(a==0x3f||a==0x1f||a==0x0f||a==0x2f)
		{
		 Warn();
		 break;
		}
	  if(m2c>steps)
		m2c=1;
	  outportb(pport,combb[m2c]|0x20);
	  m2r++;
	  m2c++;
	  m2l++;
	  delay(t);
	 }
	while(1);
	break;
	}
 case '9':			/** Motor3 Anti-Clockwise Rotation Loop **/
	{
	printf("\a\n\n\t\tMotor3 ANTI-CLOCKWISE Motion Selected.\n\n:- ");
	do
	 {
	  bi=getch();
	  if(bi=='5'||bi=='0')
		{
		 m3c--;
		 break;
		}
     a=inportb(statusport);
	  if(a==0x68||a==0x48||a==0x08||a==0x28)
		{
		 Warn();
		 break;
		}
	  if(m3c>steps)
		m3c=1;
	  outportb(pport,combb[m3c]|0x40);
	  m3r++;
	  m3c++;
	  m3l++;
	  delay(t);
	 }
	while(1);
	break;
	}
 case '6':			/** Motor4 Anti-Clockwise Rotation Loop **/
	{
	printf("\a\n\n\t\tMotor4 ANTI-CLOCKWISE Motion Selected.\n\n:- ");
	do
	 {
	  bi=getch();
	  if(bi=='5'||bi=='0')
		{
		 m4c--;
		 break;
		}
	  if(m4l<m4limit)
		{
		 Warn();
		 break;
		}
	  if(m4c>steps)
		m4c=1;
	  outportb(ctrlreg,combb4[m4c]);
	  m4r++;
	  m4c++;
	  m4l++;
	  delay(t);
	 }
	while(1);
	break;
	}
 case '0':
	{
	 tpp=c;
	 goto END;
	}
 case '+':
	{
	 c++;
	 m1pp[c]=m1r;
	 m2pp[c]=m2r;
	 m3pp[c]=m3r;
	 m4pp[c]=m4r;
	 m1r=m2r=m3r=m4r=0;
	 printf("\a\n\n\t\tPOINT RECORDED !");
	 getch();
	 break;
	}
 default:
	printf("\n PLEASE PRESS VALID KEYS !");
  }
 }
 while(1);
 END:
 outportb(pport,0x00);
 clrscr();
 sound(900);
 delay(3000);
 sound(650);
 delay(3000);
 nosound();
 printf("\n\n\tRECORD MODE COMPLETE !");
 printf("\n\n\n\tTotal Programmed Points = %d",tpp);
 printf("\n\n\tDo You Want To Playback The Recorded Motions (Y/N) :- ");
 p=getche();
 if(p=='y'||p=='Y')
  {
  tpp1=&tpp;
  Home();
  flag=0;
  Playback(tpp1);
  flag=1;
  Playback(tpp1);
 }
 return 0;
}

			/***********************/
			/* PLAYBACK SUBROUTINE */
			/***********************/

Playback(unsigned int *tpp1)
{
 int b,c,m,pc=1,count=0,moto=0x00;
 unsigned int tpp;
 int x[5]={0,0,0,0,0};
 char ap,bp,s;
 x[0]=0;
 tpp=*tpp1;
 clrscr();
 if(flag==0)
   goto SKIP;
 printf("\n\tSoftware For Controlling 4-axis Articulated Robotic Arm.");
 printf("\n\t========================================================");
 printf("\n\n How Many Times Do You Want To Playback ?");
 printf("\n  [Enter Zero For Continuous Playback] :- ");
 scanf("%d",&pc);
 count=pc;
 clrscr();
 printf("\n\t PLAYBACK IN OPERATION !\n\n");
 SKIP:
 sound(1000);
 do
 {
  for(b=1;b<=tpp;b++)
   {
	if(flag==0)
		{
		 x[1]=abs(m1i);
		 x[2]=abs(m2i);
		 x[3]=abs(m3i);
		}
	if(flag==1)
		{
		 x[1]=abs(m1pp[b]);
		 x[2]=abs(m2pp[b]);
		 x[3]=abs(m3pp[b]);
		}
	if(x[3]<x[2])							/* x[1]<x[2]<x[3] */
	 {
		a=x[3]; x[3]=x[2]; x[2]=a;
	 }
	if(x[3]<x[1])
	 {
		a=x[3]; x[3]=x[1]; x[1]=a;
	 }
	if(x[2]<x[1])
	 {
		a=x[2]; x[2]=x[1]; x[1]=a;
	 }
 if(flag==1)
  {
   if(m1pp[b]<0||m2pp[b]<0||m3pp[b]<0)
    {
     for(c=1;c<=3;c++)
	{
	 if(m1pp[b]<0&&abs(m1pp[b])>=x[c])
		moto|=0x10;
	 if(m2pp[b]<0&&abs(m2pp[b])>=x[c])
		moto|=0x20;
	 if(m3pp[b]<0&&abs(m3pp[b])>=x[c])
		moto|=0x40;
	 for(m=(x[c]-x[c-1]);m>=0;m--)
	  {
	   if(mc>steps)
		mc=1;
	   outportb(pport,(combf[mc]|moto));
           a=inportb(statusport);
	   if(a==0x57||a==0x37||a==0x67||a==0x47||a==0x27||a==0x17||a==0x07)
		Emergency();
	   delay(t);
	   printf(".");
	   mc++;
	  }
	 moto=0x00;
	 outportb(pport,0x00);
	}
     }
    if(m1pp[b]>0||m2pp[b]>0||m3pp[b]>0)
     {
	for(c=1;c<=3;c++)
	 {
	  if(m1pp[b]>0&&abs(m1pp[b])>=x[c])
		moto|=0x10;
	  if(m2pp[b]>0&&abs(m2pp[b])>=x[c])
		moto|=0x20;
	  if(m3pp[b]>0&&abs(m3pp[b])>=x[c])
		moto|=0x40;
	  for(m=(x[c]-x[c-1]);m>=0;m--)
	   {
	    if(mc<0||mc>steps)
		mc=steps;
	    outportb(pport,(combb[mc]|moto));
            a=inportb(statusport);
	    if(a==0x77||a==0x57||a==0x37||a==0x67||a==0x47||a==0x27||a==0x17||a==0x07)
		Emergency();
	    delay(t);
	    printf(".");
	    mc--;
	   }
	  moto=0x00;
	  outportb(pport,0x00);
	 }
     }
  }
 if(flag==0)
  {
   if(m1i<0||m2i<0||m3i<0)
    {
     for(c=1;c<=3;c++)
      {
       if(m1i<0&&abs(m1i)>=x[c])
       moto|=0x10;
       if(m2i<0&&abs(m2i)>=x[c])
	  moto|=0x20;
       if(m3i<0&&abs(m3i)>=x[c])
	  moto|=0x40;
       for(m=(x[c]-x[c-1]);m>=0;m--)
	{
	 if(mc>steps)
	    mc=1;
	 outportb(pport,(combf[mc]|moto));
         a=inportb(statusport);
	 if(a==0x77||a==0x57||a==0x37||a==0x67||a==0x47||a==0x27||a==0x17||a==0x07)
	 Emergency();
	 delay(t);
	 printf(".");
	 mc++;
	}
       moto=0x00;
       outportb(pport,0x00);
      }
     }
   if(m1i>0||m2i>0||m3i>0)
    {
     for(c=1;c<=3;c++)
      {
       if(m1i>0&&abs(m1i)>=x[c])
	  moto|=0x10;
       if(m2i>0&&abs(m2i)>=x[c])
	  moto|=0x20;
       if(m3i>0&&abs(m3i)>=x[c])
	  moto|=0x40;
       for(m=(x[c]-x[c-1]);m>=0;m--)
	{
	 if(mc<0||mc>steps)
		mc=steps;
	 outportb(pport,(combb[mc]|moto));
         a=inportb(statusport);
	 if(a==0x77||a==0x57||a==0x37||a==0x67||a==0x47||a==0x27||a==0x17||a==0x07)
		Emergency();
	 delay(t);
	 printf(".");
	 mc--;
	}
       moto=0x00;
       outportb(pport,0x00);
      }
    }	
  }
 if(flag==0)
    break;
 if(m4pp[b]>=0)
  {
   for(m=m4pp[b];m>0;m--)
    {
     if(mc>steps)
	mc=1;
     outportb(ctrlreg,combf4[mc]);
     a=inportb(statusport);
     if(a==0x77||a==0x57||a==0x37||a==0x67||a==0x47||a==0x27||a==0x17||a==0x07)
	Emergency();
     delay(t);
     printf(".");
     mc++;
    }
   }
  else
  outportb(ctrlreg,0x0B);
 }
 if(flag==1)
 {
  for(b=tpp;b>0;b--)
   {
    for(a=0;a<=tpp;a++)
	{
	 m1pp[a]=-m1pp[a];
	 m2pp[a]=-m2pp[a];
	 m3pp[a]=-m3pp[a];
	}
    x[1]=abs(m1pp[b]);
    x[2]=abs(m2pp[b]);
    x[3]=abs(m3pp[b]);
    if(x[3]<x[2])			/* x[1]<x[2]<x[3] */
     {
      a=x[3]; x[3]=x[2]; x[2]=a;
     }
    if(x[3]<x[1])
     {
      a=x[3]; x[3]=x[1]; x[1]=a;
     }
    if(x[2]<x[1])
     {
      a=x[2]; x[2]=x[1]; x[1]=a;
     }
    if(m1pp[b]>0||m2pp[b]>0||m3pp[b]>0)
	{
	 for(c=1;c<=3;c++)
	  {
	   if(m1pp[b]>0&&abs(m1pp[b])>=x[c])
		moto|=0x10;
	   if(m2pp[b]>0&&abs(m2pp[b])>=x[c])
		moto|=0x20;
	   if(m3pp[b]>0&&abs(m3pp[b])>=x[c])
		moto|=0x40;
	   for(m=(x[c]-x[c-1]);m>=0;m--)
	    {
	     if(mc>steps)
		mc=1;
	     outportb(pport,(combb[mc]|moto));
             a=inportb(statusport);
	     if(a==0x77||a==0x57||a==0x37||a==0x67||a==0x47||a==0x27||a==0x17||a==0x07)
		 Emergency();
	     delay(t);
	     printf(".");
	     mc++;
	    }
	   moto=0x00;
	   outportb(pport,0x00);
	  }
	}
       if(m1pp[b]<0||m2pp[b]<0||m3pp[b]<0)
	{
	 for(c=1;c<=3;c++)
	  {
	   if(m1pp[b]<0&&abs(m1pp[b])>=x[c])
		moto|=0x10;
	   if(m2pp[b]<0&&abs(m2pp[b])>=x[c])
		moto|=0x20;
	   if(m3pp[b]<0&&abs(m3pp[b])>=x[c])
		moto|=0x40;
	   for(m=(x[c]-x[c-1]);m>=0;m--)
	    {
	     if(mc<0||mc>steps)
		mc=steps;
	     outportb(pport,(combf[mc]|moto));
             a=inportb(statusport);
 	     if(a==0x77||a==0x57||a==0x37||a==0x67||a==0x47||a==0x27||a==0x17||a==0x07)
	     Emergency();
	     delay(t);
	     printf(".");
	     mc--;
	    }
	   moto=0x00;
	   outportb(pport,0x00);
	  }
	}
	if(m4pp[b]>0)
	 {
	  for(m=m4pp[b];m>0;m--)
	   {
	    if(mc>steps)
		mc=1;
	    outportb(ctrlreg,combf4[mc]);
	    a=inportb(statusport);
	    if(a==0x77||a==0x57||a==0x37||a==0x67||a==0x47||a==0x27||a==0x17||a==0x07)
		Emergency();
	    delay(t);
	    printf(".");
	    mc++;
	   }
	  }
	 else
	    outportb(ctrlreg,0x0B);
	}
   }
  count--;
  if(pc==0)
     count=2;
  a=inportb(statusport);
  if(a==0x77||a==0x57||a==0x37||a==0x67||a==0x47||a==0x27||a==0x17||a==0x07)
     Emergency();
 }
 while(count>=0);
 nosound();
 clrscr();
 if(flag==1)
  {
   sound(900);
   delay(3000);
   sound(650);
   delay(3000);
   nosound();
   printf("\n\tSoftware For Controlling 4-axis Articulated Robotic Arm.");
   printf("\n\t========================================================");
   printf("\n\n\n\n\n\t\tPLAYBACK COMPLETE !");
   printf("\n\n\n\n\tDo You Want To Save The Results (Y/N) ? ");
   s=getche();
   if(s=='Y'||s=='y')
	Save(tpp1);
  }
 return 0;
}

		/*******************/
		/* LOAD SUBROUTINE */
		/*******************/
Load()
{
 FILE *fp;
 unsigned int tpp;
 char file[50],a;
 ERROR:
 clrscr();
 printf("\n\tSoftware For Controlling 4-axis Articulated Robotic Arm.");
 printf("\n\t========================================================");
 printf("\n\n\t\tEnter The File Name [e.g. - TEST1.DAT]\n\n\t - ");
 scanf("%s",&file);
 fp=fopen(file,"r");
 if(fp==NULL)
 {
  printf("\n\aTHE FILE DOES NOT EXIST !\n\nPlease Enter A Valid Path And File Name.");
  printf("\n\n\n\tPress Any Key To Continue......");
  getch();
  goto ERROR;
 }
 fscanf(fp,"%u\n%d\n%d\n%d\n%d\n",&tpp,&m1i,&m2i,&m3i,&m4i);
 for(a=1;a<=tpp;a++)
	 fscanf(fp,"%d\n%d\n%d\n%d\n",&m1pp[a],&m2pp[a],&m3pp[a],&m4pp[a]);
 flushall();
 fcloseall();
 printf("\n\n\t\tFile Loaded !");
 sound(900);
 delay(3000);
 sound(650);
 delay(3000);
 nosound();
 getch();
 printf("\n\n\n\t\tTotal Programmed Points = %d",tpp);
 getch();
 return((int)&tpp);
}

		/*******************/
		/* SAVE SUBROUTINE */
		/*******************/
Save(unsigned int *tpp1)
{
 FILE *fp;
 unsigned int tpp=*tpp1;
 char file[50];
 clrscr();
 printf("\n\tSoftware For Controlling 4-axis Articulated Robotic Arm.");
 printf("\n\t========================================================");
 printf("\n\n\tEnter The File Name [e.g. - TEST1.DAT]\n\n\t - ");
 scanf("%s",&file);
 fp=fopen(file,"w");
 tpp=*tpp1;
 fprintf(fp,"%d\n%d\n%d\n%d\n%d\n",tpp,m1i,m2i,m3i,m4i);
 for(a=1;a<=tpp;a++)
	fprintf(fp,"%d\n%d\n%d\n%d\n",m1pp[a],m2pp[a],m3pp[a],m4pp[a]);
 printf("\n\n\t\tFile Saved !");
 sound(900);
 delay(3000);
 sound(650);
 delay(3000);
 nosound();
 fflush(stdin);
 fclose(fp);
 getch();
 return 0;
}

		/******************/
		/* END SUBROUTINE */
		/******************/
End(void)
{
 char quit;
 clrscr();
 printf("\n\tSoftware For Controlling 4-axis Articulated Robotic Arm.");
 printf("\n\t========================================================");
 printf("\n\n\t\t ARE YOU SURE TO EXIT (Y/N) ? :-");
 quit=getch();
 if(quit=='n'||quit=='N')
  {
   printf("\n Reset All Counters ?");
   a=getche();
   if(a=='Y'||a=='y')
    {
     m1i=m2i=m3i=m4i=0;
     m1c=m2c=m3c=m4c=0;
     m1r=m2r=m3r=m4r=0;
    }
   main();
  }
 Home();
 outportb(pport,0);
 outportb(ctrlreg,0x0b);
 clrscr();
 printf("\n\tSoftware For Controlling 4-axis Articulated Robotic Arm.");
 printf("\n\t========================================================\n\n");
 printf("\n\t             Thanks For Using Our ROBOT Control Software.\n\n");
 printf("\n================================================================================");
 printf("\t  MANDAR SHIVAREKAR (C), DNYANESHWAR GORE (VC), AMRIT VADER, SUJIT SATHE,");
 printf("\n\t     RISHIKESH UTPAT, NACHIKET KHADILKAR, MAYURESH ATHALEKAR.");
 printf("\n================================================================================");
 printf("\t             Project Guide :- Prof. A. M. KELKAR");
 printf("\n================================================================================");
 printf("\n\t               * B.E.(ELECTRICAL) 2000-2001 *");
 printf("\n\t                 * PVG's COET PUNE 411009. *");
 sound(8000);
 delay(2000);
 sound(5000);
 delay(2000);
 sound(3000);
 delay(2000);
 sound(1000);
 delay(2000);
 sound(800);
 delay(2000);
 sound(500);
 delay(2000);
 sound(200);
 delay(2000);
 sound(50);
 delay(2000);
 nosound();
 flushall();
 fcloseall();
 getch();
 exit(0);
 return 0;
}

		/************************/
		/* EMERGENCY SUBROUTINE */
		/************************/
Emergency()
{
 clrscr();
 flushall();
 fcloseall();
 outportb(pport,0x00);
 outportb(ctrlreg,0x0B);
 printf("\n\n\t\t    !!! INTERRUPTED BY USER !!!");
 printf("\n\n\t\t!!! HALTING ROBOT IMMEDIATELY. !!!");
 printf("\n\n\t\t PRESS INTERRUPT KEY BRIEFLY AGAIN TO EXIT.");
 do
 {
  sound(800);
  delay(5000);
  sound(700);
  delay(5000);
  a=inportb(statusport);
  if(a==0x77||a==0x57||a==0x37||a==0x67||a==0x47||a==0x27||a==0x17||a==0x07)
    {
     nosound();
     exit(0);
    }
 }
 while(1);
}

			/**********************/
			/* Warning Subroutine */
			/**********************/
Warn()
{
 clrscr();
 printf("\n\n\n\t\t               !!! DANGER !!!");
 printf("\n\n\n\t\t!!! THE ROBOT EXCEEDS THE SAFETY LIMITS !!! ");
 sound(900);
 delay(5000);
 sound(650);
 delay(5000);
 nosound();
 getch();
 return 0;
}