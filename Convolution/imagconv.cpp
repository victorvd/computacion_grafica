#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <fstream>
//#include <cassert>
#include "convolution.h"

using namespace std;
using namespace cv;

int main(int argc, char** argv)
{
	//assert(("Formato de entrada no válido", argc != 5));
	
	if (argc != 5) {
		printf("Formato: ./ImgConv <Image_Path> <Kernel_Path> <Padding_type> <ImageOut_Path>\n");
		return -1;
	}
	
	//Cargar imagen-------------------------------------------------------------------------------
	Mat image;		//Variable para almacenar imagen en formato OpenCV
	string fname;

	image = imread(argv[1], 1);
	vecthreed imVector(image.rows, vectwod(image.cols, vector<float>(image.channels())));
	
	if (!image.data) {
		printf("No se puede abrir el archivo de imagen\n");
		return -1;
	}
	else {
		//Convertir imagen en formato de vector de M canales------------------------------------------
		for (int i = 0; i < image.rows; i++) {
			for (int j = 0; j < image.cols; j++) {
				Vec3f intensity = image.at<Vec3b>(i, j);	//Implicit type conversion from cv::Vec3b to cv::Vec3f
				for(int k = 0; k < image.channels(); k++) {
					imVector[i][j][k] = intensity.val[k];
				}
			}
		}
		
		printf("\nArchivo de imagen cargado: %dx%dx%d\n",image.rows,image.cols,image.channels());
	}
	
	//Cargar kernel y pasar a vector 2D------------------------------------------------------------
	fstream file (argv[2], ios::in);

	vector<vector<float>> kernel;		//Variable para almacenar kernel
	vector<float> row;
	string line, word;

	if(file.is_open())
	{
		while(getline(file, line))
		{
			row.clear();

			stringstream str(line);

			while(getline(str, word, ','))
			row.push_back(stof(word.c_str()));
			
			kernel.push_back(row);
		}
		printf("Archivo de kernel cargado: ");
		cout<<kernel.size()<<"x"<<kernel[0].size()<<endl;
	}
	else{
		printf("No se puede abrir el archivo de kernel\n");
		return -1;
	}

	/*for(int i=0;i<kernel.size();i++)	//Imprimir Kernel
	{
		for(int j=0;j<kernel[i].size();j++)
		{
			cout<<kernel[i][j]<<" ";
		}
		cout<<"\n";
	}*/
	
	vecthreed imPadded;	//Variable para imagen con padding
	
	//Elegir el método de padding--------------------------------------------------------------------
	if(!strcmp(argv[3], "c")){		//Padding valor constante
	
		int cons;
		cout<<"Ingrese el valor de la constante: ";
		cin>>cons;
		
		imPadded = constPad(imVector,kernel,cons);
		cout<<"Padding valor constante aplicado: ";
	}
	else if(!strcmp(argv[3], "m")){		//Padding espejo
	
		imPadded = mirrPad(imVector,kernel);
		cout<<"Padding espejo aplicado: ";
		
	}
	else if(!strcmp(argv[3], "e")){		//Padding repeticiones periódicas
	
		imPadded = expPad(imVector,kernel);
		cout<<"Padding expansión aplicado: ";
		
	}
	else if(!strcmp(argv[3], "p")){		//Padding repeticiones periódicas
	
		imPadded = repPad(imVector,kernel);
		cout<<"Padding repeticiones periódicas aplicado: ";
		
	}
	else{
		cout<<"Método de padding no válido"<<endl;
		return -1;
	}
	cout<<imPadded.size()<<"x"<<imPadded[0].size()<<"x"<<imPadded[0][0].size()<<endl;
	
	/*
	cout<<imPadded.size()<<endl;
	cout<<imPadded[0].size()<<endl;
	cout<<imPadded[0][0].size()<<endl;
	*/
	
	vecthreed& imConvolved = imPadded;			//A reference(&) to the same variable
	imConvolved = convolve(imPadded,kernel);		//Variable para almacenar imagen convolucionada
	
	cout<<"Convolución aplicada: ";	
	cout<<imConvolved.size()<<"x"<<imConvolved[0].size()<<"x"<<imConvolved[0][0].size()<<endl;
	
	Mat& newImage = image;					//A reference(&) to the same OpenCV variable

	//vecthreed imConvolved = imPadded;					//Para testear los métodos	
	//Mat newImage(imConvolved.size(),imConvolved[0].size(),image.type());	//de padding sobre la imagen
	
	for (int i = 0; i < imConvolved.size(); i++) {
		for (int j = 0; j < imConvolved[0].size(); j++) {;
			Vec3f newIntensity;	//Variable para almacenar valor de píxel por canal
			for(int k = 0; k < imConvolved[0][0].size(); k++) {
				newIntensity.val[k] = imConvolved[i][j][k];
			}
			newImage.at<Vec3b>(i, j) = newIntensity;
			//cout<<newImage.at<Vec3b>(i, j)<<endl;
		}
	}
	
	//cout<<"New image size: "<<newImage.rows<<" x "<<newImage.cols<<" x "<<newImage.channels()<<endl<<endl;
	
	//Guardar y mostrar imagen resultante----------------------------------------------------------------
	string imageName = argv[4];
	
	cout<<"Guardando imagen...\n";
	imwrite(imageName, newImage);
	
	cout<<"Mostrando imagen...\n\n";
	resize(newImage, newImage, Size(newImage.cols/2, newImage.rows/2)); // to half size or even smaller
	imshow("Computación Gráfica - Trabajo extra", newImage);
	waitKey(0);
	
	return 0;
}
