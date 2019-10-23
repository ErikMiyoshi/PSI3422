#include <cekeikon.h>

int main(int argc, char **argv) {
	
	if(argc != 2){
		printf("Numero de argumentos invalido\n");
		erro("Erro: Numero de argumentos invalido");
	}
	VideoCapture w(0);
	VideoWriter vo(argv[1], CV_FOURCC('X','V','I','D'), 20, Size(320,240));
	if (!w.isOpened()) erro("Erro: Abertura de webcam 0.");
	Mat_<COR> a;
	namedWindow("janela");
	while (true) {
		w >> a; // get a new frame from camera
		imshow("janela",a);
		vo << a;
		int ch=(signed char)(waitKey(30)); // E necessario (signed char)
		if (ch>=0) break;
	}
}
