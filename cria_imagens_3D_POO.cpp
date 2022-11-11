#include <iostream>
#include <fstream>
#include <climits>
#include <cstdlib>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <stack>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

/*
Autor: Igor Ferrazza Capeletti
Disciplina: Computacao Grafica
*/

//objeto 'image'
class image{
    private:
        int maxcolor;			//valor maximo de cor RGB/escala cinza = 255
        int largura;			//largura da imagem(pixels)
        int altura;			    //altura da imagem(pixels)
        int** red;			    //matriz contento a representatividade da cor Red em cada pixel
        int** green;			//matriz contento a representatividade da cor Green em cada pixel
        int** blue;			    //matriz contento a representatividade da cor Blue em cada pixel
        int** cinza;			//matriz contento a representatividade da cor Cinza em cada pixel
        int** visao_camera;
        double media_vz_3d;
        double menor_vz_3d;
        double maior_vz_3d;
        vector <float> vx;
        vector <float> vy;
        vector <float> vz;
        vector <int> f_v1;
        vector <int> f_v2;
        vector <int> f_v3;
    
    public:
        image(int altu, int larg);
        ~image();

        //metodos para manipular imagens 2D ------------------------------------------------------------
        void carrega_image_ppm_arquivo(string nome_arquivo_ler);
        void redimensiona(int larg, int altu);
        void printa_image();
        void salva_image_rgb(string nome_arquivo_salvar);
        void salva_image_cinza(string nome_arquivo_salvar);
        void put_pixel(int pos_x, int pos_y, int r, int g, int b);
        void gera_image_branca();
        void flood_fill_iterativo(int x, int y, int r, int g, int b);
        
        //metodos para manipular objetos 3D ------------------------------------------------------------
        void carrega_image_obj_3d(string nome_arquivo_ler);
        int gera_image_2d_pelo_tamanho_imagem_3d(int x_centro, int y_centro, int escala);
        void printa_image_3d();
        void inicializa_visao_camera_3d();
        void rotaciona_image_3d(double thetax, double thetay, double thetaz);
        void drawline(int opcao, int x1, int y1, int x2, int y2, int r, int g, int b);
        
        //metodo para pintura das linhas do obj 3D num obj 2D
        void pinta_linhas_das_faces_obj_3d_em_image_2d(int opcao, int centrox, int centroy, int proporcao, int r, int g, int b);
        
        void seta_faces_visao_camera(int x1, int y1, int x2, int y2, int x3, int y3, int r, int g, int b);
        
        void mergeSort(int inicio, int fim);
        void merge(int inicio, int meio, int fim);

        //metodo para pintura dos triangulos(faces) do obj 3D
        void pinta_triangulo(int x1, int y1, int x2, int y2, int x3, int y3, int r, int g, int b);
        void preenche_triangulo(int opcao, int x1, int y1, int x2, int y2, int x3, int y3, int r, int g, int b);
        void pinta_faces_triangulos_image_3d_em_image_2d(int opcao, int ocultacao, int opcao_fundo, int centrox, int centroy, int proporcao, int r_frente, int g_frente, int b_frente, int r_fundo, int g_fundo, int b_fundo);     
};


//Merge Sort para ordenar as faces atraves da media dos pontos z dos 3 vertices(menor ate maior)
void image::merge(int inicio, int meio, int fim){
    int pos1=inicio, pos2=meio+1, i=0, tam=(fim-inicio)+1;
    float mediaz1, mediaz2;
    int *aux_v1= new int[tam];
    int *aux_v2= new int[tam];
    int *aux_v3= new int[tam];
    while(pos1 <= meio && pos2 <= fim){
        mediaz1= (vz[f_v1[pos1]]+vz[f_v2[pos1]]+vz[f_v3[pos1]])/3;
        mediaz2= (vz[f_v1[pos2]]+vz[f_v2[pos2]]+vz[f_v3[pos2]])/3;
        if(mediaz1 < mediaz2) {
            aux_v1[i]= f_v1[pos1];
            aux_v2[i]= f_v2[pos1];
            aux_v3[i]= f_v3[pos1];
            pos1++;
        } else {
            aux_v1[i]= f_v1[pos2];
            aux_v2[i]= f_v2[pos2];
            aux_v3[i]= f_v3[pos2];
            pos2++;
        }
        i++;
    }
    while(i <= (tam-1)){
        if(pos2 > fim) {
            aux_v1[i]= f_v1[pos1];
            aux_v2[i]= f_v2[pos1];
            aux_v3[i]= f_v3[pos1];
            pos1++;
        } else {
            aux_v1[i]= f_v1[pos2];
            aux_v2[i]= f_v2[pos2];
            aux_v3[i]= f_v3[pos2];
            pos2++;
        }
        i++;
    }
    for(i= inicio; i <= fim; i++){
        f_v1[i]= aux_v1[i-inicio];
        f_v2[i]= aux_v2[i-inicio];
        f_v3[i]= aux_v3[i-inicio];
    }
    delete[] aux_v1;
    delete[] aux_v2;
    delete[] aux_v3;
}
void image::mergeSort(int inicio, int fim){
    if(inicio < fim){
        int meio= (fim+inicio)/2;
        mergeSort(inicio, meio);
        mergeSort(meio+1, fim);
        merge(inicio, meio, fim);
    }
}


