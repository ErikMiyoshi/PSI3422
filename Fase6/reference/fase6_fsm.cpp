#define PWM 100
#define tamanho_limiar 53
#define threshold 10 //numero de reconhecimentos

//funções
void parado(){ 
    //0 ambos os motores
}
void anda_frente(){
    //tempo = 
    //pwm ambos os motores
}
void vira_90esq(){
    //tempo = 
    //pwm motor dir
}
void vira_90dir(){
    //tempo = 
    //pwm motor esq
}
void vira_180esq(){
    //tempo = 
    //pwm motor dir
}
void vira_180dir(){
    //tempo = 
    //pwm motor esq
}

/** FSM **/

estado0: //inicio
    parado();

estado1: //segue placa
    //rotina seguir placa - fase 4
    
    if(tamanho > tamanho_limiar){
        parado();
        goto estado2;
    } else goto estado1;

estado2: //reconhecimento
    //rotina reconhecer digito - fase 5
    parado();
    if(reconhecimento){
        int reconhecido = 5; //var com o digito reconhecido
        goto estado3;
    }

estado3: //checa digito reconhecido
    parado();
    switch(reconhecido){
        case 0:
            dig0++;
            if(dig0>=threshold) goto estado4;
            else goto estado2;
        case 1:
            dig1++;
            if(dig1>=threshold) goto estado4;
            else goto estado2;
        case 2:
            dig2++;
            if(dig2>=threshold) goto estado5;
            else goto estado2;
        case 3:
            dig3++;
            if(dig3>=threshold) goto estado6;
            else goto estado2;
        case 4:
            dig4++;
            if(dig4>=threshold) goto estado7;
            else goto estado2;
        case 5:
            dig5++;
            if(dig5>=threshold) goto estado7;
            else goto estado2;
        case 6:
            dig6++;
            if(dig6>=threshold) goto estado8;
            else goto estado2;
        case 7:
            dig7++;
            if(dig7>=threshold) goto estado8;
            else goto estado2;
        case 8:
            dig8++;
            if(dig8>=threshold) goto estado9;
            else goto estado2;
        case 9:
            dig9++;
            if(dig9>=threshold) goto estado9;
            else goto estado2;
    }
estado4: //dig 0 ou 1
    parado();
    goto fim;
estado5: //dig 2
    vira_180esq();
    goto estado0;
estado6: //dig 3
    vira_180dir();
    goto estado0;
estado7: //dig 4 ou 5
    anda_frente();
    goto estado0;
estado8: //dig 6 ou 7
    vira_90esq();
    goto estado0;
estado9: //dig 8 ou 9
    vira_90dir();
    goto estado0;
fim:

