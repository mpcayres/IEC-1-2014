#include <stdio.h>
#include <jpeglib.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

unsigned char *img_adapt = NULL;

int largura;
int altura;
int bytes_por_pixel;
int espaco_cor = JCS_RGB;

int LeituraJPEG(char *nome_arq, int opcao){
	struct jpeg_decompress_struct dados;
	struct jpeg_error_mgr erro;

	int i, j, linha_atual, k, max = 255, min = 0, brilho, contr;
	float verm = 1, verd = 1, azul = 1, valor, sat;
	int dir_coluna = 0, esq_coluna = 0;

	JSAMPROW ptr_linha[1];
	
	FILE *arq_entr = fopen(nome_arq, "rb");
	unsigned long cont = 0;
	
	if (!arq_entr){
		printf("\033[31mErro ao abrir arquivo jpeg '%s'.\033[0m\n", nome_arq);
		return EXIT_FAILURE;
	}

	dados.err = jpeg_std_error(&erro);
	jpeg_create_decompress(&dados);
	jpeg_stdio_src(&dados, arq_entr);
	jpeg_read_header(&dados, TRUE);
	
	largura = dados.image_width;
	altura = dados.image_height;
	bytes_por_pixel = dados.num_components;

	jpeg_start_decompress(&dados);
	img_adapt = (unsigned char*)malloc((largura-dir_coluna-esq_coluna)*altura*bytes_por_pixel);
	ptr_linha[0] = (unsigned char *)malloc(largura*altura*bytes_por_pixel);

	if(opcao == 4 || opcao == 6){
		max = 0;
		min = 255;
		if (opcao == 6) opcao = 5;
		else opcao = 3;
	}

	if(opcao == 7){
		for(linha_atual=0; linha_atual < altura; linha_atual++) {
		        jpeg_read_scanlines(&dados, ptr_linha, 1);
			for(i=(largura*bytes_por_pixel)-1; i >= 0; i-=3){
				img_adapt[cont++] = ptr_linha[0][i-2];
				img_adapt[cont++] = ptr_linha[0][i-1];
				img_adapt[cont++] = ptr_linha[0][i];
			}
		}

		jpeg_finish_decompress(&dados);
		jpeg_destroy_decompress(&dados);
		free(ptr_linha[0]);
		fclose(arq_entr);
	
		return EXIT_SUCCESS;
	}

	if(opcao == 10){
		printf("\nCorte vertical:\nPosição a esquerda [0-%d]: ", largura/2);
		scanf("%d", &esq_coluna);
		while(esq_coluna > largura/2 || esq_coluna < 0){
			printf("\033[31mDigite um valor válido [0-%d]: \033[0m", largura/2);
			scanf("%d", &esq_coluna);
		}
		printf("Posição a direita[0-%d]: ", largura/2);
		scanf("%d", &dir_coluna);
		while(dir_coluna > largura/2 || dir_coluna < 0){
			printf("\033[31mDigite um valor válido [0-%d]: \033[0m", largura/2);
			scanf("%d", &dir_coluna);
		}
	}

	if(opcao == 14){
		printf("Faixa de mudança do brilho[-100 - 100]: ");
		scanf("%d", &brilho);
		while(brilho > 100 || brilho < -100){
			printf("\033[31mDigite um valor válido [-100 - 100]: \033[0m");
			scanf("%d", &brilho);
		}
	}

	if(opcao == 15){
		printf("Faixa de mudança da saturação [0 - 2.0]: ");
		scanf("%f", &sat);
		while(sat > 2 || sat < 0){
			printf("\033[31mDigite um valor válido [0 - 2.0]: \033[0m");
			scanf("%f", &sat);
		}
	}

	if(opcao == 16){
		printf("Porcentagem do contraste [-100 - 100]: ");
		scanf("%d", &contr);
		while(contr > 100 || contr < -100){
			printf("\033[31mDigite um valor válido [-100 - 100]: \033[0m");
			scanf("%d", &contr);
		}
		contr += 100;
		contr /= 100;
		contr *= contr;
	}

	if(opcao == 17){
		printf("Qual será a intesidade? [1-2] ");
		scanf("%d", &k);
		while(k > 2 || k < 1){
			printf("\033[31mDigite um valor válido [1-2]: \033[0m");
			scanf("%d", &k);
		}
		if(k == 2){
			max = 3;
			min = 85;
		}
		else{
			max = 5;
			min = 51;
		}
	}

	if(opcao == 18){
		printf("\nDigite as porcentagens[0-1.0]:\nVermelho: ");
		scanf("%f", &verm);
		while(verm > 1 || verm < 0){
			printf("\033[31mDigite um valor válido [0-1.0]: \033[0m");
			scanf("%f", &verm);
		}
		printf("Verde: ");
		scanf("%f", &verd);
		while(verd > 1 || verd < 0){
			printf("\033[31mDigite um valor válido [0-1.0]: \033[0m");
			scanf("%f", &verd);
		}
		printf("Azul: ");
		scanf("%f", &azul);
		while(azul > 1 || azul < 0){
			printf("\033[31mDigite um valor válido [0-1.0]: \033[0m");
			scanf("%f", &azul);
		}
	}

	if(opcao == 21) verm = verd = azul = 3;

	for(linha_atual=0; linha_atual < altura; linha_atual++) {
    		jpeg_read_scanlines(&dados, ptr_linha, 1);
	        for(i=(esq_coluna*bytes_por_pixel); i < ((largura-dir_coluna)*bytes_por_pixel); i+=3){
			switch(opcao){
				case 2:
					for(j=0; j<3; j++)
						img_adapt[cont++] = (float) (ptr_linha[0][i]+ptr_linha[0][i+1]+ptr_linha[0][i+2])/3;
					break;
				case 3:
					for(j=0; j<3; j++, cont++){
						img_adapt[cont] = ptr_linha[0][i];
						if(img_adapt[cont] > 128) img_adapt[cont] = max;
						else img_adapt[cont] = min;
					}
					break;
				case 5:
					for(j=0; j<3; j++, cont++){
						img_adapt[cont] = ptr_linha[0][i+j];
						if (img_adapt[cont] > 128) img_adapt[cont] = max;
						else img_adapt[cont] = min;
					}
					break;
				case 9:
					for(j=0; j<3; j++) img_adapt[cont++] = 255 - ptr_linha[0][i+j];
					break;
				case 14:
					for(j=0; j<3; j++)
						if(brilho < 0)
							img_adapt[cont++] = ptr_linha[0][i+j]+brilho >= 0 ?  ptr_linha[0][i+j]+brilho : 0;
						else
							img_adapt[cont++] = ptr_linha[0][i+j]+brilho < 255 ? ptr_linha[0][i+j]+brilho : 255;
					break;
				case 15:
					k = (float) sqrt(ptr_linha[0][i]*ptr_linha[0][i]*0.299 + ptr_linha[0][i+1]*ptr_linha[0][i]*0.587 + ptr_linha[0][i+2]*ptr_linha[0][i+2]*0.114);
					for(j=0; j<3; j++){
						valor = (k + (ptr_linha[0][i+j] - k)*sat);
						if(valor > 255) valor = 255;
						if(valor < 0) valor = 0;
						img_adapt[cont++] = valor;
					}						
					break;
				case 16:
					for(j=0; j<3; j++){
						valor = ((((ptr_linha[0][i+j]/255.0)-0.5)*contr)+0.5)*255;
						if (valor < 0) valor = 0;
						if (valor > 255) valor = 255;
						img_adapt[cont++] = valor;
					}
					break;
				case 17:
					for(j=0; j<3; j++, cont++){
						img_adapt[cont] = ptr_linha[0][i+j];
						for(k=0; k<max; k++){
							if(min*k <= img_adapt[cont] && img_adapt[cont] < min*(k+1)){
								img_adapt[cont] = ((min*k)+((k+1)*min))/2;
								break;
							}
						}
					}
					break;
				case 19:
					img_adapt[cont] = (float) 0.393*ptr_linha[0][i] + 0.769*ptr_linha[0][i+1] + 0.189*ptr_linha[0][i+2] < 255 ? 0.393*ptr_linha[0][i] + 0.686*ptr_linha[0][i+1] + 0.189*ptr_linha[0][i+2] : 255;
					cont++;
					img_adapt[cont] = (float) 0.349*ptr_linha[0][i] + 0.686*ptr_linha[0][i+1] + 0.168*ptr_linha[0][i+2] < 255 ? 0.349*ptr_linha[0][i] + 0.686*ptr_linha[0][i+1] + 0.168*ptr_linha[0][i+2] : 255;
					cont++;
					img_adapt[cont] = (float) 0.272*ptr_linha[0][i] + 0.534*ptr_linha[0][i+1] + 0.131*ptr_linha[0][i+2] < 255 ? 0.272*ptr_linha[0][i] + 0.534*ptr_linha[0][i+1] + 0.131*ptr_linha[0][i+2] : 255;
					cont++;
					break;
				case 20:
					k = sqrt(ptr_linha[0][i]*ptr_linha[0][i] + ptr_linha[0][i+1]*ptr_linha[0][i] + ptr_linha[0][i+2]*ptr_linha[0][i+2]);
					for(j=0; j<3; j++)
						img_adapt[cont++] = (k + (ptr_linha[0][i+j] - k)*2) < 255 ? (k + (ptr_linha[0][i+j] - k)*2) : 255;
					
					break;
				default:
					img_adapt[cont++] = (float) verm*ptr_linha[0][i];
					img_adapt[cont++] = (float) verd*ptr_linha[0][i+1];
					img_adapt[cont++] = (float) azul*ptr_linha[0][i+2];
					break;
			}
	        }
		i = (largura*bytes_por_pixel)-1;
	}
	
	largura -= (dir_coluna + esq_coluna);

	jpeg_finish_decompress(&dados);
	jpeg_destroy_decompress(&dados);
	free(ptr_linha[0]);
	fclose(arq_entr);
	
	return EXIT_SUCCESS;
}

