#define PWM 100
#define tamanho_limiar 53

/* FUNC */
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
/* var */
bool reconhecimento; 
/** FSM **/
int main(){
    //cria matrizes

    estado0: //inicio
        parado();

    estado1: //segue placa
        //rotina seguir placa - fase 4

        //manda video cliente
        //recebe resposta template matching
        int tamanho; //template matching ou outra variavel recebida pelo cliente, indicando dist min até a placa
        if(tamanho > tamanho_limiar){
            parado();
            goto estado2;
        } else goto estado1;

    estado2: //reconhecimento
        //rotina reconhecer digito - fase 5
        
        parado();
        //manda video cliente, faz reconhec digito
        //recebe digito reconhecido do cliente
        goto estado3;

    estado3: //checa digito reconhecido
        parado();
        switch(reconhecido){
            case 0:
                goto estado4;
            case 1:
                goto estado4;
            case 2:
                goto estado5;
            case 3:
                goto estado6;
            case 4:
                goto estado7;
            case 5:
                goto estado7;
            case 6:
                goto estado8;
            case 7:
                goto estado8;
            case 8:
                goto estado9;
            case 9:
                goto estado9;
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
}

