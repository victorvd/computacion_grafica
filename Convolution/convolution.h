#include <iostream>
#include <vector>

typedef std::vector<std::vector<float>> vectwod;
typedef std::vector<std::vector<std::vector<float>>> vecthreed;

//Función convolución---------------------------------------------------------------------------------------------
vecthreed convolve(const vecthreed &image, const vectwod &kernel)
{
	int imageSizeY = image.size();
	int imageSizeX = image[0].size();
	int channels = image[0][0].size();
	int kernelSizeY = kernel.size();
	int kernelSizeX = kernel[0].size();
	int newImageY = imageSizeY - kernelSizeY + 1;
	int newImageX = imageSizeX - kernelSizeX + 1;
	int sum;
	
	int kCenterX = kernelSizeY / 2;
	int kCenterY = kernelSizeX / 2;

	vecthreed imConvolved(newImageY, vectwod(newImageX, std::vector<float>(channels)));

	//Bucle para convolución---------------------------------------------------------------------
	for (int c=0 ; c<channels ; ++c) {
		for (int i=0 ; i<newImageY ; ++i) {
			for (int j=0 ; j<newImageX ; ++j) {
				sum = 0;
				for (int h=i ; h<i+kernelSizeY ; ++h) {
					for (int w=j ; w<j+kernelSizeX ; ++w) {
						sum += kernel[h-i][w-j]*image[h][w][c];
					}
				}
				imConvolved[i][j][c] = sum;
			}
		}
	}

	return imConvolved;
}

/*std::vector<std::vector<std::vector<float>>> convolve(std::vector<std::vector<std::vector<float>>> &image, std::vector<std::vector<float>> &kernel, int times)
{
	std::vector<std::vector<std::vector<float>>> newImage = image;
	
	for(int i=0 ; i<times ; i++) {
		newImage = convolve(newImage, kernel);
	}
	
	return newImage;
}*/


//Función padding valor constante---------------------------------------------------------------------------------
vecthreed constPad(const vecthreed &image,const vectwod &kernel,int cons)
{
	int imageSizeY = image.size();
	int imageSizeX = image[0].size();
	int channels = image[0][0].size();
	int kernelSizeY = kernel.size();
	int kernelSizeX = kernel[0].size();	
	int kCenterY = kernelSizeY / 2;
	int kCenterX = kernelSizeX / 2;
	
	int newImageX = kCenterX*2 + imageSizeX;
	int newImageY = kCenterY*2 + imageSizeY;
	
	//inicializar y poblar matriz para resultado--------------------------------------------------
	vecthreed imgPad(newImageY, vectwod(newImageX, std::vector<float>(channels)));
	
	for (int c=0 ; c<channels ; ++c) {
		for (int i=0 ; i<newImageY ; ++i) {
			for (int j=0 ; j<newImageX ; ++j) {
				imgPad[i][j][c] = cons;
			}
		}
	}
	
	//Bucle centrar la imagen original dentro de la nueva imagen------------------------------------
	for (int c=0 ; c<channels ; ++c) {
		for (int i=kCenterY ; i<kCenterY + imageSizeY ; ++i) {
			for (int j=kCenterX ; j<kCenterX + imageSizeX ; ++j) {
				imgPad[i][j][c] = image[i-kCenterY][j-kCenterX][c];
			}
		}
	}
	
	return imgPad;	
}


//Función padding expansiones-----------------------------------------------------------------------------------
vecthreed expPad(const vecthreed &image,const vectwod &kernel)
{
	int imageSizeY = image.size();
	int imageSizeX = image[0].size();
	int channels = image[0][0].size();
	int kernelSizeY = kernel.size();
	int kernelSizeX = kernel[0].size();	
	int kCenterY = kernelSizeY / 2;
	int kCenterX = kernelSizeX / 2;
	
	int newImageX = kCenterX*2 + imageSizeX;
	int newImageY = kCenterY*2 + imageSizeY;

	//inicializar y poblar matriz para resultado--------------------------------------------------	
	vecthreed imgPad(newImageY, vectwod(newImageX, std::vector<float>(channels)));
	
	for (int c=0 ; c<channels ; ++c) {
		for (int i=kCenterY ; i<kCenterY + imageSizeY ; ++i) {
			for (int j=kCenterX ; j<kCenterX + imageSizeX ; ++j) {
				imgPad[i][j][c] = image[i-kCenterY][j-kCenterX][c];
			}
		}
	}
	
	//Bucle para rellenar los bordes de la imagen--------------------------------------------------
	
	//Bordes horizontales
	for (int r=1 ; r<=kCenterY ; ++r) {
		for (int c=0 ; c<channels ; ++c) {
			for (int j=0 ; j<newImageX ; ++j) {
				imgPad[kCenterY-r][j][c] = imgPad[kCenterY-r+1][j][c];
				imgPad[imageSizeY+kCenterY+r-1][j][c] = imgPad[imageSizeY+kCenterY+r-2][j][c];
			}
		}
	}
	
	//Bordes verticales
	for (int r=1 ; r<=kCenterX ; ++r) {
		for (int c=0 ; c<channels ; ++c) {
			for (int i=0 ; i<newImageY ; ++i) {
				imgPad[i][kCenterX-r][c] = imgPad[i][kCenterX-r+1][c];
				imgPad[i][imageSizeX+kCenterX+r-1][c] = imgPad[i][imageSizeX+kCenterX+r-2][c];
			}
		}
	}
	
	return imgPad;
}

