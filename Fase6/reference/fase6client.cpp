//camclient1.cpp//

#include "projeto.hpp"

#include <vector>
#include <iostream>

using namespace std;

int estado = 0;
int maquinaEstados = 0;
int estadoRecebido = 0;
int velE;
int velD;
int atraso = 0;
int qp2 = 0;
bool virou = false;
bool aproximou = false;
int aux = 0;
int aux2 = 0;
int busca = 0;
int centralizar = 0;
int aproximando = 0;

//coisas fase 5
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
Mat_<FLT> imagem_numero_flt;
Mat_<COR> imagem_numero_cor;
bool imagem_ultimo_frame = false;

int nlado = 14;//convArg(nlado,argv[1]);
bool bbox = true; //convArg(bbox,argv[2]);
MNIST mnist(nlado, true, bbox);
bool calcula_numero = false;

Mat_<FLT> MakeBbox(Mat_<FLT> a, int nlado) {
    int esq=a.cols, dir=0, cima=a.rows, baixo=0; // primeiro pixel diferente de 255.

    for (int l=0; l<a.rows; l++)
        for (int c=0; c<a.cols; c++) {
            if (a(l,c)<=0.5) {
                if (c<esq) 
                    esq=c;
                if (dir<c) 
                    dir=c;
                if (l<cima) 
                    cima=l;
                if (baixo<l) 
                    baixo=l;
            }
        }
    Mat_<FLT> roi(a, Rect(esq,cima,dir-esq+1,baixo-cima+1));
    Mat_<FLT> d; resize(roi,d,Size(nlado,nlado),0, 0, INTER_AREA);
    return d;
}