//metodo que zera matriz para controlar as partes que serao desenhadas da imagem 3D na imagem 2D
void image::inicializa_visao_camera_3d(){
    for(int i= 0; i < altura; ++i){
        for(int j= 0; j < largura; ++j){
            visao_camera[i][j]= 0;
        }
    }
}


//METODOS para CONSTRUIR IMAGEM, REDIMENSIONAR, CARREGAR, MOSTRAR NA TELA, SALVAR e DELETAR imagens ppm ##################
//metodo construtor da imagem
image::image(int larg, int altu){
    altura= altu, largura= larg; maxcolor= 255;
    red= new int*[altura];
    green= new int*[altura];
    blue= new int*[altura];
    cinza= new int*[altura];
    visao_camera= new int*[altura];
    for(int i = 0; i < altura; ++i){
        red[i] = new int[largura];
        green[i] = new int[largura];
        blue[i] = new int[largura];
        cinza[i]= new int[largura];
        visao_camera[i]= new int[largura];
    }
}


//metodo destrutor da imagem
image::~image(){
    for(int i = 0; i < altura; ++i){
        delete[] red[i];
        delete[] green[i];
        delete[] blue[i];
        delete[] cinza[i];
        delete[] visao_camera[i];
    }
    delete[] red;
    delete[] green;
    delete[] blue;
    delete[] cinza;
    delete[] visao_camera;
    vx.clear();
    vy.clear();
    vz.clear();
    f_v1.clear();
    f_v2.clear();
    f_v3.clear();
}


//metodo para redimensionar tamanho da imagem e outros atributos da classe
void image::redimensiona(int larg, int altu){
    for(int i = 0; i < altura; ++i){
        delete[] red[i];
        delete[] green[i];
        delete[] blue[i];
        delete[] cinza[i];
        delete[] visao_camera[i];
    }
    delete[] red;
    delete[] green;
    delete[] blue;
    delete[] cinza;
    delete[] visao_camera;

    largura= larg;
    altura= altu;
    red= new int*[altura];
    green= new int*[altura];
    blue= new int*[altura];
    cinza= new int*[altura];
    visao_camera= new int*[altura];
    for(int i = 0; i < altura; ++i){
        red[i] = new int[largura];
        green[i] = new int[largura];
        blue[i] = new int[largura];
        cinza[i]= new int[largura];
        visao_camera[i]= new int[largura];
    }
    inicializa_visao_camera_3d();
}


//metodo para ler uma imagem PPM 2D(.ppm)
void image::carrega_image_ppm_arquivo(string nome_arquivo_ler){
    int i,j, max_value_pixel, largura_arq, altura_arq;
    string tipo_image;
    ifstream arq_input(nome_arquivo_ler.c_str());

    //arq_input.ignore(INT_MAX, '\n');
    arq_input >> tipo_image;
    arq_input >> largura_arq >> altura_arq;
    arq_input >> max_value_pixel;

    redimensiona(largura_arq, altura_arq);     //cria imagem com tamanho do arquivo
    maxcolor= max_value_pixel;

    for(i=0; i<altura; i++){
        for(j=0; j<largura; j++){
            arq_input >> red[i][j];
            arq_input >> green[i][j];
            arq_input >> blue[i][j];
            //cinza[i][j]= blue[i][j];
            cinza[i][j]= (int)(0.299*red[i][j] + 0.587*green[i][j] + 0.114*blue[i][j]); //pega niveis de luminosidade
        }
    }
    arq_input.close();
}


//metodo para ler um objeto 3D (.obj)
void image::carrega_image_obj_3d(string nome_arquivo_ler){
    ifstream arq_input(nome_arquivo_ler.c_str());
    string line;
    stringstream ss;
    if(arq_input.good()){       //verifica se arquivo possui dados
        while(getline(arq_input, line,'\n').good()){    //percorre todo arquivo armazenando os valores de cada linha
            ss.str("");
            ss.clear();
            ss.str(line);
            string subs;
            if(line[0] == 'v'){         // o primeiro caractere da linha indica um vertice 
                getline(ss,subs,' ');   // pula o caractere
                getline(ss,subs,' ');
                vx.push_back(stof(subs));
                getline(ss,subs,' ');
                vy.push_back(stof(subs));
                getline(ss,subs,' ');
                vz.push_back(stof(subs));
            }
            if(line[0] == 'f'){         // o primeiro caractere da linha indica uma face
                getline(ss,subs,' ');   // pula o caractere
                getline(ss,subs,' ');
                f_v1.push_back(stoi(subs)-1);	   
                getline(ss,subs,' ');
                f_v2.push_back(stoi(subs)-1);	 
                getline(ss,subs,' ');
                f_v3.push_back(stoi(subs)-1);	 
            }
        }
        arq_input.close();
    }else{
        cout << "Erro de leitura!" << endl;
    }
}