//Función padding espejo---------------------------------------------------------------------------------------
vecthreed mirrPad(const vecthreed &image,const vectwod &kernel)
{
	int imageSizeY = image.size();
	int imageSizeX = image[0].size();
	int channels = image[0][0].size();
	int kernelSizeY = kernel.size();
	int kernelSizeX = kernel[0].size();	
	int kCenterY = kernelSizeY / 2;
	int kCenterX = kernelSizeX / 2;
	
	int newImageX = kCenterX*2 + imageSizeX;
	int newImageY = kCenterY*2 + imageSizeY;

	//inicializar y poblar matriz para resultado--------------------------------------------------	
	vecthreed imgPad(newImageY, vectwod(newImageX, std::vector<float>(channels)));
	
	for (int c=0 ; c<channels ; ++c) {
		for (int i=kCenterY ; i<kCenterY + imageSizeY ; ++i) {
			for (int j=kCenterX ; j<kCenterX + imageSizeX ; ++j) {
				imgPad[i][j][c] = image[i-kCenterY][j-kCenterX][c];
			}
		}
	}
	
	//Bucle para rellenar los bordes de la imagen--------------------------------------------------
	
	//Bordes horizontales
	for (int r=1 ; r<=kCenterY ; ++r) {
		for (int c=0 ; c<channels ; ++c) {
			for (int j=0 ; j<newImageX ; ++j) {
				imgPad[kCenterY-r][j][c] = imgPad[kCenterY+r+1][j][c];
				imgPad[imageSizeY+kCenterY+r-1][j][c] = imgPad[imageSizeY+kCenterY-r-1][j][c];
			}
		}
	}

	//Bordes verticales
	for (int r=1 ; r<=kCenterX ; ++r) {
		for (int c=0 ; c<channels ; ++c) {
			for (int i=0 ; i<newImageY ; ++i) {
				imgPad[i][kCenterX-r][c] = imgPad[i][kCenterX+r+1][c];
				imgPad[i][imageSizeX+kCenterX+r-1][c] = imgPad[i][imageSizeX+kCenterX-r-1][c];
			}
		}
	}
	
	return imgPad;
}

//Función padding repeticiones periódicas------------------------------------------------------------------------------------
vecthreed repPad(const vecthreed &image,const vectwod &kernel)
{
	int imageSizeY = image.size();
	int imageSizeX = image[0].size();
	int channels = image[0][0].size();
	int kernelSizeY = kernel.size();
	int kernelSizeX = kernel[0].size();	
	int kCenterY = kernelSizeY / 2;
	int kCenterX = kernelSizeX / 2;
	
	int newImageX = kCenterX*2 + imageSizeX;
	int newImageY = kCenterY*2 + imageSizeY;

	//inicializar y poblar matriz para resultado--------------------------------------------------	
	vecthreed imgPad(newImageY, vectwod(newImageX, std::vector<float>(channels)));
	
	for (int c=0 ; c<channels ; ++c) {
		for (int i=kCenterY ; i<kCenterY + imageSizeY ; ++i) {
			for (int j=kCenterX ; j<kCenterX + imageSizeX ; ++j) {
				imgPad[i][j][c] = image[i-kCenterY][j-kCenterX][c];
			}
		}
	}
	
	//Bucle para rellenar los bordes de la imagen--------------------------------------------------

	//Bordes verticales
	for (int r=1 ; r<=kCenterX ; ++r) {
		for (int c=0 ; c<channels ; ++c) {
			for (int i=0 ; i<newImageY ; ++i) {
				imgPad[i][kCenterX-r][c] = imgPad[i][kCenterX+imageSizeX-r][c];
				imgPad[i][imageSizeX+kCenterX+r-1][c] = imgPad[i][kCenterX+r-1][c];
			}
		}
	}
	
	//Bordes horizontales
	for (int r=1 ; r<=kCenterY ; ++r) {
		for (int c=0 ; c<channels ; ++c) {
			for (int j=0 ; j<newImageX ; ++j) {
				imgPad[kCenterY-r][j][c] = imgPad[kCenterY+imageSizeY-r][j][c];
				imgPad[imageSizeY+kCenterY+r-1][j][c] = imgPad[kCenterY+r-1][j][c];
			}
		}
	}
	
	return imgPad;
}
