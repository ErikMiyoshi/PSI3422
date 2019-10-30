//camclient1.cpp//

#include "projeto.hpp"

#include <vector>
#include <iostream>

using namespace std;

int estado = 0;
int estadoRecebido = 0;
int velE;
int velD;
int atraso = 0;
bool virou = false;

//coisas fase 3
double minVal; double maxVal;
Point minLoc; Point maxLoc; Point matchLoc;

double minVal_norm; double maxVal_norm;
Point minLoc_norm; Point maxLoc_norm; Point matchLoc_norm; Point last_loc;

int center_x = 0;
int center_y = 0;
int referencia_escolhida = 0; //Determinara a referencia escolhida

size_t size = 1; //Numero de imagens de amostra. Tamanho do vector
std::vector<Mat_<FLT>> imagens_referencia(size); //Cria vetor para imagens de amostra


Mat_<COR> imagem_original_aux;
Mat_<COR> imagem(240, 440, COR(0,0,0)); //240x120
Mat_<FLT> resultado;
Mat_<FLT> resultado_norm;
Mat_<FLT> imagem_original_flt;
bool imagem_ultimo_frame = false;

int melhor_template_nnorm(int ultimo_tamanho){ // Acha o tamanho ideal da imagem de referencia
	
	int tamanho = 100; //variavel que definira tamanho do template
    int minimo = 15;   //minimo tamanho a percorrer
    int maximo = 100;  //maximo tamanho a percorrer
    float max_value_aux = 0.0;  //Usado para comparar qual a maior correlacao com a imagem e o template
    Mat_<FLT> imagem_referencia_temp;

    for(int ref = 0; ref < size; ref++){ //Percorre todas as imagens de amostra

        if (ultimo_tamanho < 15) {
            minimo = 15; //Se nao viu no ultimo ou é muito pequena 15 e 60
            maximo = 80;
        } else {
            minimo = ultimo_tamanho - 10;
            maximo = ultimo_tamanho + 10;

        }
      
        for (int i = minimo; i < maximo; i = i + 5){ //Para cada um dos tamanhos ve qual a melhor correlacao
            //cout << " i = " << i;
            resize(imagens_referencia[ref], imagem_referencia_temp, Size(i,i), 0, 0, INTER_AREA);

            imagem_referencia_temp = somaAbsDois(dcReject(imagem_referencia_temp));

            matchTemplate( imagem_original_flt, imagem_referencia_temp, resultado, CV_TM_CCORR );
            minMaxLoc( resultado, &minVal, &maxVal, &minLoc, &maxLoc );
            //cout << " max Val = " << maxVal;
            //Se a imagem eh pequena a correlacao deve ser bem alta para evitar erros. No caso > 0.75
            if ( maxVal > max_value_aux){
                max_value_aux = maxVal;
                referencia_escolhida = ref;  //Atualiza o valor da amostra escolhida
                tamanho = i;
                
            }
        }
    }

    //Retorna o melhor tamanho encontrado
    //cout << tamanho;
    return tamanho;
} 

void desenha_retangulo(int tamanho){
        Mat_<FLT> templ = imagens_referencia[referencia_escolhida];
        resize(templ, templ, Size(tamanho, tamanho), 0, 0, INTER_AREA);

        line( imagem, Point( matchLoc.x + templ.cols/2 , matchLoc.y + templ.rows/2 + 0.02*templ.rows ), 
			Point( matchLoc.x + templ.cols/2 , matchLoc.y + templ.rows/2 - 0.02*templ.rows ), Scalar(0,255,255), 1, 8);

		line( imagem, Point( matchLoc.x + templ.cols/2 + 0.02*templ.rows , matchLoc.y + templ.rows/2 ), 
				Point( matchLoc.x + templ.cols/2 - 0.02*templ.rows , matchLoc.y + templ.rows/2 ), Scalar(0,255,255), 1, 8);
				
		rectangle( imagem, matchLoc, Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ), Scalar(0,255,255), 1.5, 8);
}


void on_mouse(int event, int c, int l, int flags, void* userdata) {
    if (event == EVENT_LBUTTONDOWN) {
        if ( 0 <= l && l < 80 && 0 <= c && c < 40 ) 
            estado = 7;
        else if ( 80 <= l && l < 160 && 0 <= c && c < 40 ) 
            estado = 4;
        else if ( 120 <= l && l < 240 && 0 <= c && c < 40 ) 
            estado = 1;
        else if ( 0 <= l && l < 80 && 40 <= c && c < 80 ) 
            estado = 8;
        else if ( 80 <= l && l < 160 && 40 <= c && c < 80 ) 
            estado = 5;
        else if ( 120 <= l && l < 240 && 40 <= c && c < 80 ) 
            estado = 2;
        else if ( 0 <= l && l < 80 && 80 <= c && c < 120 ) 
            estado = 9;
        else if ( 80 <= l && l < 160 && 80 <= c && c < 120 ) 
            estado = 6;
        else if ( 120 <= l && l < 240 && 80 <= c && c < 120 ) 
            estado = 3;

        else 
            estado = 0;
    } else if (event==EVENT_LBUTTONUP) {
        estado = 0;
    }
}

