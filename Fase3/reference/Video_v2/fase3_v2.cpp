#include <cekeikon.h>
#include <vector>
#include <iostream>

using namespace std;

double minVal; double maxVal;
Point minLoc; Point maxLoc; Point matchLoc;

int center_x = 0;
int center_y = 0;
int referencia_escolhida = 0; //Determinara a referencia escolhida

size_t size = 1; //Numero de imagens de amostra. Tamanho do vector
std::vector<Mat_<FLT>> imagens_referencia(size); //Cria vetor para imagens de amostra

Mat_<COR> imagem_saida;
Mat_<COR> imagem_original_aux;
Mat_<COR> imagem_original; 
Mat_<FLT> resultado;
Mat_<FLT> imagem_original_flt;

//320x240
int melhor_template(){ // Acha o tamanho ideal da imagem de referencia
	
	int tamanho = 100; //variavel que definira tamanho do template
    int minimo = 30;   //minimo tamanho a percorrer
    int maximo = 130;  //maximo tamanho a percorrer
    float max_value_aux = 0.0;  //Usado para comparar qual a maior correlacao com a imagem e o template
    Mat_<FLT> imagem_referencia_temp;

    for(int ref = 0; ref < size; ref++){ //Percorre todas as imagens de amostra

        minimo = 70; //Para imagens pequenas x<=100
        maximo = 130;
      
        if (imagens_referencia[ref].cols > 100 && imagens_referencia[ref].cols <= 220){ //Para imagens medias 100<x=<220
            minimo = 30;
            maximo = 110;
        } else if (imagens_referencia[ref].cols > 220){ //Para imagens grandes x>220
            minimo = 30;
            maximo = 110;
        }      
        for (int i = minimo; i < maximo; i = i + 10){ //Para cada um dos tamanhos ve qual a melhor correlacao
            //cout << " i = " << i;
            resize(imagens_referencia[ref], imagem_referencia_temp, Size(i,i), 0, 0, INTER_AREA);
            
            matchTemplate( imagem_original_flt, imagem_referencia_temp, resultado, CV_TM_CCOEFF_NORMED );
            minMaxLoc( resultado, &minVal, &maxVal, &minLoc, &maxLoc ); //Sempre esta dando 0
            //cout << " max Val = " << maxVal;
            if (i < 85){ //Se a imagem eh pequena a correlacao deve ser bem alta para evitar erros. No caso > 0.75
                if (maxVal > 0.75){
                    if ( maxVal > max_value_aux){
                        max_value_aux = maxVal;
                        referencia_escolhida = ref;  //Atualiza o valor da amostra escolhida
                        tamanho = i;
                    }
                }
            } else { //Para o resto das imagens somente busca a que melhor se adequa
                if ( maxVal > max_value_aux){
                    max_value_aux = maxVal;
                    referencia_escolhida = ref; //Atualiza o valor da amostra escolhida
                    tamanho = i;
                }
            }
        }
    }

    //Retorna o melhor tamanho encontrado
    //cout << tamanho;
    return tamanho;
} 

void desenha_retangulo(){
        Mat_<FLT> templ = imagens_referencia[referencia_escolhida];
        line( imagem_original, Point( matchLoc.x + templ.cols/2 , matchLoc.y + templ.rows/2 + 0.02*templ.rows ), 
			Point( matchLoc.x + templ.cols/2 , matchLoc.y + templ.rows/2 - 0.02*templ.rows ), Scalar(0,255,255), 1, 8);

		line( imagem_original, Point( matchLoc.x + templ.cols/2 + 0.05*templ.rows , matchLoc.y + templ.rows/2 ), 
				Point( matchLoc.x + templ.cols/2 - 0.05*templ.rows , matchLoc.y + templ.rows/2 ), Scalar(0,255,255), 1, 8);
				
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
            
            for (int l = 0; l < imagem_original.rows; l++)
            {
                for (int c = 0; c < imagem_original.cols; c++)
                {
                    imagem_original_aux(l,c) = imagem_original(l,c);
                }
	        }
            
            //imp(imagem_original_aux, "imagem_original_aux.png");
            converte(imagem_original_aux, imagem_original_flt);
            //le(imagem_original_flt, "imagem_original_aux.png"); 

            //string nome_saida; // Variavel que determinara o nome da imagem de saida com base na direcao
            //nome_saida = argv[2];

            // Acha o template melhor se encaixa na imagem e posteriormente o tamanho de tal template
            novo_tamanho = melhor_template();
            resize(imagens_referencia[0], imagens_referencia[0], Size(novo_tamanho, novo_tamanho), 0, 0, INTER_AREA);

            //Faz o template com a imagem de referencia escolhida e ja de tamanho mais adequado
            matchTemplate( imagem_original_flt, imagens_referencia[0], resultado, CV_TM_CCOEFF_NORMED );

            //Encontra o ponto em que a semelhanca foi maxima e com isso determina o centro do circulo a ser desenhado na placa
            minMaxLoc( resultado, &minVal, &maxVal, &minLoc, &maxLoc );
            matchLoc = maxLoc;

            desenha_retangulo(); //Funcao que desenha retangulo na imagemd e saida
           
            vo << imagem_original;
        }
    //mostra(imagem_original_aux);

    cout << "\n\t" + file + " gerado com sucesso\n";
		
    }  
}

