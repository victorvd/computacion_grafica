#include <iostream>
#include <stdio.h>
#include <vector>
#include <assert.h>
#include <fstream>

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

typedef vector<vector<float>> vectwod;
typedef vector<vector<vector<float>>> vecthreed;


//Función convolución---------------------------------------------------------------------------------------------
vecthreed convolve(vecthreed &image, vector<vector<float>> &kernel)
{
    
	//assert(image.size()!=0 && kernel.size()!=0);

	int height = image.size();
	int width = image[0].size();
	int channels = image[0][0].size();
	int kernelHeight = kernel.size();
	int kernelWidth = kernel[0].size();
	int newImageHeight = height - kernelHeight + 1;
	int newImageWidth = width - kernelWidth + 1;
	int d,i,j,h,w;
	int sum;
	
	int kCenterX = kernelHeight / 2;
	int kCenterY = kernelWidth / 2;

	vecthreed newImage(newImageHeight, vectwod(newImageWidth, vector<float>(channels)));
	
	//Mostrar valores característicos de imagen--------------------------------------------------
	cout<<"Padded height = "<<height<<endl;
	cout<<"Padded width = "<<width<<endl;
	cout<<"Channels number = "<<channels<<endl;
	cout<<"Kernel height = "<<kernelHeight<<endl;
	cout<<"Kernel width = "<<kernelWidth<<endl;
	cout<<"ImageOut height = "<<newImageHeight<<endl;
	cout<<"ImageOut width = "<<newImageWidth<<endl<<endl;

	//Bucle para convolución---------------------------------------------------------------------
	for (d=0 ; d<channels ; ++d) {
		for (i=0 ; i<newImageHeight ; ++i) {
			for (j=0 ; j<newImageWidth ; ++j) {
				sum = 0;
				for (h=i ; h<i+kernelHeight ; ++h) {
					for (w=j ; w<j+kernelWidth ; ++w) {
						sum += kernel[h-i][w-j]*image[h][w][d];
					}
				}
				newImage[i][j][d] = sum;
			}
		}
	}

	return newImage;
}

/*vecthreed convolve(vecthreed &image, vectwod &kernel, int times)
{
	vecthreed newImage = image;
	
	for(int i=0 ; i<times ; i++) {
		newImage = convolve(newImage, kernel);
	}
	
	return newImage;
}*/


//Función padding valor constante---------------------------------------------------------------------------------
vecthreed constPad(vecthreed &image,vector<vector<float>> &kernel,int cons)
{
	int imgHeight = image.size();
	int imgWidth = image[0].size();
	int kernelHeight = kernel.size();
	int kernelWidth = kernel[0].size();
	int kCenterY = kernelHeight / 2;
	int kCenterX = kernelWidth / 2;
	int channels = image[0][0].size();
	
	int newWidth = kCenterX*2 + imgWidth;
	int newHeight = kCenterY*2 + imgHeight;
	
	//inicializar y poblar matriz para resultado--------------------------------------------------
	vecthreed imgPad(newHeight, vectwod(newWidth, vector<float>(channels)));
	
	for (int d=0 ; d<channels ; ++d) {
		for (int i=0 ; i<newHeight ; ++i) {
			for (int j=0 ; j<newWidth ; ++j) {
				imgPad[i][j][d] = cons;
			}
		}
	}
	
	//Bucle centrar la imagen original dentro de la nueva imagen------------------------------------
	for (int d=0 ; d<channels ; ++d) {
		for (int i=kCenterY ; i<kCenterY + imgHeight ; ++i) {
			for (int j=kCenterX ; j<kCenterX + imgWidth ; ++j) {
				imgPad[i][j][d] = image[i-kCenterY][j-kCenterX][d];
			}
		}
	}
	
	return imgPad;
	
}