int main(int argc, char *argv[]) {

    le(imagens_referencia[0], "quadrado.png");
    //le(imagem_original_aux,"Frame1.png");

    Mat_<COR> imagem_original_aux(240, 440, COR(0,0,255));
    int novo_tamanho = 0; //Variavel que definira melhor tamanho para o tamplate matching
    int ultimo_tamanho = 0;

	COR cinza(128,128,128);
    COR vermelho(0, 0, 255);
    COR azul(255, 0, 0);

	if (argc!=2) 
		erro("client6 servidorIpAddr\n");
	CLIENT client(argv[1]);
	Mat_<COR> image;
	vector<BYTE> vb;
	namedWindow("janela");
	//resizeWindow("janela", 2 * imagem.cols, 2 * imagem.rows);
    setMouseCallback("janela", on_mouse);
    imshow("janela", imagem);
	int ch;

    vb.assign(100000,111);
    
	ch = -1;
	while (ch < 0) {
        atraso = 0;
		client.receiveInt(estadoRecebido);
		client.receiveImgComp(image);

        //desinverte a imagem
        for (int l = 0; l < 240; l++){
            for(int c = 120; c < 440; c++){
                imagem(l,c) = image((240-l), (320 - (c - 120)));
            }
        } 

        //desenha teclado virtual
        //for (int l = 0; l < 240; l++){
               // for(int c = 120; c < 440; c++){
                //    imagem(l,c) = image(l, (c - 120));
                //}
           // } 

        //
        for (int l = 0; l < imagem.rows; l++){
            for (int c = 0; c < imagem.cols; c++)
            {
                imagem_original_aux(l,c) = imagem(l,c);
            }
        }
        
        converte(imagem_original_aux, imagem_original_flt);
        imagens_referencia[0] = trataModelo(imagens_referencia[0], 0.9);

        // Acha o template melhor se encaixa na imagem e posteriormente o tamanho de tal template
        if (imagem_ultimo_frame == true) {
            novo_tamanho = melhor_template_nnorm(ultimo_tamanho);
        } else {
            novo_tamanho = melhor_template_nnorm(0);
        }

        ultimo_tamanho = novo_tamanho;
        Mat_<FLT> imagem_referencia_temp;
        resize(imagens_referencia[0], imagem_referencia_temp, Size(novo_tamanho, novo_tamanho), 0, 0, INTER_AREA);
        imagem_referencia_temp = somaAbsDois(dcReject(imagem_referencia_temp));

        matchTemplate( imagem_original_flt, imagem_referencia_temp, resultado, CV_TM_CCORR );

        //Encontra o ponto em que a semelhanca foi maxima e com isso determina o centro do circulo a ser desenhado na placa
        minMaxLoc( resultado, &minVal, &maxVal, &minLoc, &maxLoc );

        if ( imagem_ultimo_frame == true){ 
            if (((maxLoc.x - last_loc.x) * (maxLoc.x - last_loc.x) + 
                        (maxLoc.y - last_loc.y) * (maxLoc.y - last_loc.y)) < 225  && maxVal > 0.15){

                resize(imagens_referencia[0], imagem_referencia_temp, Size(novo_tamanho, novo_tamanho), 0, 0, INTER_AREA);
                matchTemplate(imagem_original_flt, imagem_referencia_temp, resultado_norm, CV_TM_CCOEFF_NORMED);
                minMaxLoc( resultado_norm, &minVal_norm, &maxVal_norm, &minLoc_norm, &maxLoc_norm );
                
                //Se a posicao dos dois templates esta proxima. d^2 = a^2 + b^2
                if (((maxLoc.x - maxLoc_norm.x) * (maxLoc.x - maxLoc_norm.x) + 
                        (maxLoc.y - maxLoc_norm.y) * (maxLoc.y - maxLoc_norm.y)) < 225 && maxVal_norm > 0.15){
                    
                    if (maxVal > maxVal_norm) {
                        matchLoc = maxLoc;
                    } else {
                        matchLoc = maxLoc_norm;
                    }

                    imagem_ultimo_frame = true;
                    last_loc = matchLoc;
                    desenha_retangulo(novo_tamanho); //Funcao que desenha retangulo na imagemd e saida
                    //cout << maxLoc << " - ";
                } else {
                    imagem_ultimo_frame = false;
                }  
            } else {
                imagem_ultimo_frame = false;
            }
            } else {
                
                if (maxVal > 0.18){

                    resize(imagens_referencia[0], imagem_referencia_temp, Size(novo_tamanho, novo_tamanho), 0, 0, INTER_AREA);
                    matchTemplate(imagem_original_flt, imagem_referencia_temp, resultado_norm, CV_TM_CCOEFF_NORMED);
                    minMaxLoc( resultado_norm, &minVal_norm, &maxVal_norm, &minLoc_norm, &maxLoc_norm );
                    
                    //Se a posicao dos dois templates esta proxima. d^2 = a^2 + b^2
                    if (((maxLoc.x - maxLoc_norm.x) * (maxLoc.x - maxLoc_norm.x) + 
                            (maxLoc.y - maxLoc_norm.y) * (maxLoc.y - maxLoc_norm.y)) < 225 && maxVal_norm > 0.18){
                        
                        if (maxVal > maxVal_norm) {
                            matchLoc = maxLoc;
                        } else {
                            matchLoc = maxLoc_norm;
                        }

                        imagem_ultimo_frame = true;
                        last_loc = matchLoc;
                        desenha_retangulo(novo_tamanho); //Funcao que desenha retangulo na imagemd e saida
                        //cout << maxLoc << " - ";
                    } else {
                        imagem_ultimo_frame = false;
                    }  
                } else {
                    imagem_ultimo_frame = false;
                }
            }
        


        velE = 0; velD = 0;
        switch (estado){
		    case 1:
                for (int l = 160; l < 240; l++)
                    for (int c = 0; c < 40; c++)
                        imagem(l,c) = vermelho;
                    velE = 0; velD = -60;
                    break;
            case 2:
                for (int l = 160; l < 240; l++)
                    for (int c = 40; c < 80; c++)
                        imagem(l,c) = vermelho;
                    velE = -60; velD = -60;
                    break;
            case 3:
                for (int l = 160; l < 240; l++)
                    for (int c = 80; c < 120; c++)
                        imagem(l,c) = vermelho;
                    velE = -60; velD = 0;
                    break;
            case 4:
                for (int l = 80; l < 160; l++)
                    for (int c = 0; c < 40; c++)
                    imagem(l,c) = vermelho;
                velE = -60; velD = 60;
                break;
            case 5:
                for (int l = 80; l < 160; l++)
                    for (int c = 40; c < 80; c++)
                        imagem(l,c) = vermelho;
                    velE = 0; velD = 0;
                    break;
            case 6:
                for (int l = 80; l < 160; l++)
                    for (int c = 80; c < 120; c++)
                        imagem(l,c) = vermelho;
                    velE = 60; velD = -60;
                    break;
            case 7:
                for (int l = 0; l < 80; l++)
                    for (int c = 0; c < 40; c++)
                        imagem(l,c) = vermelho;
                velE = 0; velD = 60;
                break;
            case 8:
                for (int l = 0; l < 80; l++)
                    for (int c = 40; c < 80; c++)
                        imagem(l,c) = vermelho;
                    velE = 60; velD = 60;
                break;
            case 9:
                for (int l = 0; l < 80; l++)
                    for (int c = 80; c < 120; c++)
                        imagem(l,c) = vermelho;
                    velE = 60; velD = 0;
                    break;
	        case 0: //Vira para o lado que tem placa
                if (virou == true){
                    virou = false;
                    velE = 0; velD = 0;
                    atraso = 150;
                } else if (matchLoc.x + novo_tamanho/2 > 355 && imagem_ultimo_frame == true){ //direita
                    velE = 50; velD = 0;
                    //atraso = 5;
                    virou = true;
                    
                } else if (matchLoc.x + novo_tamanho/2 > 120 && matchLoc.x + novo_tamanho/2 < 265 && imagem_ultimo_frame == true){
                    velE = 0; velD = 50;
                    // atraso = 5;
                    virou = true;
                    
                } else if (matchLoc.x + novo_tamanho/2 > 265 && matchLoc.x + novo_tamanho/2 < 355 && imagem_ultimo_frame == true){
                    velE = 0; velD = 0;
                } else {
                    velE = 0; velD = 0;
                }
		        //velE = 0; velD = 0;
                break;
		}


        //Linhas Auxiliares
        for (int l = 0; l < 240; l++){
            for(int c = 0; c < 120; c++){
                imagem(l,c) = cinza;
                if (l == 80 || l == 160 || c == 40 || c == 80) {
                    imagem(l,c) = azul;
                } 
            } for ( int c = 120; c < 440; c++){
                if (c == 265 || c == 310 || c == 355) {
                    imagem(l,c) = azul;
                } 
            }
        } 
   
        matchLoc.x = 0;
        matchLoc.y = 0;
        novo_tamanho = 0;
   
        
		putTxt(imagem, 20, 150, to_string(estadoRecebido));

		imshow("janela",imagem);
		client.sendInt(estado);
        client.sendInt(velE);
        client.sendInt(velD);
		client.sendVb(vb);
        client.sendInt(atraso);
		ch = waitKey(10);
	}
	
	vb.assign(100000,222);
	client.sendVb(vb);

}