//metodo para gerar imagem 2d a partir de um objeto 3d, na escala escolhida
int image::gera_image_2d_pelo_tamanho_imagem_3d(int x_centro, int y_centro, int escala){
    int i, x, y;
    float x_maior=0.0, y_maior=0.0, x_menor=0.0, y_menor=0.0, teste1, teste2;
    for(i=0; i < vx.size();i++){
        if(vx[i]>x_maior){
            x_maior= vx[i];
        }
        if(vx[i]<x_menor){
            x_menor= vx[i];
        }
    }
    for(i=0; i < vy.size();i++){
        if(vy[i]>y_maior){
            y_maior= vy[i];
        }
        if(vy[i]<y_menor){
            y_menor= vy[i];
        }
    }
    x= (int)(((x_maior-x_menor)*escala)+x_centro);
    y= (int)(((y_maior-y_menor)*escala)+y_centro);
    if(x%2 != 0){
        x= x+1;
    }
    if(y%2 != 0){
        y= y+1;
    }
    if((x_centro-(abs(x_menor)*escala)) <= -1.0 || (y_centro-(abs(y_menor)*escala)) <= -1.0){
        cout << "\n\nErro: Escala do objeto precisa ser menor, seu tamanho passa das proporcoes declaradas pelo x_central ou y_central!\n\n";
        return -1;        
    }else{
        redimensiona(x,y);
        return 0;
    }
}


//metodo para printar na tela todos os dados lidos de uma imagem PPM (.ppm)
void image::printa_image(){
    int i,j;
    printf("P3\n%d %d\n%i\n", largura, altura, maxcolor);
    for (i = 0; i < altura; i++){
        for (j = 0; j < largura; j++){
            printf("%d %d %d ", red[i][j], green[i][j], blue[i][j]);

        }
        printf ("\n");
    }
}


//metodo para printar na tela todos os dados lidos de um objeto 3D (.obj)
void image::printa_image_3d(){
    int i=0;
    for(i=0; i < vx.size(); i++){
        cout << "v " << vx[i] << ' ' << vy[i] << ' ' << vz[i] << "\n";
    }
    for(i=0; i < f_v1.size(); i++){
        cout << "f " << f_v1[i] << ' ' << f_v2[i] << ' ' << f_v3[i] << "\n";
    }
}


//metodo para salvar imagem 2D rgb para um arquivo PPM (.ppm)
void image::salva_image_rgb(string nome_arquivo_salvar){
    ofstream arq_output(nome_arquivo_salvar.c_str());

    arq_output << "P3\n" << largura << " " << altura << "\n" << maxcolor << "\n";
    for(int i=0; i<altura; i++)
        for(int j=0; j<largura; j++){
        {
            arq_output << red[i][j] << " " << green[i][j] << " " << blue[i][j] << " ";
        }
    }
    arq_output.close();
}


//metodo para salvar imagem 2D cinza para um arquivo PPM (.ppm)
void image::salva_image_cinza(string nome_arquivo_salvar){
    ofstream arq_output(nome_arquivo_salvar.c_str());

    arq_output << "P2\n" << largura << " " << altura << "\n" << maxcolor+1 << "\n";
    for(int i=0; i<altura; i++)
        for(int j=0; j<largura; j++){
        {
            arq_output << cinza[i][j] << " ";
        }
    }
    arq_output.close();
}


//#########################################################################################################
//METODOS para MANIPULAR imagens ppm #########################################

//metodo para setar cores do pixel numa posicao da imagem
void image::put_pixel(int pos_x, int pos_y, int r=0, int g=0, int b=0){
    red[pos_x][pos_y]= r;
    green[pos_x][pos_y]= g;
    blue[pos_x][pos_y]= b;
}


//metodo para setar todos os pixels de uma imagem e deixar ela toda branca
void image::gera_image_branca(){
    for (int i = 0; i <  altura; i++){
        for (int j = 0; j < largura; j++){
            red[i][j] = 255;
            green[i][j] = 255;
            blue[i][j] = 255;
        }
    }
}