int melhor_template_nnorm(int ultimo_tamanho){ // Acha o tamanho ideal da imagem de referencia
	
	int tamanho = 100; //variavel que definira tamanho do template
    int minimo = 30;   //minimo tamanho a percorrer
    int maximo = 180;  //maximo tamanho a percorrer
    float max_value_aux = 0.0;  //Usado para comparar qual a maior correlacao com a imagem e o template
    Mat_<FLT> imagem_referencia_temp;

    for(int ref = 0; ref < size; ref++){ //Percorre todas as imagens de amostra

        if (ultimo_tamanho < 15) {
            minimo = 5; //Para imagens pequenas x<=100
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

void extrai_numero(int tamanho, int xi, int yi, int xf, int yf){
    
    if ((xf-xi) > 16 && ((yf-yi) > 16)){ //tamanho do numero
        Mat_<COR> numero_extraido((xf-xi), (yf-yi), COR(255,255,255));
        //Mat_<GRY> numero_extraido_gry((xf-xi), (yf-yi), COR(255,255,255));


        for (int l = xi; l < xf; l++){
            for (int c = yi; c < yf; c++)
            {
                numero_extraido(c - yi, l - xi) = imagem(c, l);   

            }
        }
        //imagem_numero_flt = MakeBbox(imagem_numero_flt, nlado);
        //resize(imagem_numero_flt, imagem_numero_flt, Size(nlado, nlado), 0, 0, INTER_AREA);
        converte(numero_extraido, imagem_numero_flt);

        calcula_numero = false;

        bool calcula_bbox = false;

        for (int l = 0; l < imagem_numero_flt.rows; l++){
            for (int c = 0; c < imagem_numero_flt.cols; c++)
            {

                if (imagem_numero_flt(l,c) < 0.2){  //limiar para trasnformar em preto. Antes 0.35
                    imagem_numero_flt(l,c) = 0.0;
                    calcula_bbox = true;
                    calcula_numero = true;
                } else if (imagem_numero_flt(l,c) > 0.3){ //limiar para trasnformar em branco Antes 0.5
                    imagem_numero_flt(l,c) = 1.0;
                }
            }
        }

        if (calcula_bbox){
            imagem_numero_flt = MakeBbox(imagem_numero_flt, nlado);
        }


        //mostra(imagem_numero_flt);

    }

}

void desenha_retangulo(int tamanho){
        Mat_<FLT> templ = imagens_referencia[referencia_escolhida]; 
        resize(templ, templ, Size(tamanho, tamanho), 0, 0, INTER_AREA);
				
		rectangle( imagem,  Point( matchLoc.x + 10 , matchLoc.y + 10 ), 
        Point( matchLoc.x + templ.cols - 10 , matchLoc.y + templ.rows - 10 ), Scalar(0,255,255), 1.5, 8);

        //mostra(imagem_original);
        extrai_numero(tamanho, matchLoc.x + 16, matchLoc.y + 16, matchLoc.x + templ.cols - 16, matchLoc.y + templ.rows - 16);
 
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

    VideoWriter vo(argv[2], CV_FOURCC('X','V','I','D'), 10, Size(440,240));

    Mat_<COR> imagem_original_aux(240, 440, COR(0,0,255));
    int novo_tamanho = 0; //Variavel que definira melhor tamanho para o tamplate matching
    int ultimo_tamanho = 0;
    int distanciaTemplates = 900;
    int distanciaLastFrame = 400;

	COR cinza(128,128,128);
    COR vermelho(0, 0, 255);
    COR azul(255, 0, 0);

	if (argc!=3) 
		erro("client6 servidorIpAddr\n");
	CLIENT client(argv[1]);
	Mat_<COR> image;
	vector<BYTE> vb;
	namedWindow("janela");
	//resizeWindow("janela", 2 * imagem.cols, 2 * imagem.rows);
    setMouseCallback("janela", on_mouse);
    imshow("janela", imagem);
	int ch;

    //Le base de dados
    mnist.le("mnist");
    flann::Index ind(mnist.ax,flann::KDTreeIndexParams(4));

    vb.assign(100000,111);
    
	ch = -1;
	while (ch < 0) {
        atraso = 0;
		//client.receiveInt(estadoRecebido);
		client.receiveImgComp(image);

        for (int l = 0; l < 240; l++){
            for(int c = 0; c < 440; c++){
                imagem(l,c) = cinza;
            }
        }

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

        if ( imagem_ultimo_frame == true){ // Se viu no ultimo frame abaixa o limiar e ve se quadrado esta proximo 
            if (((maxLoc.x - last_loc.x) * (maxLoc.x - last_loc.x) + 
                        (maxLoc.y - last_loc.y) * (maxLoc.y - last_loc.y)) < distanciaLastFrame && maxVal > 0.10){

                resize(imagens_referencia[0], imagem_referencia_temp, Size(novo_tamanho, novo_tamanho), 0, 0, INTER_AREA);
                matchTemplate(imagem_original_flt, imagem_referencia_temp, resultado_norm, CV_TM_CCOEFF_NORMED);
                minMaxLoc( resultado_norm, &minVal_norm, &maxVal_norm, &minLoc_norm, &maxLoc_norm );
                
                //Se a posicao dos dois templates esta proxima. d^2 = a^2 + b^2
                if (((maxLoc.x - maxLoc_norm.x) * (maxLoc.x - maxLoc_norm.x) + 
                        (maxLoc.y - maxLoc_norm.y) * (maxLoc.y - maxLoc_norm.y)) < distanciaTemplates && maxVal_norm > 0.10){
                    
                    if (maxVal > maxVal_norm) {
                        matchLoc = maxLoc;
                    } else {
                        matchLoc = maxLoc_norm;
                    }

                    imagem_ultimo_frame = true;
                    last_loc = matchLoc;
                    desenha_retangulo(novo_tamanho); //Funcao que desenha retangulo na imagem de saida
                    
                    if(calcula_numero){

                        

                        vector<int> indices(1);
                        vector<float> dists(1);
                        ind.knnSearch( imagem_numero_flt.row(0), indices, dists, 1);
                        qp2 = mnist.ay(indices[0]);

                        //cout << qp2; //qual numero é
                        //cout << "-";

                        putTxt(imagem, 20, 400, to_string(qp2));
                    }
                
                
                } else {
                    imagem_ultimo_frame = false;
                }  
            } else {
                imagem_ultimo_frame = false;
            }
            } else { // Se nao viu no ultimo frame
                
                if (maxVal > 0.12){

                    resize(imagens_referencia[0], imagem_referencia_temp, Size(novo_tamanho, novo_tamanho), 0, 0, INTER_AREA);
                    matchTemplate(imagem_original_flt, imagem_referencia_temp, resultado_norm, CV_TM_CCOEFF_NORMED);
                    minMaxLoc( resultado_norm, &minVal_norm, &maxVal_norm, &minLoc_norm, &maxLoc_norm );
                    
                    //Se a posicao dos dois templates esta proxima. d^2 = a^2 + b^2
                    if (((maxLoc.x - maxLoc_norm.x) * (maxLoc.x - maxLoc_norm.x) + 
                            (maxLoc.y - maxLoc_norm.y) * (maxLoc.y - maxLoc_norm.y)) < distanciaTemplates && maxVal_norm > 0.12){
                        
                        if (maxVal > maxVal_norm) {
                            matchLoc = maxLoc;
                        } else {
                            matchLoc = maxLoc_norm;
                        }

                        imagem_ultimo_frame = true;
                        last_loc = matchLoc;
                        desenha_retangulo(novo_tamanho); //Funcao que desenha retangulo na imagem de saida
                        
                        if (calcula_numero){
                            

                            vector<int> indices(1);
                            vector<float> dists(1);
                            ind.knnSearch( imagem_numero_flt.row(0), indices, dists, 1);
                            qp2 = mnist.ay(indices[0]);

                            //cout << qp2; //qual numero é
                            //cout << "-";

                            putTxt(imagem, 20, 400, to_string(qp2));
                        }

                    } else {
                        imagem_ultimo_frame = false;
                    }  
                } else {
                    imagem_ultimo_frame = false;
                }
            }
        
            

        //putTxt(imagem, 140, 150, to_string(resultado));
        putTxt(imagem, 140, 250, to_string(maxVal_norm));
        putTxt(imagem, 140, 350, to_string(maxVal));


        velE = 0; velD = 0;
        //estado varia de acordo com o que for clicado no Mouse
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

                //maquina de estados principal
                switch (maquinaEstados){

                    case 0:
                    //Segue placa até chegar perto
                        if (virou == true){
                            aproximando = 0;
                            centralizar = 1;
                            busca = 0;
                            virou = false;
                            velE = 0; velD = 0;
                            aux == 0;
                            atraso = 100;
                            //atraso = 150;
                        } else if (aproximou == true){ //Para avancar em toquinhos
                            aproximando = 1;
                            centralizar = 0;
                            busca = 0;
                            aproximou = false;
                            velE = 0; velD = 0;
                            aux == 0;
                            atraso = 100;
                            //atraso = 150;
                        } else if (matchLoc.x + novo_tamanho/2 >= 355 && imagem_ultimo_frame == true){ //direita
                            aproximando = 0;
                            centralizar = 1;
                            busca = 0;
                            velE = 55; velD = 0;
                            //atraso = 125;
                            virou = true; 
                            aux == 0;
                        } else if (matchLoc.x + novo_tamanho/2 >= 120 && matchLoc.x + novo_tamanho/2 <= 265 && imagem_ultimo_frame == true){
                            aproximando = 0;
                            centralizar = 1;
                            busca = 0;
                            velE = 0; velD = 55;
                            virou = true; 
                            aux == 0;
                        } else if (matchLoc.x + novo_tamanho/2 >= 265 && matchLoc.x + novo_tamanho/2 <= 355 && imagem_ultimo_frame == true){
                            aproximou = true;
                            aproximando = 1;
                            centralizar = 0;
                            busca = 0;
                            velE = 50; velD = 50;
                            aux == 0;

                        } else {
                            aproximando = 0;
                            centralizar = 0;
                            busca = 1;
                            if (true){
                                if (aux2 == 10){
                                    aux2 = 0;
                                    aux = 1;
                                } else {
                                    velE = 0; velD = 0;
                                    atraso = 100;
                                    aux = 0;
                                    aux2++;
                                }
                            
                            } else if (aux == 1){
                                velE = 55; velD = 55;
                                //velE = 0; velD = 0;
                                aux = 2;
                            } else if(aux == 2){
                                velE = 0; velD = 0;
                                atraso = 1250;
                                aux = 3;
                            } else if(aux == 3){
                                velE = 55; velD = -55;
                                aux = 4;
                            } else if(aux == 4){
                                velE = 0; velD = 0;
                                atraso = 1250;
                                aux = 5;
                            } else if(aux == 5){
                                velE = -55; velD = 55;
                                aux = 6;
                            } else if(aux == 6){
                                velE = 0; velD = 0;
                                atraso = 1250;
                                aux = 7;
                            } else if(aux == 7){
                                velE = -55; velD = 55;
                                aux = 8;
                            } else if(aux == 8){
                                velE = 0; velD = 0;
                                atraso = 1250;
                                aux = 9;
                            } else if(aux == 9){
                                velE = 55; velD = -55;
                                aux = 10;
                            } else if(aux == 10){
                                velE = 0; velD = 0;
                                atraso = 1250;
                                aux = 0;
                            }
                        }

                        if (calcula_numero) { // Se chegou perto vai calcular numero e esperar 1s e mudar de estado
                            velE = 0; velD = 0;
                            atraso = 2000;
                            maquinaEstados = 1;
                        }

                        distanciaTemplates = 400;
                        distanciaLastFrame = 400;

                        break;

                    case 1:
                        //ja esperou 1 segundo e agr vai ler o o digito
                        //cout << qp2 << " - ";
                        if (qp2 == 0){
                            velE = 0; velD = 0;
                        } else if (qp2 == 1){
                            velE = 0; velD = 0;
                        } else if (qp2 == 2){ //180 graus para a esquerda
                            velE = -60; velD = 60;
                            atraso = 600;
                        } else if (qp2 == 3){
                            velE = 60; velD = -60;//180 graus para a direita //CERTO
                            atraso = 600;
                        } else if (qp2 == 4){
                            velE = 58; velD = 60;//frente
                            atraso = 2500;
                        } else if (qp2 == 5){
                            velE = 58; velD = 60;//frente
                            atraso = 2500;
                        } else if (qp2 == 6){ //90 graus a esquerda
                            velE = -60; velD = 60;
                            atraso = 150;
                        } else if (qp2 == 7){  //90 graus a esquerda // CERTO
                            velE = -60; velD = 60;
                            atraso = 175;
                        } else if (qp2 == 8){ //90 graus a direita
                            velE = 60; velD = -60;
                            atraso = 175;
                        } else if (qp2 == 9){ //90 graus a direita
                            velE = 60; velD = -60;
                            atraso = 175;
                        }
                        distanciaTemplates = 400;
                        distanciaLastFrame = 400;

                        if (qp2 == 7 || qp2 == 6){
                            maquinaEstados = 2;

                        } else {
                            maquinaEstados = 4;
                        }

                        break;

                    case 2:
                            
                        velE = 0; velD = 0;//frente
                        atraso = 500;
                        maquinaEstados = 3;
                        break;
                    
                    case 3:
                            
                        velE = 60; velD = 60;//frente
                        atraso = 500;
                        maquinaEstados = 4;
                        break;

                    case 4: //depois de executar a acao de cima, fica em uma pausa parado antes de iniciar o movimento denovo
                        velE = 0; velD = 0;
                        maquinaEstados = 0;
                        atraso = 2000;
                        virou == false;
                        ultimo_tamanho = 0;
                        novo_tamanho = 0;
                        imagem_ultimo_frame = false;
                        matchLoc.x = 0;
                        matchLoc.y = 0;
                        last_loc.x = 0;
                        last_loc.y = 0;
                        matchLoc_norm.x = 0;
                        matchLoc_norm.y = 0;

                        distanciaTemplates = 10000;
                        distanciaLastFrame = 10000;

                        for (int l = 0; l < imagem_numero_cor.rows; l++){
                            for (int c = 0; c < imagem_numero_cor.cols; c++)
                            {
                                imagem_numero_cor(l,c) = cinza;
                            }
                        }
                        
                        converte(imagem_numero_cor, imagem_numero_flt);

                        break;

                }


                
		        //velE = 0; velD = 0;
            break;
		}

        //Desenha linhas auxiliares
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


        converte(imagem_numero_flt, imagem_numero_cor);

        for (int l = 0; l < imagem_numero_cor.rows; l++){
            for (int c = 0; c < imagem_numero_cor.cols; c++)
            {
                imagem(l,c) = imagem_numero_cor(l,c);
            }
        }

        calcula_numero = false;
        matchLoc.x = 0;
        matchLoc.y = 0;

        novo_tamanho = 0;
   
   
        putTxt(imagem, 20, 150, to_string(maquinaEstados));
        if(maquinaEstados == 0){
            if(busca){
                putTxt(imagem, 50, 150, "Buscando");
            }else if(aproximando){
                putTxt(imagem, 50, 150, "Aproximando");
            }else if(centralizar){
                putTxt(imagem, 50, 150, "Centralizando");
            }
        } else if(maquinaEstados == 1){
            putTxt(imagem, 50, 150, "Realizando Tarefa");
        }
        

		putTxt(imagem, 150, 150, to_string(estado));

		imshow("janela",imagem);
        vo << imagem;
		//client.sendInt(estado);
        client.sendInt(velE);
        client.sendInt(velD);
		client.sendVb(vb);
        client.sendInt(atraso);
		ch = waitKey(10);
	}
	
	vb.assign(100000,222);
	client.sendVb(vb);

}

