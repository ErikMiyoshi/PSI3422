#include <cekeikon.h>
#include <vector>
#include <iostream>

using namespace std;

double minVal; double maxVal;
Point minLoc; Point maxLoc; Point matchLoc;

double minVal_norm; double maxVal_norm;
Point minLoc_norm; Point maxLoc_norm; Point matchLoc_norm;

int center_x = 0;
int center_y = 0;
int referencia_escolhida = 0; //Determinara a referencia escolhida

size_t size = 1; //Numero de imagens de amostra. Tamanho do vector
std::vector<Mat_<FLT>> imagens_referencia(size); //Cria vetor para imagens de amostra


Mat_<COR> imagem_original_aux;
Mat_<COR> imagem_original; 
Mat_<FLT> resultado;
Mat_<FLT> resultado_norm;
Mat_<FLT> imagem_original_flt;
bool imagem_ultimo_frame = false;

//320x240
int melhor_template_nnorm(){ // Acha o tamanho ideal da imagem de referencia
	
	int tamanho = 100; //variavel que definira tamanho do template
    int minimo = 30;   //minimo tamanho a percorrer
    int maximo = 130;  //maximo tamanho a percorrer
    float max_value_aux = 0.0;  //Usado para comparar qual a maior correlacao com a imagem e o template
    Mat_<FLT> imagem_referencia_temp;

    for(int ref = 0; ref < size; ref++){ //Percorre todas as imagens de amostra

        minimo = 15; //Para imagens pequenas x<=100
        maximo = 60;
      
  
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

        line( imagem_original, Point( matchLoc.x + templ.cols/2 , matchLoc.y + templ.rows/2 + 0.02*templ.rows ), 
			Point( matchLoc.x + templ.cols/2 , matchLoc.y + templ.rows/2 - 0.02*templ.rows ), Scalar(0,255,255), 1, 8);

		line( imagem_original, Point( matchLoc.x + templ.cols/2 + 0.02*templ.rows , matchLoc.y + templ.rows/2 ), 
				Point( matchLoc.x + templ.cols/2 - 0.02*templ.rows , matchLoc.y + templ.rows/2 ), Scalar(0,255,255), 1, 8);
				
		rectangle( imagem_original, matchLoc, Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ), Scalar(0,255,255), 1.5, 8);
}




int main(int argc, char* argv[]) {  

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
        VideoWriter vo(file, CV_FOURCC('X','V','I','D'), fps, Size(nc,nl));

        //Mat_<COR> src;
        //Mat_<GRY> src_gray;

        cout << frames << endl;

        le(imagens_referencia[0], argv[2]);
        
        le(imagem_original_aux,"Frame1.png");
        int novo_tamanho; //Variavel que definira melhor tamanho para o tamplate matching

        for (int i = 1; i <= frames; i++){

            cout << i;
            cout << "-";
            //vi >> imagem_saida;
            //vi >> imagem_original_aux;
            vi >> imagem_original;
            
            for (int l = 0; l < imagem_original.rows; l++){
                for (int c = 0; c < imagem_original.cols; c++)
                {
                    imagem_original_aux(l,c) = imagem_original(l,c);
                }
	        }
            
            converte(imagem_original_aux, imagem_original_flt);
            
            imagens_referencia[0] = trataModelo(imagens_referencia[0], 0.9);
        
            // Acha o template melhor se encaixa na imagem e posteriormente o tamanho de tal template
            novo_tamanho = melhor_template_nnorm();
            Mat_<FLT> imagem_referencia_temp;

            resize(imagens_referencia[0], imagem_referencia_temp, Size(novo_tamanho, novo_tamanho), 0, 0, INTER_AREA);
            
            imagem_referencia_temp = somaAbsDois(dcReject(imagem_referencia_temp));

            matchTemplate( imagem_original_flt, imagem_referencia_temp, resultado, CV_TM_CCORR );

            //Encontra o ponto em que a semelhanca foi maxima e com isso determina o centro do circulo a ser desenhado na placa
            minMaxLoc( resultado, &minVal, &maxVal, &minLoc, &maxLoc );

            // se a primeira correlação for maior que 0.18
            if ( maxVal > 0.18){ 
                resize(imagens_referencia[0], imagem_referencia_temp, Size(novo_tamanho, novo_tamanho), 0, 0, INTER_AREA);
                matchTemplate(imagem_original_flt, imagem_referencia_temp, resultado_norm, CV_TM_CCOEFF_NORMED);
                minMaxLoc( resultado_norm, &minVal_norm, &maxVal_norm, &minLoc_norm, &maxLoc_norm );
                
                //Se a posicao dos dois templates esta proxima. d^2 = a^2 + b^2
                if (((maxLoc.x - maxLoc_norm.x) * (maxLoc.x - maxLoc_norm.x) + 
                        (maxLoc.y - maxLoc_norm.y) * (maxLoc.y - maxLoc_norm.y)) < 400 && maxVal_norm > 0.18){
                    
                    imagem_ultimo_frame = true;
                    if (maxVal > maxVal_norm) {
                        matchLoc = maxLoc;
                    } else {
                        matchLoc = maxLoc_norm;
                    }
                    desenha_retangulo(novo_tamanho); //Funcao que desenha retangulo na imagemd e saida
                    //cout << maxLoc << " - ";
                }   
            }


           
            vo << imagem_original;
        }
    //mostra(imagem_original_aux);

    cout << "\n\t" + file + " gerado com sucesso\n";
		
    }  
}