//Preenche toda uma regiao de uma cor trocando por outra cor (funciona para qualquer tamanho)
void image::flood_fill_iterativo(int x, int y, int r, int g, int b){
    if(((red[x][y] != r) || (green[x][y] != g) || (blue[x][y] != b)) && ((x>=0) && (y>=0) && (x<altura) && (y<largura))){
        int e, d, r1, g1, b1, xp, yp;
        stack<int> pilhax;
        stack<int> pilhay;

        r1= red[x][y]; 
        g1= green[x][y]; 
        b1= blue[x][y];
        pilhax.push(x);
        pilhay.push(y);

        while((!pilhax.empty()) && (!pilhay.empty())){
            xp= pilhax.top();
            yp= pilhay.top();
            pilhax.pop();
            pilhay.pop();

            red[xp][yp]= r;
            green[xp][yp]= g;
            blue[xp][yp]= b;
            e= yp-1; 
            d= yp+1;
            while((e >= 0) && (red[xp][e] == r1) && (green[xp][e] == g1) && (blue[xp][e] == b1)){
                red[xp][e]= r;
                green[xp][e]= g;
                blue[xp][e]= b;
                e= e-1;
            }
            while((d < largura) && (red[xp][d] == r1) && (green[xp][d] == g1) && (blue[xp][d] == b1)){
                red[xp][d]= r;
                green[xp][d]= g;
                blue[xp][d]= b;
                d= d+1;
            }
            if((xp+1) < altura){
                if((red[xp+1][e+1] == r1) && (green[xp+1][e+1] == g1) && (blue[xp+1][e+1] == b1)){
                    pilhax.push(xp+1);
                    pilhay.push(e+1);
                }
                if((red[xp+1][d-1] == r1) && (green[xp+1][d-1] == g1) && (blue[xp+1][d-1] == b1)){
                    pilhax.push(xp+1);
                    pilhay.push(d-1);
                }
            }
            if((xp-1) >= 0){
                if((red[xp-1][e+1] == r1) && (green[xp-1][e+1] == g1) && (blue[xp-1][e+1] == b1)){
                    pilhax.push(xp-1);
                    pilhay.push(e+1);
                }
                if((red[xp-1][d-1] == r1) && (green[xp-1][d-1] == g1) && (blue[xp-1][d-1] == b1)){
                    pilhax.push(xp-1);
                    pilhay.push(d-1);
                }
            }
        }  
    }
}


//usado no metodo abaixo
int sgn(int x){
	if (x<0)return -1;else return 1;
}

//opcao == 0: metodo para pintar uma reta na imagem
//opcao == 1: metodo nao pinta linhas na imagem nem no objeto, apenas seta 1 em matriz auxiliar referente aos pixels 
//              em que a linha seria desenhada. Este metodo e utilizado pelo metodo "seta_faces_visao_camera"
//opcao == 2: metodo para colocar uma reta na imagem, mas essa e usada especialemente para desenhar
//              as faces internas da parte traseira do objeto, mas que aparecem na visualizacao da camera
void image::drawline(int opcao, int x1, int y1, int x2, int y2, int r, int g, int b){
    int i,dx,dy,sdx,sdy,dxabs,dyabs,x,y,px,py;
    dx=x2-x1;      // the horizontal distance of the line 
    dy=y2-y1;      // the vertical distance of the line 
    dxabs=abs(dx);
    dyabs=abs(dy);
    sdx=sgn(dx);
    sdy=sgn(dy);
    x=dyabs>>1;
    y=dxabs>>1;
    px=x1;
    py=y1;
    if(dxabs>=dyabs){ // the line is more horizontal than vertical 
        for(i=0; i<dxabs; i++){
            y+=dyabs;
            if(y>=dxabs){
                y-=dxabs;
                py+=sdy;
            }
            px+=sdx;
            if(opcao == 0){ //drawline normal
                put_pixel(px, py, r, g, b);
            }else if(opcao == 1){ //drawline_faces
                visao_camera[px][py]= 1; 
            }else if(opcao == 2){   //drawline_faces_camera
                if(visao_camera[px][py] == 0){
                    put_pixel(px, py, r, g, b);
                    visao_camera[px][py]= 1;
                }else{
                    break;
                }
            }
        }
    }else{  // the line is more vertical than horizontal 
        for(i=0; i<dyabs; i++){
            x+=dxabs;
            if(x>=dyabs){
                x-=dyabs;
                px+=sdx;
            }
            py+=sdy;
            if(opcao == 0){ //drawline normal
                put_pixel(px, py, r, g, b);
            }else if(opcao == 1){ //drawline_faces
                visao_camera[px][py]= 1; 
            }else if(opcao == 2){   //drawline_faces_camera
                if(visao_camera[px][py] == 0){
                    put_pixel(px, py, r, g, b); 
                    visao_camera[px][py]= 1;
                }else{
                    break;
                }
            }
        }
    }
}


//metodo para girar objeto 3D em torno dos eixos X, Y e Z de acordo com valores recebidos (em radianos)
void image::rotaciona_image_3d(double thetax, double thetay, double thetaz){
    int i=0;
    double xt, yt, zt, vz_maior= INT_MIN, vz_menor= INT_MAX, vz_media;
    if(thetay > 0.0){
        for(i=0; i < vx.size(); i++){   //rotaciona no eixo y
            xt= vx[i]*cos(thetay)-vz[i]*sin(thetay);
            zt= vx[i]*sin(thetay)+vz[i]*cos(thetay);
            vx[i]= xt;
            vz[i]= zt;
        }
    }
    if(thetax > 0.0){
        for(i=0; i < vx.size(); i++){   //rotaciona no eixo x
            yt= vy[i]*cos(thetax)-vz[i]*sin(thetax);
            zt= vy[i]*sin(thetax)+vz[i]*cos(thetax);
            vy[i]= yt;
            vz[i]= zt;
        }
    }
    if(thetaz > 0.0){
        for(i=0; i < vx.size(); i++){   //rotaciona no eixo z
            xt= vx[i]*cos(thetaz)-vy[i]*sin(thetaz);
            yt= vx[i]*sin(thetaz)+vy[i]*cos(thetaz);
            vx[i]= xt;
            vy[i]= yt;
        }
    }
    for(i=0; i < vz.size(); i++){
        if(vz[i] > vz_maior){
            vz_maior= vz[i];
        }
        if(vz[i] < vz_menor){
            vz_menor= vz[i];
        }
    }
    vz_media= (vz_maior+vz_menor)/2;
    menor_vz_3d= vz_menor;
    media_vz_3d= (vz_media);
    maior_vz_3d= vz_maior;
}