//Función padding expansiones-----------------------------------------------------------------------------------
vecthreed expPad(vecthreed &image,vector<vector<float>> &kernel)
{
	int imgHeight = image.size();
	int imgWidth = image[0].size();
	int kernelHeight = kernel.size();
	int kernelWidth = kernel[0].size();
	int kCenterY = kernelHeight / 2;
	int kCenterX = kernelWidth / 2;
	int channels = image[0][0].size();
	
	int newWidth = kCenterX*2 + imgWidth;
	int newHeight = kCenterY*2 + imgHeight;

	//inicializar y poblar matriz para resultado--------------------------------------------------	
	vecthreed imgPad(newHeight, vectwod(newWidth, vector<float>(channels)));
	
	for (int d=0 ; d<channels ; ++d) {
		for (int i=kCenterY ; i<kCenterY + imgHeight ; ++i) {
			for (int j=kCenterX ; j<kCenterX + imgWidth ; ++j) {
				imgPad[i][j][d] = image[i-kCenterY][j-kCenterX][d];
			}
		}
	}
	
	//Bucle para rellenar los bordes de la imagen--------------------------------------------------
	
	//Bordes horizontales
	for (int r=1 ; r<=kCenterY ; ++r) {
		for (int d=0 ; d<channels ; ++d) {
			for (int j=0 ; j<newWidth ; ++j) {
				imgPad[kCenterY-r][j][d] = imgPad[kCenterY-r+1][j][d];
				imgPad[imgHeight+kCenterY+r-1][j][d] = imgPad[imgHeight+kCenterY+r-2][j][d];
			}
		}
	}
	
	//Bordes verticales
	for (int r=1 ; r<=kCenterX ; ++r) {
		for (int d=0 ; d<channels ; ++d) {
			for (int i=0 ; i<newHeight ; ++i) {
				imgPad[i][kCenterX-r][d] = imgPad[i][kCenterX-r+1][d];
				imgPad[i][imgWidth+kCenterX+r-1][d] = imgPad[i][imgWidth+kCenterX+r-2][d];
			}
		}
	}
	
	return imgPad;
}

//Función padding espejo---------------------------------------------------------------------------------------
vecthreed mirrPad(vecthreed &image,vector<vector<float>> &kernel)
{
	int imgHeight = image.size();
	int imgWidth = image[0].size();
	int kernelHeight = kernel.size();
	int kernelWidth = kernel[0].size();
	int kCenterY = kernelHeight / 2;
	int kCenterX = kernelWidth / 2;
	int channels = image[0][0].size();
	
	int newWidth = kCenterX*2 + imgWidth;
	int newHeight = kCenterY*2 + imgHeight;

	//inicializar y poblar matriz para resultado--------------------------------------------------	
	vecthreed imgPad(newHeight, vectwod(newWidth, vector<float>(channels)));
	
	for (int d=0 ; d<channels ; ++d) {
		for (int i=kCenterY ; i<kCenterY + imgHeight ; ++i) {
			for (int j=kCenterX ; j<kCenterX + imgWidth ; ++j) {
				imgPad[i][j][d] = image[i-kCenterY][j-kCenterX][d];
			}
		}
	}
	
	//Bucle para rellenar los bordes de la imagen--------------------------------------------------
	
	//Bordes horizontales
	for (int r=1 ; r<=kCenterY ; ++r) {
		for (int d=0 ; d<channels ; ++d) {
			for (int j=0 ; j<newWidth ; ++j) {
				imgPad[kCenterY-r][j][d] = imgPad[kCenterY+r+1][j][d];
				imgPad[imgHeight+kCenterY+r-1][j][d] = imgPad[imgHeight+kCenterY-r-1][j][d];
			}
		}
	}

	//Bordes verticales
	for (int r=1 ; r<=kCenterX ; ++r) {
		for (int d=0 ; d<channels ; ++d) {
			for (int i=0 ; i<newHeight ; ++i) {
				imgPad[i][kCenterX-r][d] = imgPad[i][kCenterX+r+1][d];
				imgPad[i][imgWidth+kCenterX+r-1][d] = imgPad[i][imgWidth+kCenterX-r-1][d];
			}
		}
	}
	
	return imgPad;
}

//Función padding repeticiones periódicas------------------------------------------------------------------------------------
vecthreed repPad(vecthreed &image,vector<vector<float>> &kernel)
{
	int imgHeight = image.size();
	int imgWidth = image[0].size();
	int kernelHeight = kernel.size();
	int kernelWidth = kernel[0].size();
	int kCenterY = kernelHeight / 2;
	int kCenterX = kernelWidth / 2;
	int channels = image[0][0].size();
	
	int newWidth = kCenterX*2 + imgWidth;
	int newHeight = kCenterY*2 + imgHeight;

	//inicializar y poblar matriz para resultado--------------------------------------------------	
	vecthreed imgPad(newHeight, vectwod(newWidth, vector<float>(channels)));
	
	for (int d=0 ; d<channels ; ++d) {
		for (int i=kCenterY ; i<kCenterY + imgHeight ; ++i) {
			for (int j=kCenterX ; j<kCenterX + imgWidth ; ++j) {
				imgPad[i][j][d] = image[i-kCenterY][j-kCenterX][d];
			}
		}
	}
	
	//Bucle para rellenar los bordes de la imagen--------------------------------------------------

	//Bordes verticales
	for (int r=1 ; r<=kCenterX ; ++r) {
		for (int d=0 ; d<channels ; ++d) {
			for (int i=0 ; i<newHeight ; ++i) {
				imgPad[i][kCenterX-r][d] = imgPad[i][kCenterX+imgWidth-r][d];
				imgPad[i][imgWidth+kCenterX+r-1][d] = imgPad[i][kCenterX+r-1][d];
			}
		}
	}
	
	//Bordes horizontales
	for (int r=1 ; r<=kCenterY ; ++r) {
		for (int d=0 ; d<channels ; ++d) {
			for (int j=0 ; j<newWidth ; ++j) {
				imgPad[kCenterY-r][j][d] = imgPad[kCenterY+imgHeight-r][j][d];
				imgPad[imgHeight+kCenterY+r-1][j][d] = imgPad[kCenterY+r-1][j][d];
			}
		}
	}
	
	return imgPad;
}

