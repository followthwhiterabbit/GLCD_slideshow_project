/* 
Mechatronics Engineering Microprocessors Final Project
Slide Show Using GLCD
Author : Seyit Kaan Karaalioglu
ID     : 140223014
                                           Date: 29/12/2020
*/

#if defined(__PCM__)
#include <16F877.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)

#endif


// fast_io initializations 
#use fast_io(A)
//#use fast_io(b)
#use fast_io(C)
//#use fast_io(d)
#use fast_io(E)


//after setting up the time, we start displaying by choosing start button
#define ayar_buton pin_e0
#define onay_buton pin_a1

//7 segment define statements 
#define display_1 pin_e1
#define display_2 pin_e2





// GLCD define and include statements

#define GLCD_CS1  pin_b0  // chip select 1
#define GLCD_CS2  pin_b1 // chip select 2 
#define GLCD_DI   pin_b2 // Data or instruction input
#define GLCD_RW   pin_b3 // Read and Write pin
#define GLCD_E    pin_b4 // GLCD enable
#define GLCD_RST  pin_b5 // GLCD reset
#include <GLCD.c>


//EEPROM define and include statements 
#define EEPROM_SDA pin_b6
#define EEPROM_SCL pin_b7
#include <24512.c>



#define INTS_PER_SECOND 152     // (20000000/(4*128*256))
#define PICTURE_COUNT 4         // number of pictures to show up 

int numbers[16] ={0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7C, 0x07, 0x7F, 0x6F, 0x77 , 0x7C, 0x39, 0x5E, 0x79, 0x71}; 
// numbers as hex codes for two digit seven segments

int16 resim, blinkcounter;
unsigned char mode, cmode, setm, sayi, dig[2];
char text1[22];
int1 blink, direct;
int8 seg;

int8 timeout, time[PICTURE_COUNT];      // A running seconds counter
int8 int_count;    // Number of interrupts left before a second has elapsed
int1 on_down, on_down_onay; // on_down and on_down_onay are defined as booleans 

/*
void sevenseg(void)
{
   if (blink)
   {
   
     
      output_c( numbers[ dig[0] ] );
      output_low(display_2);
      Delay_ms(2);
      output_high(display_2);

      output_c( numbers [dig[1]] );
      output_low(display_1);
      Delay_ms(2);
      output_high(display_1);

   }

}
*/



#int_rtcc
void clock_isr() 
{    

  //------------------------------------------ 

   if (blinkcounter>5)
   {
        blinkcounter=0;
        blink=!blink;    // blink happens
   }
  else blinkcounter++;                          
   
   //-------------------------------------------
   output_high(display_1);
   output_high(display_2);
   
   if ((blink)||(direct))
   {
     if (seg==0)
     {
       seg=1;
       output_low(display_1);
     }
     else
     {
       seg=0;
       output_low(display_2);
     }
   
      output_c(numbers[dig[seg]]);
   
   }
   //--------------------------------------------
   if(--int_count==0) 
   {            // per second.
     if (timeout>0) timeout--;
     int_count=INTS_PER_SECOND;
   }
}

void Picture_load()  
{
int16 i,j,x,y,t,start_adr;
 unsigned char a;

                          x=127;
                          y=0;
                          t=0;
                          start_adr = 61 + (resim*1024)+1024;
                          
                          for(i = 0 ; i < 1024 ; i++)
                          {
                              a = read_ext_eeprom(start_adr);
         
                              t=1;
                              for(j = 0; j < 8; j++)
                              {
                                glcd_pixel( x, y,   ( (a & t)==0) );
      
                                if (x==0)
                                {
                                   x=127;
                                    y++;
                                 }
                                 else x--;
            
                                  t=t*2;
                              }
                              
                             start_adr--;
                           }
}