//metodo para setar o valor 1 em cada posicao de uma matriz auxiliar referente
//a toda a area que seria preenchida em cada face(triangulo) do objeto 3D
void image::seta_faces_visao_camera(int x1, int y1, int x2, int y2, int x3, int y3, int r, int g, int b){
    int i, dx, dy, sdx, sdy, dxabs, dyabs, x, y, px, py, gx, gy;
    for(int j=0; j<3; j++){
        if(j==0){
            dx=x2-x1;
            dy=y2-y1;
            px=x1;
            py=y1;
            gx= x3;
            gy= y3;
        }else if(j==1){
            dx=x3-x2;
            dy=y3-y2;
            px=x2;
            py=y2;
            gx= x1;
            gy= y1;
        }else{
            dx=x3-x1;
            dy=y3-y1;
            px=x1;
            py=y1;
            gx= x2;
            gy= y2;
        }
        dxabs=abs(dx);
        dyabs=abs(dy);
        sdx=sgn(dx);
        sdy=sgn(dy);
        x=dyabs>>1;
        y=dxabs>>1;
        
        if(dxabs>=dyabs){ // the line is more horizontal than vertical 
            for(i=0; i<dxabs; i++){
                y+=dyabs;
                if(y>=dxabs){
                    y-=dxabs;
                    py+=sdy;
                }
                px+=sdx;
                drawline(1 ,px, py, gx, gy, r, g, b);
            }
        }else{  // the line is more vertical than horizontal 
            for(i=0; i<dyabs; i++){
                x+=dxabs;
                if(x>=dyabs){
                    x-=dyabs;
                    px+=sdx;
                }
                py+=sdy;
                drawline(1, px, py, gx, gy, r, g, b);
            }
        }
    }
}


//metodo que pinta somente as linhas das faces visiveis para frente da camera para todos os triangulos do objeto 3D na imagem 2D,
//de acordo com uma media de todos os pontos z que foi calculado no metodo de rotacao da imagem
void image::pinta_linhas_das_faces_obj_3d_em_image_2d(int opcao, int centrox, int centroy, int proporcao, int r, int g, int b){
    int i, px, py;
    if(opcao == 0){ //somente faces do objeto viradas para frente
        for(i=0; i < f_v1.size(); i++){
            if(vz[f_v1[i]] >= media_vz_3d){
                drawline(0, centrox+(vy[f_v1[i]]*proporcao), centroy+(vx[f_v1[i]]*proporcao), centrox+(vy[f_v2[i]]*proporcao), centroy+(vx[f_v2[i]]*proporcao), r, g, b);
                drawline(0, centrox+(vy[f_v2[i]]*proporcao), centroy+(vx[f_v2[i]]*proporcao), centrox+(vy[f_v3[i]]*proporcao), centroy+(vx[f_v3[i]]*proporcao), r, g, b);
                drawline(0, centrox+(vy[f_v3[i]]*proporcao), centroy+(vx[f_v3[i]]*proporcao), centrox+(vy[f_v1[i]]*proporcao), centroy+(vx[f_v1[i]]*proporcao), r, g, b);
                seta_faces_visao_camera(centrox+(vy[f_v1[i]]*proporcao), centroy+(vx[f_v1[i]]*proporcao), centrox+(vy[f_v2[i]]*proporcao), centroy+(vx[f_v2[i]]*proporcao), centrox+(vy[f_v3[i]]*proporcao), centroy+(vx[f_v3[i]]*proporcao), r, g, b);
            }
        }
        for(i=0; i < f_v1.size(); i++){
            drawline(2, centrox+(vy[f_v1[i]]*proporcao), centroy+(vx[f_v1[i]]*proporcao), centrox+(vy[f_v2[i]]*proporcao), centroy+(vx[f_v2[i]]*proporcao), r, g, b);
            drawline(2, centrox+(vy[f_v1[i]]*proporcao), centroy+(vx[f_v1[i]]*proporcao), centrox+(vy[f_v3[i]]*proporcao), centroy+(vx[f_v3[i]]*proporcao), r, g, b);
            drawline(2, centrox+(vy[f_v2[i]]*proporcao), centroy+(vx[f_v2[i]]*proporcao), centrox+(vy[f_v1[i]]*proporcao), centroy+(vx[f_v1[i]]*proporcao), r, g, b);
            drawline(2, centrox+(vy[f_v2[i]]*proporcao), centroy+(vx[f_v2[i]]*proporcao), centrox+(vy[f_v3[i]]*proporcao), centroy+(vx[f_v3[i]]*proporcao), r, g, b);
            drawline(2, centrox+(vy[f_v3[i]]*proporcao), centroy+(vx[f_v3[i]]*proporcao), centrox+(vy[f_v1[i]]*proporcao), centroy+(vx[f_v1[i]]*proporcao), r, g, b);
            drawline(2, centrox+(vy[f_v3[i]]*proporcao), centroy+(vx[f_v3[i]]*proporcao), centrox+(vy[f_v2[i]]*proporcao), centroy+(vx[f_v2[i]]*proporcao), r, g, b);
        }
    }else{  //todas as faces do objeto
        for(i=0; i < f_v1.size(); i++){
            drawline(0, centrox+(vy[f_v1[i]]*proporcao), centroy+(vx[f_v1[i]]*proporcao), centrox+(vy[f_v2[i]]*proporcao), centroy+(vx[f_v2[i]]*proporcao), r, g, b);
            drawline(0, centrox+(vy[f_v2[i]]*proporcao), centroy+(vx[f_v2[i]]*proporcao), centrox+(vy[f_v3[i]]*proporcao), centroy+(vx[f_v3[i]]*proporcao), r, g, b);
            drawline(0, centrox+(vy[f_v3[i]]*proporcao), centroy+(vx[f_v3[i]]*proporcao), centrox+(vy[f_v1[i]]*proporcao), centroy+(vx[f_v1[i]]*proporcao), r, g, b);
        }
    }
}


