#define threshold 10 //numero de reconhecimentos

int dig0, dig1, dig2, dig3, dig4, dig5, dig6, dig7, dig8, dig9 = 0;

int main(){

    estado1: //reconhecimento
        switch(reconhecido){
            case 0:
                dig0++;
                if(dig0>=threshold) goto estado2;
                else goto estado0;
            case 1:
                dig1++;
                if(dig1>=threshold) goto estado2;
                else goto estado0;
            case 2:
                dig2++;
                if(dig2>=threshold) goto estado2;
                else goto estado0;
            case 3:
                dig3++;
                if(dig3>=threshold) goto estado2;
                else goto estado0;
            case 4:
                dig4++;
                if(dig4>=threshold) goto estado2;
                else goto estado0;
            case 5:
                dig5++;
                if(dig5>=threshold) goto estado2;
                else goto estado0;
            case 6:
                dig6++;
                if(dig6>=threshold) goto estado2;
                else goto estado0;
            case 7:
                dig7++;
                if(dig7>=threshold) goto estado2;
                else goto estado0;
            case 8:
                dig8++;
                if(dig8>=threshold) goto estado2;
                else goto estado0;
            case 9:
                dig9++;
                if(dig9>=threshold) goto estado2;
                else goto estado0;
        }
        
    estado2: 
        //envia digito reconhecido pro rasp (server)

}