int EscritaJPEG(char *nome_arq, int opcao){
	struct jpeg_compress_struct dados;
	struct jpeg_error_mgr erro;
	int linha_atual, i, j;
	float verm = 1, verd = 0.5, azul = 0;
	int inicio_linha = 0, fim_linha = 0;
	unsigned char *img_aux = NULL;
	unsigned long cont;
	
	JSAMPROW ptr_linha[1];
	FILE *arq_sai = fopen(nome_arq, "wb");
	
	if (!arq_sai)	{
		printf("\033[31mErro ao abrir arquivo de saída jpeg '%s'.\033[0m\n", nome_arq);
		return EXIT_FAILURE;
	}

	dados.err = jpeg_std_error(&erro);
	jpeg_create_compress(&dados);
	jpeg_stdio_dest(&dados, arq_sai);

	if(opcao == 10){
		printf("\nCorte horizontal:\nParte de cima [0-%d]: ", altura/2);
		scanf("%d", &inicio_linha);
		while(inicio_linha > altura/2 || inicio_linha < 0){
			printf("\033[31mDigite um valor válido [0-%d]: \033[0m", altura/2);
			scanf("%d", &inicio_linha);
		}
		printf("Parte de baixo [0-%d]: ", altura/2);
		scanf("%d", &fim_linha);
		while(fim_linha > altura/2 || fim_linha < 0){
			printf("\033[31mDigite um valor válido [0-%d]: \033[0m", altura/2);
			scanf("%d", &fim_linha);
		}
	}
	
	dados.image_width = largura;
	dados.image_height = (altura - fim_linha - inicio_linha);
	dados.input_components = bytes_por_pixel;
	dados.in_color_space = espaco_cor;

	if(opcao == 13) img_aux = (unsigned char*)malloc(largura*altura*bytes_por_pixel);	

	if(opcao == 11){
		dados.image_width = largura*4;
		dados.image_height = altura*4;
	}

	if(opcao == 12 || opcao == 13){
		dados.image_width = largura*2;
		dados.image_height = altura*2;
	}

	jpeg_set_defaults(&dados);
	jpeg_start_compress(&dados, TRUE);

	if(opcao == 8){
		for(linha_atual = (altura-1); linha_atual >= 0; linha_atual--){
			ptr_linha[0] = &img_adapt[linha_atual*largura*bytes_por_pixel];
			jpeg_write_scanlines(&dados, ptr_linha, 1);
		}
	}

	if(opcao == 11){
		for(i=0; i<4; i++){
			for(linha_atual=0; linha_atual < altura; linha_atual++){
				ptr_linha[0] = &img_adapt[linha_atual*largura*bytes_por_pixel];
				jpeg_write_scanlines(&dados, ptr_linha, 1);
			}
		}
	}

	if(opcao == 12){
		for(i=0; i<2; i++){
			for(linha_atual=0; linha_atual < altura; linha_atual++){
				ptr_linha[0] = &img_adapt[linha_atual*largura*bytes_por_pixel];
				jpeg_write_scanlines(&dados, ptr_linha, 1);
			}
		}
	}
	
	if(opcao == 13){
		for(linha_atual=0, cont = 0; linha_atual < altura; linha_atual++){
			for (j=0; j < (largura*bytes_por_pixel); j+=3){
				img_aux[cont] = img_adapt[cont];
				cont++;
				img_aux[cont] = img_adapt[cont];
				cont++;
				img_aux[cont] = img_adapt[cont];
				cont++;
			}
		}
		for(i=0; i<2; i++){
			for(linha_atual=0, cont = 0; linha_atual < altura; linha_atual++){
				for (j=0; j < (largura*bytes_por_pixel); j+=3){
						img_adapt[cont] = (float) verm*img_aux[cont];
						cont++;
						img_adapt[cont] = (float) verd*img_aux[cont];
						cont++;
						img_adapt[cont] = (float) azul*img_aux[cont];
						cont++;
					}
			}
			if(i==0) verm = 1, verd = 0.1, azul = 0.1;
			else verm = 0.1, verd = 1, azul = 0.1;
			for(linha_atual=0, cont = 0; linha_atual < altura; linha_atual++){
				for (j=0; j < (largura*bytes_por_pixel); j+=3){
					img_adapt[cont] = (float) verm*img_aux[cont];
					cont++;
					img_adapt[cont] = (float) verd*img_aux[cont];
					cont++;
					img_adapt[cont] = (float) azul*img_aux[cont];
					cont++;
				}
				ptr_linha[0] = &img_adapt[linha_atual*largura*bytes_por_pixel];
				jpeg_write_scanlines(&dados, ptr_linha, 1);
			}
			verm = 0.1, verd = 0.1, azul = 1;
		}
		free(img_aux);
	}

	if(opcao != 8 && opcao != 11 && opcao != 12 && opcao != 13){
		for(linha_atual=0; (linha_atual + inicio_linha) < (altura - fim_linha); linha_atual++){
			ptr_linha[0] = &img_adapt[(inicio_linha + linha_atual)*largura*bytes_por_pixel];
			jpeg_write_scanlines(&dados, ptr_linha, 1);
		}
	}

	jpeg_finish_compress(&dados);
	jpeg_destroy_compress(&dados);
	fclose(arq_sai);

	return EXIT_SUCCESS;
}