//metodo que preenche uma face(triangulo) de objeto 3D com cor desejada, funcionando sem falhas (preenche com linhas)
//  Funcionamento: encontra o ponto central de cada triangulo e vai pintar linhas percorrendo:
//      pega cada ponto da linha entre v1 e v2, pintando uma linha sempre desse ponto ate o ponto central do triangulo
//      pega cada ponto da linha entre v1 e v3, pintando uma linha sempre desse ponto ate o ponto central do triangulo
//      pega cada ponto da linha entre v2 e v3, pintando uma linha sempre desse ponto ate o ponto central do triangulo

//  OBS: primeiro tentei da forma que o professor havia falado em aula, mas ficou com algumas falhas nos triangulos, 
//      entao optei por este metodo com mais linhas dentro de cada triangulo e nao encontrei falhas
void image::preenche_triangulo(int opcao, int x1, int y1, int x2, int y2, int x3, int y3, int r, int g, int b){
    int i, dx, dy, sdx, sdy, dxabs, dyabs, x, y, px, py, gx, gy;
    for(int j=0; j<3; j++){
        if(j==0){
            dx=x2-x1;
            dy=y2-y1;
            px=x1;
            py=y1;
            gx= x3;
            gy= y3;
        }else if(j==1){
            dx=x3-x2;
            dy=y3-y2;
            px=x2;
            py=y2;
            gx= x1;
            gy= y1;
        }else{
            dx=x3-x1;
            dy=y3-y1;
            px=x1;
            py=y1;
            gx= x2;
            gy= y2;
        }
        dxabs=abs(dx);
        dyabs=abs(dy);
        sdx=sgn(dx);
        sdy=sgn(dy);
        x=dyabs>>1;
        y=dxabs>>1;
        
        if(dxabs>=dyabs){ // the line is more horizontal than vertical 
            for(i=0; i<dxabs; i++){
                y+=dyabs;
                if(y>=dxabs){
                    y-=dxabs;
                    py+=sdy;
                }
                px+=sdx;
                if(opcao == 0){
                    drawline(0, px, py, gx, gy, r, g, b);
                }else if(opcao == 2){
                    drawline(2, px, py, gx, gy, r, g, b);
                }
            }
        }else{  // the line is more vertical than horizontal 
            for(i=0; i<dyabs; i++){
                x+=dxabs;
                if(x>=dyabs){
                    x-=dyabs;
                    px+=sdx;
                }
                py+=sdy;
                if(opcao == 0){
                    drawline(0, px, py, gx, gy, r, g, b);
                }else if(opcao == 2){
                    drawline(2, px, py, gx, gy, r, g, b);
                }
            }
        }
    }
}


