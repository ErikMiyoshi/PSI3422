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
Mat_<FLT> imagem_original; 
Mat_<FLT> resultado;

//320x240
int melhor_template(){ // Acha o tamanho ideal da imagem de referencia
	
	int tamanho = 100; //variavel que definira tamanho do template
    int minimo = 70;   //minimo tamanho a percorrer
    int maximo = 130;  //maximo tamanho a percorrer
    float max_value_aux = 0.0;  //Usado para comparar qual a maior correlacao com a imagem e o template
    Mat_<FLT> imagem_referencia_temp;

    for(int ref = 0; ref < size; ref++){ //Percorre todas as imagens de amostra

        minimo = 70; //Para imagens pequenas x<=100
        maximo = 130;

        if (imagens_referencia[ref].cols > 100 && imagens_referencia[ref].cols <= 220){ //Para imagens medias 100<x=<220
            minimo = 100;
            maximo = 260;
        } else if (imagens_referencia[ref].cols > 220){ //Para imagens grandes x>220
            minimo = 20;
            maximo = 200;
        }      
        for (int i = minimo; i < maximo; i = i + 10){ //Para cada um dos tamanhos ve qual a melhor correlacao

            resize(imagens_referencia[ref], imagem_referencia_temp, Size(i,i), 0, 0, INTER_AREA);
            matchTemplate( imagem_original, imagem_referencia_temp, resultado, CV_TM_CCOEFF_NORMED );
            mostra(imagem_original);
            minMaxLoc( resultado, &minVal, &maxVal, &minLoc, &maxLoc );
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
    return tamanho;
} 

void desenha_retangulo(){
        Mat_<FLT> templ = imagens_referencia[referencia_escolhida];
        line( imagem_saida, Point( matchLoc.x + templ.cols/2 , matchLoc.y + templ.rows/2 + 0.02*templ.rows ), 
			Point( matchLoc.x + templ.cols/2 , matchLoc.y + templ.rows/2 - 0.02*templ.rows ), Scalar(0,255,255), 1, 8);

		line( imagem_saida, Point( matchLoc.x + templ.cols/2 + 0.05*templ.rows , matchLoc.y + templ.rows/2 ), 
				Point( matchLoc.x + templ.cols/2 - 0.05*templ.rows , matchLoc.y + templ.rows/2 ), Scalar(0,255,255), 1, 8);
				
		rectangle( imagem_saida, matchLoc, Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ), Scalar(0,255,255), 1.5, 8);
}


int main(int argc, char* argv[]) {  

     // Avisa problemas de erro de sintaxe se o numero de argumentos nao for 2
     // Para imagem inexistente, mensagem amigavel ja eh gerada
    if(argc != 3){
        cout << " ep1: Detecta placa proibido virar" << endl;
        cout << " sintaxe: fase3 entrada.png saida.png" << endl;
        cout << " Erro: Numero de argumentos invalido" << endl;

    // Programa Principal
    } else {

        // Descomentar para medir o tempo de execucao. Linhas 197, 198 e 199 tambem
        TimePoint t1=timePoint();


        string nome_saida; // Variavel que determinara o nome da imagem de saida com base na direcao
        nome_saida = argv[2];

        int novo_tamanho; //Variavel que definira melhor tamanho para o tamplate matching

        //Le a imagem a ser analisada e filtra o branco. Branco atrapalhava alguns template matchings
        le(imagem_saida, argv[1]);
        le(imagem_original_aux, argv[1]);

        
        //filtra_branco();

        //le(imagem_original,"imagem_filtrada.jpg");
        le(imagem_original, argv[1]);
        //mostra(imagem_original);
        //remove("imagem_filtrada.jpg"); //Apaga a imagem auxiliar filtrada
        
        //Le imagens de amostra
        le(imagens_referencia[0],"quadrado.png");
        // le(imagens_referencia[1],"quadrado.png");
        // le(imagens_referencia[2],"quadrado.png");        
        // le(imagens_referencia[3],"quadrado.png");
        // le(imagens_referencia[4],"quadrado.png");
        // le(imagens_referencia[5],"quadrado.png");
        // le(imagens_referencia[6],"quadrado.png");
        // le(imagens_referencia[7],"quadrado.png");

        // Acha o template melhor se encaixa na imagem e posteriormente o tamanho de tal template
        novo_tamanho = melhor_template();
        resize(imagens_referencia[referencia_escolhida], imagens_referencia[referencia_escolhida], Size(novo_tamanho, novo_tamanho), 0, 0, INTER_AREA);
        cout << novo_tamanho;
        //Faz o template com a imagem de referencia escolhida e ja de tamanho mais adequado
        matchTemplate( imagem_original, imagens_referencia[referencia_escolhida], resultado, CV_TM_CCOEFF_NORMED );

        //Encontra o ponto em que a semelhanca foi maxima e com isso determina o centro do circulo a ser desenhado na placa
        minMaxLoc( resultado, &minVal, &maxVal, &minLoc, &maxLoc );
        matchLoc = maxLoc;

        desenha_retangulo(); //Funcao que desenha circulo na placa

        // mostra(resultado);
        imp(imagem_saida, nome_saida);
        mostra(imagem_saida);

        //Descomentar para medir o tempo de execução
        TimePoint t2=timePoint();
        double t=timeSpan(t1,t2);
        impTempo(t1);
		
    }  
}