int main(){
	int decisao, fim;
	char entrada[50], saida[50];

	printf("\033[2J");	
	printf("\033[1;1f");	
	printf("Digite o nome do arquivo de entrada: ");
	scanf("%[^\n]s", entrada);
	strncat(entrada, ".jpg", 4);
	getchar();
	printf("Digite o nome do arquivo de saída: ");
	scanf("%[^\n]s", saida);
	strncat(saida, ".jpg", 4);

	do{	
		printf("\033[2J");	
		printf("\033[1;1f");
		printf ("\033[33m*******Editor de Imagens JPEG*******\033[0m\n\n");	
		printf("1. Normal\n2. Escala de cinza\n3. Halftone\n4. Halftone 2\n5. Cartoon\n6. Cartoon 2\n7. Imagem espelhada\n8. Inverter\n9. Negativo\n10. Redimensionar\n11. Repetição 8x\n12. Repetição 4x\n13. Pop-art\n14. Brilho\n15. Saturação\n16. Contraste\n17. Posterize\n18. Mudança de cores\n19. Sépia\n20. Psicodélico\n21. Psicodélico 2\n\033[31mDigite sua opção: \033[0m");
		scanf("%d", &decisao);
		while(decisao > 21 || decisao < 1){
			printf("\033[31mDigite um valor válido [1-21]: \033[0m");
			scanf("%d", &decisao);
		}

		if(LeituraJPEG(entrada, decisao) == EXIT_SUCCESS){
			if(EscritaJPEG(saida, decisao) == EXIT_FAILURE){
				free(img_adapt);
				return EXIT_FAILURE;
			}
		}
		else return EXIT_FAILURE;

		printf("\n\033[31mDigite 0 para sair do programa ou 1 para efetuar mais mudanças: \033[0m");
		scanf("%d", &fim);
		strcpy(entrada, saida);
	} while (fim != 0);

	free(img_adapt);
	return EXIT_SUCCESS;
}