//metodo que preenche com cor todas as faces(triangulos) do objeto 3D na imagem 2D 
//de acordo com escolha de ocultacao de faces
void image::pinta_faces_triangulos_image_3d_em_image_2d(int opcao, int ocultacao, int opcao_fundo, int centrox, int centroy, int proporcao, int r_frente, int g_frente, int b_frente, int r_fundo, int g_fundo, int b_fundo){
    int i;
    inicializa_visao_camera_3d();
    //ordena faces da menor para maior, pois abaixo o algoritmo vai pintar por ultimo as faces mais proximas da camera
    if(ocultacao == 0){
        mergeSort(0, f_v1.size()-1);
    }
    if(opcao == 0){ //pintura com cor de frente e de fundo do objeto
        float valor= 0.0, valor_maior;
        float vetor[vz.size()];
        float media;

        valor-= menor_vz_3d;
        valor_maior= maior_vz_3d+valor;
        for(i=0; i < vz.size(); i++){
            vetor[i]= vz[i]+valor;
            //printf("%lf ",vetor[i]);
        }
        for(i=0; i < f_v1.size(); i++){
            //if(vz[f_v1[i]] >= media_vz_3d){
                media= (vetor[f_v1[i]]+vetor[f_v2[i]]+vetor[f_v3[i]])/3;
                preenche_triangulo(0, centrox+(vy[f_v1[i]]*proporcao), centroy+(vx[f_v1[i]]*proporcao), centrox+(vy[f_v2[i]]*proporcao), centroy+(vx[f_v2[i]]*proporcao), centrox+(vy[f_v3[i]]*proporcao), centroy+(vx[f_v3[i]]*proporcao), (media/valor_maior)*r_frente, (media/valor_maior)*g_frente, (media/valor_maior)*b_frente);
                seta_faces_visao_camera(centrox+(vy[f_v1[i]]*proporcao), centroy+(vx[f_v1[i]]*proporcao), centrox+(vy[f_v2[i]]*proporcao), centroy+(vx[f_v2[i]]*proporcao), centrox+(vy[f_v3[i]]*proporcao), centroy+(vx[f_v3[i]]*proporcao), (media/valor_maior)*r_frente, (media/valor_maior)*g_frente, (media/valor_maior)*b_frente);
            //}
        }
        for(i=0; i < f_v1.size(); i++){
            media= (vetor[f_v1[i]]+vetor[f_v2[i]]+vetor[f_v3[i]])/3;
            preenche_triangulo(2, centrox+(vy[f_v1[i]]*proporcao), centroy+(vx[f_v1[i]]*proporcao), centrox+(vy[f_v2[i]]*proporcao), centroy+(vx[f_v2[i]]*proporcao), centrox+(vy[f_v3[i]]*proporcao), centroy+(vx[f_v3[i]]*proporcao), (media/valor_maior)*r_frente, (media/valor_maior)*g_frente, (media/valor_maior)*b_frente);
        }
    }else if(opcao == 1){   //pintura com cores aleatorias para cada face
        for(i=0; i < f_v1.size(); i++){
            //if(vz[f_v1[i]] >= media_vz_3d){
                preenche_triangulo(0, centrox+(vy[f_v1[i]]*proporcao), centroy+(vx[f_v1[i]]*proporcao), centrox+(vy[f_v2[i]]*proporcao), centroy+(vx[f_v2[i]]*proporcao), centrox+(vy[f_v3[i]]*proporcao), centroy+(vx[f_v3[i]]*proporcao), rand()%256, rand()%256, rand()%256);
                seta_faces_visao_camera(centrox+(vy[f_v1[i]]*proporcao), centroy+(vx[f_v1[i]]*proporcao), centrox+(vy[f_v2[i]]*proporcao), centroy+(vx[f_v2[i]]*proporcao), centrox+(vy[f_v3[i]]*proporcao), centroy+(vx[f_v3[i]]*proporcao), rand()%256, rand()%256, rand()%256);
            //}
        }
        for(i=0; i < f_v1.size(); i++){
            preenche_triangulo(2, centrox+(vy[f_v1[i]]*proporcao), centroy+(vx[f_v1[i]]*proporcao), centrox+(vy[f_v2[i]]*proporcao), centroy+(vx[f_v2[i]]*proporcao), centrox+(vy[f_v3[i]]*proporcao), centroy+(vx[f_v3[i]]*proporcao), rand()%256, rand()%256, rand()%256);
        }
    }   
    if(opcao_fundo == 0){
        for(i=0; i < altura; i++){
            for(int j=0; j < largura; j++){
                visao_camera[i][j]= visao_camera[i][j]-1;
                visao_camera[i][j]= visao_camera[i][j]*(-1);
                put_pixel(visao_camera[i][j]*i,visao_camera[i][j]*j, r_fundo, g_fundo, b_fundo);
            }
        }
    }
}