//----------------------------------------------------------------------------------------------------------------
//--------------------------------------------------MAIN----------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------
int main(int argc, char** argv)
{

	if (argc != 5) {
		printf("Formato: ./ImgConv <Image_Path> <Kernel_Path> <Padding_type> <ImageOut_Path>\n");
		return -1;
	}
	
	//Cargar imagen-------------------------------------------------------------------------------
	Mat image;		//Variable para almacenar imagen en formato OpenCV
	string fname;

	image = imread(argv[1], 1);
	
	if (!image.data) {
		printf("No se puede abrir el archivo de imagen\n");
		return -1;
	}
	
	//Convertir imagen en formato de vector de M canales------------------------------------------
	vecthreed mchannel(image.rows, vectwod(image.cols, vector<float>(image.channels())));
	
	for (int i = 0; i < image.rows; i++) {
		for (int j = 0; j < image.cols; j++) {
			Vec3f intensity = image.at<Vec3b>(i, j);	//Implicit type conversion from cv::Vec3b to cv::Vec3f
			for(int k = 0; k < image.channels(); k++) {
				mchannel[i][j][k] = intensity.val[k];
			}
			//cout<<image.at<Vec3b>(i, j);
		}
		//cout<<endl<<endl;
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
	
	vecthreed newImgPad;	//Variable para imagen con padding
	
	//Elegir el método de padding--------------------------------------------------------------------
	if(!strcmp(argv[3], "c")){	//Padding valor constante
	
		int cons;
		cout<<"Ingrese el valor de la constante: ";
		cin>>cons;
		
		newImgPad = constPad(mchannel,kernel,cons);
	}
	else if(!strcmp(argv[3], "m")){		//Padding espejo
	
		newImgPad = mirrPad(mchannel,kernel);
		
	}
	else if(!strcmp(argv[3], "e")){		//Padding repeticiones periódicas
	
		newImgPad = expPad(mchannel,kernel);
		
	}
	else if(!strcmp(argv[3], "p")){		//Padding repeticiones periódicas
	
		newImgPad = repPad(mchannel,kernel);
		
	}
	else{
		cout<<"Método de padding no válido"<<endl;
		return -1;
	}
	
	/*
	cout<<newImgPad.size()<<endl;
	cout<<newImgPad[0].size()<<endl;
	cout<<newImgPad[0][0].size()<<endl;
	*/
	
	vecthreed newChnImg = convolve(newImgPad,kernel);	//Variable para almacenar imagen convolucionada
	//vecthreed newChnImg = newImgPad;			//Para testear los métodos de padding sobre la imagen
	
	Mat newImage(newChnImg.size(),newChnImg[0].size(),image.type());
	Vec3f newIntensity;	//Variable para almacenar valor de píxel por canal
	
	//cout<<"New matrix size: "<<newChnImg.size()<<" x "<<newChnImg[0].size()<<" x "<<newChnImg[0][0].size()<<endl;
	
	for (int i = 0; i < newChnImg.size(); i++) {
		for (int j = 0; j < newChnImg[0].size(); j++) {;
			for(int k = 0; k < newChnImg[0][0].size(); k++) {
				newIntensity.val[k] = newChnImg[i][j][k];
			}
			newImage.at<Vec3b>(i, j) = newIntensity;
			//cout<<newImage.at<Vec3b>(i, j)<<endl;
		}
	}
	
	cout<<"New image size: "<<newImage.rows<<" x "<<newImage.cols<<" x "<<newImage.channels()<<endl<<endl;
	
	//Guardar y mostrar imagen resultante----------------------------------------------------------------
	string imageName = argv[4];
	
	imwrite(imageName, newImage);
	
	resize(newImage, newImage, Size(newImage.cols/2, newImage.rows/2)); // to half size or even smaller
	imshow("Computación Gráfica - Trabajo extra", newImage);
	waitKey(0);
	
	return 0;
}
