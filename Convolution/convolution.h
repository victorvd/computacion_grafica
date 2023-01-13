
//Función convolución---------------------------------------------------------------------------------------------
std::vector<std::vector<std::vector<float>>> convolve(const std::vector<std::vector<std::vector<float>>> &image, const std::vector<std::vector<float>> &kernel)
{
    
	//assert(image.size()!=0 && kernel.size()!=0);

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

	std::vector<std::vector<std::vector<float>>> imConvolved(newImageY, std::vector<std::vector<float>>(newImageX, std::vector<float>(channels)));
	
	//Mostrar valores característicos de imagen--------------------------------------------------
	std::cout<<"Padded height = "<<imageSizeY<<std::endl;
	std::cout<<"Padded width = "<<imageSizeX<<std::endl;
	std::cout<<"Channels number = "<<channels<<std::endl;
	std::cout<<"Kernel height = "<<kernelSizeY<<std::endl;
	std::cout<<"Kernel width = "<<kernelSizeX<<std::endl;
	std::cout<<"ImageOut height = "<<newImageY<<std::endl;
	std::cout<<"ImageOut width = "<<newImageX<<std::endl<<std::endl;

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
std::vector<std::vector<std::vector<float>>> constPad(const std::vector<std::vector<std::vector<float>>> &image,const std::vector<std::vector<float>> &kernel,int cons)
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
	std::vector<std::vector<std::vector<float>>> imgPad(newImageY, std::vector<std::vector<float>>(newImageX, std::vector<float>(channels)));
	
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
std::vector<std::vector<std::vector<float>>> expPad(const std::vector<std::vector<std::vector<float>>> &image,const std::vector<std::vector<float>> &kernel)
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
	std::vector<std::vector<std::vector<float>>> imgPad(newImageY, std::vector<std::vector<float>>(newImageX, std::vector<float>(channels)));
	
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
std::vector<std::vector<std::vector<float>>> mirrPad(const std::vector<std::vector<std::vector<float>>> &image,const std::vector<std::vector<float>> &kernel)
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
	std::vector<std::vector<std::vector<float>>> imgPad(newImageY, std::vector<std::vector<float>>(newImageX, std::vector<float>(channels)));
	
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
std::vector<std::vector<std::vector<float>>> repPad(const std::vector<std::vector<std::vector<float>>> &image,const std::vector<std::vector<float>> &kernel)
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
	std::vector<std::vector<std::vector<float>>> imgPad(newImageY, std::vector<std::vector<float>>(newImageX, std::vector<float>(channels)));
	
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
