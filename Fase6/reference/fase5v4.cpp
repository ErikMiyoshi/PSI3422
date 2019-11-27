#include <cekeikon.h>
#include <vector>
#include <iostream>

using namespace std;

double minVal; double maxVal;
Point minLoc; Point maxLoc; Point matchLoc;

double minVal_norm; double maxVal_norm;
Point minLoc_norm; Point maxLoc_norm; Point matchLoc_norm; Point last_loc;

int center_x = 0;
int center_y = 0;
int referencia_escolhida = 0; //Determinara a referencia escolhida

size_t size = 1; //Numero de imagens de amostra. Tamanho do vector
std::vector<Mat_<FLT>> imagens_referencia(size); //Cria vetor para imagens de amostra



Mat_<COR> imagem_original;
Mat_<COR> imagem_output(240, 440, COR(0,0,0));  
Mat_<FLT> resultado;
Mat_<FLT> resultado_norm;
Mat_<FLT> imagem_original_flt;
Mat_<FLT> imagem_numero_flt;
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

//320x240
int melhor_template_nnorm(int ultimo_tamanho){ // Acha o tamanho ideal da imagem de referencia
	
	int tamanho = 100; //variavel que definira tamanho do template
    int minimo = 30;   //minimo tamanho a percorrer
    int maximo = 130;  //maximo tamanho a percorrer
    float max_value_aux = 0.0;  //Usado para comparar qual a maior correlacao com a imagem e o template
    Mat_<FLT> imagem_referencia_temp;

    for(int ref = 0; ref < size; ref++){ //Percorre todas as imagens de amostra

        if (ultimo_tamanho < 15) {
            minimo = 15; //Para imagens pequenas x<=100
            maximo = 60;
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
    
    if ((xf-xi) > 14 && ((yf-yi) > 14)){ //tamanho do quadro minimo para ler numero
        Mat_<COR> numero_extraido((xf-xi), (yf-yi), COR(255,255,255));
        //Mat_<GRY> numero_extraido_gry((xf-xi), (yf-yi), COR(255,255,255));


        for (int l = xi; l < xf; l++){
            for (int c = yi; c < yf; c++)
            {
                numero_extraido(c - yi, l - xi) = imagem_output(c, l);   
                //printf("[0] = %d, ", numero_extraido(l,c)[0]);
                //printf("[1] = %d, ", numero_extraido(l,c)[1]);
                //printf("[2] = %d, \n", numero_extraido(l,c)[2]);
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
                //printf(" l = %d,  c= %d =", l,c);
                //printf("[2] = %f, \n", imagem_numero_flt(l,c));
                if (imagem_numero_flt(l,c) < 0.2){ //limiar para trasnformar em preto
                    imagem_numero_flt(l,c) = 0.0;
                    calcula_bbox = true;
                    calcula_numero = true;
                } else if (imagem_numero_flt(l,c) > 0.3){ //limiar para trasnformar em branco
                    imagem_numero_flt(l,c) = 1.0;
                }
            }
        }

        if (calcula_bbox){
            imagem_numero_flt = MakeBbox(imagem_numero_flt, nlado);
        }

        //mostra(imagem_numero_flt);
    

        //putTxt(frame_cor, 14, 28, to_string(qp2));
        //vo << frame_cor;
    }

}

void desenha_retangulo(int tamanho){
        Mat_<FLT> templ = imagens_referencia[referencia_escolhida];
       
        resize(templ, templ, Size(tamanho, tamanho), 0, 0, INTER_AREA);

				
		rectangle( imagem_output,  Point( matchLoc.x + 10 , matchLoc.y + 10 ), 
        Point( matchLoc.x + templ.cols - 10 , matchLoc.y + templ.rows - 10 ), Scalar(0,255,255), 1.5, 8);

        //mostra(imagem_original);
        extrai_numero(tamanho, matchLoc.x + 16, matchLoc.y + 16, matchLoc.x + templ.cols - 16, matchLoc.y + templ.rows - 16);
 
}




int main(int argc, char* argv[]) {  

    COR cinza(128,128,128);
    COR vermelho(0, 0, 255);
    COR azul(255, 0, 0);

     // Avisa problemas de erro de sintaxe se o numero de argumentos nao for 2
     // Para imagem inexistente, mensagem amigavel ja eh gerada
    if(argc != 4){
        cout << " ep1: Detecta placa proibido virar" << endl;
        cout << " sintaxe: fase3 entrada.avi quadrado.png saida.avi" << endl;
        cout << " Erro: Numero de argumentos invalido" << endl;

    // Programa Principal
    } else {

        // Descomentar para medir o tempo de execucao. Linhas 197, 198 e 199 tambem
        // TimePoint t1=timePoint();

        string file = argv[1];
        VideoCapture vi(file);

        float fps = vi.get(CV_CAP_PROP_FPS);
        int nc = vi.get(CV_CAP_PROP_FRAME_WIDTH);
        int nl = vi.get(CV_CAP_PROP_FRAME_HEIGHT);
        int frames = vi.get(CV_CAP_PROP_FRAME_COUNT);
    
        /// Creat new video
        file.erase( file.end()-4, file.end() );
        file = argv[3];
        VideoWriter vo(file, CV_FOURCC('X','V','I','D'), fps, Size(440,240));

        //Mat_<COR> src;
        //Mat_<GRY> src_gray;

        cout << frames << endl;

        le(imagens_referencia[0], argv[2]);
        
        //le(imagem_original_aux,"Frame1.png");
        int novo_tamanho = 0; //Variavel que definira melhor tamanho para o tamplate matching
        int ultimo_tamanho = 0;



       
        mnist.le("C:\\Users\\marco\\Desktop\\Poli\\Materias\\LabSistEletronicos\\Parte2\\Fase5\\mnist");
        flann::Index ind(mnist.ax,flann::KDTreeIndexParams(4));


        for (int i = 1; i <= frames; i++){

            // cout << i;
            // cout << "-";
            //vi >> imagem_saida;
            //vi >> imagem_original_aux;
            vi >> imagem_original;

            Mat_<COR> imagem_original_aux(240, 440, COR(0,0,255)); ;
            

        //desenha teclado virtual
        for (int l = 0; l < 240; l++){
               for(int c = 120; c < 440; c++){
                   imagem_output(l,c) = imagem_original(l, (c - 120));
                }
           } 
        for (int l = 0; l < 240; l++){
            for(int c = 0; c < 120; c++){
                imagem_output(l,c) = cinza;
                if (l == 80 || l == 160 || c == 40 || c == 80) {
                    imagem_output(l,c) = azul;
                } 
            } 
            //for ( int c = 120; c < 440; c++){
            //     if (c == 240 || c == 280 || c == 320) {
            //         imagem_output(l,c) = azul;
            //     } 
            // }
        } 
        //
        for (int l = 0; l < imagem_output.rows; l++){
            for (int c = 0; c < imagem_output.cols; c++)
            {
                imagem_original_aux(l,c) = imagem_output(l,c);
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

            // se a primeira correlação for maior que 0.18
            if ( imagem_ultimo_frame == true){ 
                if (((maxLoc.x - last_loc.x) * (maxLoc.x - last_loc.x) + 
                            (maxLoc.y - last_loc.y) * (maxLoc.y - last_loc.y)) < 400  && maxVal > 0.10){

                    resize(imagens_referencia[0], imagem_referencia_temp, Size(novo_tamanho, novo_tamanho), 0, 0, INTER_AREA);
                    matchTemplate(imagem_original_flt, imagem_referencia_temp, resultado_norm, CV_TM_CCOEFF_NORMED);
                    minMaxLoc( resultado_norm, &minVal_norm, &maxVal_norm, &minLoc_norm, &maxLoc_norm );
                    
                    //Se a posicao dos dois templates esta proxima. d^2 = a^2 + b^2
                    if (((maxLoc.x - maxLoc_norm.x) * (maxLoc.x - maxLoc_norm.x) + 
                            (maxLoc.y - maxLoc_norm.y) * (maxLoc.y - maxLoc_norm.y)) < 400 && maxVal_norm > 0.10){
                        
                        if (maxVal > maxVal_norm) {
                            matchLoc = maxLoc;
                        } else {
                            matchLoc = maxLoc_norm;
                        }

                        imagem_ultimo_frame = true;
                        last_loc = matchLoc;
                        desenha_retangulo(novo_tamanho); //Funcao que desenha retangulo na imagemd e saida
                        
                        if(calcula_numero){

                            int qp2;

                            vector<int> indices(1);
                            vector<float> dists(1);
                            ind.knnSearch( imagem_numero_flt.row(0), indices, dists, 1);
                            qp2 = mnist.ay(indices[0]);

                            cout << qp2; //qual numero é
                            cout << "-";

                            putTxt(imagem_output, 100, 28, to_string(qp2));
                        }

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
                            (maxLoc.y - maxLoc_norm.y) * (maxLoc.y - maxLoc_norm.y)) < 400 && maxVal_norm > 0.18){
                        
                        if (maxVal > maxVal_norm) {
                            matchLoc = maxLoc;
                        } else {
                            matchLoc = maxLoc_norm;
                        }

                        imagem_ultimo_frame = true;
                        last_loc = matchLoc;
                        desenha_retangulo(novo_tamanho); //Funcao que desenha retangulo na imagemd e saida
                        //cout << maxLoc << " - ";
                        if (calcula_numero){
                            int qp2;

                            vector<int> indices(1);
                            vector<float> dists(1);
                            ind.knnSearch( imagem_numero_flt.row(0), indices, dists, 1);
                            qp2 = mnist.ay(indices[0]);

                            cout << qp2; //qual numero é
                            cout << "-";

                            putTxt(imagem_output, 100, 28, to_string(qp2));
                        }

                    } else {
                        imagem_ultimo_frame = false;
                    }  
                } else {
                    imagem_ultimo_frame = false;
                }
            }

            calcula_numero = false;
            // if (i > 1055) {
            //     mostra(imagem_numero_flt);
            // }
            //mostra(imagem_output);
            vo << imagem_output;
            
        }
    //mostra(imagem_original_aux);

    cout << "\n\t" + file + " gerado com sucesso\n";
		
    }  
}

