#include "ce_watchdog.h"
int main(){
   int state;

   if(getuid()!=0){
      printf("This program needs elevated privileges.\n");
      return 1;
   }
   else {
        printf("This program is running on elevated priviledges.\n");
    }
   CE_Watchdog wd;
   wd.Begin();
   printf("Watchdog interval: %d s \n", wd.GetInterval());
/*
   if(wd.IsLastBootByWatchdog()){
    printf("Last boot is by watchdog \n");
   }
   else {
    printf("Last boot is by power-on-reset \n");
   }
*/
   printf("Enter p to pat the watchdog \n");
   printf("Enter q to quit \n");
   do{
      state = getchar();
      if(state=='p'){
         printf("pat... \n");
         wd.Pat();         
      }
   } while (state!='q');
   printf("Closing the application\n");
   wd.Close();
   return 0;
}