//funcao principal para coordenar todos os processos
int main(int argc, char **argv){
    time_t t;
    srand((unsigned) time(&t));
    string arq_obj_3d, img_2d_destino_final, opcao_image_2d, nome_image_2d, ocultacao_face, fundo_image_2d, pintura_colorida;
    double rotacao_x, rotacao_y, rotacao_z;
    int resp_fundo=0, resp;
    int red_frente, green_frente, blue_frente, red_fundo, green_fundo, blue_fundo, pos_centro_x, pos_centro_y, escala;

    //recebe os argumentos passados via execucao do script
    if(argc == 15){     
        arq_obj_3d= argv[1];                                //carrega endereco/nome_obj_3d.obj que sera lido
        img_2d_destino_final= argv[2];                      // endereco/nome_img_2d.ppm que sera salva no final de todo processo
        rotacao_x= (atoi(argv[3]))*(3.141592654/180);       //recebe angulo X em graus e transforma em radianos
        rotacao_y= (atoi(argv[4]))*(3.141592654/180);       //recebe angulo Y em graus e transforma em radianos
        rotacao_z= (atoi(argv[5]))*(3.141592654/180);       //recebe angulo Z em graus e transforma em radianos
        pos_centro_x= atoi(argv[7]);                    //ponto central do eixo X da imagem 2D que ser pintado o obj 3D
        pos_centro_y= atoi(argv[6]);                    //ponto central do eixo Y da imagem 2D que ser pintado o obj 3D
        escala= atoi(argv[8]);                          //escala do objeto 3D para pintar na imagem 2D
        red_frente= atoi(argv[9]);                      //cor red fixa do obj 3D
        green_frente= atoi(argv[10]);                   //cor green fixa do obj 3D
        blue_frente= atoi(argv[11]);                    //cor blue fixa do obj 3D
        red_fundo= atoi(argv[12]);                      //cor red fixa do fundo do obj 3D
        green_fundo= atoi(argv[13]);                    //cor green fixa do fundo do obj 3D
        blue_fundo= atoi(argv[14]);                     //cor blue fixa do fundo do obj 3D

        image* figura= new image(10,10);                //cria alocacao para uma imagem bem pequena
        figura->carrega_image_obj_3d(arq_obj_3d);       //carrega obj 3D para 
        figura->rotaciona_image_3d(rotacao_x, rotacao_y, rotacao_z);

        cout << "\nDigite suas respostas somente 's' ou 'n', em minusculo.";
        cout << "\nDeseja carregar objeto 3D em cima de uma outra imagem 2D? ";
        cin >> opcao_image_2d;
        if(opcao_image_2d == "n"){
            resp= figura->gera_image_2d_pelo_tamanho_imagem_3d(pos_centro_x, pos_centro_y, escala);
            if(resp < 0){
                return resp;
            }
        }else if(opcao_image_2d == "s"){
            cout << "Digite o nome da imagem 2D dentro da pasta: ";
            cin >> nome_image_2d;
            cout << "Deseja manter imagem escolhida como fundo do objeto 3D? ";
            cin >> fundo_image_2d;
            if(fundo_image_2d == "s"){
                resp_fundo= 1;
            }
            figura->carrega_image_ppm_arquivo(nome_image_2d);
            //figura->carrega_image_ppm_arquivo("night.ppm");
        }else{
            cout << "\nOpcao incorreta! Digite 's' ou 'n'\n";
            return -1;
        }

        cout << "Deseja esconder faces ocultas? ";
        cin >> ocultacao_face;
        cout << "Que forma deseja pintar o objeto 3D na imagem? \n\tPintura colorida aleatoriamente digite 's'.\n\tSe quiser com as cores passadas, digite 'n'. \n\tResposta: ";
        cin >> pintura_colorida;

        if(ocultacao_face == "n"){
            figura->pinta_linhas_das_faces_obj_3d_em_image_2d(1, pos_centro_x, pos_centro_y, escala, red_frente, green_frente, blue_frente);
            if(pintura_colorida == "s"){
                figura->pinta_faces_triangulos_image_3d_em_image_2d(1, 1, resp_fundo, pos_centro_x, pos_centro_y, escala, red_frente, green_frente, blue_frente, red_fundo, green_fundo, blue_fundo);
            }else if(pintura_colorida == "n"){
                figura->pinta_faces_triangulos_image_3d_em_image_2d(0, 1, resp_fundo, pos_centro_x, pos_centro_y, escala, red_frente, green_frente, blue_frente, red_fundo, green_fundo, blue_fundo);
            }else{
                cout << "\nOpcao incorreta! Digite 's' ou 'n'\n";
                return -1;
            }
        }else if(ocultacao_face == "s"){
            figura->pinta_linhas_das_faces_obj_3d_em_image_2d(0, pos_centro_x, pos_centro_y, escala, red_frente, green_frente, blue_frente);
            if(pintura_colorida == "s"){
                figura->pinta_faces_triangulos_image_3d_em_image_2d(1, 0, resp_fundo, pos_centro_x, pos_centro_y, escala, red_frente, green_frente, blue_frente, red_fundo, green_fundo, blue_fundo);
            }else if(pintura_colorida == "n"){
                figura->pinta_faces_triangulos_image_3d_em_image_2d(0, 0, resp_fundo, pos_centro_x, pos_centro_y, escala, red_frente, green_frente, blue_frente, red_fundo, green_fundo, blue_fundo);
            }else{
                cout << "\nOpcao incorreta! Digite 's' ou 'n'\n";
                return -1;
            }
        }else{
            cout << "\nOpcao incorreta! Digite 's' ou 'n'\n";
            return -1;
        }
        figura->salva_image_rgb(img_2d_destino_final);
        figura->~image();
        cout << "\nImagem gerada: imagem_gerada.ppm\n";
    }else{
        printf("Erro com parametros passados!\nEscreva:\n1) g++ -o exec cria_imagens_3D_POO.cpp -std=c++11\n");
        printf("2) ./exec arq_origem.obj arq_destino.ppm x y z centro_x centro_y escala red_frente green_frente blue_frente red_fundo green_fundo blue_fundo\n");
        return -1;
    }
    return 0;
}