void main()
{
   int j;
  
   setup_psp(PSP_DISABLED);
   setup_timer_1(T1_DISABLED);    // timer1 disabled
   setup_timer_2(T2_DISABLED,0,1);  // timer 2 disabled
   setup_CCP1(CCP_OFF);
   setup_CCP2(CCP_OFF);

   set_tris_a(0x03); // 0b0000 0011 ra1 and ra0 are inputs
   set_tris_b(0xC0); //rb6 and rb7 are inputs
   set_tris_c(0x00); //all c pins are output
   set_tris_d(0x00); //all d pins are output
   set_tris_e(0x01);  //0b 0000 0001
   
   output_a(0x00); 
   output_b(0x00);
   output_d(0x00);
   output_e(0x06); //0b 0000 0110 
   
   
   
   output_low(display_2);
   output_low(display_1);
   
   
   int_count=INTS_PER_SECOND;
   setup_timer_0(RTCC_INTERNAL|RTCC_DIV_128|RTCC_8_BIT);
   enable_interrupts(INT_RTCC);
   enable_interrupts(GLOBAL); 
   
   setup_adc_ports(AN0); // ANO analog
   setup_adc(ADC_CLOCK_INTERNAL);
   set_adc_channel(0);
   
   
   glcd_init(on);
   
   //glcd_rect(10, 10, 20, 20, 1, 1);
   init_ext_eeprom();
   
   /*
         for(j = 0; j < 8; j++)
         {
         a = read_ext_eeprom(j);
         sprintf(text1,"%02X ",a);
         glcd_text57(0, j*8, &text1[0], 1, 1);
         }
         */
  

 
  
   sprintf(text1,"Mikroislemciler Proje");
   glcd_text57(0, 0, &text1[0], 1, 1);
   
   sprintf(text1,"140223014");
   glcd_text57(34,10,&text1[0], 1, 1);
   
   sprintf(text1,"Seyit Kaan");
   glcd_text57(32, 20, &text1[0], 1, 1);
   
   sprintf(text1,"Karaalioglu");
   glcd_text57(28, 30, &text1[0], 1, 1);
   
   sprintf(text1, "click");
   glcd_text57(40, 45, &text1[0], 1, 1);
   
   sprintf(text1, "ayar buton");
   glcd_text57(30, 55, &text1[0], 1, 1);
   

   Delay_ms(1500);
   
   
    //-----------------------------
    // load values from internal eeprom
    
    cmode = read_eeprom( 0 );
    resim = read_eeprom( 1 );
    
    for(j=0; j<PICTURE_COUNT; ++j) 
    {
       time[j] = read_eeprom( 2+ j );
    }
   
   // defaults
    time[0]= 9;
    time[1]= 2;
    time[2]= 11;
    time[3]= 5;
    
    cmode=2; // 
    resim=0; // we define resim variable for the logos
   //-----------------------
  
   mode = 1; 
   direct=true;
   blinkcounter=0;
   sayi=resim+1;
   dig[0]=0;
   dig[1]=0;
   timeout=0;
   on_down=false;
   
   //setup_wdt(WDT_2304MS);
   
while(true)
 
{
     
           if(input(onay_buton))
           {
             if (mode!=0)
             {
               if (!on_down_onay)
               {
                 on_down_onay=true;
                 if (cmode<2) cmode++; else cmode=0;
                  glcd_rect(0, 0, 127, 15, 1, 1);
                   switch (cmode)
                   {
                     case 0: sprintf(text1,"1..n"); break;
                     case 1: sprintf(text1,"n..1"); break;
                     case 2: sprintf(text1,"1..n..1"); break;
                     
                     
                   }
                  glcd_text57(0, 0, &text1[0], 1, 0);
               }
             }
           
           }
           else on_down_onay=false;
        
   
        if(input(ayar_buton) )
        {
           
           if (!on_down)
           {
             on_down=true;
             iF(mode!=0)
             {
               mode = 0;
               resim=0;
            //   Picture_load();
             }
             else
             {
                if (mode==0)
                {
                  if (resim< (PICTURE_COUNT-1))
                  {
                     resim++;
                  }
                  else resim=0;
                  
                 // Picture_load();
                }
              }
              
             
         }
           
        }
        else on_down=false;
        
        
        
   
        switch (mode)
        {
          case 0:         
                               direct=false;
                               sayi = read_adc()/16;
                                
                                if (sayi<16)
                                {
                                  dig[0]=sayi;
                                  time[resim]= sayi;
                                }
                                
                                
                                dig[1]=resim+1;
          
          
                           if(input(onay_buton))
                           {
                              for(j=0; j<PICTURE_COUNT; ++j) 
                              {
                                 write_eeprom( 2+ j,time[j] );
                              }
                             mode = 1; 
                           }
          
         
                       
                       /*
                           dig[0]=sayi;
                           if (sayi==15)
                           sayi=0; else sayi++;
                           
                         */  
                           
                           
                           break;
          
          case 1:
                           direct=true;
                           Picture_load();
                           timeout = time[resim];
                           mode=2;
                           
                           dig[1]=resim;
                           dig[0]=timeout;
                           break;
           
               //--------------------------------   
           case 2:
                      direct=true;
                   if (timeout==0)
                   {
                     //-------------------------------------------
                      if (cmode==0)
                      {
                      //0 to n;
                          if (resim==(PICTURE_COUNT-1))
                          {
                           resim=0;
                          }
                          else resim++;
                      }
                      else if (cmode==1)
                      {
                      //n to 0
                           if (resim==0)
                          {
                           resim=PICTURE_COUNT-1;
                          }
                          else resim--;
                      
                      }
                      else 
                      {
                         //0 to n to 0
                         
                         if (setm==0)
                         {
                             if (resim==(PICTURE_COUNT-1))
                             {
                              resim=PICTURE_COUNT-2;
                              setm=1;
                             }
                             else resim++;
                         }
                         else
                         {
                               
                            if (resim==0)
                            {
                              resim=1;
                              setm=0;
                            }
                            else resim--;
                         }
                      
                      
                      }
                     write_eeprom( 2, resim );
                     //-------------------------------------------
                     
                   
                     
                     mode=1;
                     sayi = resim+1;
                   }
                     
                   dig[1]=resim;
                   dig[0]=timeout;
                   break;
                  
       }

   

   
    restart_wdt();
 

}


}
